/**
 * b_plus_tree.h
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain actual data.
 * (1) We only support unique key
 * (2) support insert & remove
 * (3) The structure should shrink and grow dynamically
 * (4) Implement index iterator for range scan
 */
#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include "b_plus_tree/b_plus_tree_header_page.h"
#include "b_plus_tree/b_plus_tree_internal_page.h"
#include "b_plus_tree/b_plus_tree_leaf_page.h"
#include "b_plus_tree/page_guard.h"
#include "my_stl/list.hpp"

namespace sjtu {

/**
 * @brief Definition of the Context class.
 *
 * Hint: This class is designed to help you keep track of the pages
 * that you're modifying or accessing.
 */
class Context {
 public:
  // When you insert into / remove from the B+ tree, store the write guard of header page here.
  // Remember to drop the header page guard and set it to nullopt when you want to unlock all.
  std::optional<WritePageGuard> header_page_{std::nullopt};

  // Save the root page id here so that it's easier to know if the current page is the root page.
  int root_page_id_{-1};

  // Store the write guards of the pages that you're modifying here.
  list<WritePageGuard> write_set_;

  // You may want to use this when getting value, but not necessary.
  list<ReadPageGuard> read_set_;

  list<int> which_son_;

  auto IsRootPage(int page_id) -> bool { return page_id == root_page_id_; }
};

#define BPLUSTREE_TYPE BPlusTree<KeyType, ValueType, KeyComparator, RoughKeyComparator>

// Main class providing the API for the Interactive B+ Tree.
INDEX_TEMPLATE_ARGUMENTS
class BPlusTree {
  using InternalPage = BPlusTreeInternalPage<KeyType, int, KeyComparator, RoughKeyComparator>;
  using LeafPage = BPlusTreeLeafPage<KeyType, ValueType, KeyComparator, RoughKeyComparator>;

 public:
  explicit BPlusTree(std::string name,
                     int leaf_max_size = LEAF_PAGE_SLOT_CNT,
                     int internal_max_size = INTERNAL_PAGE_SLOT_CNT);

  ~BPlusTree();

  // Returns true if this B+ tree has no keys and values.
  auto IsEmpty() const -> bool;

  // Insert a key-value pair into this B+ tree.
  auto Insert(const KeyType &key, const ValueType &value) -> bool;

  // Remove a key and its value from this B+ tree.
  void Remove(const KeyType &key);

  // Return the value associated with a given key
  auto GetValue(const KeyType &key, vector<ValueType> *result) -> bool;

  // Return all value satisfied rough comparator
  void GetAllValue(const KeyType &key, vector<ValueType> *result);

  void GetAll(vector<ValueType> *result);

  // Return the page id of the root node
  auto GetRootPageId() const -> int;

  // Return the size of bpt
  auto GetSize() const -> int;

  void Clean();

 private:

  // member variable
  std::string index_name_;
  std::shared_ptr<DiskManager> disk_manager_;
  BufferPoolManager *bpm_;
  KeyComparator comparator_;
  RoughKeyComparator rough_comparator_;
  int leaf_max_size_;
  int internal_max_size_;
  int header_page_id_;
};

}

#endif