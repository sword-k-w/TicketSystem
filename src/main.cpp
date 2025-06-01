#include "system/user_system/user.h"
#include "system/output.hpp"
#include "system/system.h"

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);
  sjtu::System system("sword");
  system.Run();
  return 0;
}