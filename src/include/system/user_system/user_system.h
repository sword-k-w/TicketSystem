#ifndef USER_SYSTEM_H
#define USER_SYSTEM_H

#include "system/user_system/user.h"
#include "b_plus_tree/b_plus_tree.h"

namespace sjtu {

class UserSystem {
public:
  auto AddUser(const User &user) -> bool;
  auto QueryUser(const array<char, 20> &username) -> User;
  void RemoveUser(const array<char, 20> &username);
  auto IsEmpty() -> bool;
  void Clean();
  UserSystem() = delete;
  explicit UserSystem(const std::string &name) : users_(name) {}
private:
  BPlusTree<array<char, 20>, User, UserComparator, UserComparator> users_;
};

}

#endif //USER_SYSTEM_H
