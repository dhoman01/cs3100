#ifndef IO_HPP
#define IO_HPP

#include "task.hpp"
#include <queue>
#include <vector>

struct IO {
  IO() : to_do(), r_id(-1), busy(false){};
  IO(int r) : to_do(), r_id(r), busy(false){};
  std::queue<Task> to_do;
  int r_id;
  bool busy;
  int size() { return to_do.size(); }
  bool empty() { return to_do.empty(); }
  Task pop() {
    auto task = to_do.front();
    to_do.pop();
    return task;
  }
  void push(Task t) { to_do.push(t); }
};

using io_devices = std::vector<IO *>;

#endif
