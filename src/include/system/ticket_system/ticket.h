#ifndef TICKET_H
#define TICKET_H

#include "system/output.hpp"
#include "system/train_system/train_system.h"

namespace sjtu {

struct Ticket {
  int train_id_{-1};
  int start_station_;
  int end_station_;
  int start_time_;
  int end_time_;
  int price_;
  int seat_;
  void Print(TrainSystem *train_system) const {
    std::cout << ArrayToString<20>(train_system->QueryTrain(train_id_).trainID_) << " ";
    std::cout << ChineseToString<10>(train_system->StationName(start_station_)) << " ";
    PrintTime(start_time_);
    std::cout << " -> ";
    std::cout << ChineseToString<10>(train_system->StationName(end_station_)) << " ";
    PrintTime(end_time_);
    std::cout << " " << price_ << " " << seat_ << '\n';
  }
};

struct RawTicket {
  array<char, 20> train_id_;
  int start_station_;
  int end_station_;
  int start_time_;
  int end_time_;
  int price_;
  int seat_;
  void Print(TrainSystem *train_system) const {
    std::cout << ArrayToString<20>(train_id_) << " ";
    std::cout << ChineseToString<10>(train_system->StationName(start_station_)) << " ";
    PrintTime(start_time_);
    std::cout << " -> ";
    std::cout << ChineseToString<10>(train_system->StationName(end_station_)) << " ";
    PrintTime(end_time_);
    std::cout << " " << price_ << " " << seat_ << '\n';
  }
};

inline bool TimeFirstComparator(const RawTicket &x, const RawTicket &y) {
  if (x.end_time_ - x.start_time_ == y.end_time_ - y.start_time_) {
    return x.train_id_ < y.train_id_;
  }
  return x.end_time_ - x.start_time_ < y.end_time_ - y.start_time_;
}

inline bool CostFirstComparator(const RawTicket &x, const RawTicket &y) {
  if (x.price_ == y.price_) {
    return x.train_id_ < y.train_id_;
  }
  return x.price_ < y.price_;
}

struct EndStationFirstComparator {
  bool operator () (const Ticket &x, const Ticket &y) { // for map
    if (x.end_station_ == y.end_station_) {
      return x.train_id_ < y.train_id_;
    }
    return x.end_station_ < y.end_station_;
  }
};

struct TransferTicket {
  Ticket first_;
  Ticket second_;
};

struct BuyInfo {
  array<char, 20> user_;
  int buy_time_;
};

struct BuyInfoComparator {
  int operator () (const BuyInfo &x, const BuyInfo &y) {
    if (x.user_ < y.user_) {
      return -1;
    }
    if (x.user_ > y.user_) {
      return 1;
    }
    if (x.buy_time_ < y.buy_time_) {
      return -1;
    }
    if (x.buy_time_ > y.buy_time_) {
      return 1;
    }
    return 0;
  }
};

struct RoughBuyInfoComparator {
  int operator () (const BuyInfo &x, const BuyInfo &y) {
    if (x.user_ < y.user_) {
      return -1;
    }
    if (x.user_ > y.user_) {
      return 1;
    }
    return 0;
  }
};

struct TimeComparator {
  int operator () (const int &x, const int &y) {
    if (x < y) {
      return -1;
    }
    if (x > y) {
      return 1;
    }
    return 0;
  }
};

struct Order {
  BuyInfo info_;
  Ticket ticket_;
  enum {
    kSuccess, kPending, kRefunded
  } state_;
};

}

#endif //TICKET_H
