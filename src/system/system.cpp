#include "system/system.h"
#include "system/output.hpp"

namespace sjtu {

System::System(const std::string &name) : user_system_(name + "_user") {}

void System::PrintTimestamp() const {
  std::cout << "[" << timestamp_ << "] ";
}

void System::Run() {
  while (true) {
    timestamp_ = input_.GetTimestamp();
    PrintTimestamp();
    auto command = input_.GetCommand();
    if (command == "add_user") {
      AddUser();
    } else if (command == "login") {
      Login();
    } else if (command == "logout") {
      Logout();
    } else if (command == "query_profile") {
      QueryProfile();
    } else if (command == "modify_profile") {
      ModifyProfile();
    } else {
      assert(command == "exit");
      std::cout << "bye\n";
      break;
    }
  }
}

void System::AddUser() {
  array<char, 20> cur_username;
  User user;
  while (true) {
    auto key = input_.GetKey();
    if (key == 'c') {
      cur_username = input_.GetString<20>();
    } else if (key == 'u') {
      user.username_ = input_.GetString<20>();
    } else if (key == 'p') {
      user.password_ = input_.GetString<30>();
    } else if (key == 'n') {
      user.name_ = input_.GetChinese<5>();
    } else if (key == 'm') {
      user.mailAddr_ = input_.GetString<30>();
    } else if (key == 'g') {
      user.privilege_ = input_.GetInteger();
    } else {
      assert(key == '\n');
      break;
    }
  }
  if (user_system_.IsEmpty()) {
    user.privilege_ = 10;
    user_system_.AddUser(user);
    std::cout << "0\n";
  } else {
    auto it = online_users_.find(cur_username);
    if (it == online_users_.end() || it->second.privilege_ <= user.privilege_) {
      std::cout << "-1\n";
    } else if (user_system_.AddUser(user)) {
      std::cout << "0\n";
    } else {
      std::cout << "-1\n";
    }
  }
}

void System::Login() {
  array<char, 20> username;
  array<char, 30> password;
  while (true) {
    auto key = input_.GetKey();
    if (key == 'u') {
      username = input_.GetString<20>();
    } else if (key == 'p') {
      password = input_.GetString<30>();
    } else {
      assert(key == '\n');
      break;
    }
  }
  if (online_users_.find(username) != online_users_.end()) {
    std::cout << "-1\n";
  } else {
    auto user = user_system_.QueryUser(username);
    if (user.privilege_ > 10 || user.password_ != password) {
      std::cout << "-1\n";
    } else {
      online_users_[username] = user;
      std::cout << "0\n";
    }
  }
}

void System::Logout() {
  assert(input_.GetKey() == 'u');
  array<char, 20> username = input_.GetString<20>();
  assert(input_.GetKey() == '\n');
  auto it = online_users_.find(username);
  if (it == online_users_.end()) {
    std::cout << "-1\n";
  } else {
    online_users_.erase(it);
    std::cout << "0\n";
  }
}

void System::QueryProfile() {
  array<char, 20> cur_username;
  array<char, 20> username;
  while (true) {
    auto key = input_.GetKey();
    if (key == 'c') {
      cur_username = input_.GetString<20>();
    } else if (key == 'u') {
      username = input_.GetString<20>();
    } else {
      assert(key == '\n');
      break;
    }
  }
  auto it = online_users_.find(cur_username);
  if (it == online_users_.end()) {
    std::cout << "-1\n";
  } else {
    auto user = user_system_.QueryUser(username);
    if (it->second.privilege_ >= user.privilege_) { // if not found, user.privilege == 11
      std::cout << ArrayToString<20>(username) << " " << ChineseToString<5>(user.name_)
         << " " << ArrayToString<30>(user.mailAddr_) << " " << user.privilege_ << '\n';
    } else {
      std::cout << "-1\n";
    }
  }
}

void System::ModifyProfile() {
  array<char, 20> cur_username;
  User user;

  while (true) {
    auto key = input_.GetKey();
    if (key == 'c') {
      cur_username = input_.GetString<20>();
    } else if (key == 'u') {
      user.username_ = input_.GetString<20>();
    } else if (key == 'p') {
      user.password_ = input_.GetString<30>();
    } else if (key == 'n') {
      user.name_ = input_.GetChinese<5>();
    } else if (key == 'm') {
      user.mailAddr_ = input_.GetString<30>();
    } else if (key == 'g') {
      user.privilege_ = input_.GetInteger();
    } else {
      assert(key == '\n');
      break;
    }
  }
  auto it = online_users_.find(cur_username);
  if (it == online_users_.end()) {
    std::cout << "-1\n";
  } else {
    auto old_user = user_system_.QueryUser(user.username_);
    if (it->second.privilege_ >= old_user.privilege_ && (user.privilege_ == 11 || user.privilege_ < it->second.privilege_)) {
      if (user.password_[0] == '\0') {
        user.password_ = old_user.password_;
      }
      if (user.name_[0] == 0) {
        user.name_ = old_user.name_;
      }
      if (user.mailAddr_[0] == '\0') {
        user.mailAddr_ = old_user.mailAddr_;
      }
      if (user.privilege_ == 11) {
        user.privilege_ = old_user.privilege_;
      }
      user_system_.RemoveUser(user.username_);
      user_system_.AddUser(user);
      std::cout << ArrayToString<20>(user.username_) << " " << ChineseToString<5>(user.name_)
         << " " << ArrayToString<30>(user.mailAddr_) << " " << user.privilege_ << '\n';
    } else {
      std::cout << "-1\n";
    }
  }
}

}