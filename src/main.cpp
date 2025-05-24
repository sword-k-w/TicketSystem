#include "system/user_system/user.h"
#include "system/output.hpp"
#include "system/system.h"

int main() {
  sjtu::System system("sword");
  system.Run();
  return 0;
}