#include "system/system.h"
#include "gtest/gtest.h"

namespace sjtu {

TEST(TrainSystemTests, DISABLED_BasicQueryTrainTest) {
  std::stringstream input_stream;
  input_stream << "[1] add_train -i HAPPY_TRAIN -n 3 -m 1000 -s 上院|中院|下院 -p 114|514 -x 19:19 -t 600|600 -o 5 -d 06-01|08-17 -y G\n";
  input_stream << "[2] query_train -d 07-01 -i HAPPY_TRAIN\n";
  input_stream << "[3] exit\n";
  std::cin.rdbuf(input_stream.rdbuf());
  std::cerr << input_stream.str();

  std::streambuf* originalCoutBuf = std::cout.rdbuf();

  std::stringstream output_stream;
  std::cout.rdbuf(output_stream.rdbuf());

  System system("sword");
  system.Run();

  EXPECT_EQ(output_stream.str(), "[1] 0\n[2] HAPPY_TRAIN G\n上院 xx-xx xx:xx -> 07-01 19:19 0 1000\n中院 07-02 05:19 -> 07-02 05:24 114 1000\n下院 07-02 15:24 -> xx-xx xx:xx 628 x\n[3] bye\n");

  std::cout.rdbuf(originalCoutBuf);
}

TEST(TrainSystemTests, DISABLED_BasicQueryTicketTest) {
  std::stringstream input_stream;
  input_stream << "[1] add_train -i HAPPY_TRAIN -n 3 -m 1000 -s 上院|中院|下院 -p 114|514 -x 19:19 -t 600|600 -o 5 -d 06-01|08-17 -y G\n";
  input_stream << "[2] release_train -i HAPPY_TRAIN\n";
  input_stream << "[3] query_ticket -s 中院 -t 下院 -d 08-17\n";
  input_stream << "[4] exit\n";
  std::cin.rdbuf(input_stream.rdbuf());
  std::cerr << input_stream.str();

  std::streambuf* originalCoutBuf = std::cout.rdbuf();

  std::stringstream output_stream;
  std::cout.rdbuf(output_stream.rdbuf());

  System system("sword");
  system.Run();

  EXPECT_EQ(output_stream.str(), "[1] 0\n[2] 0\n[3] 1\nHAPPY_TRAIN 中院 08-17 05:24 -> 下院 08-17 15:24 514 1000\n[4] bye\n");

  std::cout.rdbuf(originalCoutBuf);
}

TEST(TrainSystemTests, DISABLED_BasicQueryTransferTest) {
  std::stringstream input_stream;
  input_stream << "[1] add_train -i HAPPY_TRAIN -n 3 -m 1000 -s 上院|中院|下院 -p 114|514 -x 19:19 -t 600|600 -o 5 -d 06-01|08-17 -y G\n";
  input_stream << "[2] release_train -i HAPPY_TRAIN\n";
  input_stream << "[3] add_train -i HAHA_TRAIN -n 2 -m 500 -s 中院|下院 -p 600 -x 10:25 -t 660 -o _ -d 06-01|08-17 -y H\n";
  input_stream << "[4] release_train -i HAHA_TRAIN\n";
  input_stream << "[5] query_transfer -s 上院 -t 下院 -d 08-16\n";
  input_stream << "[6] exit\n";
  std::cin.rdbuf(input_stream.rdbuf());
  std::cerr << input_stream.str();

  std::streambuf* originalCoutBuf = std::cout.rdbuf();

  std::stringstream output_stream;
  std::cout.rdbuf(output_stream.rdbuf());

  System system("sword");
  system.Run();

  EXPECT_EQ(output_stream.str(), "[1] 0\n[2] 0\n[3] 0\n[4] 0\n[5] HAPPY_TRAIN 上院 08-16 19:19 -> 中院 08-17 05:19 114 500\nHAHA_TRAIN 中院 08-17 10:25 -> 下院 08-17 21:25 600 500\n[6] bye\n");

  std::cout.rdbuf(originalCoutBuf);
}

}