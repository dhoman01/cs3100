#ifndef IO_QUEUE_HPP
#define IO_QUEUE_HPP

#include "Task.hpp"
#include <queue>

class IOQueue {
private:
  using IOQ = std::queue<TaskHolder::Task>;
  IOQ ioQueue;
  int r_id;

public:
  IOQueue(int id) { r_id = id; }
  void push(TaskHolder::Task task) { ioQueue.push(task); }
  TaskHolder::Task pop() {
    auto task = ioQueue.front();
    ioQueue.pop();
    // TODO: Do required stats gathering
    return task;
  }
  int size() { return ioQueue.size(); }
  bool empty() { return ioQueue.empty(); }
  int getRid() { return r_id; }
};

#endif
