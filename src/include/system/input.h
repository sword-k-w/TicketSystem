#ifndef INPUT_H
#define INPUT_H

#include <cassert>
#include <string>
#include "my_stl/array.hpp"

namespace sjtu {

class Input {
public:

  auto GetTimestamp() -> int;
  auto GetCommand() -> std::string;
  auto GetKey() -> char;
  auto GetInteger() -> int;

  template<int len>
  auto GetString() -> array<char, len>;

  template<int len>
  auto GetChinese() -> array<unsigned int, len>;

private:
  char las_c_{'\n'};
  auto GetSingleChinese() -> unsigned int;
};

}

#endif //INPUT_H
