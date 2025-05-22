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

  sjtu::BPlusTree<sjtu::Key, int, sjtu::Comparator, sjtu::RoughComparator> tree("sword_index");
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
  return 0;
}