#pragma once

#include <nano_socket.h>

class KlusterClient
{
public:
  KlusterClient();

  void Connect(const std::string& brokerAddress) throw();

  void SendJobRequest(
    const std::string& jobPath,
    const std::string& tasksPath,
    const std::string& cmdLine) throw();

  void WaitForResults() throw();

private:
  kluster::transport::NanoSocket m_socket;
};
