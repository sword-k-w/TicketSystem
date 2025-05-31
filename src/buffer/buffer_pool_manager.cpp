#include "buffer/buffer_pool_manager.h"
#include "config.h"
#include <iostream>

namespace sjtu {

/**
 * @brief The constructor for a `FrameHeader` that initializes all fields to default values.
 *
 * See the documentation for `FrameHeader` in "buffer/buffer_pool_manager.h" for more information.
 *
 * @param frame_id The frame ID / index of the frame we are creating a header for.
 */
FrameHeader::FrameHeader(int frame_id, int page_id)
    : frame_id_(frame_id), data_(BUSTUB_PAGE_SIZE, 0), page_id_(page_id) {
  Reset();
}

/**
 * @brief Get a raw const pointer to the frame's data.
 *
 * @return const char* A pointer to immutable data that the frame stores.
 */
auto FrameHeader::GetData() const -> const char * { return data_.data(); }

/**
 * @brief Get a raw mutable pointer to the frame's data.
 *
 * @return char* A pointer to mutable data that the frame stores.
 */
auto FrameHeader::GetDataMut() -> char * { return data_.data(); }

/**
 * @brief Resets a `FrameHeader`'s member fields.
 */
void FrameHeader::Reset() {
  std::fill(data_.begin(), data_.end(), 0);
  pin_count_ = 0;
  is_dirty_ = false;
}

/**
 * @brief Creates a new `BufferPoolManager` instance and initializes all fields.
 *
 * See the documentation for `BufferPoolManager` in "buffer/buffer_pool_manager.h" for more information.
 *
 * ### Implementation
 *
 * We have implemented the constructor for you in a way that makes sense with our reference solution. You are free to
 * change anything you would like here if it doesn't fit with you implementation.
 *
 * Be warned, though! If you stray too far away from our guidance, it will be much harder for us to help you. Our
 * recommendation would be to first implement the buffer pool manager using the stepping stones we have provided.
 *
 * Once you have a fully working solution (all Gradescope test cases pass), then you can try more interesting things!
 *
 * @param num_frames The size of the buffer pool.
 * @param disk_manager The disk manager.
 * @param k_dist The backward k-distance for the LRU-K replacer.
 * @param log_manager The log manager. Please ignore this for P1.
 */
BufferPoolManager::BufferPoolManager(size_t num_frames, std::shared_ptr<DiskManager> disk_manager, size_t k_dist)
    : num_frames_(num_frames),
      next_page_id_(0),
      replacer_(std::make_shared<LRUKReplacer>(num_frames, k_dist)),
      disk_manager_(disk_manager) {
  // Initialize the monotonically increasing counter at 0.
  next_page_id_ = 0;

  // Initialize all of the frame headers, and fill the free frame list with all possible frame IDs (since all frames are
  // initially free).
  for (size_t i = 0; i < num_frames_; i++) {
    frames_.push_back(std::make_shared<FrameHeader>(i));
    free_frames_.push_back(static_cast<int>(i));
  }
}

void BufferPoolManager::Clean() {
  next_page_id_ = 0;
  replacer_->Clean();
  disk_manager_->Clean();
  page_table_.clear();
  free_frames_.clear();
  for (size_t i = 0; i < num_frames_; ++i) {
    free_frames_.push_back(static_cast<int>(i));
  }
}


/**
 * @brief Destroys the `BufferPoolManager`, freeing up all memory that the buffer pool was using.
 */
BufferPoolManager::~BufferPoolManager() = default;

void BufferPoolManager::InitPageCnt(int page_cnt) {
  next_page_id_ = page_cnt;
  disk_manager_->IncreaseDiskSpace(page_cnt);
}

int BufferPoolManager::PageCnt() {
  return next_page_id_;
}

/**
 * @brief Returns the number of frames that this buffer pool manages.
 */
auto BufferPoolManager::Size() const -> size_t { return num_frames_; }

/**
 * @brief Allocates a new page on disk.
 *
 * ### Implementation
 *
 * You will maintain a thread-safe, monotonically increasing counter in the form of a `std::atomic<int>`.
 * See the documentation on [atomics](https://en.cppreference.com/w/cpp/atomic/atomic) for more information.
 *
 * Also, make sure to read the documentation for `DeletePage`! You can assume that you will never run out of disk
 * space (via `DiskScheduler::IncreaseDiskSpace`), so this function _cannot_ fail.
 *
 * Once you have allocated the new page via the counter, make sure to call `DiskScheduler::IncreaseDiskSpace` so you
 * have enough space on disk!
 *
 *
 * @return The page ID of the newly allocated page.
 */
auto BufferPoolManager::NewPage() -> int {
  ++next_page_id_;
  disk_manager_->IncreaseDiskSpace(next_page_id_);
  return next_page_id_;
}

/**
 * @brief Removes a page from the database, both on disk and in memory.
 *
 * If the page is pinned in the buffer pool, this function does nothing and returns `false`. Otherwise, this function
 * removes the page from both disk and memory (if it is still in the buffer pool), returning `true`.
 *
 * ### Implementation
 *
 * Think about all of the places a page or a page's metadata could be, and use that to guide you on implementing this
 * function. You will probably want to implement this function _after_ you have implemented `CheckedReadPage` and
 * `CheckedWritePage`.
 *
 * Ideally, we would want to ensure that all space on disk is used efficiently. That would mean the space that deleted
 * pages on disk used to occupy should somehow be made available to new pages allocated by `NewPage`.
 *
 * If you would like to attempt this, you are free to do so. However, for this implementation, you are allowed to
 * assume you will not run out of disk space and simply keep allocating disk space upwards in `NewPage`.
 *
 * For (nonexistent) style points, you can still call `DeallocatePage` in case you want to implement something slightly
 * more space-efficient in the future.
 *
 *
 * @param page_id The page ID of the page we want to delete.
 * @return `false` if the page exists but could not be deleted, `true` if the page didn't exist or deletion succeeded.
 */
auto BufferPoolManager::DeletePage(int page_id) -> bool {
  auto it = page_table_.find(page_id);
  if (it == page_table_.end()) {
    disk_manager_->DeletePage(page_id);
    return true;
  }
  auto frame_id = it->second;
  if (frames_[frame_id]->pin_count_ > 0) {
    return false;
  }
  replacer_->Remove(frame_id);
  free_frames_.push_back(frame_id);
  page_table_.erase(it);
  return true;
}

/**
 * @brief Acquires an optional write-locked guard over a page of data. The user can specify an `AccessType` if needed.
 *
 * If it is not possible to bring the page of data into memory, this function will return a `std::nullopt`.
 *
 * Page data can _only_ be accessed via page guards. Users of this `BufferPoolManager` are expected to acquire either a
 * `ReadPageGuard` or a `WritePageGuard` depending on the mode in which they would like to access the data, which
 * ensures that any access of data is thread-safe.
 *
 * There can only be 1 `WritePageGuard` reading/writing a page at a time. This allows data access to be both immutable
 * and mutable, meaning the thread that owns the `WritePageGuard` is allowed to manipulate the page's data however they
 * want. If a user wants to have multiple threads reading the page at the same time, they must acquire a `ReadPageGuard`
 * with `CheckedReadPage` instead.
 *
 * ### Implementation
 *
 * There are 3 main cases that you will have to implement. The first two are relatively simple: one is when there is
 * plenty of available memory, and the other is when we don't actually need to perform any additional I/O. Think about
 * what exactly these two cases entail.
 *
 * The third case is the trickiest, and it is when we do not have any _easily_ available memory at our disposal. The
 * buffer pool is tasked with finding memory that it can use to bring in a page of memory, using the replacement
 * algorithm you implemented previously to find candidate frames for eviction.
 *
 * Once the buffer pool has identified a frame for eviction, several I/O operations may be necessary to bring in the
 * page of data we want into the frame.
 *
 * There is likely going to be a lot of shared code with `CheckedReadPage`, so you may find creating helper functions
 * useful.
 *
 * These two functions are the crux of this project, so we won't give you more hints than this. Good luck!
 *
 *
 * @param page_id The ID of the page we want to write to.
 * @param access_type The type of page access.
 * @return std::optional<WritePageGuard> An optional latch guard where if there are no more free frames (out of memory)
 * returns `std::nullopt`, otherwise returns a `WritePageGuard` ensuring exclusive and mutable access to a page's data.
 */
auto BufferPoolManager::WritePage(int page_id) -> WritePageGuard {
  auto frame_opt = FetchPage(page_id);
  if (!frame_opt.has_value()) {
    throw std::exception();
  }
  return WritePageGuard(page_id, frame_opt.value(), replacer_, disk_manager_);
}

/**
 * @brief Acquires an optional read-locked guard over a page of data. The user can specify an `AccessType` if needed.
 *
 * If it is not possible to bring the page of data into memory, this function will return a `std::nullopt`.
 *
 * Page data can _only_ be accessed via page guards. Users of this `BufferPoolManager` are expected to acquire either a
 * `ReadPageGuard` or a `WritePageGuard` depending on the mode in which they would like to access the data, which
 * ensures that any access of data is thread-safe.
 *
 * There can be any number of `ReadPageGuard`s reading the same page of data at a time across different threads.
 * However, all data access must be immutable. If a user wants to mutate the page's data, they must acquire a
 * `WritePageGuard` with `CheckedWritePage` instead.
 *
 * ### Implementation
 *
 * See the implementation details of `CheckedWritePage`.
 *
 *
 * @param page_id The ID of the page we want to read.
 * @param access_type The type of page access.
 * @return std::optional<ReadPageGuard> An optional latch guard where if there are no more free frames (out of memory)
 * returns `std::nullopt`, otherwise returns a `ReadPageGuard` ensuring shared and read-only access to a page's data.
 */
auto BufferPoolManager::ReadPage(int page_id) -> ReadPageGuard {
  auto frame_opt = FetchPage(page_id);
  if (!frame_opt.has_value()) {
    throw std::exception();
  }
  return ReadPageGuard(page_id, frame_opt.value(), replacer_, disk_manager_);
}

/**
 * @brief Flushes a page's data out to disk.
 *
 * This function will write out a page's data to disk if it has been modified. If the given page is not in memory, this
 * function will return `false`.
 *
 * ### Implementation
 *
 * You should probably leave implementing this function until after you have completed `CheckedReadPage` and
 * `CheckedWritePage`, as it will likely be much easier to understand what to do.
 *
 * @param page_id The page ID of the page to be flushed.
 * @return `false` if the page could not be found in the page table, otherwise `true`.
 */
auto BufferPoolManager::FlushPage(int page_id) -> bool {
  auto it = page_table_.find(page_id);
  if (it == page_table_.end() || !frames_[it->second]->is_dirty_) {
    return false;
  }
  if (frames_[it->second]->is_dirty_) {
    disk_manager_->WritePage(page_id, frames_[it->second]->GetDataMut());
    frames_[it->second]->is_dirty_ = false;
  }
  return true;
}

/**
 * @brief Flushes all page data that is in memory to disk.
 *
 * ### Implementation
 *
 * You should probably leave implementing this function until after you have completed `CheckedReadPage`,
 * `CheckedWritePage`, and `FlushPage`, as it will likely be much easier to understand what to do.
 */
void BufferPoolManager::FlushAllPages() {
  for (auto &frame : frames_) {
    if (frame != nullptr) {
      if (frame->is_dirty_) {
        disk_manager_->WritePage(frame->page_id_, frame->GetDataMut());
        frame->is_dirty_ = false;
      }
    }
  }
}

/**
 * @brief Retrieves the pin count of a page. If the page does not exist in memory, return `std::nullopt`.
 *
 * This function is thread safe. Callers may invoke this function in a multi-threaded environment where multiple threads
 * access the same page.
 *
 * This function is intended for testing purposes. If this function is implemented incorrectly, it will definitely cause
 * problems with the test suite and autograder.
 *
 * # Implementation
 *
 * We will use this function to test if your buffer pool manager is managing pin counts correctly. Since the
 * `pin_count_` field in `FrameHeader` is an atomic type, you do not need to take the latch on the frame that holds the
 * page we want to look at. Instead, you can simply use an atomic `load` to safely load the value stored. You will still
 * need to take the buffer pool latch, however.
 *
 * Again, if you are unfamiliar with atomic types, see the official C++ docs
 * [here](https://en.cppreference.com/w/cpp/atomic/atomic).
 *
 * @param page_id The page ID of the page we want to get the pin count of.
 * @return std::optional<size_t> The pin count if the page exists, otherwise `std::nullopt`.
 */
auto BufferPoolManager::GetPinCount(int page_id) -> std::optional<size_t> {
  if (page_table_.find(page_id) == page_table_.end()) {
    return std::nullopt;
  }
  return frames_[page_table_[page_id]]->pin_count_;
}

auto BufferPoolManager::FetchPage(int page_id)
    -> std::optional<std::shared_ptr<FrameHeader>> {
  if (page_table_.find(page_id) != page_table_.end()) {
    replacer_->RecordAccess(page_table_[page_id]);
    return frames_[page_table_[page_id]];
  }
  if (!free_frames_.empty()) {
    auto new_frame = free_frames_.back();
    free_frames_.pop_back();
    frames_[new_frame] = std::make_shared<FrameHeader>(new_frame, page_id);
    replacer_->RecordAccess(new_frame);
    page_table_[page_id] = new_frame;
    disk_manager_->ReadPage(page_id, frames_[new_frame]->GetDataMut());
    return frames_[new_frame];
  }
  auto evicted_frame = replacer_->Evict();
  if (!evicted_frame.has_value()) {
    return std::nullopt;
  }
  auto frame_id = evicted_frame.value();
  if (frames_[frame_id]->is_dirty_) {
    disk_manager_->WritePage(frames_[frame_id]->page_id_, frames_[frame_id]->GetDataMut());
  }
  page_table_.erase(page_table_.find(frames_[frame_id]->page_id_));
  frames_[frame_id] = std::make_shared<FrameHeader>(frame_id, page_id);
  page_table_[page_id] = frame_id;
  replacer_->RecordAccess(frame_id);
  disk_manager_->ReadPage(page_id, frames_[frame_id]->GetDataMut());
  return frames_[frame_id];
}

}