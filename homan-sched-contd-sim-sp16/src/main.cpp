#include "cache.hpp"
#include "memorysim.hpp"

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <typeinfo>

template <typename T>
void write_sim_data_to_file(MEMORY_SIM::SIMULATOR<T> &simulator, int i);

// Controls the number of tasks created
const int N = 2048;

// Controls the number of trials
const int T = 10;

template <typename T>
void printElement(char align, T t, const int &width, char fill) {
  std::cout << (align == 'L' ? std::left : std::right) << std::setw(width)
            << std::setfill(fill) << t;
}

int main() {
  for (int i = 0; i < T; i++) {
    MEMORY_SIM::SIMULATOR<MEMORY::CACHE> fifo_sim;
    MEMORY_SIM::SIMULATOR<MEMORY::MRU_CACHE> mru_sim;
    MEMORY_SIM::SIMULATOR<MEMORY::LRU_CACHE> lru_sim;
    MEMORY_SIM::SIMULATOR<MEMORY::SC_CACHE> sc_sim;
    fifo_sim.doSIM(8, .05, 6, N, 1);
    write_sim_data_to_file(fifo_sim, i);
    mru_sim.doSIM(8, .05, 6, N, 1);
    write_sim_data_to_file(mru_sim, i);
    lru_sim.doSIM(8, .05, 6, N, 1);
    write_sim_data_to_file(lru_sim, i);
    sc_sim.doSIM(8, .05, 6, N, 1);
    write_sim_data_to_file(sc_sim, i);
  }

  return EXIT_SUCCESS;
}

// This function writes each Event
// and Task from the trial
template <typename T>
void write_sim_data_to_file(MEMORY_SIM::SIMULATOR<T> &simulator, int i) {
  std::ofstream file;
  std::stringstream ss;
  ss << "tasks-" << typeid(simulator).name() << "-" << i << ".csv";
  file.open(ss.str());
  file << "Trial,Id,Time Created,Response Time,Time Completed,Latency"
       << std::endl;
  std::for_each(simulator.tasks.begin(), simulator.tasks.end(), [&](auto &t) {
    file << i << "," << t->id << "," << t->time_created << ","
         << t->response_time << "," << t->time_completed << ","
         << t->time_completed - t->time_created << std::endl;
  });
}
