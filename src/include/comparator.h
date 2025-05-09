#ifndef COMPARATOR_H
#define COMPARATOR_H

namespace sjtu {

constexpr int kMod1 = 1e9 + 7;
constexpr int kMod2 = 1e9 + 9;
constexpr int kBase = 19260817;

struct Key {
  int hash1_;
  int hash2_;
  char key_[64];
  int value_;
  Key(const std::string &key = "", const int &value = -1) : value_(value) {
    int size = key.size();
    hash1_ = 0;
    hash2_ = 0;
    for (int i = 0; i < size; ++i) {
      hash1_ = (1ll * hash1_ * kBase + (key[i] - 'a' + 1)) % kMod1;
      hash2_ = (1ll * hash2_ * kBase + (key[i] - 'a' + 1)) % kMod2;
    }
  }
};

struct Comparator {
  int operator () (const Key &x, const Key &y) {
    if (x.hash1_ > y.hash1_) {
      return 1;
    }
    if (x.hash1_ < y.hash1_) {
      return -1;
    }
    if (x.hash2_ > y.hash2_) {
      return 1;
    }
    if (x.hash2_ < y.hash2_) {
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
    if (x.hash1_ > y.hash1_) {
      return 1;
    }
    if (x.hash1_ < y.hash1_) {
      return -1;
    }
    if (x.hash2_ > y.hash2_) {
      return 1;
    }
    if (x.hash2_ < y.hash2_) {
      return -1;
    }
    return 0;
  }
};

}
#endif //COMPARATOR_H
