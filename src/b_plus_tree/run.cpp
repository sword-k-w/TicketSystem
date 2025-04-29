#include "b_plus_tree/b_plus_tree.h"
#include <string>
#include "comparator.h"

int main() {
  // freopen("input.txt", "r", stdin);
  // freopen("output.txt", "w", stdout);

  // std::ios::sync_with_stdio(false);
  // std::cin.tie(nullptr);

  int n;
  std::cin >> n;

  auto disk_manager = std::make_shared<sjtu::DiskManager>("index");
  auto *buffer_pool_manager = new sjtu::BufferPoolManager(100, disk_manager, 10);
  int page_id = buffer_pool_manager->NewPage();
  Comparator comparator;
  RoughComparator rough_comparator;

  sjtu::BPlusTree<Key, int, Comparator, RoughComparator> tree("tester", page_id, buffer_pool_manager, comparator, rough_comparator);
  while (n--) {
    std::string type;
    std::string key;
    int value;
    std::cin >> type >> key;
    size_t size = key.size();
    for (size_t i = 0; i < size; ++i) {
      assert(std::isprint(key[i]));
    }
    assert(size > 0 && size <= 64);
    if (size == 64) {
      while (true);
    }
    if (type == "insert") {
      std::cin >> value;
      Key real_key(key, value);
      tree.Insert(real_key, value);
    } else if (type == "delete") {
      std::cin >> value;
      Key real_key(key, value);
      tree.Remove(real_key);
    } else {
      assert(type == "find");
      std::vector<int> result;
      Key real_key(key);
      tree.GetAllValue(real_key, &result);
      if (result.empty()) {
        std::cout << "null\n";
      } else {
        size_t size = result.size();
        for (size_t i = 0; i < size; ++i) {
          std::cout << result[i] << " ";
          if (i && result[i] <= result[i - 1]) {
            exit(1);
          }
        }
        std::cout << '\n';
      }
    }
  }
  buffer_pool_manager->WritePage(page_id).AsMut<sjtu::BPlusTreeHeaderPage>()->page_cnt_ = buffer_pool_manager->PageCnt();
  buffer_pool_manager->FlushAllPages();
  delete buffer_pool_manager;
  return 0;
}