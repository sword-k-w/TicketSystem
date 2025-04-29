#ifndef COMPARATOR_H
#define COMPARATOR_H

struct Key {
  char key_[64];
  int value_;
  Key(const std::string &key = "", const int &value = -1) : value_(value) {
    int size = key.size();
    for (int i = 0; i < size; ++i) {
      key_[i] = key[i];
    }
    for (int i = size; i < 64; ++i) {
      key_[i] = '\0';
    }
  }
  friend std::ostream &operator << (std::ostream &os, const Key &x) {
    os << '(' << x.key_[0] << ' ' << x.value_ << ')';
    return os;
  }
};

struct Comparator {
  int operator () (const Key &x, const Key &y) {
    for (int i = 0; i < 64; ++i) {
      if (x.key_[i] != y.key_[i]) {
        if (x.key_[i] > y.key_[i]) {
          return 1;
        }
        return -1;
      }
    }
    if (x.value_ > y.value_) {
      return 1;
    }
    if (x.value_ < y.value_) {
      return -1;
    }
    return 0;
  }
};

struct RoughComparator {
  int operator () (const Key &x, const Key &y) {
    for (int i = 0; i < 64; ++i) {
      if (x.key_[i] != y.key_[i]) {
        if (x.key_[i] > y.key_[i]) {
          return 1;
        }
        return -1;
      }
    }
    return 0;
  }
};

#endif //COMPARATOR_H
