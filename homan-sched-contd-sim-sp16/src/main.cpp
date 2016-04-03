#include "asjf.hpp"
#include "roundrobin.hpp"
#include "sim.hpp"
#include "sjfsim.hpp"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

void write_sim_data_to_file(int i);
void write_sjf_data_to_file(int i);
void write_rr_data_to_file(int i);
void write_asjf_data_to_file(int i);

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
  SIM::doFIFO(8, .05, 6, N, 1);
  SJF_SIM::doSJF(8, .05, 6, N, 1);
  ROUND_ROBIN::doRR(8, .05, 6, N, 1);
  ASJF_SIM::doASJF(8, .05, 6, N, 1);

  write_sim_data_to_file(1);
  write_sjf_data_to_file(1);
  write_rr_data_to_file(1);
  write_asjf_data_to_file(1);

  return EXIT_SUCCESS;
}

// This function writes each Event
// and Task from the trial
void write_sim_data_to_file(int i) {
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
  file.open(ss2.str());
  file << "Trial,Id,Time Created,Response Time,Time Completed,Latency"
       << std::endl;
  std::for_each(SIM::tasks.begin(), SIM::tasks.end(), [&](auto &t) {
    file << i << "," << t->id << "," << t->time_created << ","
         << t->response_time << "," << t->time_completed << ","
         << t->time_completed - t->time_created << std::endl;
  });
}

void write_sjf_data_to_file(int i) {
  std::ofstream file;
  std::stringstream ss;
  ss << "cpu_util-sjf-" << i << ".csv";
  file.open(ss.str());
  file << "SJFTrial,SJFTime,SJFCPU Utilization,SJFCPU "
          "Cores,SJFType,SJFResource Used"
       << std::endl;
  std::for_each(SJF_SIM::done.begin(), SJF_SIM::done.end(), [&](auto &e) {
    if (e.cpu_util < 0)
      e.cpu_util = 0;
    file << i << "," << e.time << "," << e.cpu_util << "," << SIM::cpu.cores
         << "," << e.TypeNames[e.type] << "," << e.resource_used << std::endl;
  });
  file.close();
  std::stringstream ss2;
  ss2 << "tasks-sjf-" << i << ".csv";
  file.open(ss2.str());
  file << "SJFTrial,SJFId,SJFTime Created,SJFResponse Time,SJFTime "
          "Completed,SJFLatency"
       << std::endl;
  std::for_each(SJF_SIM::tasks.begin(), SJF_SIM::tasks.end(), [&](auto &t) {
    file << i << "," << t->id << "," << t->time_created << ","
         << t->response_time << "," << t->time_completed << ","
         << t->time_completed - t->time_created << std::endl;
  });
}

void write_rr_data_to_file(int i) {
  std::ofstream file;
  std::stringstream ss;
  ss << "cpu_util-rr-" << i << ".csv";
  file.open(ss.str());
  file << "RRTrial,RRTime,RRCPU Utilization,RRCPU Cores,RRType,RRResource Used"
       << std::endl;
  std::for_each(ROUND_ROBIN::done.begin(), ROUND_ROBIN::done.end(),
                [&](auto &e) {
                  if (e.cpu_util < 0)
                    e.cpu_util = 0;
                  file << i << "," << e.time << "," << e.cpu_util << ","
                       << SIM::cpu.cores << "," << e.TypeNames[e.type] << ","
                       << e.resource_used << std::endl;
                });
  file.close();
  std::stringstream ss2;
  ss2 << "tasks-rr-" << i << ".csv";
  file.open(ss2.str());
  file << "RRTrial,RRId,RRTime Created,RRResponse Time,RRTime "
          "Completed,RRLatency"
       << std::endl;
  std::for_each(ROUND_ROBIN::tasks.begin(), ROUND_ROBIN::tasks.end(),
                [&](auto &t) {
                  file << i << "," << t->id << "," << t->time_created << ","
                       << t->response_time << "," << t->time_completed << ","
                       << t->time_completed - t->time_created << std::endl;
                });
}

void write_asjf_data_to_file(int i) {
  std::ofstream file;
  std::stringstream ss;
  ss << "cpu_util-asjf-" << i << ".csv";
  file.open(ss.str());
  file << "ASJFTrial,ASJFTime,ASJFCPU Utilization,ASJFCPU "
          "Cores,ASJFType,ASJFResource Used"
       << std::endl;
  std::for_each(ASJF_SIM::done.begin(), ASJF_SIM::done.end(), [&](auto &e) {
    if (e.cpu_util < 0)
      e.cpu_util = 0;
    file << i << "," << e.time << "," << e.cpu_util << "," << SIM::cpu.cores
         << "," << e.TypeNames[e.type] << "," << e.resource_used << std::endl;
  });
  file.close();
  std::stringstream ss2;
  ss2 << "tasks-asjf-" << i << ".csv";
  file.open(ss2.str());
  file << "ASJFTrial,ASJFId,ASJFTime Created,ASJFResponse Time,ASJFTime "
          "Completed,ASJFLatency"
       << std::endl;
  std::for_each(ASJF_SIM::tasks.begin(), ASJF_SIM::tasks.end(), [&](auto &t) {
    file << i << "," << t->id << "," << t->time_created << ","
         << t->response_time << "," << t->time_completed << ","
         << t->time_completed - t->time_created << std::endl;
  });
}
