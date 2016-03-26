#ifndef SIM_HPP
#define SIM_HPP

#include <algorithm>
#include <iostream>
#include <vector>

#include "cpu.hpp"
#include "event.hpp"
#include "io.hpp"
#include "task.hpp"

namespace SIM {

using done_list = std::vector<Event>;

event_queue eq;
done_list done;
ready_queue rq;
CPU cpu;
io_devices io_d;

void init(int cores, int context_switch, int number_io, int number_task) {
  eq.push(Event(10000, NULL, Event::Type::halt, -1, 0));
  cpu = CPU(cores, context_switch);
  N_IO = number_io;

  for (int i = 0; i < number_io; i++) {
    io_d.push_back(new IO(i + 1));
  }

  for (double i = 0.0; i < number_task; i++) {
    eq.push(Event(i * getRand(0.0, 5.0)));
  }
}

void event_create(double now) {
  auto task = make_task(now);
  if (cpu.cores > 0) {
    cpu.utilization += task.front_duration();
    eq.push(Event(now + task.front_duration() + cpu.context_switch, task,
                  Event::Type::cpu_done, 0, cpu.utilization));
    cpu.cores--;
  } else {
    rq.push(task);
  }
}

void do_io_task(Task &task, double now) {
  if (io_d[task.front_resource() - 1]->busy) {
    io_d[task.front_resource() - 1]->push(task);
  } else {
    eq.push(Event(now + task.front_duration(), task, Event::Type::io_done,
                  task.front_resource(), cpu.utilization));
    io_d[task.front_resource() - 1]->busy = true;
  }
}

void do_cpu_task(Task &task, double now) {
  if (cpu.cores > 0) {
    cpu.utilization += task.front_duration();
    eq.push(Event(now + task.front_duration(), task, Event::Type::cpu_done, 0,
                  cpu.utilization));
    cpu.cores--;
  } else {
    rq.push(task);
  }
}

void event_cpu_done(double now, Event &event) {
  Task &task = event.task;
  task.do_front(now);
  cpu.cores++;
  if (!task.empty()) {
    do_io_task(task, now);
  }
  if (!rq.empty()) {
    task = rq.front();
    rq.pop();
    cpu.utilization += task.front_duration();
    eq.push(Event(now + task.front_duration(), task, Event::Type::cpu_done, 0,
                  cpu.utilization));
    cpu.cores--;
  }
}

void event_io_done(double now, Event &event) {
  Task &task = event.task;
  auto r_id = task.front_resource();
  io_d[r_id - 1]->busy = false;
  task.do_front(now);
  if (!task.empty()) {
    do_cpu_task(task, now);
  }
  if (!io_d[r_id - 1]->empty()) {
    task = io_d[r_id - 1]->pop();
    eq.push(Event(now + task.front_duration(), task, Event::Type::io_done,
                  task.front_resource(), cpu.utilization));
    io_d[task.front_resource() - 1]->busy = true;
  }
}

void print_done_events(done_list done) {
  std::for_each(done.begin(), done.end(),
                [](auto event) { std::cout << event.toString() << std::endl; });
}

void doFIFO(int cores, int context_switch, int number_io, int number_task) {
  init(cores, context_switch, number_io, number_task);

  while (!eq.empty()) {
    auto event = eq.top();
    eq.pop();

    double now = event.time;

    switch (event.type) {
    case Event::Type::create:
      event_create(now);
      break;
    case Event::Type::cpu_done:
      event_cpu_done(now, event);
      break;
    case Event::Type::io_done:
      event_io_done(now, event);
      break;
    case Event::Type::halt:
      break;
    }

    if (event.type == Event::Type::halt)
      break;
    done.push_back(event);
  }
  // print_done_events(done);
};
}
#endif
