#include "system/ticket_system/ticket_system.h"

namespace sjtu {

void TicketSystem::AddOrder(const Order &order) {
  orders_.Insert({order.user_, order.buy_time_}, order);
  if (order.state_ == Order::kPending) {
    queue_.Insert(order.buy_time_, order);
  }
}

}