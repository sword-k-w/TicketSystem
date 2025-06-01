#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

namespace sjtu {

template <typename T>
void swap(T &x, T &y) {
  static T tmp;
  tmp = x;
  x = y;
  y = tmp;
}

template <typename T>
auto Divide(T *begin, T *end, bool (*cmp)(const T&, const T&)) -> T * {
  T *pivot = begin;
  for (T *cur = begin + 1; cur != end; ++cur) {
    if (cmp(*cur, *pivot)) {
      if (pivot + 1 == cur) {
        swap(*pivot, *cur);
      } else {
        swap(*(pivot + 1), *cur);
        swap(*pivot, *(pivot + 1));
      }
      ++pivot;
    }
  }
  return pivot;
}

template <typename T>
void Sort(T *begin, T *end, bool (*cmp)(const T&, const T&)) {
  if (begin == end) {
    return;
  }
  T *pivot = Divide(begin, end, cmp);
  Sort(begin, pivot, cmp);
  Sort(pivot + 1, end, cmp);
}

}

#endif