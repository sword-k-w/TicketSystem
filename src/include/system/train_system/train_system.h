#ifndef TRAIN_SYSTEM_H
#define TRAIN_SYSTEM_H

#include "system/train_system/train.h"
#include "b_plus_tree/b_plus_tree.h"

namespace sjtu {

class TrainSystem {
public:
  auto StationID(const array<unsigned int, 10> &station) -> int;
  auto AddTrain(const Train &train) -> bool;
  void DeleteTrain(const array<char, 20> &trainID);
  void ReleaseTrain(Train &train);
  auto QueryTrain(const array<char, 20> &trainID) -> Train;
  TrainSystem() = delete;
  explicit TrainSystem(const std::string &name) : trains_(name + "_main"),
    station_id_(name + "_station_id"), station_info_(name + "_station_info") {}

private:
  BPlusTree<array<char, 20>, Train, TrainComparator, TrainComparator> trains_;
  BPlusTree<array<unsigned int, 10>, int, StationComparator, StationComparator> station_id_;
  BPlusTree<StationTrain, array<char, 20>, StationTrainComparator, StationIDComparator> station_info_;
};

}

#endif //TRAIN_SYSTEM_H
