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

TEST(InputTests, ChineseArrayTest) {
  std::stringstream input_stream;
  input_stream << "[0] 你好世界|世界你好|呃呃\n";
  std::cin.rdbuf(input_stream.rdbuf());

  Input input;
  EXPECT_EQ(input.GetTimestamp(), 0);
  auto res = input.GetChineseArray<10, 100>();
  EXPECT_EQ(ChineseToString<10>(res[0]), "你好世界");
  EXPECT_EQ(ChineseToString<10>(res[1]), "世界你好");
  EXPECT_EQ(ChineseToString<10>(res[2]), "呃呃");
  EXPECT_EQ(res[3][0], 0);
}

TEST(InputTests, IntegerArrayTest) {
  std::stringstream input_stream;
  input_stream << "[0] 0|1|114\n";
  std::cin.rdbuf(input_stream.rdbuf());

  Input input;
  EXPECT_EQ(input.GetTimestamp(), 0);
  auto res = input.GetIntegerArray<99>();
  EXPECT_EQ(res[0], 0);
  EXPECT_EQ(res[1], 1);
  EXPECT_EQ(res[2], 114);
  EXPECT_EQ(res[3], 0);
}

TEST(InputTests, DateTest) {
  std::stringstream input_stream;
  input_stream << "[0] 06-10|08-31\n";
  std::cin.rdbuf(input_stream.rdbuf());

  Input input;
  EXPECT_EQ(input.GetTimestamp(), 0);
  EXPECT_EQ(input.GetDate(), 9);
  EXPECT_EQ(input.GetDate(), 91);
}

TEST(InputTests, TimeTest) {
  std::stringstream input_stream;
  input_stream << "[0] 23:51\n";
  std::cin.rdbuf(input_stream.rdbuf());

  Input input;
  EXPECT_EQ(input.GetTimestamp(), 0);
  EXPECT_EQ(input.GetTime(), 23 * 60 + 51);
}

}