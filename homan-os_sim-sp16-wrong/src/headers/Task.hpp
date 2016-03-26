#ifndef TASK_HPP
#define TASK_HPP

#include <queue>
#include <random>

namespace TaskHolder {
struct TaskItem {
  bool cpu;
  double duration;
  int resource;
};

using Task = std::queue<TaskItem>;

const double MIX = 0.5;

const double CPU_BOUND_LOWER_CPU = 3.0;
const double CPU_BOUND_UPPER_CPU = 15.0;
const double IO_BOUND_LOWER_CPU = 0.5;
const double IO_BOUND_UPPER_CPU = 5.0;

const double CPU_BOUND_LOWER_IO = 3.0;
const double CPU_BOUND_UPPER_IO = 15.0;
const double IO_BOUND_LOWER_IO = 0.5;
const double IO_BOUND_UPPER_IO = 5.0;

const double CPU_BOUND_CONTINUE = 0.75;
const double IO_BOUND_CONTINUE = 0.75;

int number_io;

double getNormal(double l, double u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::normal_distribution<double> d(l, u);
  return d(mt);
}

double getRand(double l, double u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::uniform_real_distribution<> d(l, u);
  return d(mt);
}

int getRand(int l, int u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::uniform_int_distribution<> i(l, u);
  return i(mt);
}

TaskItem make_cputask(double l, double u) {
  TaskItem task_item;
  task_item.cpu = true;
  task_item.duration = getRand(l, u);
  task_item.resource = 0;
  return task_item;
}

TaskItem make_iotask(double l, double u) {
  TaskItem task_item;
  task_item.cpu = false;
  task_item.duration = getRand(l, u);
  task_item.resource = getRand(1, number_io);
  return task_item;
}

Task make_cpu_bound() {
  Task result;
  int limit = 0;
  result.push(make_cputask(CPU_BOUND_LOWER_CPU, CPU_BOUND_UPPER_CPU));
  while (CPU_BOUND_CONTINUE > getNormal(0.0, 1.0) && ++limit < 50) {
    if (MIX < getRand(0.0, 1.0))
      result.push(make_cputask(CPU_BOUND_LOWER_CPU, CPU_BOUND_UPPER_CPU));
    else
      result.push(make_iotask(CPU_BOUND_LOWER_IO, CPU_BOUND_UPPER_IO));
  }

  return result;
}

Task make_io_bound() {
  Task result;
  int limit = 0;
  result.push(make_cputask(IO_BOUND_LOWER_CPU, IO_BOUND_UPPER_CPU));
  while (IO_BOUND_CONTINUE > getNormal(0.0, 1.0) && ++limit < 50) {
    if (MIX < getRand(0.0, 1.0))
      result.push(make_cputask(IO_BOUND_LOWER_CPU, IO_BOUND_UPPER_CPU));
    else
      result.push(make_iotask(IO_BOUND_LOWER_IO, IO_BOUND_UPPER_IO));
  }

  return result;
}

Task make_task() {
  if (MIX < getRand(0.0, 1.0))
    return make_cpu_bound();
  else
    return make_io_bound();
}
};

#endif
