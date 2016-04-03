#ifndef EVENT_HPP
#define EVENT_HPP

#include "task.hpp"
#include <algorithm>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

template <typename T> struct Event {
  enum Type { create = 0, cpu_done = 1, io_done = 2, halt = 3 };
  std::string TypeNames[4] = {"CREATE", "CPU DONE", "IO DONE", "HALT"};
  Event() : time(-1), task(), type(), resource_used(-1){};
  Event(double now)
      : time(now), task(), type(Type::create), resource_used(-1){};
  Event(double now, std::shared_ptr<T> t, Type ty, int r, double u)
      : time(now), task(t), type(ty), resource_used(r), cpu_util(u){};
  double time;
  std::shared_ptr<T> task;
  Type type;
  double cpu_util;
  int resource_used;

  // toString for debugging
  std::string toString() {
    std::stringstream ss;
    ss << "Event: " << std::endl;
    ss << "    Time: " << time << std::endl;
    ss << "    Type: " << TypeNames[type] << std::endl;
    ss << "    CPU Util: " << cpu_util << std::endl;
    // ss << "    " << task->toString() << std::endl;
    // std::for_each(task->done.begin(), task->done.end(), [&ss](auto item) {
    //   ss << "        " << item.toString() << std::endl;
    // });
    return ss.str();
  }
};

struct C {
  template <typename T> bool operator()(Event<T> const &a, Event<T> const &b) {
    return a.time > b.time;
  }
};
template <typename T>
using event_queue = std::priority_queue<Event<T>, std::vector<Event<T>>, C>;

#endif
