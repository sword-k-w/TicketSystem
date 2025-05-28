#include "system/train_system/train_system.h"

namespace sjtu {

auto TrainSystem::StationID(array<unsigned int, 10> &station) -> int {
  vector<int> tmp;
  if (station_id_.GetValue(station, &tmp)) {
    return tmp[0];
  }
  int new_id = station_id_.GetSize() + 1;
  station_id_.Insert(station, new_id);
  station_name_.Update(station, new_id);
  return new_id;
}

auto TrainSystem::StationName(const int &id) -> array<unsigned int, 10> {
  array<unsigned int, 10> res;
  station_name_.Read(res, id);
  return res;
}


auto TrainSystem::AddTrain(const Train &train) -> bool {
  return trains_.Insert(train.trainID_, train);
}

void TrainSystem::DeleteTrain(const array<char, 20> &trainID) {
  trains_.Remove(trainID);
}

void TrainSystem::ReleaseTrain(Train &train) {
  DeleteTrain(train.trainID_);
  train.is_released_ = true;
  AddTrain(train);
  // add station info
  for (int i = 0; i < train.stationNum_; ++i) {
    station_info_.Insert({train.stations_[i], train.trainID_}, train.trainID_);
  }
}

auto TrainSystem::QueryTrain(const array<char, 20> &trainID) -> Train {
  vector<Train> tmp;
  if (trains_.GetValue(trainID, &tmp)) {
    return {};
  }
  assert(tmp.size() == 1);
  return tmp[0];
}

}