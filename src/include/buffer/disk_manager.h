#ifndef DISK_MANAGER_H
#define DISK_MANAGER_H

#include <filesystem>
#include <fstream>
#include <string>

namespace sjtu {

/**
 * DiskManager takes care of the allocation and deallocation of pages within a database. It performs the reading and
 * writing of pages to and from disk, providing a logical file layer within the context of a database management system.
 */
class DiskManager {
 public:
  /**
   * Creates a new disk manager that writes to the specified database file.
   * @param db_file the file name of the database file to write to
   */
  explicit DiskManager(const std::filesystem::path &db_file);

  /** FOR TEST / LEADERBOARD ONLY, used by DiskManagerMemory */
  DiskManager() = default;

  virtual ~DiskManager() = default;

  /**
   * Shut down the disk manager and close all the file resources.
   */
  void ShutDown();

  /**
   * @brief Increases the size of the database file.
   *
   * This function works like a dynamic array, where the capacity is doubled until all pages can fit.
   *
   * @param pages The number of pages the caller wants the file used for storage to support.
   */
  virtual void IncreaseDiskSpace(size_t pages);

  /**
   * Write a page to the database file.
   * @param page_id id of the page
   * @param page_data raw page data
   */
  virtual void WritePage(int page_id, const char *page_data);

  /**
   * Read a page from the database file.
   * @param page_id id of the page
   * @param[out] page_data output buffer
   */
  virtual void ReadPage(int page_id, char *page_data);

  /**
   * Delete a page from the database file. Reclaim the disk space.
   * @param page_id id of the page
   */
  virtual void DeletePage(int page_id);

  /** @return the number of disk flushes */
  auto GetNumFlushes() const -> int;

  /** @return the number of disk writes */
  auto GetNumWrites() const -> int;

  /** @return the number of deletions */
  auto GetNumDeletes() const -> int;

 protected:
  auto GetFileSize(const std::string &file_name) -> int;
  // stream to write db file
  std::fstream db_io_;
  std::filesystem::path file_name_;
  int num_flushes_{0};
  int num_writes_{0};
  int num_deletes_{0};

  /** @brief The number of pages allocated to the DBMS on disk. */
  size_t pages_{0};
  /** @brief The capacity of the file used for storage on disk. */
  size_t page_capacity_{16};
};

}

#endif
