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
CMAKE_SOURCE_DIR = /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/build

# Include any dependencies generated for this target.
include CMakeFiles/shell.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/shell.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/shell.dir/flags.make

CMakeFiles/shell.dir/main.cpp.o: CMakeFiles/shell.dir/flags.make
CMakeFiles/shell.dir/main.cpp.o: /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/shell.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/shell.dir/main.cpp.o -c /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/src/main.cpp

CMakeFiles/shell.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/shell.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/src/main.cpp > CMakeFiles/shell.dir/main.cpp.i

CMakeFiles/shell.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/shell.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/src/main.cpp -o CMakeFiles/shell.dir/main.cpp.s

CMakeFiles/shell.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/shell.dir/main.cpp.o.requires

CMakeFiles/shell.dir/main.cpp.o.provides: CMakeFiles/shell.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/shell.dir/build.make CMakeFiles/shell.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/shell.dir/main.cpp.o.provides

CMakeFiles/shell.dir/main.cpp.o.provides.build: CMakeFiles/shell.dir/main.cpp.o


# Object files for target shell
shell_OBJECTS = \
"CMakeFiles/shell.dir/main.cpp.o"

# External object files for target shell
shell_EXTERNAL_OBJECTS =

shell: CMakeFiles/shell.dir/main.cpp.o
shell: CMakeFiles/shell.dir/build.make
shell: CMakeFiles/shell.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable shell"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/shell.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/shell.dir/build: shell

.PHONY : CMakeFiles/shell.dir/build

CMakeFiles/shell.dir/requires: CMakeFiles/shell.dir/main.cpp.o.requires

.PHONY : CMakeFiles/shell.dir/requires

CMakeFiles/shell.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/shell.dir/cmake_clean.cmake
.PHONY : CMakeFiles/shell.dir/clean

CMakeFiles/shell.dir/depend:
	cd /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/src /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/src /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/build /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/build /Users/dustinhoman/Documents/cs3100/homan-shell-1-sp16/build/CMakeFiles/shell.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/shell.dir/depend

