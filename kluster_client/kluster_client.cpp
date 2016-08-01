#include "kluster_client.h"

#include <reqrep.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <messages.h>

#include <iostream>

using namespace kluster::transport;
using namespace kluster::messages;

namespace fs = boost::filesystem;
namespace uuids = boost::uuids;

namespace
{
  FilesCollection CollectFiles(const std::string& path)
  {
    std::cout << "Collecting files from " << path << std::endl;
    FilesCollection result;

    fs::path filesPath {path};

    fs::directory_iterator dirIt {fs::absolute(filesPath)};
    const fs::directory_iterator dirEnd;
    uintmax_t totalSize = 0;
    while(dirIt != dirEnd)
    {
      const fs::path entryPath = (*dirIt).path();
      if (fs::is_regular_file(entryPath))
      {
        const auto size = fs::file_size(entryPath);

        FileData fileData;
        fileData.name = entryPath.filename().wstring();

        fs::ifstream stream {entryPath, std::ios::in | std::ios::binary};
        if (stream.is_open())
        {
          fileData.data.reserve(size);
		  std::copy(std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}, std::back_inserter(fileData.data));
          totalSize += size;
        }

        std::wcout << "Entry " << fileData.name << ", size=" << fileData.data.size() << std::endl;
        result.push_back(std::move(fileData));
      }
      ++dirIt;
    }

    std::wcout << "Total size " << totalSize << std::endl;

    return result;
  }
}

KlusterClient::KlusterClient(): m_socket {NN_REQ} {}

void KlusterClient::Connect(const std::string& brokerAddress) throw()
{
  m_socket.Connect(brokerAddress);
  std::cout << "Connected to broker " << brokerAddress << std::endl;
}

void KlusterClient::SendJobRequest(
  const std::string& jobPath,
  const std::string& tasksPath,
  const std::string& cmdLine) throw()
{
  NanoMessage message;
  JobRequestMessage jobRequest;

  jobRequest.jobId = uuids::to_wstring(uuids::random_generator()());
  jobRequest.jobFiles = CollectFiles(jobPath);
  jobRequest.taskFiles = CollectFiles(tasksPath);
  jobRequest.cmdLine = cmdLine;


  std::wcout << "Job id " << jobRequest.jobId << std::endl;
  message.SetMessageData(jobRequest);
  m_socket.Send(message);
}

void KlusterClient::WaitForResults() throw()
{
  std::cout << "Waiting for reply" << std::endl;
  try
  {
    NanoMessage message = m_socket.Recv();
    std::cout << "Got message type " << message.header.type << std::endl;

    if (message.header.type == message_type::JobResponse)
    {
      JobResponseMessage response;
      message.GetMessageData(response);
      for (const auto& resultFile : response.taskResults)
      {
        std::wcout << "Got result file " << resultFile.name << ", size " << resultFile.data.size() << std::endl;
        fs::path path {resultFile.name};
        fs::ofstream stream {path, std::ios::out | std::ios::binary};
        if (stream.is_open())
        {
		  std::copy(resultFile.data.begin(), resultFile.data.end(), std::ostreambuf_iterator<char>(stream));
		  stream.close();
        }
      }
    }

    return;
  }
  catch(const std::exception&)
  {

  }

}
