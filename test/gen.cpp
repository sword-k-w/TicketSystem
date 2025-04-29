#include <iostream>

int main() {
  int n = 300000;
  std::cout << n << '\n';
  for (int i = 1; i <= n; ++i) {
    std::cout << "delete a " << i << '\n';
  }
  return 0;
}