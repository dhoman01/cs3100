#ifndef TASK_HPP
#define TASK_HPP

#include <memory>
#include <vector>

struct TaskItem {
  class enum Type { cpu, io };
  Type type;
  double duration;
  int resource;
};

using Task = std::vector<TaskItem> v;
std::unique_ptr<Task> make_task();

#endif
