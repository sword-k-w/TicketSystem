#include "system/input.h"
#include "system/output.hpp"
#include "gtest/gtest.h"
#include "my_stl/array.hpp"

namespace sjtu {

TEST(InputTests, BasicInputTest) {
  std::stringstream input_stream;
  input_stream << "[114514] orz -a %%%\n";
  std::cin.rdbuf(input_stream.rdbuf());

  Input input;
  EXPECT_EQ(input.GetTimestamp(), 114514);
  EXPECT_EQ(input.GetCommand(), "orz");
  EXPECT_EQ(input.GetKey(), 'a');
  array<char, 20> res;
  res[0] = '%';
  res[1] = '%';
  res[2] = '%';
  EXPECT_EQ(input.GetString<20>(), res);
}

TEST(InputTests, ChineseTest) {
  std::stringstream input_stream;
  input_stream << "[0] 你好世界\n";
  std::cin.rdbuf(input_stream.rdbuf());

  Input input;
  EXPECT_EQ(input.GetTimestamp(), 0);
  EXPECT_EQ(ChineseToString<5>(input.GetChinese<5>()), "你好世界");
}

}