#ifndef OS_HPP
#define OS_HPP

#include "CPU.hpp"
#include "IO.hpp"
#include "IOQueue.hpp"
#include "ReadyQueue.hpp"
#include "Task.hpp"
#include <algorithm>
#include <memory>

struct OS {
  void startFIFO(int cores, int number_io, int number_task) {
    EventQueue eq;
    ReadyQueue rq;
    TaskHolder::number_io = number_io;
    std::vector<IOQueue> io_devices;
    for (int i = 1; i <= number_io; i++) {
      io_devices.push_back(IOQueue(i));
    }

    while (!eq.empty()) {
      auto event = eq.front();
      eq.pop();
    }
  }
  void init(int cores, int number_io, int number_task) {
    CPU cpu;
    cpu.cores = cores;
    IO io;
    TaskHolder::number_io = number_io;
    ReadyQueue rq;
    std::vector<IOQueue> io_devices;
    bool io_complete = false;
    for (int i = 1; i <= number_io; i++) {
      io_devices.push_back(IOQueue(i));
    }

    for (int i = 0; i < number_task; i++) {
      rq.push(TaskHolder::make_task());
    }

    // rq.toString();

    while (!rq.empty() || !io_complete) {

      for (int i = 0; i < cores && !rq.empty(); i++) {
        auto task = rq.pop();
        task = cpu.do_task(task);
        if (task.size() > 0 && task.front().cpu) {
          rq.push(task);
        } else if (task.size() > 0) {
          io_devices[task.front().resource - 1].push(task);
        }
      }
      int empty_io = 0;
      std::for_each(io_devices.begin(), io_devices.end(), [&](auto &iod) {
        if (iod.size() > 0) {
          auto task = iod.pop();
          task = io.do_task(task);
          if (task.size() > 0 && task.front().cpu) {
            rq.push(task);
          } else if (task.size() > 0) {
            io_devices[task.front().resource - 1].push(task);
          }
        }
        if (iod.size() == 0)
          empty_io++;
      });
      io_complete = (empty_io == number_io);
    }
  }
};

#endif
