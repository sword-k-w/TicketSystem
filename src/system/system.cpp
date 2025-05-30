#include "system/system.h"
#include "system/output.hpp"
#include "config.h"

namespace sjtu {

System::System(const std::string &name) : user_system_(name + "_user"),
                                          train_system_(name + "_train"),
                                          ticket_system_(name + "_ticket") {}

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
    } else if (command == "query_ticket") {
      QueryTicket();
    } else if (command == "query_transfer") {
      QueryTransfer();
    } else if (command == "buy_ticket") {
      BuyTicket();
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
  int seat;
  while (true) {
    auto key = input_.GetKey();
    if (key == 'i') {
      train.trainID_ = input_.GetString<20>();
    } else if (key == 'n') {
      train.stationNum_ = input_.GetInteger();
    } else if (key == 'm') {
      seat = input_.GetInteger();
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
  if (train_system_.QueryTrain(train.trainID_).trainID_[0] != '\0') {
    std::cout << "-1\n";
  } else {
    for (int i = train.saleDate_start_; i <= train.saleDate_end_; ++i) {
      for (int j = 0; j < train.stationNum_; ++j) {
        train.seatNum_[i][j] = seat;
      }
    }
    for (int i = 0; i < train.stationNum_; ++i) {
      train.stations_[i] = train_system_.StationID(stations[i], true);
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
      std::cout << ChineseToString<10>(train_system_.StationName(train.stations_[i])) << " ";
      if (i == 0) {
        std::cout << "xx-xx xx:xx";
      } else {
        PrintTime(date * 1440 + train.arrivingTimes_[i]);
      }

      std::cout << " -> ";

      if (i + 1 == train.stationNum_) {
        std::cout << "xx-xx xx:xx";
      } else {
        PrintTime(date * 1440 + train.arrivingTimes_[i] + (i == 0 ? 0 : train.stopoverTimes_[i - 1]));
      }

      std::cout << ' ' << total_price << ' ';
      if (i + 1 == train.stationNum_) {
        std::cout << "x\n";
      } else {
        total_price += train.prices_[i];
        std::cout << train.seatNum_[date][i] << '\n';
      }
    }
  } else {
    std::cout << "-1\n";
  }
}

void System::QueryTicket() {
  array<unsigned int, 10> start;
  array<unsigned int, 10> end;
  array<char, 4> option;
  int date;
  while (true) {
    auto key = input_.GetKey();
    if (key == 's') {
      start = input_.GetChinese<10>();
    } else if (key == 't') {
      end = input_.GetChinese<10>();
    } else if (key == 'd') {
      date = input_.GetDate();
    } else if (key == 'p') {
      option = input_.GetString<4>();
    } else {
      assert(key == '\n');
      break;
    }
  }

  int start_station = train_system_.StationID(start, false);
  if (start_station == -1) {
    std::cout << "0\n";
    return;
  }
  int end_station = train_system_.StationID(end, false);
  if (end_station == -1) {
    std::cout << "0\n";
    return;
  }

  map<Ticket, bool, TimeFirstComparator> ticket_time;
  map<Ticket, bool, CostFirstComparator> ticket_cost;
  if (option[0] == '\0') {
    option[0] = 't';
  }

  vector<int> trains;
  train_system_.QueryStationInfo(start_station, &trains);
  size_t size = trains.size();
  for (size_t i = 0; i < size; ++i) {
    auto train = train_system_.QueryTrain(trains[i]);
    int start_pos;
    int end_pos = -1;
    for (int j = 0; j < train.stationNum_; ++j) {
      if (train.stations_[j] == start_station) {
        start_pos = j;
      } else if (train.stations_[j] == end_station) {
        end_pos = j;
      }
    }
    if (end_pos > start_pos) {
      int start_total_time = train.arrivingTimes_[start_pos];
      if (start_pos > 0) {
        start_total_time += train.stopoverTimes_[start_pos - 1];
      }
      int start_date = date - start_total_time / 1440;
      if (start_date < train.saleDate_start_ || train.saleDate_end_ < start_date) {
        continue;
      }
      int start_time = start_total_time + start_date * 1440;
      int end_time = train.arrivingTimes_[end_pos] + start_date * 1440;
      int seat = MAX_SEAT_NUM;
      int total_price = 0;
      for (int j = start_pos; j < end_pos; ++j) {
        if (seat > train.seatNum_[start_date][j]) {
          seat = train.seatNum_[start_date][j];
        }
        total_price += train.prices_[j];
      }
      if (option[0] == 't') {
        ticket_time[{trains[i], start_station, end_station, start_time, end_time, total_price, seat}] = true;
      } else {
        ticket_cost[{trains[i], start_station, end_station, start_time, end_time, total_price, seat}] = true;
      }
    }
  }
  if (option[0] == 't') {
    std::cout << ticket_time.size() << '\n';
    for (auto it = ticket_time.begin(); it != ticket_time.end(); ++it) {
      it->first.Print(&train_system_);
    }
  } else {
    std::cout << ticket_cost.size() << '\n';
    for (auto it = ticket_cost.begin(); it != ticket_cost.end(); ++it) {
      it->first.Print(&train_system_);
    }
  }
}

void System::QueryTransfer() {
  array<unsigned int, 10> start;
  array<unsigned int, 10> end;
  array<char, 4> option;
  int date;
  while (true) {
    auto key = input_.GetKey();
    if (key == 's') {
      start = input_.GetChinese<10>();
    } else if (key == 't') {
      end = input_.GetChinese<10>();
    } else if (key == 'd') {
      date = input_.GetDate();
    } else if (key == 'p') {
      option = input_.GetString<4>();
    } else {
      assert(key == '\n');
      break;
    }
  }

  int start_station = train_system_.StationID(start, false);
  if (start_station == -1) {
    std::cout << "0\n";
    return;
  }
  int end_station = train_system_.StationID(end, false);
  if (end_station == -1) {
    std::cout << "0\n";
    return;
  }

  if (option[0] == '\0') {
    option[0] = 't';
  }

  vector<int> start_trains;
  vector<int> end_trains;
  train_system_.QueryStationInfo(start_station, &start_trains);
  train_system_.QueryStationInfo(end_station, &end_trains);
  map<Ticket, bool, EndStationFirstComparator> transfer_candidate;

  size_t size = start_trains.size();
  for (size_t i = 0; i < size; ++i) {
    auto train = train_system_.QueryTrain(start_trains[i]);

    for (int j = 0; j < train.stationNum_; ++j) {
      if (train.stations_[j] == start_station) {
        int start_total_time = train.arrivingTimes_[j];
        if (j > 0) {
          start_total_time += train.stopoverTimes_[j - 1];
        }
        int start_date = date - start_total_time / 1440;
        if (start_date < train.saleDate_start_ || train.saleDate_end_ < start_date) {
          break;
        }
        int start_time = start_total_time + start_date * 1440;
        int seat = MAX_SEAT_NUM;
        int total_price = 0;
        for (int k = j + 1; k < train.stationNum_; ++k) {
          if (seat > train.seatNum_[start_date][k - 1]) {
            seat = train.seatNum_[start_date][k - 1];
          }
          total_price += train.prices_[k - 1];
          int end_time = train.arrivingTimes_[k] + start_date * 1440;
          transfer_candidate[{start_trains[i], train.stations_[j], train.stations_[k], start_time, end_time,
            total_price, seat}] = true;
        }
        break;
      }
    }
  }

  TransferTicket ticket;
  size = end_trains.size();
  for (size_t i = 0; i < size; ++i) {
    auto train = train_system_.QueryTrain(end_trains[i]);
    for (int j = train.stationNum_ - 1; j >= 0; --j) {
      if (train.stations_[j] == end_station) {
        int total_price = 0;
        for (int k = j - 1; k >= 0; --k) {
          total_price += train.prices_[k];
          Ticket tmp;
          tmp.end_station_ = train.stations_[k];
          auto it = transfer_candidate.lower_bound(tmp);
          int next_start_total_time = train.arrivingTimes_[k];
          if (k > 0) {
            next_start_total_time += train.stopoverTimes_[k - 1];
          }
          while (it != transfer_candidate.end() && it->first.end_station_ == train.stations_[k]) {
            int next_start_date = it->first.end_time_ / 1440 - next_start_total_time / 1440;
            if (next_start_date > train.saleDate_end_ || it->first.train_id_ == end_trains[i]) {
              ++it;
              continue;
            }
            if (next_start_date < train.saleDate_start_) {
              next_start_date = train.saleDate_start_;
            }
            int seat = MAX_SEAT_NUM;
            for (int l = k; l < j; ++l) {
              if (seat > train.seatNum_[next_start_date][l]) {
                seat = train.seatNum_[next_start_date][l];
              }
            }
            if (it->first.seat_ > 0 && seat > 0) {
              Ticket next_ticket{end_trains[i], train.stations_[k], end_station,
              next_start_date * 1440 + next_start_total_time, next_start_date * 1440 + train.arrivingTimes_[j],
              total_price, seat};
              TransferTicket new_ticket{it->first, next_ticket};
              if (new_ticket.first_.seat_ < new_ticket.second_.seat_) {
                new_ticket.second_.seat_ = new_ticket.first_.seat_;
              } else {
                new_ticket.first_.seat_ = new_ticket.second_.seat_;
              }
              if (ticket.first_.train_id_ == -1
                || (option[0] == 't' && TransferTimeFirstComparator()(new_ticket, ticket))
                || (option[0] == 'c' && TransferCostFirstComparator()(new_ticket, ticket))) {
                ticket = new_ticket;
              }
            }
            ++it;
          }
        }
      }
    }
  }
  if (ticket.first_.train_id_ == -1) {
    std::cout << "0\n";
  } else {
    ticket.first_.Print(&train_system_);
    ticket.second_.Print(&train_system_);
  }
}

void System::BuyTicket() {
  Order order;
  order.buy_time_ = timestamp_;
  std::string option = "false";
  while (true) {
    auto key = input_.GetKey();
    if (key == 'u') {
      order.user_ = input_.GetString<20>();
    } else if (key == 'i') {
      order.trainID_ = input_.GetString<20>();
    } else if (key == 'd') {
      order.date_ = input_.GetDate();
    } else if (key == 'n') {
      order.number_ = input_.GetInteger();
    } else if (key == 'f') {
      array<unsigned int, 10> start_station = input_.GetChinese<10>();
      order.start_station_ = train_system_.StationID(start_station, false);
    } else if (key == 't') {
      array<unsigned int, 10> end_station = input_.GetChinese<10>();
      order.end_station_ = train_system_.StationID(end_station, false);
    } else if (key == 'q') {
      option = input_.GetCommand();
    } else {
      assert(key == '\n');
      break;
    }
  }
  if (order.start_station_ == -1 || order.end_station_ == -1 || online_users_.find(order.user_) == online_users_.end()) {
    std::cout << "-1\n";
    return;
  }
  auto train_id = train_system_.TrainID(order.trainID_);
  auto train = train_system_.QueryTrain(train_id);
  if (!train.is_released_) {
    std::cout << "-1\n";
    return;
  }
  int start_pos = -1;
  int end_pos = -1;
  for (int i = 0; i < train.stationNum_; ++i) {
    if (train.stations_[i] == order.start_station_) {
      start_pos = i;
    } else if (train.stations_[i] == order.end_station_) {
      end_pos = i;
    }
  }
  if (start_pos >= 0 && end_pos >= 0 && start_pos > end_pos) {
    int start_date = order.date_ - train.arrivingTimes_[start_pos] / 1440;
    if (start_date < train.saleDate_start_ || start_date > train.saleDate_end_) {
      std::cout << "-1\n";
      return;
    }
    int seat = MAX_SEAT_NUM;
    for (int i = start_pos; i < end_pos; ++i) {
      if (train.seatNum_[start_date][i] < seat) {
        seat = train.seatNum_[start_date][i];
      }
    }
    if (seat < order.number_) {
      if (option[0] == 'f') {
        std::cout << "-1\n";
      } else {
        order.state_ = Order::kPending;
        ticket_system_.AddOrder(order);
        std::cout << "queue\n";
      }
    } else {
      for (int i = start_pos; i < end_pos; ++i) {
        train.seatNum_[start_date][i] -= seat;
      }
      train_system_.UpdateTrain(train_id, train);
      order.state_ = Order::kSuccess;
      ticket_system_.AddOrder(order);
    }
  } else {
    std::cout << "-1\n";
  }
}

}