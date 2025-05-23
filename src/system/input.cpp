#include "system/input.h"
#include <iostream>

namespace sjtu {

auto Input::GetTimestamp() -> int {
  assert(las_c_ == '\n');
  std::cin.get(las_c_);
  assert(las_c_ == '[');
  int res = 0;
  std::cin.get(las_c_);
  while (las_c_ != ']') {
    res = res * 10 + las_c_ - '0';
    std::cin.get(las_c_);
  }
  std::cin.get(las_c_);
  assert(las_c_ == ' ');
  return res;
}

auto Input::GetCommand() -> std::string {
  assert(las_c_ == ' ');
  std::cin.get(las_c_);
  std::string res;
  while (las_c_ != ' ' && las_c_ != '\n') {
    res += las_c_;
    std::cin.get(las_c_);
  }
  return res;
}

auto Input::GetKey() -> char {
  if (las_c_ == '\n') {
    return '\n';
  }
  assert(las_c_ == ' ');
  std::cin.get(las_c_);
  assert(las_c_ == '-');
  std::cin.get(las_c_);
  char res = las_c_;
  std::cin.get(las_c_);
  assert(las_c_ == ' ');
  return res;
}

template<int len>
auto Input::GetString() -> array<char, len> {
  assert(las_c_ == ' ');
  int pos = 0;
  array<char, len> res;
  std::cin.get(las_c_);
  while (las_c_ != ' ' && las_c_ != '\n') {
    res[pos++] = las_c_;
    std::cin.get(las_c_);
  }
  return res;
}

auto Input::GetSingleChinese() -> unsigned int {
  unsigned int res = 0;
  std::cin.get(las_c_);
  if ((las_c_ & 0x80) == 0) { // 0xxxxxxxx
    assert(las_c_ == ' ' || las_c_ == '\n');
    return 0;
  }
  if ((las_c_ & 0xE0) == 0xC0) { // 110xxxxx 10xxxxxxx
    res = (las_c_ & 0x1F) << 6;
    assert(std::cin.get(las_c_));
    res |= (las_c_ & 0x3F);
  } else if ((las_c_ & 0xF0) == 0xE0) { // 1110xxxx 10xxxxxxx 10xxxxxx
    res = (las_c_ & 0x0F) << 12;
    assert(std::cin.get(las_c_));
    res |= (las_c_ & 0x3F) << 6;
    assert(std::cin.get(las_c_));
    res |= las_c_ & 0x3F;
  } else {
    assert((las_c_ & 0xF8) == 0xF0); // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    res = (las_c_ & 0x07) << 18;
    assert(std::cin.get(las_c_));
    res |= (las_c_ & 0x3F) << 12;
    assert(std::cin.get(las_c_));
    res |= (las_c_ & 0x3F) << 6;
    assert(std::cin.get(las_c_));
    res |= las_c_ & 0x3F;
  }
  return res;
}

template<int len>
auto Input::GetChinese() -> array<unsigned int, len> {
  assert(las_c_ == ' ');
  int pos = 0;
  array<unsigned int, len> res;
  unsigned int val = GetSingleChinese();
  while (val) {
    res[pos++] = val;
    val = GetSingleChinese();
  }
  return res;
}

template auto Input::GetString<20>() -> array<char, 20>;
template auto Input::GetString<30>() -> array<char, 30>;
template auto Input::GetChinese<5>() -> array<unsigned int, 5>;
template auto Input::GetChinese<10>() -> array<unsigned int, 10>;

}