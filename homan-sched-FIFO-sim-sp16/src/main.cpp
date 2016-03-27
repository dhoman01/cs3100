#include "sim.hpp"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

void write_data_to_file(int i);

// Controls the number of tasks created
const int N = 1024;

// Controls the number of trials conducted
const int T = 8;

template <typename T>
void printElement(char align, T t, const int &width, char fill) {
  std::cout << (align == 'L' ? std::left : std::right) << std::setw(width)
            << std::setfill(fill) << t;
}

int main() {
  // Vectors to hold results of trials
  std::vector<std::pair<int, double>> trial_avg_rt;
  std::vector<std::pair<int, double>> trial_avg_lat;
  std::vector<std::pair<int, double>> trial_thrpt;
  std::vector<std::pair<int, double>> trial_eff;
  std::vector<std::pair<int, double>> total_useful_work;

  for (int i = 0; i < T; i++) {
    // Do the simulation
    SIM::doFIFO(i + 1, .05, 6, N, i);

    // collect the data
    double sum_lat = 0.0;
    double sum_res = 0.0;
    double sum_task_time = 0.0;
    std::for_each(SIM::tasks.begin(), SIM::tasks.end(), [&](auto &t) {
      sum_lat += t->time_completed - t->time_created;
      sum_res += t->response_time;
      std::for_each(t->done.begin(), t->done.end(), [&](auto &item) {
        if (item.type == TaskItem::Type::cpu)
          sum_task_time += item.duration;
      });
    });
    trial_thrpt.push_back(
        std::make_pair((i + 1), (N / SIM::done.back().task->time_completed)));
    trial_eff.push_back(std::make_pair(
        (i + 1), (SIM::done.back().cpu_util) /
                     SIM::done.back().task->time_completed * (i + 1)));
    total_useful_work.push_back(std::make_pair((i + 1), sum_task_time));
    trial_avg_rt.push_back(std::make_pair((i + 1), sum_res / N));
    trial_avg_lat.push_back(std::make_pair((i + 1), sum_lat / N));

    write_data_to_file(i + 1);
  }

  // Print trial avgs to screen
  std::stringstream ss;
  ss << "Trial,Avg Response Item, Avg Latency,Throughput,Efficiency,Total "
        "Useful Work"
     << std::endl;
  printElement('L', "Trial", 8, ' ');
  std::cout << "|";
  printElement('R', "Avg Resp.", 15, ' ');
  printElement('R', "Avg Lat", 15, ' ');
  printElement('R', "Throu.", 15, ' ');
  printElement('R', "Efficiency", 15, ' ');
  std::cout << std::endl;
  printElement('R', "-", 75, '-');
  std::cout << std::endl;
  for (int i = 0; i < T; i++) {
    printElement('L', (i + 1), 8, ' ');
    std::cout << "|";
    printElement('R', trial_avg_rt[i].second, 15, ' ');
    printElement('R', trial_avg_lat[i].second, 15, ' ');
    printElement('R', trial_thrpt[i].second, 15, ' ');
    printElement('R', trial_eff[i].second, 15, ' ');
    std::cout << std::endl;
    ss << (i + 1) << "," << trial_avg_rt[i].second << ","
       << trial_avg_lat[i].second << "," << trial_thrpt[i].second << ","
       << trial_eff[i].second << "," << total_useful_work[i].second
       << std::endl;
  }

  // Write trial info to csv
  std::ofstream file;
  file.open("trial-avgs.csv");
  file << ss.str();
  file.close();

  return EXIT_SUCCESS;
}

// This function writes each Event
// and Task from the trial
void write_data_to_file(int i) {
  std::ofstream file;
  std::stringstream ss;
  ss << "cpu_util-" << i << ".csv";
  file.open(ss.str());
  file << "Trial,Time,CPU Utilization,CPU Cores,Type,Resource Used"
       << std::endl;
  std::for_each(SIM::done.begin(), SIM::done.end(), [&](auto &e) {
    if (e.cpu_util < 0)
      e.cpu_util = 0;
    file << i << "," << e.time << "," << e.cpu_util << "," << SIM::cpu.cores
         << "," << e.TypeNames[e.type] << "," << e.resource_used << std::endl;
  });
  file.close();
  std::stringstream ss2;
  ss2 << "tasks-" << i << ".csv";
  file.open(ss.str());
  file << "Trial,Id,Time Created,Response Time,Time Completed,Latency"
       << std::endl;
  std::for_each(SIM::tasks.begin(), SIM::tasks.end(), [&](auto &t) {
    file << i << "," << t->id << "," << t->time_created << ","
         << t->response_time << "," << t->time_completed << ","
         << t->time_completed - t->time_created << std::endl;
  });
}
