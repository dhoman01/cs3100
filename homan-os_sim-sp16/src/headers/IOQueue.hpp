#ifndef IO_QUEUE_HPP
#define IO_QUEUE_HPP

#include "Task.cpp"
#include <queue>

using IOQ = std::queue<Task>;

class IOQueue {
private:
  IOQ ioQueue;

public:
  void push(Task task) { ioQueue.push(task); }
  Task pop() {
    auto task = ioQueue.front();
    ioQueue.pop();
    // TODO: Do required stats gathering
    return task;
  }
};

#endif
