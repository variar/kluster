#include "gtest/gtest.h"
#include <messages.h>

using namespace kluster::messages;

class MessagesTest : public ::testing::Test
{

};

TEST_F(MessagesTest, PingMessageShouldHavePingType)
{
  NodeId from {L"123"};
  auto m = CreateWorkerPingMessage(from);
  EXPECT_EQ(message_type::WorkerPing, m.header.type);
  EXPECT_EQ(from, m.header.senderNode);
}

TEST_F(MessagesTest, AllowToSetEmbededMessage)
{
  NodeId from {L"123"};
  auto m = NanoMessage{ {message_type::JobRequest, from} };
  JobRequestMessage data;

  m.SetMessageData(data);

}

TEST_F(MessagesTest, SaveAndLoadMessagesAreSame)
{
  NodeId from {L"123"};
  auto m = CreateWorkerPingMessage(from);

  auto data = NanoMessage::SaveMessage(m);
  auto m1 = NanoMessage::LoadMessage(data);

  EXPECT_EQ(m.header.type, m1.header.type);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
