#include "system/system.h"
#include "gtest/gtest.h"

namespace sjtu {

TEST(TrainSystemTests, BasicTrainTest) {
  std::stringstream input_stream;
  input_stream << "[1] add_train -i HAPPY_TRAIN -n 3 -m 1000 -s 上院|中院|下院 -p 114|514 -x 19:19 -t 600|600 -o 5 -d 06-01|08-17 -y G\n";
  input_stream << "[2] query_train -d 07-01 -i HAPPY_TRAIN\n";
  std::cin.rdbuf(input_stream.rdbuf());

  std::stringstream output_stream;
  std::cout.rdbuf(output_stream.rdbuf());

  System system("sword");
  system.Run();

  EXPECT_EQ(output_stream.str(), "[1] 0\n[2] HAPPY_TRAIN G\n上院 xx-xx xx:xx -> 07-01 19:19 0 1000\n上院 xx-xx xx:xx -> 07-01 19:19 0 1000\n下院 07-02 15:24 -> xx-xx xx:xx 628 x\n");
}

}