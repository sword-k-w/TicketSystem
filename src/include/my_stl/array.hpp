#ifndef SJTU_ARRAY_HPP
#define SJTU_ARRAY_HPP

namespace sjtu {

template<typename T, int len>
class array {
  T a_[len];
  array() = default;
  array(const T &val) {
    for (int i = 0; i < len; ++i) {
      a_[i] = val;
    }
  }
  ~array() = default;
  T &operator [] (const int &index) {
    return a_[index];
  }
  const T &operator [] (const int &index) const {
    return a_[index];
  }
};

}

#endif