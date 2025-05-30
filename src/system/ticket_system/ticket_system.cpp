#include "system/ticket_system/ticket_system.h"

namespace sjtu {

void TicketSystem::AddOrder(const Order &order) {
  orders_.Insert(order.info_, order);
  if (order.state_ == Order::kPending) {
    queue_.Insert(order.info_.buy_time_, order);
  }
}

void TicketSystem::DeleteOrder(const Order &order) {
  orders_.Remove(order.info_);
}

void TicketSystem::QueryOrder(const array<char, 20> &user, vector<Order> *tmp) {
  orders_.GetAllValue({user, 0}, tmp);
}

void TicketSystem::GetQueue(vector<Order> *tmp) {
  queue_.GetAll(tmp);
}

void TicketSystem::RemoveFromQueue(const int &time) {
  queue_.Remove(time);
}

void TicketSystem::Clean() {
  orders_.Clean();
  queue_.Clean();
}

}