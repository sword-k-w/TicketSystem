#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <string>
#include <iostream>
#include <cassert>
#include "my_stl/array.hpp"

namespace sjtu {

inline std::string SingleChineseToString(unsigned int tmp) {
  std::string res;
  if (tmp <= 0x7F) { // 0xxxxxxx
    res += tmp;
  } else if (tmp <= 0x7FF) { // 110xxxxx 10xxxxxx
    res += (0xC0 | (tmp >> 6));
    res += (0x80 | (tmp & 0x3F));
  } else if (tmp <= 0xFFFF) { // 1110xxxx 10xxxxxx 10xxxxxx
    res += (0xE0 | (tmp >> 12));
    res += (0x80 | ((tmp >> 6) & 0x3F));
    res += (0x80 | (tmp & 0x3F));
  } else { // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    res += (0xF0 | (tmp >> 18));
    res += (0x80 | ((tmp >> 12) & 0x3F));
    res += (0x80 | ((tmp >> 6) & 0x3F));
    res += (0x80 | (tmp & 0x3F));
  }
  return res;
}

template<int len>
std::string ChineseToString(array<unsigned int, len> tmp) {
  std::string res;
  for (int i = 0; i < len; ++i) {
    if (tmp[i] == 0) {
      break;
    }
    res += SingleChineseToString(tmp[i]);
  }
  return res;
}

template<int len>
std::string ArrayToString(array<char, len> tmp) {
  std::string res;
  for (int i = 0; i < len; ++i) {
    if (tmp[i] == '\0') {
      break;
    }
    res += tmp[i];
  }
  return res;
}

inline void PrintTwoBitNumber(int x) {
  if (x < 10) {
    std::cout << '0' << x;
  } else {
    assert(x < 100);
    std::cout << x;
  }
}

inline void PrintTime(int time) {
  int day = time / 1440;
  int hour = time % 1440 / 60;
  int minute = time % 1440 % 60;
  if (day < 30) {
    std::cout << "06-";
    PrintTwoBitNumber(day + 1);
  } else if (day < 61) {
    std::cout << "07-";
    PrintTwoBitNumber(day - 29);
  } else {
    std::cout << "08-";
    PrintTwoBitNumber(day - 60);
  }
  std::cout << ' ';
  PrintTwoBitNumber(hour);
  std::cout << ':';
  PrintTwoBitNumber(minute);
}

}

#endif