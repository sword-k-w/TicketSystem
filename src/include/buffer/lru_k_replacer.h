#ifndef LRU_K_REPLACER_H
#define LRU_K_REPLACER_H

#include <list>
#include <optional>
#include <unordered_map>

namespace sjtu {

class LRUKNode {
 private:
  /** History of last seen K timestamps of this page. Least recent timestamp stored in front. */
  // Remove maybe_unused if you start using them. Feel free to change the member variables as you want.

  std::list<size_t> history_;
  size_t k_;
  bool is_evictable_{false};

 public:
  explicit LRUKNode(size_t k = 0) : k_(k) {}
  auto IsEvictable() const -> bool { return is_evictable_; }
  auto ToggleState(bool set_evictable) -> bool {
    if (is_evictable_ ^ set_evictable) {
      is_evictable_ = set_evictable;
      return true;
    }
    return false;
  }
  auto LeastRecent() const -> size_t { return history_.back(); }
  auto LeastRecentK() const -> size_t { return history_.size() < k_ ? 0 : history_.front(); }
  void AddVisit(size_t time);
};

/**
 * LRUKReplacer implements the LRU-k replacement policy.
 *
 * The LRU-k algorithm evicts a frame whose backward k-distance is maximum
 * of all frames. Backward k-distance is computed as the difference in time between
 * current timestamp and the timestamp of kth previous access.
 *
 * A frame with less than k historical references is given
 * +inf as its backward k-distance. When multiple frames have +inf backward k-distance,
 * classical LRU algorithm is used to choose victim.
 */
class LRUKReplacer {
 public:
  explicit LRUKReplacer(size_t num_frames, size_t k);

  /**
   *
   * @brief Destroys the LRUReplacer.
   */
  ~LRUKReplacer() = default;

  auto Evict() -> std::optional<int>;

  void RecordAccess(int frame_id);

  void SetEvictable(int frame_id, bool set_evictable);

  void Remove(int frame_id);

  auto Size() -> size_t;

 private:
  // Remove maybe_unused if you start using them.
  std::unordered_map<int, LRUKNode> node_store_;
  size_t current_timestamp_{0};
  size_t curr_size_{0};
  size_t evictable_size_{0};
  size_t replacer_size_;
  size_t k_;
};

}  // namespace bustub

#endif
