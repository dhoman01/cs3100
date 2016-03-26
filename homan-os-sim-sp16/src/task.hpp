#ifndef TASK_HPP
#define TASK_HPP

#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <vector>

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

static int task_id_pool = 1;

struct TaskItem {
  enum class Type { cpu, io };
  TaskItem() : duration(0), resource(-1), type(Type::cpu), completed(-1){};
  TaskItem(double d, int r, Type t)
      : duration(d), resource(r), type(t), completed(-1){};
  double duration;
  double completed;
  int resource;
  Type type;
  std::string toString() {
    std::stringstream ss;
    ss << "TaskItem: " << std::endl;
    ss << "    duration: " << duration << std::endl;
    ss << "    resource: " << resource << std::endl;
    ss << "    completed: " << completed << std::endl;
    ss << "    type: " << (type == Type::cpu ? "CPU" : "IO") << std::endl;
    return ss.str();
  }
};

struct Task {
  Task()
      : to_do(), done(), time_created(-1), response_time(-1),
        time_completed(-1), id(-1){};
  Task(double now)
      : to_do(), done(), time_created(now), response_time(-1),
        time_completed(-1), id(-1){};
  Task(double now, int i)
      : to_do(), done(), time_created(now), response_time(-1),
        time_completed(-1), id(i){};
  Task(const Task &t)
      : to_do(t.to_do), done(t.done), time_created(t.time_created),
        response_time(t.response_time), time_completed(t.time_completed),
        id(t.id){};
  std::queue<TaskItem> to_do;
  std::vector<TaskItem> done;
  double time_created;
  double response_time;
  double time_completed;
  int id;
  void do_front(double now) {
    auto item = to_do.front();
    to_do.pop();
    if (response_time < 0 && item.type == TaskItem::Type::io)
      response_time = now - time_created;
    if (to_do.size() == 0) {
      time_completed = now;
    }
    item.completed = now;
    done.push_back(item);
  }
  bool empty() { return to_do.empty(); }
  int size() { return to_do.size(); }
  double front_duration() { return to_do.front().duration; }
  int front_resource() { return to_do.front().resource; }
  std::string toString() {
    std::stringstream ss;
    ss << "Task: " << id << std::endl;
    ss << "    time_created: " << time_created << std::endl;
    ss << "    response_time: " << response_time << std::endl;
    ss << "    time_completed: " << time_completed << std::endl;

    return ss.str();
  }
};

int N_IO = 1;

double getNormal(double l, double u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::normal_distribution<double> d(l, u);
  return d(mt);
};

double getRand(double l, double u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::uniform_real_distribution<> d(l, u);
  return d(mt);
};

int getRand(int l, int u) {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  std::uniform_int_distribution<> i(l, u);
  return i(mt);
};

TaskItem make_cputask(double l, double u) {
  TaskItem task_item(getRand(l, u), 0, TaskItem::Type::cpu);
  return task_item;
};

TaskItem make_iotask(double l, double u) {
  TaskItem task_item(getRand(l, u), getRand(1, N_IO), TaskItem::Type::io);
  return task_item;
};

Task make_cpu_bound(double now) {
  Task result(now, task_id_pool);
  task_id_pool++;
  int limit = 0;
  result.to_do.push(make_cputask(CPU_BOUND_LOWER_CPU, CPU_BOUND_UPPER_CPU));
  while (CPU_BOUND_CONTINUE > getNormal(0.0, 1.0) && limit++ < 50) {
    if (limit % 2 == 0)
      result.to_do.push(make_cputask(CPU_BOUND_LOWER_CPU, CPU_BOUND_UPPER_CPU));
    else
      result.to_do.push(make_iotask(CPU_BOUND_LOWER_IO, CPU_BOUND_UPPER_IO));
  }

  return result;
};

Task make_io_bound(double now) {
  Task result(now, task_id_pool);
  task_id_pool++;
  int limit = 0;
  result.to_do.push(make_cputask(IO_BOUND_LOWER_CPU, IO_BOUND_UPPER_CPU));
  while (IO_BOUND_CONTINUE > getNormal(0.0, 1.0) && limit++ < 50) {
    if (limit % 2 == 0)
      result.to_do.push(make_cputask(IO_BOUND_LOWER_CPU, IO_BOUND_UPPER_CPU));
    else
      result.to_do.push(make_iotask(IO_BOUND_LOWER_IO, IO_BOUND_UPPER_IO));
  }

  return result;
};

Task make_task(double now) {
  if (MIX < getRand(0.0, 1.0))
    return make_cpu_bound(now);
  else
    return make_io_bound(now);
};

#endif
