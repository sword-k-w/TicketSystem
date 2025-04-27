#ifndef RID_H
#define RID_H

#include <cstdint>
#include <sstream>
#include <string>

#include "config.h"

namespace sjtu {

class RID {
 public:
  /** The default constructor creates an invalid RID! */
  RID() = default;

  /**
   * Creates a new Record Identifier for the given page identifier and slot number.
   * @param page_id page identifier
   * @param slot_num slot number
   */
  RID(int page_id, uint32_t slot_num) : page_id_(page_id), slot_num_(slot_num) {}

  explicit RID(int64_t rid) : page_id_(static_cast<int>(rid >> 32)), slot_num_(static_cast<uint32_t>(rid)) {}

  inline auto Get() const -> int64_t { return (static_cast<int64_t>(page_id_)) << 32 | slot_num_; }

  inline auto GetPageId() const -> int { return page_id_; }

  inline auto GetSlotNum() const -> uint32_t { return slot_num_; }

  inline void Set(int page_id, uint32_t slot_num) {
    page_id_ = page_id;
    slot_num_ = slot_num;
  }

  inline auto ToString() const -> std::string {
    std::stringstream os;
    os << "page_id: " << page_id_;
    os << " slot_num: " << slot_num_ << "\n";

    return os.str();
  }

  friend auto operator<<(std::ostream &os, const RID &rid) -> std::ostream & {
    os << rid.ToString();
    return os;
  }

  auto operator==(const RID &other) const -> bool { return page_id_ == other.page_id_ && slot_num_ == other.slot_num_; }

 private:
  int page_id_{-1};
  uint32_t slot_num_{0};  // logical offset from 0, 1...
};

}
#endif
