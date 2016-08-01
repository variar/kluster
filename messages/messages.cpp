#include "messages.h"

namespace kluster
{
  namespace messages
  {
    NanoMessage CreateWorkerPingMessage(const NodeId& from)
    {
      return NanoMessage{{message_type::WorkerPing, from}};
    }

    NanoMessage CreateWorkerPongMessage(const NodeId& from)
    {
      return NanoMessage{{message_type::WorkerPong, from}};
    }

    std::string NanoMessage::SaveMessage(const NanoMessage& message) throw()
    {
      return Serialize(message);
    }

    NanoMessage NanoMessage::LoadMessage(const std::string& data) throw()
    {
      NanoMessage message;
      Deserialize<NanoMessage>(data, message);
      return message;
    }
  }
}
