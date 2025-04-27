#include <iostream>

int main() {
  int n = 10000;
  std::cout << n << '\n';
  for (int i = 1; i <= n; ++i) {
    std::cout << "insert a " << i << '\n';
  }
  return 0;
}