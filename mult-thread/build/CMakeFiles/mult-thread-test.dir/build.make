# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.0

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/Desktop/cs3100/mult-thread/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/Desktop/cs3100/mult-thread/build

# Include any dependencies generated for this target.
include CMakeFiles/mult-thread-test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/mult-thread-test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mult-thread-test.dir/flags.make

CMakeFiles/mult-thread-test.dir/main.cpp.o: CMakeFiles/mult-thread-test.dir/flags.make
CMakeFiles/mult-thread-test.dir/main.cpp.o: /home/pi/Desktop/cs3100/mult-thread/src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/pi/Desktop/cs3100/mult-thread/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/mult-thread-test.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mult-thread-test.dir/main.cpp.o -c /home/pi/Desktop/cs3100/mult-thread/src/main.cpp

CMakeFiles/mult-thread-test.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mult-thread-test.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/pi/Desktop/cs3100/mult-thread/src/main.cpp > CMakeFiles/mult-thread-test.dir/main.cpp.i

CMakeFiles/mult-thread-test.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mult-thread-test.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/pi/Desktop/cs3100/mult-thread/src/main.cpp -o CMakeFiles/mult-thread-test.dir/main.cpp.s

CMakeFiles/mult-thread-test.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/mult-thread-test.dir/main.cpp.o.requires

CMakeFiles/mult-thread-test.dir/main.cpp.o.provides: CMakeFiles/mult-thread-test.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/mult-thread-test.dir/build.make CMakeFiles/mult-thread-test.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/mult-thread-test.dir/main.cpp.o.provides

CMakeFiles/mult-thread-test.dir/main.cpp.o.provides.build: CMakeFiles/mult-thread-test.dir/main.cpp.o

# Object files for target mult-thread-test
mult__thread__test_OBJECTS = \
"CMakeFiles/mult-thread-test.dir/main.cpp.o"

# External object files for target mult-thread-test
mult__thread__test_EXTERNAL_OBJECTS =

mult-thread-test: CMakeFiles/mult-thread-test.dir/main.cpp.o
mult-thread-test: CMakeFiles/mult-thread-test.dir/build.make
mult-thread-test: CMakeFiles/mult-thread-test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable mult-thread-test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mult-thread-test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mult-thread-test.dir/build: mult-thread-test
.PHONY : CMakeFiles/mult-thread-test.dir/build

CMakeFiles/mult-thread-test.dir/requires: CMakeFiles/mult-thread-test.dir/main.cpp.o.requires
.PHONY : CMakeFiles/mult-thread-test.dir/requires

CMakeFiles/mult-thread-test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mult-thread-test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mult-thread-test.dir/clean

CMakeFiles/mult-thread-test.dir/depend:
	cd /home/pi/Desktop/cs3100/mult-thread/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Desktop/cs3100/mult-thread/src /home/pi/Desktop/cs3100/mult-thread/src /home/pi/Desktop/cs3100/mult-thread/build /home/pi/Desktop/cs3100/mult-thread/build /home/pi/Desktop/cs3100/mult-thread/build/CMakeFiles/mult-thread-test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mult-thread-test.dir/depend

