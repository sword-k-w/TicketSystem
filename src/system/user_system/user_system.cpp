#include "system/user_system/user_system.h"
#include "my_stl/vector.hpp"

namespace sjtu {

auto UserSystem::AddUser(const User &user) -> bool {
  return users_.Insert(user.username_, user);
}

auto UserSystem::QueryUser(const array<char, 20> &username) -> User {
  vector<User> tmp;
  if (!users_.GetValue(username, &tmp)) {
    return {};
  }
  assert(tmp.size() == 1);
  return tmp[0];
}

void UserSystem::RemoveUser(const array<char, 20> &username) {
  users_.Remove(username);
}

auto UserSystem::IsEmpty() -> bool {
  return users_.IsEmpty();
}

void UserSystem::Clean() {
  users_.Clean();
}

}