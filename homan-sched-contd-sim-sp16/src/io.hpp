#ifndef IO_HPP
#define IO_HPP

#include "task.hpp"
#include <queue>
#include <vector>

template <typename T> struct IO {
  IO() : to_do(), r_id(-1), busy(false){};
  IO(int r) : to_do(), r_id(r), busy(false){};
  std::queue<std::shared_ptr<T>> to_do;
  int r_id;
  bool busy;
  int size() { return to_do.size(); }
  bool empty() { return to_do.empty(); }
  auto pop() {
    auto task = to_do.front();
    to_do.pop();
    return task;
  }
  void push(std::shared_ptr<T> t) { to_do.push(t); }
};

template <typename T> using io_devices = std::vector<IO<T> *>;

#endif
