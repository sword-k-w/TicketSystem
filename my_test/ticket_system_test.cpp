#include "system/system.h"
#include "gtest/gtest.h"

namespace sjtu {
TEST(TicketSystemTests, BasicBuyTicketTest) {
  std::stringstream input_stream;
  input_stream << "[1] clean\n";
  input_stream << "[2] add_train -i HAPPY_TRAIN -n 3 -m 1000 -s 上院|中院|下院 -p 114|514 -x 19:19 -t 600|600 -o 5 -d 06-01|08-17 -y G\n";
  input_stream << "[3] release_train -i HAPPY_TRAIN\n";
  input_stream << "[4] add_user -c a -g 1 -u Texas -p 114514 -n 强 -m @sjtu.edu.cn\n";
  input_stream << "[5] login -u Texas -p 114514\n";
  input_stream << "[6] buy_ticket -u Texas -i HAPPY_TRAIN -d 08-17 -n 800 -f 中院 -t 下院\n";
  input_stream << "[7] buy_ticket -u Texas -i HAPPY_TRAIN -d 08-17 -n 800 -f 中院 -t 下院\n";
  input_stream << "[8] exit\n";
  std::cin.rdbuf(input_stream.rdbuf());
  std::cerr << input_stream.str();

  std::streambuf* originalCoutBuf = std::cout.rdbuf();

  std::stringstream output_stream;
  std::cout.rdbuf(output_stream.rdbuf());

  System system("sword");
  system.Run();

  EXPECT_EQ(output_stream.str(), "[1] 0\n[2] 0\n[3] 0\n[4] 0\n[5] 0\n[6] 411200\n[7] -1\n[8] bye\n");

  std::cout.rdbuf(originalCoutBuf);
}
}