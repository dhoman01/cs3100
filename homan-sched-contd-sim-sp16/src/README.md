# Variables to control simulation

## memorysim.hpp
// These variables control the spacing
// between tasks.
const double TASK_SPACE_LOW
const double TASK_SPACE_HIGH

// Cache parameters
const double MISS_PENALTY = 1;
const int CACHE_SIZE = 1024;

## main.cpp

const int N // Number of tasks to create
const int T // Number of trials to run

## task.hpp

// The limit on page number to pull from
const int MAX_PAGE_NUMBER = 2048;

// MIX of CPU_BOUND vs IO_BOUND tasks
const double MIX

// CPU Task lengths
const double CPU_BOUND_LOWER_CPU
const double CPU_BOUND_UPPER_CPU
const double IO_BOUND_LOWER_CPU
const double IO_BOUND_UPPER_CPU

// IO Task lenghts
const double CPU_BOUND_LOWER_IO
const double CPU_BOUND_UPPER_IO
const double IO_BOUND_LOWER_IO
const double IO_BOUND_UPPER_IO

// Probability that another task
// item follows
const double CPU_BOUND_CONTINUE
const double IO_BOUND_CONTINUE

// Minimum number of taskitems to add to each task
const double MIN_TASK_LENGTH
