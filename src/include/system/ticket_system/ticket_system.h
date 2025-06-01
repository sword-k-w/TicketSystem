#ifndef TICKET_SYSTEM_H
#define TICKET_SYSTEM_H

#include "system/ticket_system/ticket.h"
#include "b_plus_tree/b_plus_tree.h"

namespace sjtu {

class TicketSystem {
public:
  void AddOrder(const Order &order);
  void DeleteOrder(const Order &order);
  void QueryOrder(const array<char, 20> &user, vector<Order> *tmp);
  void GetQueue(vector<Order> *tmp);
  void RemoveFromQueue(const int &time);
  void Clean();
  TicketSystem() = delete;
  explicit TicketSystem(const std::string &name) : orders_(name + "_order"), queue_(name + "_queue") {}

private:
  BPlusTree<BuyInfo, Order, BuyInfoComparator, RoughBuyInfoComparator> orders_;
  BPlusTree<int, Order, TimeComparator, TimeComparator> queue_;
};

}

#endif //TICKET_SYSTEM_H
