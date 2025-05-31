#include <sys/stat.h>
#include <cassert>
#include <cstring>
#include <string>
#include <iostream>

#include "buffer/disk_manager.h"
#include "config.h"

namespace sjtu {

static char *buffer_used;

/**
 * Constructor: open/create a single database file & log file
 * @input db_file: database file name
 */
DiskManager::DiskManager(const std::filesystem::path &db_file) : file_name_(db_file) {
  db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);
  // directory or file does not exist
  if (!db_io_.is_open()) {
    db_io_.clear();
    // create a new file
    db_io_.open(db_file, std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
    if (!db_io_.is_open()) {
      throw std::exception();
    }
  } else {
    db_io_.seekg(BUSTUB_PAGE_SIZE);
    db_io_.read(reinterpret_cast<char *>(&page_capacity_), 4);
    if (page_capacity_ < 16) {
      page_capacity_ = 16;
    }
  }

  // Initialize the database file.
  std::filesystem::resize_file(db_file, (page_capacity_ + 1) * BUSTUB_PAGE_SIZE);
  assert(static_cast<size_t>(GetFileSize(file_name_)) >= page_capacity_ * BUSTUB_PAGE_SIZE);
  buffer_used = nullptr;
}

/**
 * Close all file streams
 */
void DiskManager::ShutDown() {
  {
    db_io_.close();
  }
}

/**
 * @brief Increases the size of the file to fit the specified number of pages.
 */
void DiskManager::IncreaseDiskSpace(size_t pages) {
  if (pages < pages_) {
    return;
  }

  pages_ = pages;
  while (page_capacity_ < pages_) {
    page_capacity_ *= 2;
  }

  std::filesystem::resize_file(file_name_, (page_capacity_ + 1) * BUSTUB_PAGE_SIZE);

  assert(static_cast<size_t>(GetFileSize(file_name_)) >= page_capacity_ * BUSTUB_PAGE_SIZE);
}

/**
 * Write the contents of the specified page into disk file
 */
void DiskManager::WritePage(int page_id, const char *page_data) {
  size_t offset = static_cast<size_t>(page_id) * BUSTUB_PAGE_SIZE;

  // Set the write cursor to the page offset.
  num_writes_ += 1;
  db_io_.seekp(offset);
  db_io_.write(page_data, BUSTUB_PAGE_SIZE);

  if (db_io_.bad()) {
    throw std::exception();
    return;
  }

  // Flush the write to disk.
  db_io_.flush();
}

/**
 * Read the contents of the specified page into the given memory area
 */
void DiskManager::ReadPage(int page_id, char *page_data) {
  int offset = page_id * BUSTUB_PAGE_SIZE;

  // Check if we have read beyond the file length.
  if (offset > GetFileSize(file_name_)) {
    throw std::exception();
    return;
  }

  // Set the read cursor to the page offset.
  db_io_.seekg(offset);
  db_io_.read(page_data, BUSTUB_PAGE_SIZE);

  if (db_io_.bad()) {
    throw std::exception();
    return;
  }

  // Check if the file ended before we could read a full page.
  int read_count = db_io_.gcount();
  if (read_count < BUSTUB_PAGE_SIZE) {
    throw std::exception();
    db_io_.clear();
    memset(page_data + read_count, 0, BUSTUB_PAGE_SIZE - read_count);
  }
}

/**
 * Note: This is a no-op for now without a more complex data structure to
 * track deallocated pages.
 */
void DiskManager::DeletePage(int page_id) { num_deletes_ += 1; }

void DiskManager::Clean() {
  db_io_.close();
  db_io_.open(file_name_, std::ios::out);
  db_io_.close();
  db_io_.open(file_name_, std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
  page_capacity_ = 16;
  pages_ = 0;
  num_deletes_ = 0;
  num_flushes_ = 0;
  num_writes_ = 0;
  std::filesystem::resize_file(file_name_, (page_capacity_ + 1) * BUSTUB_PAGE_SIZE);
  assert(static_cast<size_t>(GetFileSize(file_name_)) >= page_capacity_ * BUSTUB_PAGE_SIZE);
  buffer_used = nullptr;
}

/**
 * Returns number of flushes made so far
 */
auto DiskManager::GetNumFlushes() const -> int { return num_flushes_; }

/**
 * Returns number of Writes made so far
 */
auto DiskManager::GetNumWrites() const -> int { return num_writes_; }

/**
 * Returns number of deletions made so far
 */
auto DiskManager::GetNumDeletes() const -> int { return num_deletes_; }

/**
 * Private helper function to get disk file size
 */
auto DiskManager::GetFileSize(const std::string &file_name) -> int {
  struct stat stat_buf;
  int rc = stat(file_name.c_str(), &stat_buf);
  return rc == 0 ? static_cast<int>(stat_buf.st_size) : -1;
}

}