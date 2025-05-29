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


}

#endif //TICKET_H
