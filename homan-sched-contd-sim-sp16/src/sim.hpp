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

using done_list = std::vector<Event<Task>>;
using Tasks = std::vector<std::shared_ptr<Task>>;

// These variables control the spacing
// between tasks.
const double TASK_SPACE_LOW = 0.0;
const double TASK_SPACE_HIGH = 5.0;

event_queue<Task> eq;
done_list done;
ready_queue<Task> rq;
CPU cpu;
io_devices<Task> io_d;
Tasks tasks;

// Set up simulation
void init(int cores, double context_switch, int number_io, int number_task) {
  eq.push(Event<Task>(1000000000, NULL, Event<Task>::Type::halt, -1, 0));
  cpu = CPU(cores, context_switch);
  N_IO = number_io;

  for (int i = 0; i < number_io; i++) {
    io_d.push_back(new IO<Task>(i + 1));
  }

  for (double i = 0.0; i < number_task; i++) {
    eq.push(Event<Task>(i * getRand(TASK_SPACE_LOW, TASK_SPACE_HIGH)));
  }
}

// Event of type create
void event_create(double now) {
  auto task = make_task(now);
  tasks.push_back(task);
  if (cpu.cores > 0) {
    cpu.utilization += task->front_duration();
    eq.push(Event<Task>(now + task->front_duration() + cpu.context_switch, task,
                        Event<Task>::Type::cpu_done, 0, cpu.utilization));
    cpu.cores--;
  } else {
    rq.push(task);
  }
}

// do an IO task from CPU done event
void do_io_task(std::shared_ptr<Task> task, double now) {
  if (io_d[task->front_resource() - 1]->busy) {
    io_d[task->front_resource() - 1]->push(task);
  } else {
    eq.push(Event<Task>(now + task->front_duration(), task,
                        Event<Task>::Type::io_done, task->front_resource(),
                        cpu.utilization));
    io_d[task->front_resource() - 1]->busy = true;
  }
}

// do a CPU task from IO done event
void do_cpu_task(std::shared_ptr<Task> task, double now) {
  if (cpu.cores > 0) {
    cpu.utilization += task->front_duration();
    eq.push(Event<Task>(now + task->front_duration() + cpu.context_switch, task,
                        Event<Task>::Type::cpu_done, 0, cpu.utilization));
    cpu.cores--;
  } else {
    rq.push(task);
  }
}

// Event of type cpu_done
void event_cpu_done(double now, Event<Task> &event) {
  auto task = event.task;
  task->do_front(now);
  cpu.cores++;
  if (!task->empty()) {
    do_io_task(task, now);
  }
  if (!rq.empty()) {
    task = rq.front();
    rq.pop();
    cpu.utilization += task->front_duration();
    eq.push(Event<Task>(now + task->front_duration() + cpu.context_switch, task,
                        Event<Task>::Type::cpu_done, 0, cpu.utilization));
    cpu.cores--;
  }
}

// Event of type io_done
void event_io_done(double now, Event<Task> &event) {
  auto task = event.task;
  auto r_id = task->front_resource();
  io_d[r_id - 1]->busy = false;
  task->do_front(now);
  if (!task->empty()) {
    do_cpu_task(task, now);
  }
  if (!io_d[r_id - 1]->empty()) {
    task = io_d[r_id - 1]->pop();
    eq.push(Event<Task>(now + task->front_duration(), task,
                        Event<Task>::Type::io_done, task->front_resource(),
                        cpu.utilization));
    io_d[task->front_resource() - 1]->busy = true;
  }
}

// function for use in debugging
void print_done_events(done_list done) {
  std::for_each(done.begin(), done.end(), [](auto &event) {
    std::cout << event.toString() << std::endl;
  });
}

// Do the simulation
void doFIFO(int cores, double context_switch, int number_io, int number_task,
            int t_no) {
  trial_no = t_no;
  init(cores, context_switch, number_io, number_task);

  while (!eq.empty()) {
    // Get an event
    auto event = eq.top();
    eq.pop();

    // Set current time to the
    // time of the event
    double now = event.time;

    switch (event.type) {
    case Event<Task>::Type::create:
      event_create(now);
      break;
    case Event<Task>::Type::cpu_done:
      event_cpu_done(now, event);
      break;
    case Event<Task>::Type::io_done:
      event_io_done(now, event);
      break;
    case Event<Task>::Type::halt:
      break;
    }

    if (event.type == Event<Task>::Type::halt)
      break;
    done.push_back(event);
  }
  // print_done_events(done);
};
}

#endif
