#ifndef CPU_HPP
#define CPU_HPP

#include "Task.hpp"

class CPU {
private:
  int cores;

public:
  CPU(int n) { cores = n; }
  Task do_task(Task t);
  int getCores() { return cores; }
};

#endif
