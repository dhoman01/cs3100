#include "CPU.hpp"
#include "Task.hpp"

namespace {

const double CONTEXT_SWITCH_COST = .02;

Task do_task(Task t) {
  auto task_item = t.front();
  t.pop();
  // TODO: do cpu stuff

  return t;
}
}
