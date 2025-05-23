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

  template<int len>
  auto GetString() -> array<char, len>;

private:
  char las_c_{'\n'};
  auto GetChinese() -> unsigned int;
};

}

#endif //INPUT_H
