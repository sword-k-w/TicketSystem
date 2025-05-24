#ifndef USER_H
#define USER_H

#include "my_stl/array.hpp"

namespace sjtu {

// If privilege > 10, the user is invalid
struct User {
  array<char, 20> username_;
  array<char, 30> password_;
  array<unsigned int, 5> name_;
  array<char, 30> mailAddr_;
  unsigned char privilege_{11};
};

struct UserComparator {
  int operator () (const array<char, 20> &x, const array<char, 20> &y) {
    for (int i = 0; i < 20; ++i) {
      if (x[i] != y[i]) {
        if (x[i] > y[i]) {
          return 1;
        }
        return -1;
      }
    }
    return 0;
  }
};

}

#endif //USER_H
