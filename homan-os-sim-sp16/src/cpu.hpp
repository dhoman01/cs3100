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

using ready_queue = std::queue<Task>;

#endif
