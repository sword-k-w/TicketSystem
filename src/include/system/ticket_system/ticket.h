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

struct TimeFirstComparator {
  bool operator () (const Ticket &x, const Ticket &y) { // x < y return true, for map only
    if (x.end_time_ - x.start_time_ == y.end_time_ - y.start_time_) {
      return x.train_id_ < y.train_id_;
    }
    return x.end_time_ - x.start_time_ < y.end_time_ - y.start_time_;
  }
};

struct CostFirstComparator {
  bool operator () (const Ticket &x, const Ticket &y) { // same as above
    if (x.price_ == y.price_) {
      return x.train_id_ < y.train_id_;
    }
    return x.price_ < y.price_;
  }
};

struct EndStationFirstComparator {
  bool operator () (const Ticket &x, const Ticket &y) { // same as above
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

struct TransferTimeFirstComparator {
  bool operator () (const TransferTicket &x, const TransferTicket &y) {
    if (x.second_.end_time_ - x.first_.start_time_ != y.second_.end_time_ - y.first_.start_time_) {
      return x.second_.end_time_ - x.first_.start_time_ < y.second_.end_time_ - y.first_.start_time_;
    }
    if (x.first_.price_ + x.second_.price_ != y.first_.price_ + y.second_.price_) {
      return x.first_.price_ + x.second_.price_ < y.first_.price_ + y.second_.price_;
    }
    if (x.first_.train_id_ != y.first_.train_id_) {
      return x.first_.train_id_ < y.first_.train_id_;
    }
    return x.second_.train_id_ < y.second_.train_id_;
  }
};

struct TransferCostFirstComparator {
  bool operator () (const TransferTicket &x, const TransferTicket &y) {
    if (x.first_.price_ + x.second_.price_ != y.first_.price_ + y.second_.price_) {
      return x.first_.price_ + x.second_.price_ < y.first_.price_ + y.second_.price_;
    }
    if (x.second_.end_time_ - x.first_.start_time_ != y.second_.end_time_ - y.first_.start_time_) {
      return x.second_.end_time_ - x.first_.start_time_ < y.second_.end_time_ - y.first_.start_time_;
    }
    if (x.first_.train_id_ != y.first_.train_id_) {
      return x.first_.train_id_ < y.first_.train_id_;
    }
    return x.second_.train_id_ < y.second_.train_id_;
  }
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
  array<char, 20> user_;
  int buy_time_;
  array<char, 20> trainID_;
  int date_;
  int start_station_;
  int end_station_;
  int number_;
  enum {
    kSuccess, kPending, kRefunded
  } state_;
};

}

#endif //TICKET_H
