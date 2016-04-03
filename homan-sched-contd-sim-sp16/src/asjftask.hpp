#ifndef ASJF_HPP
#define ASJF_HPP

#include "task.hpp"

namespace ASJF {
class ASJFTask : public Task {
public:
  ASJFTask() : Task(), mean_running_time(0){};
  ASJFTask(double now) : Task(now), mean_running_time(0){};
  ASJFTask(double now, std::string i) : Task(now, i), mean_running_time(0){};
  ASJFTask(const ASJFTask &t)
      : Task(t), mean_running_time(t.mean_running_time){};
  double mean_running_time;
  virtual void do_front(double now) {
    auto item = to_do.front();
    to_do.pop();
    if (response_time < 0 && item.type == TaskItem::Type::io)
      response_time = now - time_created;
    if (to_do.size() == 0) {
      time_completed = now;
    }
    mean_running_time += (mean_running_time + item.duration) / 2;
    item.completed = now;
    done.push_back(item);
  }
};

std::shared_ptr<ASJFTask> make_cpu_bound(double now) {
  std::stringstream ss;
  ss << "Task-" << task_id_pool << "-" << trial_no;
  std::shared_ptr<ASJFTask> result = std::make_unique<ASJFTask>(now, ss.str());
  task_id_pool++;
  int limit = 0;
  while ((CPU_BOUND_CONTINUE > getNormal(0.0, 1.0) && limit < 500) ||
         result->size() < MIN_TASK_LENGTH) {
    if (limit % 2 == 0)
      result->to_do.push(
          make_cputask(CPU_BOUND_LOWER_CPU, CPU_BOUND_UPPER_CPU));
    else
      result->to_do.push(make_iotask(CPU_BOUND_LOWER_IO, CPU_BOUND_UPPER_IO));
    limit++;
  }

  return result;
};

std::shared_ptr<ASJFTask> make_io_bound(double now) {
  std::stringstream ss;
  ss << "Task-" << task_id_pool << "-" << trial_no;
  std::shared_ptr<ASJFTask> result = std::make_unique<ASJFTask>(now, ss.str());
  task_id_pool++;
  int limit = 0;
  while ((IO_BOUND_CONTINUE > getNormal(0.0, 1.0) && limit < 500) ||
         result->size() < MIN_TASK_LENGTH) {
    if (limit % 2 == 0)
      result->to_do.push(make_cputask(IO_BOUND_LOWER_CPU, IO_BOUND_UPPER_CPU));
    else
      result->to_do.push(make_iotask(IO_BOUND_LOWER_IO, IO_BOUND_UPPER_IO));
    limit++;
  }

  return result;
};

std::shared_ptr<ASJFTask> make_task(double now) {
  if (MIX < getRand(0.0, 1.0))
    return make_cpu_bound(now);
  else
    return make_io_bound(now);
};
}

#endif
