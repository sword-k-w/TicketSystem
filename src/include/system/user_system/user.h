#ifndef USER_H
#define USER_H

#include "my_stl/array.hpp"

namespace sjtu {

struct User {
  array<char, 20> username_;
  array<char, 30> password_;
  array<unsigned int, 5> name_;
  array<char, 30> mailAddr_;
  unsigned char privilege_;
};

}

#endif //USER_H
