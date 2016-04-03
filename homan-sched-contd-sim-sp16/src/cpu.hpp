#ifndef CPU_HPP
#define CPU_HPP

#include "task.hpp"
#include <queue>

struct CPU {
  CPU() : cores(0), utilization(-1), context_switch(-1){};
  CPU(int c, double cs) : cores(c), utilization(0), context_switch(cs){};
  int cores;
  double utilization;
  double context_switch;
};

struct Comp {
  template <typename T> bool operator()(T const a, T const b) {
    return a->front_resource() > b->front_resource();
  }
};

struct ASJFComp {
  template <typename T> bool operator()(T const a, T const b) {
    return a->mean_running_time > b->mean_running_time;
  }
};

template <typename T> using ready_queue = std::queue<std::shared_ptr<T>>;
using sjf_queue = std::priority_queue<std::shared_ptr<Task>,
                                      std::vector<std::shared_ptr<Task>>, Comp>;
using asjf_queue =
    std::priority_queue<std::shared_ptr<ASJF::ASJFTask>,
                        std::vector<std::shared_ptr<ASJF::ASJFTask>>, ASJFComp>;

#endif
