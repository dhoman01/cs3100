#include "OS.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv) {
  OS os;
  os.init(4, 4, 10);
  return EXIT_SUCCESS;
}
