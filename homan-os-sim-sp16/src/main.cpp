#include "gnuplot-iostream.h"
#include "sim.hpp"
#include <cstdlib>
#include <iostream>
#include <map>

int main() {
  Gnuplot gp;
  SIM::doFIFO(4, 1, 4, 10);
  std::vector<std::pair<double, double>> time_cpu_pts;
  std::for_each(SIM::done.begin(), SIM::done.end(), [&](auto e) {
    time_cpu_pts.push_back(std::make_pair(e.time, e.cpu_util));
  });

  gp << "set xrange [0:200]\nset yrange [0:200]\n";
  gp << "set xlabel 'Time in seconds'\n set ylabel 'CPU Utilization in "
        "seconds'\nshow xlabel\nshow ylabel\n";
  gp << "plot" << gp.file1d(time_cpu_pts, "mydata.png")
     << "with lines title 'CPU Utilization'" << std::endl;
  // gp.flush();
  // std::cout << "Press enter to exit." << std::endl;
  // std::cin.get();
  return EXIT_SUCCESS;
}
