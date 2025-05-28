#ifndef SJTU_ARRAY_HPP
#define SJTU_ARRAY_HPP

namespace sjtu {

template<typename T, int len>
class array {
public:
  array() {
    for (int i = 0; i < len; ++i) {
      a_[i] = T();
    }
  }
  explicit array(const T &val) {
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

  friend bool operator == (const array &x, const array &y) {
    for (int i = 0; i < len; ++i) {
      if (x[i] != y[i]) {
        return false;
      }
    }
    return true;
  }
  friend bool operator != (const array &x, const array &y) {
    for (int i = 0; i < len; ++i) {
      if (x[i] != y[i]) {
        return true;
      }
    }
    return false;
  }
  friend bool operator < (const array &x, const array &y) {
    for (int i = 0; i < len; ++i) {
      if (x[i] != y[i]) {
        return x[i] < y[i];
      }
    }
    return false;
  }
  friend bool operator > (const array &x, const array &y) {
    for (int i = 0; i < len; ++i) {
      if (x[i] != y[i]) {
        return x[i] > y[i];
      }
    }
    return false;
  }
private:
  T a_[len];
};

}

#endif