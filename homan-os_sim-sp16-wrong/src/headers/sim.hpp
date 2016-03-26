#ifndef SIM_HPP
#define SIM_HPP

#include <queue>
#include <vector>

struct TaskItem {
  enum class Type { cpu, io };
  TaskItem() : duration(0), resource(-1), type(Type::cpu), completed(-1){};
  TaskItem(double d, int r, Type t)
      : duration(d), resource(r), type(t), completed(-1){};
  double duration;
  double completed;
  int resource;
  Type type;
};

struct Task {
  Task()
      : to_do(), done(), time_created(-1), response_time(-1),
        time_completed(-1){};
  Task(double now)
      : to_do(), done(), time_created(now), response_time(-1),
        time_completed(-1){};
  std::queue<TaskItem> to_do;
  std::vector<TaskItem> done;
  double time_created;
  double response_time;
  double time_completed;
  void do_front(double now) {
    auto item = to_do.front();
    to_do.pop();
    item.completed = now;
    done.push_back(item);
  }
  bool empty() { return to_do.empty(); }
  int size() { return to_do.size(); }
};

struct Event {
  enum class Type { create, cpu_done, io_done };
  Event() : time(-1), task(), type(), resource_used(-1){};
  Event(double now, Task t, Type ty, int r)
      : time(now), task(task), type(ty), resource_used(r){};
  double time;
  Task task;
  Type type;
  int resource_used;
};

struct CPU {
  CPU() : cores(0), utilization(-1), context_switch(-1){};
  CPU(int c, int cs) : cores(c), utilization(0), context_switch(cs){};
  int cores;
  double utilization;
  double context_switch;
};

struct IO {
  IO() : to_do(), r_id(-1), busy(false){};
  IO(int r) : to_do(), r_id(r), busy(false){};
  std::queue<Task> to_do;
  int r_id;
  bool busy;
};

struct C {
  bool operator()(T const &a, T const &b) { return a.time < b.time; }
};

using event_queue = std::priority_queue<Event, std::vector<Event>, C>;
using io_devices = std::vector<IO *>;

void doFIFO(int cores, int context_switch, int number_io, int number_task) {
  event_queue eq;
  io_devices io_d;
  for (int i = 0; i < number_io; i++) {
    io_d.push_back(new IO(i + 1));
  }
};

#endif
