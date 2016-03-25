#ifndef IO_HPP
#define IO_HPP

#include "Task.hpp"
#include <iostream>

struct IO {
  int number_io;
  TaskHolder::Task do_task(TaskHolder::Task t) {
    auto task_item = t.front();
    t.pop();
    // TODO: do IO stuff
    std::cout << "Doing " << task_item.duration
              << " amount of IO work on device " << task_item.resource
              << std::endl;
    return t;
  };
};

#endif
