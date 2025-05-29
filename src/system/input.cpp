#include "system/input.h"
#include <iostream>

namespace sjtu {

void Input::Skip() {
  std::cin.get(las_c_);
}

auto Input::GetTimestamp() -> int {
  assert(las_c_ == '\n');
  std::cin.get(las_c_);
  if (las_c_ != '[') {
    std::cerr << las_c_ << '\n';
    std::cin.get(las_c_);
    std::cerr << las_c_ << '\n';
    std::cin.get(las_c_);
    std::cerr << las_c_ << '\n';
    std::cin.get(las_c_);
    std::cerr << las_c_ << '\n';
    exit(1);
  }
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

auto Input::GetChar() -> char {
  assert(las_c_ == ' ');
  std::cin.get(las_c_);
  char res = las_c_;
  std::cin.get(las_c_);
  return res;
}

auto Input::GetInteger() -> int {
  assert(las_c_ == ' ' || las_c_ == '|');
  int res = 0;
  std::cin.get(las_c_);
  if (las_c_ == '_') {
    Skip();
    return -1;
  }
  while (las_c_ >= '0' && las_c_ <= '9') {
    res = res * 10 + las_c_ - '0';
    std::cin.get(las_c_);
  }
  return res;
}

auto Input::GetDate() -> int {
  Skip();
  int res = 0;
  std::cin.get(las_c_);
  assert(las_c_ > '5' && las_c_ < '9');
  if (las_c_ == '7') {
    res = 30;
  } else if (las_c_ == '8') {
    res = 61;
  }
  Skip();
  std::cin.get(las_c_);
  res += 10 * (las_c_ - '0');
  std::cin.get(las_c_);
  res += las_c_ - '1';
  Skip();
  return res;
}

auto Input::GetTime() -> int {
  int res = 0;
  std::cin.get(las_c_);
  res = (las_c_ - '0') * 600;
  std::cin.get(las_c_);
  res += (las_c_ - '0') * 60;
  Skip();
  std::cin.get(las_c_);
  res += (las_c_ - '0') * 10;
  std::cin.get(las_c_);
  res += las_c_ - '0';
  std::cin.get(las_c_);
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
    assert(las_c_ == ' ' || las_c_ == '\n' || las_c_ == '|');
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
  assert(las_c_ == ' ' || las_c_ == '|');
  int pos = 0;
  array<unsigned int, len> res;
  unsigned int val = GetSingleChinese();
  while (val) {
    res[pos++] = val;
    val = GetSingleChinese();
  }
  return res;
}

template<int len>
auto Input::GetIntegerArray() -> array<int, len> {
  assert(las_c_ == ' ');
  int pos = 0;
  array<int, len> res;
  int val = GetInteger();
  if (val == -1) {
    return res;
  }
  while (las_c_ == '|') {
    res[pos++] = val;
    val = GetInteger();
  }
  res[pos] = val;
  return res;
}

template<int len1, int len2>
auto Input::GetChineseArray() -> array<array<unsigned int, len1>, len2> {
  assert(las_c_ == ' ');
  int pos = 0;
  array<array<unsigned int, len1>, len2> res;
  array<unsigned int, len1> val = GetChinese<len1>();
  while (las_c_ == '|') {
    res[pos++] = val;
    val = GetChinese<len1>();
  }
  res[pos] = val;
  return res;
}

template auto Input::GetString<4>() -> array<char, 4>;
template auto Input::GetString<20>() -> array<char, 20>;
template auto Input::GetString<30>() -> array<char, 30>;
template auto Input::GetChinese<5>() -> array<unsigned int, 5>;
template auto Input::GetChinese<10>() -> array<unsigned int, 10>;
template auto Input::GetIntegerArray<98>() -> array<int, 98>;
template auto Input::GetIntegerArray<99>() -> array<int, 99>;
template auto Input::GetChineseArray<10, 100>() -> array<array<unsigned int, 10>, 100>;

}