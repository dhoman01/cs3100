# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.4

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.4.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.4.3/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/build

# Include any dependencies generated for this target.
include CMakeFiles/sim.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/sim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sim.dir/flags.make

CMakeFiles/sim.dir/sources/main.cpp.o: CMakeFiles/sim.dir/flags.make
CMakeFiles/sim.dir/sources/main.cpp.o: /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/src/sources/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sim.dir/sources/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sim.dir/sources/main.cpp.o -c /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/src/sources/main.cpp

CMakeFiles/sim.dir/sources/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sim.dir/sources/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/src/sources/main.cpp > CMakeFiles/sim.dir/sources/main.cpp.i

CMakeFiles/sim.dir/sources/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sim.dir/sources/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/src/sources/main.cpp -o CMakeFiles/sim.dir/sources/main.cpp.s

CMakeFiles/sim.dir/sources/main.cpp.o.requires:

.PHONY : CMakeFiles/sim.dir/sources/main.cpp.o.requires

CMakeFiles/sim.dir/sources/main.cpp.o.provides: CMakeFiles/sim.dir/sources/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/sim.dir/build.make CMakeFiles/sim.dir/sources/main.cpp.o.provides.build
.PHONY : CMakeFiles/sim.dir/sources/main.cpp.o.provides

CMakeFiles/sim.dir/sources/main.cpp.o.provides.build: CMakeFiles/sim.dir/sources/main.cpp.o


# Object files for target sim
sim_OBJECTS = \
"CMakeFiles/sim.dir/sources/main.cpp.o"

# External object files for target sim
sim_EXTERNAL_OBJECTS =

sim: CMakeFiles/sim.dir/sources/main.cpp.o
sim: CMakeFiles/sim.dir/build.make
sim: CMakeFiles/sim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable sim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sim.dir/build: sim

.PHONY : CMakeFiles/sim.dir/build

CMakeFiles/sim.dir/requires: CMakeFiles/sim.dir/sources/main.cpp.o.requires

.PHONY : CMakeFiles/sim.dir/requires

CMakeFiles/sim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sim.dir/clean

CMakeFiles/sim.dir/depend:
	cd /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/src /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/src /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/build /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/build /Users/dustinhoman/Documents/cs3100/homan-os_sim-sp16/build/CMakeFiles/sim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sim.dir/depend
