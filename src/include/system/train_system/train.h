#ifndef TRAIN_H
#define TRAIN_H

#include "my_stl/array.hpp"

namespace sjtu {

struct Train {
  array<char, 20> trainID_;
  int stationNum_;
  array<int, 24> stations_;
  int max_seatNum_;
  array<array<int, 23>, 92> seatNum_;
  array<int, 23> prices_;
  array<int, 24> arrivingTimes_;
  array<int, 22> stopoverTimes_;
  int saleDate_start_;
  int saleDate_end_;
  char type_;
  bool is_released_{false};
};

struct TrainComparator {
  int operator () (const array<char, 20> &x, const array<char, 20> &y) {
    for (int i = 0; i < 20; ++i) {
      if (x[i] != y[i]) {
        if (x[i] > y[i]) {
          return 1;
        }
        return -1;
      }
    }
    return 0;
  }
};

struct StationComparator {
  int operator () (const array<unsigned int, 10> &x, const array<unsigned int, 10> &y) {
    for (int i = 0; i < 10; ++i) {
      if (x[i] != y[i]) {
        if (x[i] > y[i]) {
          return 1;
        }
        return -1;
      }
    }
    return 0;
  }
};

struct StationTrain {
  int station_id_;
  int train_id_;
};

struct StationTrainComparator {
  int operator () (const StationTrain &x, const StationTrain &y) {
    if (x.station_id_ > y.station_id_) {
      return 1;
    }
    if (x.station_id_ < y.station_id_) {
      return -1;
    }
    if (x.train_id_ > y.train_id_) {
      return 1;
    }
    if (x.train_id_ < y.train_id_) {
      return -1;
    }
    return 0;
  }
};

struct StationIDComparator {
  int operator () (const StationTrain &x, const StationTrain &y) {
    if (x.station_id_ > y.station_id_) {
      return 1;
    }
    if (x.station_id_ < y.station_id_) {
      return -1;
    }
    return 0;
  }
};

struct TrainStation {
  int train_id_;
  int pos_;
};

}

#endif //TRAIN_H
