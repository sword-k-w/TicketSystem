#ifndef TICKET_H
#define TICKET_H

#include "system/output.hpp"
#include "system/train_system/train_system.h"

namespace sjtu {

struct Ticket {
  array<char, 20> trainID_;
  int start_station_;
  int end_station_;
  int start_time_;
  int end_time_;
  int price_;
  int seat_;
  void Print(TrainSystem *train_system) const {
    std::cout << ArrayToString<20>(trainID_) << " ";
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
      return x.trainID_ < y.trainID_;
    }
    return x.end_time_ - x.start_time_ < y.end_time_ - y.start_time_;
  }
};

struct CostFirstComparator {
  bool operator () (const Ticket &x, const Ticket &y) { // same as above
    if (x.price_ == y.price_) {
      return x.trainID_ < y.trainID_;
    }
    return x.price_ < y.price_;
  }
};

struct EndStationFirstComparator {
  bool operator () (const Ticket &x, const Ticket &y) { // same as above
    if (x.end_station_ == y.end_station_) {
      return x.trainID_ < y.trainID_;
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
    if (x.first_.trainID_ != y.first_.trainID_) {
      return x.first_.trainID_ < y.first_.trainID_;
    }
    return x.second_.trainID_ < y.second_.trainID_;
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
    if (x.first_.trainID_ != y.first_.trainID_) {
      return x.first_.trainID_ < y.first_.trainID_;
    }
    return x.second_.trainID_ < y.second_.trainID_;
  }
};

}

#endif //TICKET_H
