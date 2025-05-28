#ifndef INPUT_H
#define INPUT_H

#include <cassert>
#include <string>
#include "my_stl/array.hpp"

namespace sjtu {

class Input {
public:

  void Skip();
  auto GetTimestamp() -> int;
  auto GetCommand() -> std::string;
  auto GetKey() -> char;
  auto GetChar() -> char;
  auto GetInteger() -> int;
  auto GetDate() -> int;
  auto GetTime() -> int;

  template<int len>
  auto GetString() -> array<char, len>;

  template<int len>
  auto GetChinese() -> array<unsigned int, len>;

  template<int len>
  auto GetIntegerArray() -> array<int, len>;

  template<int len1, int len2>
  auto GetChineseArray() -> array<array<unsigned int, len1>, len2>;

private:
  char las_c_{'\n'};
  auto GetSingleChinese() -> unsigned int;
};

}

#endif //INPUT_H
