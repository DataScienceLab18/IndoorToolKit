# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src

# Include any dependencies generated for this target.
include CMakeFiles/walkinggraph.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/walkinggraph.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/walkinggraph.dir/flags.make

CMakeFiles/walkinggraph.dir/walkinggraph.cpp.o: CMakeFiles/walkinggraph.dir/flags.make
CMakeFiles/walkinggraph.dir/walkinggraph.cpp.o: walkinggraph.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/walkinggraph.dir/walkinggraph.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/walkinggraph.dir/walkinggraph.cpp.o -c /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src/walkinggraph.cpp

CMakeFiles/walkinggraph.dir/walkinggraph.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/walkinggraph.dir/walkinggraph.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src/walkinggraph.cpp > CMakeFiles/walkinggraph.dir/walkinggraph.cpp.i

CMakeFiles/walkinggraph.dir/walkinggraph.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/walkinggraph.dir/walkinggraph.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src/walkinggraph.cpp -o CMakeFiles/walkinggraph.dir/walkinggraph.cpp.s

# Object files for target walkinggraph
walkinggraph_OBJECTS = \
"CMakeFiles/walkinggraph.dir/walkinggraph.cpp.o"

# External object files for target walkinggraph
walkinggraph_EXTERNAL_OBJECTS =

libwalkinggraph.a: CMakeFiles/walkinggraph.dir/walkinggraph.cpp.o
libwalkinggraph.a: CMakeFiles/walkinggraph.dir/build.make
libwalkinggraph.a: CMakeFiles/walkinggraph.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libwalkinggraph.a"
	$(CMAKE_COMMAND) -P CMakeFiles/walkinggraph.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/walkinggraph.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/walkinggraph.dir/build: libwalkinggraph.a

.PHONY : CMakeFiles/walkinggraph.dir/build

CMakeFiles/walkinggraph.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/walkinggraph.dir/cmake_clean.cmake
.PHONY : CMakeFiles/walkinggraph.dir/clean

CMakeFiles/walkinggraph.dir/depend:
	cd /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src /home/bo/indoor/IndoorToolKit/Kalman/QuerySimulation-master/src/CMakeFiles/walkinggraph.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/walkinggraph.dir/depend

