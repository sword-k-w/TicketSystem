#ifndef TRAIN_SYSTEM_H
#define TRAIN_SYSTEM_H

#include "system/train_system/train.h"
#include "b_plus_tree/b_plus_tree.h"
#include "memory_river/memory_river.hpp"

namespace sjtu {

class TrainSystem {
public:
  auto TrainID(const array<char, 20> &train) -> int;
  auto StationID(array<unsigned int, 10> &station, bool add_new) -> int;
  auto StationName(const int &id) -> array<unsigned int, 10>;
  auto AddTrain(Train &train) -> bool;
  void DeleteTrain(const array<char, 20> &trainID);
  void ReleaseTrain(Train &train);
  auto QueryTrain(const int &train_id) -> Train;
  auto QueryTrain(const array<char, 20> &trainID) -> Train;
  void UpdateTrain(const int &train_id, Train &new_train);
  void QueryStationInfo(const int &id, vector<int> *info);
  void Clean();
  TrainSystem() = delete;
  explicit TrainSystem(const std::string &name) : train_id_(name + "_train_id"), trains_(name + "_trains"),
    station_id_(name + "_station_id"), station_info_(name + "_station_info"), station_name_(name + "_station_name") {
    station_name_.Initialise();
    trains_.Initialise();
  }

private:
  BPlusTree<array<char, 20>, int, TrainComparator, TrainComparator> train_id_;
  BPlusTree<array<unsigned int, 10>, int, StationComparator, StationComparator> station_id_;
  MemoryRiver<array<unsigned int, 10>> station_name_;
  MemoryRiver<Train> trains_;
  BPlusTree<StationTrain, int, StationTrainComparator, StationIDComparator> station_info_;
};

}

#endif //TRAIN_SYSTEM_H
