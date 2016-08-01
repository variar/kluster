#include <nn.h>
#include <nano_socket.h>
#include <messages.h>

#include <pubsub.h>
#include <pipeline.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/program_options.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

using namespace kluster::transport;
using namespace kluster::messages;

class TmpFilesStorage
{
public:
  TmpFilesStorage(const FilesCollection& files)
  {
    m_tmpPath = fs::temp_directory_path() / "kluster" / fs::unique_path();

    std::wcout << "Tmp path " << m_tmpPath << std::endl;

    fs::create_directories(m_tmpPath);

    for (const FileData& file: files)
    {
      fs::path path = m_tmpPath / file.name;
      fs::ofstream stream {path, std::ios::out | std::ios::trunc | std::ios::binary};

      if (stream.is_open())
      {
        stream.write(&file.data[0], file.data.size());
        stream.close();
        std::wcout << "Saved file to " << path.wstring() << std::endl;
      }
    }
  }

  ~TmpFilesStorage()
  {
    fs::remove_all(m_tmpPath);
  }

  std::wstring GetPath() const
  {
    return m_tmpPath.wstring();
  }

private:
  fs::path m_tmpPath;

};

class KlusterWorker
{
public:
  KlusterWorker()
  : m_jobSocket{NN_SUB}
  , m_tasksSocket{NN_PULL}
  , m_resultsSocket{NN_PUSH}
  {}

  void Join(const std::string& brokerAddress, int brokerPort) throw()
  {
    std::stringstream jobPubAddress;
    jobPubAddress << brokerAddress << ":" << brokerPort;
    m_jobSocket.Connect(jobPubAddress.str());
    m_jobSocket.SetSubscription();

    std::cout << "Connected job pub to " << jobPubAddress.str() << std::endl;

    std::stringstream taskSourceAddress;
    taskSourceAddress << brokerAddress << ":" << brokerPort+1;
    m_tasksSocket.Connect(taskSourceAddress.str());

    std::cout << "Tasks source to " << taskSourceAddress.str() << std::endl;

    std::stringstream taskResultsAddress;
    taskResultsAddress << brokerAddress << ":" << brokerPort+2;
    m_resultsSocket.Connect(taskResultsAddress.str());

    std::cout << "Tasks sink to " << taskResultsAddress.str() << std::endl;
  }

  void ProcessMessages()
  {
    std::cout << "Started message loop" << std::endl;
    for(;;)
    {
      try
      {
        NanoMessage message = m_jobSocket.Recv();
        std::cout << "Got message type " << message.header.type << std::endl;
        if (message.header.type == message_type::JobRequest)
        {
          std::wcout << "Got job request from " << message.header.senderNode << std::endl;
          JobRequestMessage jobRequest;

          if (message.GetMessageData<JobRequestMessage>(jobRequest))
          {
            DoJob(jobRequest);
          }
        }
      }
      catch(const std::exception&)
      {

      }
    }
  }

  void DoJob(const JobRequestMessage& job) throw()
  {
    TmpFilesStorage jobFiles {job.jobFiles};

    NanoSocket* sockets[2] = {&m_jobSocket, &m_tasksSocket};
    for(;;)
    {
      std::pair<int, boost::optional<NanoMessage>> pollMessage = NanoSocket::Poll(sockets, 2, std::chrono::milliseconds(0));
      if (pollMessage.second)
      {
        const NanoMessage& message = *pollMessage.second;
        if (message.header.type == message_type::EndJob)
        {
          std::cout << "Got end job" << std::endl;
          break;
        }
        if (message.header.type == message_type::TaskRequest)
        {
          TaskRequestMessage request;
          message.GetMessageData<TaskRequestMessage>(request);
          std::wcout << "Got task " << request.taskFile.name << std::endl;

          TaskResponseMessage response;
          response.jobId = request.jobId;
          m_resultsSocket.Send(response);
        }
      }
    }
  }

private:
  NanoSocket m_jobSocket;
  NanoSocket m_tasksSocket;
  NanoSocket m_resultsSocket;
};

int main(int argc, const char** argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("broker_ip", po::value<std::string>(), "broker ip")
      ("broker_port", po::value<int>(), "broker port")
      ("local_port", po::value<int>(), "local port");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  std::string brokerAddress = "tcp://127.0.0.1";
  if (vm.count("broker_ip"))
  {
      brokerAddress = vm["broker_ip"].as<std::string>();
  }
  int brokerPort = 12346;
  if (vm.count("broker_port"))
  {
      brokerPort = vm["broker_port"].as<int>();
  }
  std::cout << "Broker at " << brokerAddress << ":" << brokerPort << std::endl;

  KlusterWorker kw;
  kw.Join(brokerAddress, brokerPort);
  kw.ProcessMessages();

  return 0;
}
