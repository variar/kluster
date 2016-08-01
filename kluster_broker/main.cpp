#include <nano_socket.h>
#include <messages.h>

#include <reqrep.h>
#include <pipeline.h>
#include <pubsub.h>

#include <sstream>
#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

using namespace kluster::transport;
using namespace kluster::messages;

class KlusterBroker
{
public:
  KlusterBroker()
  : m_clientSocket{NN_REP}
  , m_jobSocket{NN_PUB}
  , m_tasksSocket{NN_PUSH}
  , m_resultsSocket{NN_PULL}
  {}

  void CreateConnections(int port)
  {
    std::stringstream localBrokerAddres;
    localBrokerAddres << "tcp://*:" << port;
    m_clientSocket.Bind(localBrokerAddres.str());

    std::cout << "Created broker at " << localBrokerAddres.str() << std::endl;

    std::stringstream jobPubAddress;
    jobPubAddress << "tcp://*:" << port + 1;
    m_jobSocket.Bind(jobPubAddress.str());

    std::cout << "Job pub at " << jobPubAddress.str() << std::endl;

    std::stringstream taskSourceAddress;
    taskSourceAddress << "tcp://*:" << port + 2;
    m_tasksSocket.Bind(taskSourceAddress.str());

    std::cout << "Tasks source at " << taskSourceAddress.str() << std::endl;

    std::stringstream taskResultsAddress;
    taskResultsAddress << "tcp://*:" << port + 3;
    m_resultsSocket.Bind(taskResultsAddress.str());

    std::cout << "Tasks sink at " << taskResultsAddress.str() << std::endl;
  }

  void ProcessMessages()
  {
    std::cout << "Started message loop" << std::endl;
    while(1)
    {
      try
      {
        NanoMessage message = m_clientSocket.Recv();
        std::cout << "Got message type " << message.header.type << std::endl;
        if (message.header.type == message_type::JobRequest)
        {

          JobRequestMessage jobRequest;
          if (message.GetMessageData<JobRequestMessage>(jobRequest))
          {
            OrchestrateJob(std::move(jobRequest));
          }
        }
      }
      catch(const std::exception&)
      {

      }
    }
  }

  void OrchestrateJob(JobRequestMessage jobRequest)
  {
      std::wcout << "Got job request " << jobRequest.jobId << std::endl;

      FilesCollection taskFiles = std::move(jobRequest.taskFiles);

      m_jobSocket.Send(jobRequest);

      size_t tasksSize = taskFiles.size();
      while(!taskFiles.empty())
      {
        TaskRequestMessage taskRequest;
        taskRequest.jobId = jobRequest.jobId;
        taskRequest.taskFile = std::move(taskFiles.back());
        taskFiles.pop_back();

        std::wcout << "Sending task " << taskRequest.taskFile.name << std::endl;

        m_tasksSocket.Send(taskRequest);
      }

      JobResponseMessage response;
      response.jobId = jobRequest.jobId;

      while (tasksSize > 0)
      {
        NanoMessage message = m_resultsSocket.Recv();
        if (message.header.type == message_type::TaskResponse)
        {
          TaskResponseMessage taskResponse;
          message.GetMessageData<TaskResponseMessage>(taskResponse);
          if (taskResponse.jobId == jobRequest.jobId)
          {
            tasksSize--;
            std::wcout << "Got reply. Waiting for " << tasksSize << " more" << std::endl;
            std::copy(std::begin(taskResponse.taskResults),
                      std::end(taskResponse.taskResults),
                      std::back_inserter(response.taskResults));

          }
        }
      }

      EndJobMessage endJobMessage;
      endJobMessage.jobId = jobRequest.jobId;
      m_jobSocket.Send(endJobMessage);
      m_clientSocket.Send(response);
  }

private:
  NanoSocket m_clientSocket;
  NanoSocket m_jobSocket;
  NanoSocket m_tasksSocket;
  NanoSocket m_resultsSocket;
};

int main(int argc, const char** argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("broker_port", po::value<int>(), "broker port");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  int brokerPort = 12345;
  if (vm.count("broker_port"))
  {
      brokerPort = vm["broker_port"].as<int>();
  }
  std::cout << "Broker at port " << brokerPort << std::endl;

  KlusterBroker kb;
  kb.CreateConnections(brokerPort);
  kb.ProcessMessages();

  return 0;
}
