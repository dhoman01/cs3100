#ifndef CPU_HPP
#define CPU_HPP

#include "Task.hpp"
#include <iostream>

struct CPU {
  int cores;
  const double CONTEXT_SWITCH_COST = .02;
  TaskHolder::Task do_task(TaskHolder::Task t) {
    cores--;
    auto task_item = t.front();
    t.pop();
    // TODO: do cpu stuff
    std::cout << "Doing " << task_item.duration << " work on CPU." << std::endl;
    cores++;
    return t;
  };
};

#endif
