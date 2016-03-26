#ifndef READY_QUEUE_HPP
#define READY_QUEUE_HPP

#include "Task.hpp"
#include <algorithm>
#include <queue>

class ReadyQueue {
private:
  using RQ = std::queue<TaskHolder::Task>;
  RQ readyQueue;

public:
  ReadyQueue(){};
  void push(TaskHolder::Task task) { readyQueue.push(task); }
  TaskHolder::Task pop() {
    auto task = readyQueue.front();
    readyQueue.pop();
    // TODO: Do required stats gathering
    return task;
  }
  bool empty() { return readyQueue.empty(); }
  int size() { return readyQueue.size(); }
  void toString() {
    RQ temp = readyQueue;
    while (!temp.empty()) {
      auto task = temp.front();
      temp.pop();
      std::cout << "Task.size() " << task.size() << std::endl;
      while (!task.empty()) {
        auto item = task.front();
        task.pop();
        std::cout << (item.cpu ? "CPU" : "IO") << " " << item.duration
                  << "s on " << item.resource << std::endl;
      }
      std::cout << "\n" << std::endl;
    }
  }
};

#endif
