#ifndef RR_TASK_HPP
#define RR_TASK_HPP

#include "task.hpp"

namespace RR_TASK {
class RRTask : public Task {
public:
  RRTask() : Task(){};
  RRTask(double now) : Task(now){};
  RRTask(double now, std::string i) : Task(now, i){};
  RRTask(const RRTask &t) : Task(t){};
  virtual void do_front(double now, double amount) {
    auto item = to_do.front();
    if (item.type == TaskItem::Type::cpu) {
      item.duration = item.duration =
          item.duration - amount > 0 ? item.duration - amount : 0;
      if (item.duration > 0)
        return;
      to_do.pop();
      if (to_do.size() == 0) {
        time_completed = now;
      }
      item.completed = now;
      done.push_back(item);
    } else {
      to_do.pop();
      if (response_time < 0)
        response_time = now - time_created;
      if (to_do.size() == 0) {
        time_completed = now;
      }
      item.completed = now;
      done.push_back(item);
    }
  }
};

std::shared_ptr<RRTask> make_cpu_bound(double now) {
  std::stringstream ss;
  ss << "Task-" << task_id_pool << "-" << trial_no;
  std::shared_ptr<RRTask> result = std::make_unique<RRTask>(now, ss.str());
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

std::shared_ptr<RRTask> make_io_bound(double now) {
  std::stringstream ss;
  ss << "Task-" << task_id_pool << "-" << trial_no;
  std::shared_ptr<RRTask> result = std::make_unique<RRTask>(now, ss.str());
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

std::shared_ptr<RRTask> make_task(double now) {
  if (MIX < getRand(0.0, 1.0))
    return make_cpu_bound(now);
  else
    return make_io_bound(now);
};
}

#endif
