#ifndef MEMORYRIVER_HPP
#define MEMORYRIVER_HPP

#include <fstream>
#include <iostream>

template<class T>
class MemoryRiver {
private:
  std::fstream file_;
  std::string file_name_;
  const size_t sizeofT_ = sizeof(T);
public:
  MemoryRiver() = default;

  MemoryRiver(const std::string& file_name_) : file_name_(file_name_) {}
  ~MemoryRiver() {
    if (!file_.is_open()) {
      file_.open(file_name_, std::ios::in | std::ios::out | std::ios::binary);
    }
    file_.close();
  }
  void Initialise(std::string FN = "") {
    if (file_.is_open()) {
      file_.close();
    }
    if (FN != "") file_name_ = FN;
#ifdef FileClear
    file_.open(file_name_, std::ios::out);
    size_t tmp = 0;
    for (size_t i = 0; i < info_len; ++i) {
      file_.write(reinterpret_cast<char *>(&tmp), sizeof(size_t));
    }
    file_.close();
#else
    file_.open(file_name_, std::ios::in);
    if (!file_) {
      file_.open(file_name_, std::ios::out);
    }
    file_.close();
#endif
  }

  void Update(T &t, const size_t index) {
    if (!file_.is_open()) {
      file_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
    }
    size_t tmp = index * sizeofT_;
    if (file_.tellp() != tmp) {
      file_.seekp(tmp);
    }
    file_.write(reinterpret_cast<char *>(&t), sizeofT_);
  }

  void Read(T &t, const size_t index) {
    if (!file_.is_open()) {
      file_.open(file_name_, std::ios::binary | std::ios::in | std::ios::out);
    }
    size_t tmp = index * sizeofT_;
    if (file_.tellg() != tmp) {
      file_.seekg(tmp);
    }
    file_.read(reinterpret_cast<char *>(&t), sizeofT_);
  }
};

#endif // MEMORYRIVER_HPP