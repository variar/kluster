#pragma once

#include <string>

#include <chrono>

#include <boost/optional.hpp>

#include <messages.h>

#include "nano_exception.h"

using kluster::messages::NanoMessage;

namespace kluster
{
  namespace transport
  {
    class NanoSocket
    {
    public:
      NanoSocket(int socketType) throw();
      ~NanoSocket();

      void Bind(const std::string& address) throw();
      void Connect(const std::string& address) throw();

      void Send(const NanoMessage& message) throw();

      template<typename T>
      void Send(const T& message) throw()
      {
        NanoMessage nm;
        nm.SetMessageData(message);
        Send(nm);
      }

      NanoMessage Recv() throw();

      void SetSubscription(const std::string& sub = std::string()) throw();

      static std::pair<int, boost::optional<NanoMessage>> Poll(NanoSocket** sockets, size_t len, std::chrono::milliseconds timeout) throw();

    private:
      int m_socket;
    };
  }
}
