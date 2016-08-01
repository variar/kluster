#pragma once

#include <string>
#include <sstream>
#include <vector>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/base_object.hpp>

namespace kluster
{
  namespace messages
  {

    namespace
    {
      template<typename T>
      std::string Serialize(const T& data) throw()
      {
        std::stringstream stream;
        boost::archive::binary_oarchive ar {stream};
        ar << data;
        return stream.str();
      }

      template<typename T>
      void Deserialize(const std::string& data, T& result) throw()
      {
        std::stringstream stream {data};
        boost::archive::binary_iarchive ar {stream};
        ar >> result;
      }
    }

    namespace message_type
    {
      enum Enum {
        Unknown,
        WorkerPing,
        WorkerPong,
        JobRequest,
        JobResponse,
        TaskRequest,
        TaskResponse,
        EndJob,
		CancelJob,
		UpdateWorker,
      };
    }

    typedef std::wstring NodeId;

    struct MessageHeader
    {
      MessageHeader() : type{message_type::Unknown} {}
      MessageHeader(message_type::Enum type_) : type{type_} {}

      MessageHeader(message_type::Enum type_, const NodeId& from)
      : type{type_}, senderNode {from} {}

      MessageHeader(message_type::Enum type_, const NodeId& from, const NodeId& to)
       : type{type_}, senderNode {from}, receiverNode {to} {}

      message_type::Enum type;

      NodeId senderNode;
      NodeId receiverNode;

    private:
      friend class boost::serialization::access;
      template<typename Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
          ar & type;
          ar & senderNode;
          ar & receiverNode;
      }
    };

    struct NanoMessage
    {
      NanoMessage() {}
      NanoMessage(const MessageHeader& header_) : header{header_} {}

      MessageHeader header;

      template<typename T>
      void SetMessageData(const T& data) throw()
      {
        header.type = data.type;
        m_message = Serialize(data);
      }

      template<typename T>
      bool GetMessageData(T& data) const throw()
      {
        if (data.type != header.type)
        {
          return false;
        }

        Deserialize<T>(m_message, data);
        return true;
      }

      static std::string SaveMessage(const NanoMessage& message) throw();
      static NanoMessage LoadMessage(const std::string& data) throw();

    private:
      friend class boost::serialization::access;
      template<typename Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
          ar & header;
          ar & m_message;
      }

      std::string m_message;
    };

    struct FileData
    {
      std::wstring name;
      std::vector<char> data;

    private:
      friend class boost::serialization::access;
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
          ar & name;
          ar & data;
      }
    };

    typedef std::vector<FileData> FilesCollection;

    struct TypedMessage
    {
      TypedMessage(message_type::Enum type_)
       : type {type_} {}

      const message_type::Enum type;
    };

    typedef std::wstring JobId;

	struct JobMessage : public TypedMessage
	{
		JobMessage(message_type::Enum type_) : TypedMessage(type_)
		{}

		JobId jobId;

	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & jobId;
		}
	};

    struct JobRequestMessage : public JobMessage
    {
      JobRequestMessage() : JobMessage(message_type::JobRequest) {}

      FilesCollection jobFiles;
      FilesCollection taskFiles;

      std::string cmdLine;
      uint32_t timeout;

    private:
      friend class boost::serialization::access;
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
		  ar & boost::serialization::base_object<JobMessage>(*this);

          ar & jobFiles;
          ar & taskFiles;
          ar & cmdLine;
          ar & timeout;
      }
    };

    struct JobResponseMessage : public JobMessage
    {
      JobResponseMessage() : JobMessage(message_type::JobResponse) {}
      FilesCollection taskResults;
      std::string error;

    private:
      friend class boost::serialization::access;
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
		  ar & boost::serialization::base_object<JobMessage>(*this);

          ar & taskResults;
          ar & error;
      }
    };

    struct EndJobMessage : public JobMessage
    {
      EndJobMessage() : JobMessage(message_type::EndJob) {}

    private:
      friend class boost::serialization::access;
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
		  ar & boost::serialization::base_object<JobMessage>(*this);
      }
    };

	struct CancelJobMessage : public JobMessage
	{
		CancelJobMessage() : JobMessage(message_type::CancelJob) {}

	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<JobMessage>(*this);
		}
	};

    struct TaskRequestMessage : public JobMessage
    {
      TaskRequestMessage() : JobMessage(message_type::TaskRequest) {}

      FileData taskFile;
      uint32_t timeout;

    private:
      friend class boost::serialization::access;
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
		  ar & boost::serialization::base_object<JobMessage>(*this);
          ar & taskFile;
          ar & timeout;
      }
    };

    struct TaskResponseMessage : public JobMessage
    {
      TaskResponseMessage() : JobMessage(message_type::TaskResponse) {}

      FilesCollection taskResults;
      std::string error;

    private:
      friend class boost::serialization::access;
      template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
      {
		  ar & boost::serialization::base_object<JobMessage>(*this);
          ar & taskResults;
          ar & error;
      }
    };

	struct UpdateWorkerMessage : public TypedMessage
	{
		UpdateWorkerMessage() : TypedMessage(message_type::UpdateWorker) {}

		FileData workerFile;
		
	private:
		friend class boost::serialization::access;
		template<class Archive>
		void serialize(Archive & ar, const unsigned int version)
		{
			ar & workerFile;
		}
	};
	

    NanoMessage CreateWorkerPingMessage(const NodeId& from);
    NanoMessage CreateWorkerPongMessage(const NodeId& from);

  }
}
