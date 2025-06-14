#ifndef SYSTEM_H
#define SYSTEM_H

#include "system/user_system/user_system.h"
#include "system/train_system/train_system.h"
#include "system/ticket_system/ticket_system.h"
#include "system/input.h"

namespace sjtu {

class System {
public:
  System() = delete;
  explicit System(const std::string &name);
  void Run();
  void AddUser();
  void Login();
  void Logout();
  void QueryProfile();
  void ModifyProfile();
  void AddTrain();
  void DeleteTrain();
  void ReleaseTrain();
  void QueryTrain();
  void QueryTicket();
  void QueryTransfer();
  void BuyTicket();
  void QueryOrder();
  void RefundTicket();
  void Clean();
private:
  UserSystem user_system_;
  TrainSystem train_system_;
  TicketSystem ticket_system_;
  map<array<char, 20>, User> online_users_;
  Input input_;
  int timestamp_;
  void PrintTimestamp() const;
};

}

#endif //SYSTEM_H
