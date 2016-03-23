#include "Task.hpp"
#include <random>

namespace {

double getRand(double l, double u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::uniform_real_distribution d(l, u);
  return d(mt);
}

int getIntRand(int l, int u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::uniform_int_distribution i(l, u);
  return i(mt);
}

const int IO_RESOURCES = {1, 2, 3, 4}

const double MIX = 0.5;

const double CPU_BOUND_LOWER_CPU = 3.0;
const double CPU_BOUND_UPPER_CPU = 15.0;
const double IO_BOUND_LOWER_CPU = 0.5;
const double IO_BOUND_UPPER_CPU = 5.0;

const double CPU_BOUND_LOWER_IO = 3.0;
const double CPU_BOUND_UPPER_IO = 15.0;
const double IO_BOUND_LOWER_IO = 0.5;
const double IO_BOUND_UPPER_IO = 5.0;

const double CPU_BOUND_CONTINUE = 0.3;
const double IO_BOUND_CONTINUE = 0.4;
}

TaskItem make_cputask() {
  return (Type::cpu, getRand(CPU_BOUND_LOWER_CPU, CPU_BOUND_UPPER_CPU), 0);
}

TaskItem make_iotask() {
  return (Type::io, getRand(IO_BOUND_LOWER_IO, IO_BOUND_UPPER_IO),
          getIntRand(1, 4));
}

std::unique_ptr<Task> make_task(int n) {
  auto result = std::make_unique<Task>();
  result->push(make_cputask());
  for (int i = 0; i < n; i++) {
    if (MIX < getRand(0.0, 1.0))
      result->push(make_cputask());
    else
      result->push(make_iotask());
  }

  return result;
}
