#include "system/system.h"
#include "system/output.hpp"

namespace sjtu {

System::System(const std::string &name) : user_system_(name + "_user"), train_system_(name + "_train") {}

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
    } else if (command == "add_train") {
      AddTrain();
    } else if (command == "delete_train") {
      DeleteTrain();
    } else if (command == "release_train") {
      ReleaseTrain();
    } else if (command == "query_train") {
      QueryTrain();
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
    if (it->second.privilege_ > user.privilege_ || cur_username == username) { // if not found, user.privilege == 11
      std::cout << ArrayToString<20>(username) << " " << ChineseToString<5>(user.name_)
         << " " << ArrayToString<30>(user.mailAddr_) << " " << static_cast<unsigned int>(user.privilege_) << '\n';
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
         << " " << ArrayToString<30>(user.mailAddr_) << " " << static_cast<unsigned int>(user.privilege_) << '\n';
    } else {
      std::cout << "-1\n";
    }
  }
}

void System::AddTrain() {
  Train train;
  array<array<unsigned int, 10>, 100> stations;
  int startTime;
  array<int, 99> travelTimes;
  while (true) {
    auto key = input_.GetKey();
    if (key == 'i') {
      train.trainID_ = input_.GetString<20>();
    } else if (key == 'n') {
      train.stationNum_ = input_.GetInteger();
    } else if (key == 'm') {
      train.seatNum_ = array<int, 99>(input_.GetInteger());
    } else if (key == 's') {
      stations = input_.GetChineseArray<10, 100>();
    } else if (key == 'p') {
      train.prices_ = input_.GetIntegerArray<99>();
    } else if (key == 'x') {
      startTime = input_.GetTime();
    } else if (key == 't') {
      travelTimes = input_.GetIntegerArray<99>();
    } else if (key == 'o') {
      train.stopoverTimes_ = input_.GetIntegerArray<98>();
    } else if (key == 'd') {
      train.saleDate_start_ = input_.GetDate();
      train.saleDate_end_ = input_.GetDate();
    } else if (key == 'y') {
      train.type_ = input_.GetChar();
    } else {
      assert(key == '\n');
      break;
    }
  }
  if (train_system_.QueryTrain(train.trainID_).trainID_[0] == '\0') {
    std::cout << "-1\n";
  } else {
    for (int i = 0; i < train.stationNum_; ++i) {
      train.stations_[i] = train_system_.StationID(stations[i]);
    }
    train.arrivingTimes_[0] = startTime;
    for (int i = 1; i < train.stationNum_; ++i) {
      train.arrivingTimes_[i] = train.arrivingTimes_[i - 1] + travelTimes[i - 1];
      if (i > 1) {
        train.arrivingTimes_[i] += train.stopoverTimes_[i - 2];
      }
    }
    train_system_.AddTrain(train);
    std::cout << "0\n";
  }
}

void System::DeleteTrain() {
  assert(input_.GetKey() == 'i');
  array<char, 20> trainID = input_.GetString<20>();
  assert(input_.GetKey() == '\n');
  auto train = train_system_.QueryTrain(trainID);
  if (train.trainID_[0] != '\0' && !train.is_released_) {
    train_system_.DeleteTrain(trainID);
    std::cout << "0\n";
  } else {
    std::cout << "-1\n";
  }
}

void System::ReleaseTrain() {
  assert(input_.GetKey() == 'i');
  array<char, 20> trainID = input_.GetString<20>();
  assert(input_.GetKey() == '\n');
  auto train = train_system_.QueryTrain(trainID);
  if (train.trainID_[0] != '\0' && !train.is_released_) {
    train_system_.ReleaseTrain(train);
    std::cout << "0\n";
  } else {
    std::cout << "-1\n";
  }
}

void System::QueryTrain() {
  array<char, 20> trainID;
  int date;
  while (true) {
    auto key = input_.GetKey();
    if (key == 'i') {
      trainID = input_.GetString<20>();
    } else if (key == 'd') {
      date = input_.GetDate();
    } else {
      assert(key == '\n');
      break;
    }
  }
  auto train = train_system_.QueryTrain(trainID);
  if (train.trainID_[0] != '\0' || (train.saleDate_start_ <= date && date <= train.saleDate_end_)) {
    int total_price = 0;
    std::cout << ArrayToString<20>(train.trainID_) << ' ' << train.type_ << '\n';
    for (int i = 0; i < train.stationNum_; ++i) {
      std::cout << ChineseToString<10>(train_system_.StationName(train.stations_[0])) << " ";
      if (i == 0) {
        std::cout << "xx-xx xx:xx";
      } else {
        PrintTime(date * 14400 + train.arrivingTimes_[i]);
      }

      std::cout << " -> ";

      if (i + 1 == train.stationNum_) {
        std::cout << "xx-xx xx:xx";
      } else {
        PrintTime(date * 14400 + train.arrivingTimes_[i] + (i == 0 ? 0 : train.stopoverTimes_[i - 1]));
      }

      std::cout << ' ' << total_price << ' ';
      if (i + 1 == train.stationNum_) {
        std::cout << "x\n";
      } else {
        total_price += train.prices_[i];
        std::cout << train.seatNum_[i] << '\n';
      }
    }
  } else {
    std::cout << "-1\n";
  }
}

}