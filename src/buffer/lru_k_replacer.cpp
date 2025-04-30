#include "buffer/lru_k_replacer.h"

namespace sjtu {

void LRUKNode::AddVisit(size_t time) {
  history_.push_front(time);
  if (history_.size() > k_) {
    history_.pop_back();
  }
}

/**
 *
 * @brief a new LRUKReplacer.
 * @param num_frames the maximum number of frames the LRUReplacer will be required to store
 */
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

/**
 *
 * @brief Find the frame with largest backward k-distance and evict that frame. Only frames
 * that are marked as 'evictable' are candidates for eviction.
 *
 * A frame with less than k historical references is given +inf as its backward k-distance.
 * If multiple frames have inf backward k-distance, then evict frame whose oldest timestamp
 * is furthest in the past.
 *
 * Successful eviction of a frame should decrement the size of replacer and remove the frame's
 * access history.
 *
 * @return true if a frame is evicted successfully, false if no frames can be evicted.
 */
auto LRUKReplacer::Evict() -> std::optional<int> {
  ++current_timestamp_;
  int min_id = -1;
  size_t k_min_time = 1e18;
  size_t min_time = 1e18;
  for (const auto &frame : node_store_) {
    if (frame.second.IsEvictable()) {
      size_t k_time = frame.second.LeastRecentK();
      size_t time = frame.second.LeastRecent();
      if (k_time < k_min_time || (k_time == k_min_time && time < min_time)) {
        min_id = frame.first;
        k_min_time = k_time;
        min_time = time;
      }
    }
  }
  if (min_id == -1) {
    return std::nullopt;
  }
  node_store_.erase(node_store_.find(min_id));
  --evictable_size_;
  --curr_size_;
  return min_id;
}

/**
 *
 * @brief Record the event that the given frame id is accessed at current timestamp.
 * Create a new entry for access history if frame id has not been seen before.
 *
 * If frame id is invalid (ie. larger than replacer_size_), throw an exception. You can
 * also use BUSTUB_ASSERT to abort the process if frame id is invalid.
 *
 * @param frame_id id of frame that received a new access.
 * @param access_type type of access that was received. This parameter is only needed for
 * leaderboard tests.
 */
void LRUKReplacer::RecordAccess(int frame_id) {
  ++current_timestamp_;
  if (frame_id > static_cast<int>(replacer_size_)) {
    throw std::exception();
  }
  if (node_store_.find(frame_id) == node_store_.end()) {
    node_store_[frame_id] = LRUKNode(k_);
    ++curr_size_;
  }
  node_store_[frame_id].AddVisit(current_timestamp_);
}

/**
 *
 * @brief Toggle whether a frame is evictable or non-evictable. This function also
 * controls replacer's size. Note that size is equal to number of evictable entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then size should
 * decrement. If a frame was previously non-evictable and is to be set to evictable,
 * then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void LRUKReplacer::SetEvictable(int frame_id, bool set_evictable) {
  ++current_timestamp_;
  if (frame_id > static_cast<int>(replacer_size_)) {
    throw std::exception();
  }
  if (node_store_.find(frame_id) == node_store_.end()) {
    return;
  }
  auto tmp = node_store_[frame_id];
  if (tmp.ToggleState(set_evictable)) {
    node_store_[frame_id] = tmp;
    if (set_evictable) {
      ++evictable_size_;
    } else {
      --evictable_size_;
    }
  }
}

/**
 *
 * @brief Remove an evictable frame from replacer, along with its access history.
 * This function should also decrement replacer's size if removal is successful.
 *
 * Note that this is different from evicting a frame, which always remove the frame
 * with largest backward k-distance. This function removes specified frame id,
 * no matter what its backward k-distance is.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 * If specified frame is not found, directly return from this function.
 *
 * @param frame_id id of frame to be removed
 */
void LRUKReplacer::Remove(int frame_id) {
  ++current_timestamp_;
  if (node_store_.find(frame_id) == node_store_.end()) {
    return;
  }
  if (!node_store_[frame_id].IsEvictable()) {
    throw std::exception();
  }
  node_store_.erase(node_store_.find(frame_id));
  --curr_size_;
  --evictable_size_;
}

/**
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto LRUKReplacer::Size() -> size_t {
  return evictable_size_;
}

}  // namespace bustub