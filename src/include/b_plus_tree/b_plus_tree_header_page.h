#ifndef B_PLUS_TREE_HEADER_PAGE_H
#define B_PLUS_TREE_HEADER_PAGE_H

namespace sjtu {

  /**
   * The header page is just used to retrieve the root page,
   * preventing potential race condition under concurrent environment.
   */
  class BPlusTreeHeaderPage {
  public:
    // Delete all constructor / destructor to ensure memory safety
    BPlusTreeHeaderPage() = delete;
    BPlusTreeHeaderPage(const BPlusTreeHeaderPage &other) = delete;

    int root_page_id_;
    int page_cnt_;
  };

}

#endif