# Variables to control simulation

## main.cpp

const int N // Number of tasks to create
const int T // Number of trials to run

## sim.hpp
// These variables control the spacing
// between tasks.
const double TASK_SPACE_LOW
const double TASK_SPACE_HIGH

## task.hpp
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
