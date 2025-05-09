#include "b_plus_tree/b_plus_tree.h"
#include <string>
#include "comparator.h"

int main() {
  // freopen("input.txt", "r", stdin);
  // freopen("output.txt", "w", stdout);

  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n;
  std::cin >> n;

  auto disk_manager = std::make_shared<sjtu::DiskManager>("sword_index");
  auto *buffer_pool_manager = new sjtu::BufferPoolManager(2000, disk_manager, 10);
  int page_id = buffer_pool_manager->NewPage();
  sjtu::Comparator comparator;
  sjtu::RoughComparator rough_comparator;

  sjtu::BPlusTree<sjtu::Key, int, sjtu::Comparator, sjtu::RoughComparator> tree("tester", page_id, buffer_pool_manager, comparator, rough_comparator);
  while (n--) {
    std::string type;
    std::string key;
    int value;
    std::cin >> type >> key;
    if (type == "insert") {
      std::cin >> value;
      sjtu::Key real_key(key, value);
      tree.Insert(real_key, value);
    } else if (type == "delete") {
      std::cin >> value;
      sjtu::Key real_key(key, value);
      tree.Remove(real_key);
    } else {
      sjtu::vector<int> result;
      sjtu::Key real_key(key);
      tree.GetAllValue(real_key, &result);
      if (result.empty()) {
        std::cout << "null\n";
      } else {
        size_t size = result.size();
        for (size_t i = 0; i < size; ++i) {
          std::cout << result[i] << " ";
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