#ifndef COMPARATOR_H
#define COMPARATOR_H

namespace sjtu {

constexpr int kMod = 1e9 + 7;
constexpr int kBase = 19260817;

struct Key {
  int hash_;
  int value_;
  Key(const std::string &key = "", const int &value = -1) : value_(value) {
    int size = key.size();
    hash_ = 0;
    for (int i = 0; i < size; ++i) {
      hash_ = (1ll * hash_ * kBase + (key[i] - 'a' + 1)) % kMod;
    }
  }
};

struct Comparator {
  int operator () (const Key &x, const Key &y) {
    if (x.hash_ > y.hash_) {
      return 1;
    }
    if (x.hash_ < y.hash_) {
      return -1;
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
    if (x.hash_ > y.hash_) {
      return 1;
    }
    if (x.hash_ < y.hash_) {
      return -1;
    }
    return 0;
  }
};

}
#endif //COMPARATOR_H
