#ifndef READY_QUEUE_HPP
#define READY_QUEUE_HPP

#include "Task.cpp"
#include <queue>

using RQ = std::queue<Task>;

class ReadyQueue {
private:
  RQ readyQueue;

public:
  ReadyQueue(){};
  void push(Task task) { readyQueue.push(task); }
  Task pop() {
    auto task = readyQueue.front();
    readyQueue.pop();
    // TODO: Do required stats gathering
    return task;
  }
};

#endif
