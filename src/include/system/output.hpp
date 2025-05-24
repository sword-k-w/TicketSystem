#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <string>

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

}

#endif