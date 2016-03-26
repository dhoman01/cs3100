#include "Task.hpp"

struct Event {
  enum Type { create, cpu_done, io_done };
  TaskHolder::Task task;
  double time;
}

struct C {
  bool operator()(T const &a, T const &b) { return a.time < b.time; }
}

using EventQueue = std::priority_queue<Event, std::vector<Event>, C>;
