#include "system/train_system/train_system.h"

namespace sjtu {

auto TrainSystem::TrainID(const array<char, 20> &train) -> int {
  vector<int> tmp;
  assert(train_id_.GetValue(train, &tmp));
  return tmp[0];
}

auto TrainSystem::StationID(array<unsigned int, 10> &station, bool add_new) -> int {
  vector<int> tmp;
  if (station_id_.GetValue(station, &tmp)) {
    return tmp[0];
  }
  if (add_new) {
    int new_id = station_id_.GetSize() + 1;
    station_id_.Insert(station, new_id);
    station_name_.Update(station, new_id);
    return new_id;
  }
  return -1;
}

auto TrainSystem::StationName(const int &id) -> array<unsigned int, 10> {
  array<unsigned int, 10> res;
  station_name_.Read(res, id);
  return res;
}

auto TrainSystem::AddTrain(Train &train) -> bool {
  int new_id = train_id_.GetSize() + 1;
  if (!train_id_.Insert(train.trainID_, new_id)) {
    return false;
  }
  trains_.Update(train, new_id);
  return true;
}

void TrainSystem::DeleteTrain(const array<char, 20> &trainID) {
  train_id_.Remove(trainID);
}

void TrainSystem::ReleaseTrain(Train &train) {
  train.is_released_ = true;
  int id = TrainID(train.trainID_);
  trains_.Update(train, id);
  for (int i = 0; i < train.stationNum_; ++i) {
    station_info_.Insert({train.stations_[i], id}, id);
  }
}

auto TrainSystem::QueryTrain(const int &train_id) -> Train {
  Train res;
  trains_.Read(res, train_id);
  return res;
}

auto TrainSystem::QueryTrain(const array<char, 20> &trainID) -> Train {
  vector<int> tmp;
  if (!train_id_.GetValue(trainID, &tmp)) {
    return {};
  }
  assert(tmp.size() == 1);
  return QueryTrain(tmp[0]);
}

void TrainSystem::UpdateTrain(const int &train_id, Train &new_train) {
  trains_.Update(new_train, train_id);
}

void TrainSystem::QueryStationInfo(const int &id, vector<int> *info) {
  station_info_.GetAllValue({id}, info);
}


}
