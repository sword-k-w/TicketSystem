#include "b_plus_tree/b_plus_tree.h"
#include "comparator.h"
#include "system/user_system/user.h"
#include "system/train_system/train.h"
#include "system/ticket_system/ticket.h"

namespace sjtu {

INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::BPlusTree(std::string name, int leaf_max_size, int internal_max_size)
    : index_name_(std::move(name)),
      disk_manager_(std::make_shared<DiskManager>(index_name_)),
      bpm_(new BufferPoolManager(1200, disk_manager_, 10)),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size),
      header_page_id_(bpm_->NewPage()) {
  WritePageGuard guard = bpm_->WritePage(header_page_id_);
  auto root_page = guard.AsMut<BPlusTreeHeaderPage>();
  if (root_page->root_page_id_ == 0) {
    root_page->root_page_id_ = -1;
  } else {
    bpm_->InitPageCnt(root_page->page_cnt_);
  }
}

INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::~BPlusTree() {
  bpm_->WritePage(header_page_id_).AsMut<BPlusTreeHeaderPage>()->page_cnt_ = bpm_->PageCnt();
  bpm_->FlushAllPages();
  delete bpm_;
}

/**
 * @brief Helper function to decide whether current b+tree is empty
 * @return Returns true if this B+ tree has no keys and values.
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::IsEmpty() const -> bool { return GetRootPageId() == -1; }

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/**
 * @brief Return the only value that associated with input key
 *
 * This method is used for point query
 *
 * @param key input key
 * @param[out] result vector that stores the only value that associated with input key, if the value exists
 * @return : true means key exists
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetValue(const KeyType &key, vector<ValueType> *result) -> bool {
  // Declaration of context instance. Using the Context is not necessary but advised.
  Context ctx;
  ctx.root_page_id_ = GetRootPageId();
  if (ctx.root_page_id_ == -1) {
    return false;
  }
  ctx.read_set_.emplace_back(bpm_->ReadPage(ctx.root_page_id_));
  while (true) {
    auto it = --ctx.read_set_.end();
    auto page = it->As<BPlusTreePage>();
    auto size = page->GetSize();
    if (page->IsLeafPage()) {
      auto leaf_page = it->As<LeafPage>();
      for (int i = 0; i < size; ++i) {
        if (comparator_(leaf_page->KeyAt(i), key) == 0) {
          result->push_back(leaf_page->RidAt(i));
          return true;
        }
      }
      return false;
    }
    auto internal_page = it->As<InternalPage>();
    int pos = size;
    for (int i = 1; i < size; ++i) {
      if (comparator_(key, internal_page->KeyAt(i)) < 0) {
        pos = i;
        break;
      }
    }
    ctx.read_set_.emplace_back(bpm_->ReadPage(internal_page->ValueAt(pos - 1)));
  }
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/**
 * @brief Return all value that satisfied rough comparator
 *
 * @param key input key
 * @param[out] result vector that stores all value that satisfied rough comparator
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::GetAllValue(const KeyType &key, vector<ValueType> *result) {
  // Declaration of context instance. Using the Context is not necessary but advised.
  Context ctx;
  ctx.root_page_id_ = GetRootPageId();
  if (ctx.root_page_id_ == -1) {
    return;
  }
  ctx.read_set_.emplace_back(bpm_->ReadPage(ctx.root_page_id_));
  while (true) {
    auto it = --ctx.read_set_.end();
    auto page = it->As<BPlusTreePage>();
    auto size = page->GetSize();
    if (it->GetPageId() == 0) {
      exit(0);
    }
    if (page->IsLeafPage()) {
      auto leaf_page = it->As<LeafPage>();
      for (int i = 0; i < size; ++i) {
        if (rough_comparator_(leaf_page->KeyAt(i), key) > 0) {
          return;
        }
        if (rough_comparator_(leaf_page->KeyAt(i), key) == 0) {
          auto page_id = it->GetPageId();
          ctx.read_set_.pop_back();
          auto guard = bpm_->ReadPage(page_id);
          leaf_page = guard.As<LeafPage>();
          while (rough_comparator_(leaf_page->KeyAt(i), key) == 0) {
            result->push_back(leaf_page->RidAt(i));
            ++i;
            if (i == size) {
              auto nxt = leaf_page->GetNextPageId();
              if (nxt == -1) {
                return;
              }
              page_id = nxt;
              guard = bpm_->ReadPage(page_id);
              leaf_page = guard.As<LeafPage>();
              i = 0;
              size = leaf_page->GetSize();
            }
          }
          return;
        }
        if (i + 1 == size) {
          int nxt = leaf_page->GetNextPageId();
          if (nxt == -1) {
            return;
          }
          ctx.read_set_.pop_back();
          ctx.read_set_.emplace_back(bpm_->ReadPage(nxt));
          it = --ctx.read_set_.end();
          leaf_page = it->As<LeafPage>();
          size = leaf_page->GetSize();
          i = -1;
        }
      }
      return;
    }
    auto internal_page = it->As<InternalPage>();
    int pos = size;
    for (int i = 1; i < size; ++i) {
      if (rough_comparator_(key, internal_page->KeyAt(i)) <= 0) {
        pos = i;
        break;
      }
    }
    ctx.read_set_.emplace_back(bpm_->ReadPage(internal_page->ValueAt(pos - 1)));
  }
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/**
 * @brief Insert constant key & value pair into b+ tree
 *
 * if current tree is empty, start new tree, update root page id and insert
 * entry, otherwise insert into leaf page.
 *
 * @param key the key to insert
 * @param value the value associated with key
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Insert(const KeyType &key, const ValueType &value) -> bool {
  // Declaration of context instance. Using the Context is not necessary but advised.
  Context ctx;
  ctx.root_page_id_ = GetRootPageId();
  if (ctx.root_page_id_ == -1) {
    auto root_page_id = bpm_->NewPage();
    auto guard = bpm_->WritePage(root_page_id);
    auto root_page = guard.AsMut<LeafPage>();
    root_page->Init(leaf_max_size_);
    root_page->ChangeSizeBy(1);
    root_page->SetKeyAt(0, key);
    root_page->SetRidAt(0, value);
    auto head_page = bpm_->WritePage(header_page_id_);
    head_page.AsMut<BPlusTreeHeaderPage>()->root_page_id_ = root_page_id;
    ++head_page.AsMut<BPlusTreeHeaderPage>()->size_;
    return true;
  }
  ctx.write_set_.emplace_back(bpm_->WritePage(ctx.root_page_id_));
  while (true) {
    auto it = --ctx.write_set_.end();
    auto page = it->As<BPlusTreePage>();
    auto size = page->GetSize();
    if (page->IsLeafPage()) {
      auto leaf_page = it->AsMut<LeafPage>();
      int pos = size;
      for (int i = 0; i < size; ++i) {
        auto res = comparator_(leaf_page->KeyAt(i), key);
        if (res == 0) {
          return false;
        }
        if (res > 0) {
          pos = i;
          break;
        }
      }
      if (size < leaf_max_size_) {
        leaf_page->ChangeSizeBy(1);
        for (int i = size - 1; i >= pos; --i) {
          leaf_page->SetKeyAt(i + 1, leaf_page->KeyAt(i));
          leaf_page->SetRidAt(i + 1, leaf_page->RidAt(i));
        }
        leaf_page->SetKeyAt(pos, key);
        leaf_page->SetRidAt(pos, value);
      } else {
        // split the leaf node
        vector<KeyType> leaf_key(leaf_max_size_ + 1);
        vector<ValueType> leaf_rid(leaf_max_size_ + 1);
        for (int i = 0; i < pos; ++i) {
          leaf_key[i] = leaf_page->KeyAt(i);
          leaf_rid[i] = leaf_page->RidAt(i);
        }
        leaf_key[pos] = key;
        leaf_rid[pos] = value;
        for (int i = pos; i < leaf_max_size_; ++i) {
          leaf_key[i + 1] = leaf_page->KeyAt(i);
          leaf_rid[i + 1] = leaf_page->RidAt(i);
        }
        auto new_leaf_id = bpm_->NewPage();
        auto new_leaf_guard = bpm_->WritePage(new_leaf_id);
        auto new_leaf_page = new_leaf_guard.AsMut<LeafPage>();
        new_leaf_page->Init(leaf_max_size_);
        auto new_size = (leaf_max_size_ + 1) / 2;
        auto remain_size = leaf_max_size_ + 1 - new_size;
        new_leaf_page->SetSize(new_size);
        for (int i = 0; i < new_size; ++i) {
          new_leaf_page->SetKeyAt(i, leaf_key[remain_size + i]);
          new_leaf_page->SetRidAt(i, leaf_rid[remain_size + i]);
        }
        new_leaf_page->SetNextPageId(leaf_page->GetNextPageId());

        leaf_page->SetSize(remain_size);
        for (int i = 0; i < remain_size; ++i) {
          leaf_page->SetKeyAt(i, leaf_key[i]);
          leaf_page->SetRidAt(i, leaf_rid[i]);
        }
        leaf_page->SetNextPageId(new_leaf_id);

        // try to insert a new node into the chain to the root
        auto new_page_id = new_leaf_id;
        auto new_key = leaf_key[remain_size];
        bool flag = false;
        ctx.write_set_.pop_back();
        while (!ctx.write_set_.empty()) {
          auto cur_page = (--ctx.write_set_.end())->AsMut<InternalPage>();
          auto cur_size = cur_page->GetSize();
          int cur_pos = ctx.which_son_.back();
          if (cur_size < internal_max_size_) {  // stop split
            cur_page->ChangeSizeBy(1);
            for (int i = cur_size - 1; i > cur_pos; --i) {
              cur_page->SetKeyAt(i + 1, cur_page->KeyAt(i));
              cur_page->SetValueAt(i + 1, cur_page->ValueAt(i));
            }
            cur_page->SetKeyAt(cur_pos + 1, new_key);
            cur_page->SetValueAt(cur_pos + 1, new_page_id);
            flag = true;
            break;
          }

          vector<KeyType> cur_key_vec(internal_max_size_ + 1);
          vector<int> cur_page_vec(internal_max_size_ + 1, 0);
          for (int i = 0; i <= cur_pos; ++i) {
            cur_key_vec[i] = cur_page->KeyAt(i);
            cur_page_vec[i] = cur_page->ValueAt(i);
          }
          cur_key_vec[cur_pos + 1] = new_key;
          cur_page_vec[cur_pos + 1] = new_page_id;
          for (int i = cur_pos + 1; i < internal_max_size_; ++i) {
            cur_key_vec[i + 1] = cur_page->KeyAt(i);
            cur_page_vec[i + 1] = cur_page->ValueAt(i);
          }

          auto split_id = bpm_->NewPage();
          auto split_guard = bpm_->WritePage(split_id);
          auto split_page = split_guard.AsMut<InternalPage>();
          split_page->Init(internal_max_size_);
          new_size = (internal_max_size_ + 1) / 2;
          remain_size = internal_max_size_ + 1 - new_size;
          split_page->SetSize(new_size);
          for (int i = 0; i < new_size; ++i) {
            split_page->SetKeyAt(i, cur_key_vec[i + remain_size]);
            split_page->SetValueAt(i, cur_page_vec[i + remain_size]);
          }
          cur_page->SetSize(remain_size);
          for (int i = 0; i < remain_size; ++i) {
            cur_page->SetKeyAt(i, cur_key_vec[i]);
            cur_page->SetValueAt(i, cur_page_vec[i]);
          }
          new_page_id = split_id;
          new_key = cur_key_vec[remain_size];

          ctx.write_set_.pop_back();
          ctx.which_son_.pop_back();
        }
        if (!flag) {  // new root
          auto new_root_id = bpm_->NewPage();
          auto new_root_guard = bpm_->WritePage(new_root_id);
          auto new_root_page = new_root_guard.AsMut<InternalPage>();
          new_root_page->Init(internal_max_size_);
          new_root_page->SetSize(2);
          new_root_page->SetKeyAt(0, bpm_->ReadPage(ctx.root_page_id_).As<InternalPage>()->KeyAt(0));
          new_root_page->SetValueAt(0, ctx.root_page_id_);
          new_root_page->SetKeyAt(1, new_key);
          new_root_page->SetValueAt(1, new_page_id);
          bpm_->WritePage(header_page_id_).AsMut<BPlusTreeHeaderPage>()->root_page_id_ = new_root_id;
        }
      }
      ++bpm_->WritePage(header_page_id_).AsMut<BPlusTreeHeaderPage>()->size_;
      return true;
    }
    auto internal_page = it->As<InternalPage>();
    int pos = size;
    for (int i = 1; i < size; ++i) {
      if (comparator_(key, internal_page->KeyAt(i)) < 0) {
        pos = i;
        break;
      }
    }
    --pos;
    ctx.which_son_.push_back(pos);
    ctx.write_set_.emplace_back(bpm_->WritePage(internal_page->ValueAt(pos)));
  }
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/**
 * @brief Delete key & value pair associated with input key
 * If current tree is empty, return immediately.
 * If not, User needs to first find the right leaf page as deletion target, then
 * delete entry from leaf page. Remember to deal with redistribute or merge if
 * necessary.
 *
 * @param key input key
 */
INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Remove(const KeyType &key) {
  // Declaration of context instance.
  Context ctx;
  ctx.root_page_id_ = GetRootPageId();
  if (ctx.root_page_id_ == -1) {
    return;
  }
  auto guard = bpm_->WritePage(ctx.root_page_id_);
  if (guard.As<BPlusTreePage>()->IsLeafPage()) {
    auto root_page = guard.AsMut<LeafPage>();
    auto size = root_page->GetSize();
    for (int i = 0; i < size; ++i) {
      if (comparator_(root_page->KeyAt(i), key) == 0) {
        for (int j = i + 1; j < size; ++j) {
          root_page->SetKeyAt(j - 1, root_page->KeyAt(j));
          root_page->SetRidAt(j - 1, root_page->RidAt(j));
        }
        root_page->ChangeSizeBy(-1);
        if (root_page->GetSize() == 0) {
          guard.Drop();
          bpm_->DeletePage(ctx.root_page_id_);
          bpm_->WritePage(header_page_id_).AsMut<BPlusTreeHeaderPage>()->root_page_id_ = -1;
        }
        return;
      }
    }
    return;
  }
  ctx.write_set_.emplace_back(std::move(guard));
  while (true) {
    auto it = --ctx.write_set_.end();
    auto page = it->As<BPlusTreePage>();
    auto size = page->GetSize();
    if (page->IsLeafPage()) {
      auto leaf_page = it->AsMut<LeafPage>();
      int pos = -1;
      for (int i = 0; i < size; ++i) {
        if (comparator_(leaf_page->KeyAt(i), key) == 0) {
          pos = i;
          break;
        }
      }
      if (pos == -1) {
        return;
      }
      if (size > leaf_page->GetMinSize()) {
        for (int i = pos + 1; i < size; ++i) {
          leaf_page->SetKeyAt(i - 1, leaf_page->KeyAt(i));
          leaf_page->SetRidAt(i - 1, leaf_page->RidAt(i));
        }
        leaf_page->ChangeSizeBy(-1);
        return;
      }
      auto fa_page = (--(--ctx.write_set_.end()))->AsMut<InternalPage>();
      ctx.write_set_.pop_back();
      auto son_id = *--ctx.which_son_.end();
      auto leaf_guard = bpm_->WritePage(fa_page->ValueAt(son_id));
      leaf_page = leaf_guard.template AsMut<LeafPage>();
      vector<KeyType> leaf_key(leaf_max_size_ * 2);
      vector<ValueType> leaf_rid(leaf_max_size_ * 2);
      if (son_id >= 1) {
        auto sibling_guard = bpm_->WritePage(fa_page->ValueAt(son_id - 1));
        auto sibling_page = sibling_guard.template AsMut<LeafPage>();
        auto sibling_size = sibling_page->GetSize();
        for (int i = 0; i < sibling_size; ++i) {
          leaf_key[i] = sibling_page->KeyAt(i);
          leaf_rid[i] = sibling_page->RidAt(i);
        }
        for (int i = 0; i < pos; ++i) {
          leaf_key[i + sibling_size] = leaf_page->KeyAt(i);
          leaf_rid[i + sibling_size] = leaf_page->RidAt(i);
        }
        for (int i = pos + 1; i < size; ++i) {
          leaf_key[i + sibling_size - 1] = leaf_page->KeyAt(i);
          leaf_rid[i + sibling_size - 1] = leaf_page->RidAt(i);
        }
        size += sibling_size - 1;
        if (size >= 2 * leaf_page->GetMinSize()) {
          int left_size = size / 2;
          int right_size = size - left_size;
          sibling_page->SetSize(left_size);
          for (int i = 0; i < left_size; ++i) {
            sibling_page->SetKeyAt(i, leaf_key[i]);
            sibling_page->SetRidAt(i, leaf_rid[i]);
          }
          leaf_page->SetSize(right_size);
          for (int i = 0; i < right_size; ++i) {
            leaf_page->SetKeyAt(i, leaf_key[i + left_size]);
            leaf_page->SetRidAt(i, leaf_rid[i + left_size]);
          }
          fa_page->SetKeyAt(son_id, leaf_key[left_size]);
          return;
        }
        sibling_page->SetSize(size);
        for (int i = 0; i < size; ++i) {
          sibling_page->SetKeyAt(i, leaf_key[i]);
          sibling_page->SetRidAt(i, leaf_rid[i]);
        }
        sibling_page->SetNextPageId(leaf_page->GetNextPageId());
        --son_id;
      } else {
        auto sibling_guard = bpm_->WritePage(fa_page->ValueAt(son_id + 1));
        auto sibling_page = sibling_guard.template AsMut<LeafPage>();
        auto sibling_size = sibling_page->GetSize();
        for (int i = 0; i < pos; ++i) {
          leaf_key[i] = leaf_page->KeyAt(i);
          leaf_rid[i] = leaf_page->RidAt(i);
        }
        for (int i = pos + 1; i < size; ++i) {
          leaf_key[i - 1] = leaf_page->KeyAt(i);
          leaf_rid[i - 1] = leaf_page->RidAt(i);
        }
        for (int i = 0; i < sibling_size; ++i) {
          leaf_key[i + size - 1] = sibling_page->KeyAt(i);
          leaf_rid[i + size - 1] = sibling_page->RidAt(i);
        }
        size += sibling_size - 1;
        if (size >= 2 * leaf_page->GetMinSize()) {
          int left_size = size / 2;
          int right_size = size - left_size;
          leaf_page->SetSize(left_size);
          for (int i = 0; i < left_size; ++i) {
            leaf_page->SetKeyAt(i, leaf_key[i]);
            leaf_page->SetRidAt(i, leaf_rid[i]);
          }
          sibling_page->SetSize(right_size);
          for (int i = 0; i < right_size; ++i) {
            sibling_page->SetKeyAt(i, leaf_key[i + left_size]);
            sibling_page->SetRidAt(i, leaf_rid[i + left_size]);
          }
          fa_page->SetKeyAt(son_id + 1, leaf_key[left_size]);
          return;
        }
        leaf_page->SetSize(size);
        for (int i = 0; i < size; ++i) {
          leaf_page->SetKeyAt(i, leaf_key[i]);
          leaf_page->SetRidAt(i, leaf_rid[i]);
        }
        leaf_page->SetNextPageId(sibling_page->GetNextPageId());
      }
      // merge page
      bpm_->DeletePage(fa_page->ValueAt(son_id + 1));
      auto remove_pos = son_id + 1;
      ctx.which_son_.pop_back();
      while (true) {
        if (ctx.write_set_.size() == 1) {
          auto root_page = ctx.write_set_.begin()->AsMut<InternalPage>();
          auto root_size = root_page->GetSize();
          if (root_size == 2) {
            bpm_->WritePage(header_page_id_).AsMut<BPlusTreeHeaderPage>()->root_page_id_ = root_page->ValueAt(0);
            bpm_->DeletePage(ctx.root_page_id_);
          } else {
            for (int i = remove_pos + 1; i < root_size; ++i) {
              root_page->SetKeyAt(i - 1, root_page->KeyAt(i));
              root_page->SetValueAt(i - 1, root_page->ValueAt(i));
            }
            root_page->ChangeSizeBy(-1);
          }
          return;
        }
        auto cur_page = (--ctx.write_set_.end())->AsMut<InternalPage>();
        auto cur_size = cur_page->GetSize();
        auto cur_pos = *--ctx.which_son_.end();
        if (cur_size > cur_page->GetMinSize()) {
          for (int i = remove_pos + 1; i < cur_size; ++i) {
            cur_page->SetKeyAt(i - 1, cur_page->KeyAt(i));
            cur_page->SetValueAt(i - 1, cur_page->ValueAt(i));
          }
          cur_page->ChangeSizeBy(-1);
          return;
        }
        fa_page = (--(--ctx.write_set_.end()))->AsMut<InternalPage>();
        vector<KeyType> internal_key_vec(internal_max_size_ * 2);
        vector<int> internal_page_vec(internal_max_size_ * 2, 0);
        if (cur_pos >= 1) {
          auto sibling_guard = bpm_->WritePage(fa_page->ValueAt(cur_pos - 1));
          auto sibling_page = sibling_guard.template AsMut<InternalPage>();
          auto sibling_size = sibling_page->GetSize();
          for (int i = 0; i < sibling_size; ++i) {
            internal_key_vec[i] = sibling_page->KeyAt(i);
            internal_page_vec[i] = sibling_page->ValueAt(i);
          }
          for (int i = 0; i < remove_pos; ++i) {
            internal_key_vec[i + sibling_size] = cur_page->KeyAt(i);
            internal_page_vec[i + sibling_size] = cur_page->ValueAt(i);
          }
          for (int i = remove_pos + 1; i < cur_size; ++i) {
            internal_key_vec[i + sibling_size - 1] = cur_page->KeyAt(i);
            internal_page_vec[i + sibling_size - 1] = cur_page->ValueAt(i);
          }
          cur_size += sibling_size - 1;
          if (cur_size >= 2 * cur_page->GetMinSize()) {
            int left_size = cur_size / 2;
            int right_size = cur_size - left_size;
            sibling_page->SetSize(left_size);
            for (int i = 0; i < left_size; ++i) {
              sibling_page->SetKeyAt(i, internal_key_vec[i]);
              sibling_page->SetValueAt(i, internal_page_vec[i]);
            }
            cur_page->SetSize(right_size);
            for (int i = 0; i < right_size; ++i) {
              cur_page->SetKeyAt(i, internal_key_vec[i + left_size]);
              cur_page->SetValueAt(i, internal_page_vec[i + left_size]);
            }
            fa_page->SetKeyAt(cur_pos, internal_key_vec[left_size]);
            return;
          }
          sibling_page->SetSize(cur_size);
          for (int i = 0; i < cur_size; ++i) {
            sibling_page->SetKeyAt(i, internal_key_vec[i]);
            sibling_page->SetValueAt(i, internal_page_vec[i]);
          }
          --cur_pos;
        } else {
          auto sibling_guard = bpm_->WritePage(fa_page->ValueAt(cur_pos + 1));
          auto sibling_page = sibling_guard.template AsMut<InternalPage>();
          auto sibling_size = sibling_page->GetSize();
          for (int i = 0; i < remove_pos; ++i) {
            internal_key_vec[i] = cur_page->KeyAt(i);
            internal_page_vec[i] = cur_page->ValueAt(i);
          }
          for (int i = remove_pos + 1; i < cur_size; ++i) {
            internal_key_vec[i - 1] = cur_page->KeyAt(i);
            internal_page_vec[i - 1] = cur_page->ValueAt(i);
          }
          for (int i = 0; i < sibling_size; ++i) {
            internal_key_vec[i + cur_size - 1] = sibling_page->KeyAt(i);
            internal_page_vec[i + cur_size - 1] = sibling_page->ValueAt(i);
          }
          cur_size += sibling_size - 1;
          if (cur_size >= 2 * cur_page->GetMinSize()) {
            int left_size = cur_size / 2;
            int right_size = cur_size - left_size;
            cur_page->SetSize(left_size);
            for (int i = 0; i < left_size; ++i) {
              cur_page->SetKeyAt(i, internal_key_vec[i]);
              cur_page->SetValueAt(i, internal_page_vec[i]);
            }
            sibling_page->SetSize(right_size);
            for (int i = 0; i < right_size; ++i) {
              sibling_page->SetKeyAt(i, internal_key_vec[i + left_size]);
              sibling_page->SetValueAt(i, internal_page_vec[i + left_size]);
            }
            fa_page->SetKeyAt(cur_pos + 1, internal_key_vec[left_size]);
            return;
          }
          cur_page->SetSize(cur_size);
          for (int i = 0; i < cur_size; ++i) {
            cur_page->SetKeyAt(i, internal_key_vec[i]);
            cur_page->SetValueAt(i, internal_page_vec[i]);
          }
        }
        bpm_->DeletePage(fa_page->ValueAt(cur_pos + 1));
        remove_pos = cur_pos + 1;
        ctx.write_set_.pop_back();
        ctx.which_son_.pop_back();
      }
    }
    auto internal_page = it->As<InternalPage>();
    int pos = size;
    for (int i = 1; i < size; ++i) {
      if (comparator_(key, internal_page->KeyAt(i)) < 0) {
        pos = i;
        break;
      }
    }
    --pos;
    ctx.which_son_.push_back(pos);
    ctx.write_set_.emplace_back(bpm_->WritePage(internal_page->ValueAt(pos)));
  }
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::GetAll(vector<ValueType> *result) {
  Context ctx;
  ctx.root_page_id_ = GetRootPageId();
  if (ctx.root_page_id_ == -1) {
    return;
  }
  ctx.read_set_.emplace_back(bpm_->ReadPage(ctx.root_page_id_));
  while (true) {
    auto it = --ctx.read_set_.end();
    auto page = it->As<BPlusTreePage>();
    auto size = page->GetSize();
    if (page->IsLeafPage()) {
      auto leaf_page = it->As<LeafPage>();
      while (true) {
        for (int i = 0; i < size; ++i) {
          result->push_back(leaf_page->RidAt(i));
        }
        if (leaf_page->GetNextPageId() == -1) {
          return;
        }
        ctx.read_set_.pop_back();
        ctx.read_set_.emplace_back(bpm_->ReadPage(leaf_page->GetNextPageId()));
        it = --ctx.read_set_.end();
        leaf_page = it->As<LeafPage>();
        size = leaf_page->GetSize();
      }
    }
    auto internal_page = it->As<InternalPage>();
    ctx.read_set_.emplace_back(bpm_->ReadPage(internal_page->ValueAt(0)));
  }
}

INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Clean() {
  bpm_->Clean();
  header_page_id_ = bpm_->NewPage();
  WritePageGuard guard = bpm_->WritePage(header_page_id_);
  auto root_page = guard.AsMut<BPlusTreeHeaderPage>();
  root_page->root_page_id_ = -1;
  root_page->page_cnt_ = 0;
  root_page->size_ = 0;
}

/**
 * @return Page id of the root of this tree
 *
 * You may want to implement this while implementing Task #3.
 */
INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetRootPageId() const -> int {
  return bpm_->ReadPage(header_page_id_).As<BPlusTreeHeaderPage>()->root_page_id_;
}

INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetSize() const -> int {
  return bpm_->ReadPage(header_page_id_).As<BPlusTreeHeaderPage>()->size_;
}

template class BPlusTree<Key, int, Comparator, RoughComparator>;
template class BPlusTree<array<char, 20>, User, UserComparator, UserComparator>;
template class BPlusTree<array<char, 20>, int, TrainComparator, TrainComparator>;
template class BPlusTree<array<unsigned int, 10>, int, StationComparator, StationComparator>;
template class BPlusTree<StationTrain, int, StationTrainComparator, StationIDComparator>;
template class BPlusTree<BuyInfo, Order, BuyInfoComparator, RoughBuyInfoComparator>;
template class BPlusTree<int, Order, TimeComparator, TimeComparator>;

}