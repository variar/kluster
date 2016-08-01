#include "nano_socket.h"

#include <nn.h>
#include <pubsub.h>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include <iostream>

namespace ios = boost::iostreams;

namespace kluster
{
  namespace transport
  {

    NanoSocket::NanoSocket(int socketType) throw()
    {
      m_socket = nn_socket(AF_SP, socketType);
      if (m_socket < 0)
       throw NanoException{};

      std::cout << "Created socket " << m_socket << ", type " << socketType << std::endl;
    }

    NanoSocket::~NanoSocket()
    {
      nn_shutdown(m_socket, 0);
    }

    void NanoSocket::Bind(const std::string& address) throw()
    {
      if (nn_bind(m_socket, address.c_str()) < 0)
        throw NanoException{};
    }

    void NanoSocket::Connect(const std::string& address) throw()
    {
      if (nn_connect(m_socket, address.c_str()) < 0)
        throw NanoException{};
    }

    void NanoSocket::Send(const NanoMessage& message) throw()
    {
      std::stringstream data;
      data << NanoMessage::SaveMessage(message);

      ios::filtering_streambuf<ios::input> compressor;
      compressor.push(ios::zlib_compressor());
      compressor.push(data);

      std::stringstream compressed;
      ios::copy(compressor, compressed);

      const std::string compressedData = compressed.str();

      std::cout << "Socket " << m_socket << ": "
       << "Sending message type " << message.header.type << ", size " << compressedData.size() << std::endl;
      if (nn_send(m_socket, compressedData.data(), compressedData.size(), 0) < 0)
        throw NanoException{};
    }

    NanoMessage NanoSocket::Recv() throw()
    {
      char *buf = NULL;
      int size = nn_recv (m_socket, &buf, NN_MSG, 0);
      if (size < 0)
      {
        throw NanoException{};
      }

      std::stringstream compressed;
      compressed.write(buf, size);
      nn_freemsg (buf);

      ios::filtering_streambuf<ios::input> decompressor;
      decompressor.push(ios::zlib_decompressor());
      decompressor.push(compressed);

      std::stringstream decompressed;
      ios::copy(decompressor, decompressed);

      NanoMessage nm = NanoMessage::LoadMessage(decompressed.str());
      std::cout << "Socket " << m_socket << ": "
        << "got message type " << nm.header.type << std::endl;
      return nm;
    }

    void NanoSocket::SetSubscription(const std::string& sub) throw()
    {
      if (nn_setsockopt (m_socket, NN_SUB, NN_SUB_SUBSCRIBE, sub.c_str(), 0) < 0)
      {
        throw NanoException{};
      }
    }

    std::pair<int, boost::optional<NanoMessage>> NanoSocket::Poll(NanoSocket **sockets, size_t len, std::chrono::milliseconds timeout) throw()
    {
      std::vector<nn_pollfd> pfds;

      std::stringstream trace;
      trace << "Polling [ ";
      for (size_t i=0; i<len; ++i)
      {
        nn_pollfd pfd;
        pfd.fd = sockets[i]->m_socket;
        pfd.events = NN_POLLIN;
        pfds.push_back(pfd);
        trace << pfd.fd << " ";
      }

      std::cout << trace.str() << "]" << std::endl;

      boost::optional<NanoMessage> message;
      int rc = nn_poll(&pfds[0], len, -1);

      if (rc == -1) {
          throw NanoException();
      }

      int socketIndex = -1;
      if (rc > 0)
      {
        for (size_t i=0; i<len; ++i)
        {
          if (pfds[i].revents & NN_POLLIN)
          {
            socketIndex = i;
            break;
          }
        }
      }

      if (socketIndex >= 0)
      {
        message = sockets[socketIndex]->Recv();
      }

      return std::make_pair(socketIndex, message);
    }
  }
}
