# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/waxymantis/projects/lazarus-ds

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/waxymantis/projects/lazarus-ds/build

# Include any dependencies generated for this target.
include CMakeFiles/lazarus.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/lazarus.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/lazarus.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lazarus.dir/flags.make

CMakeFiles/lazarus.dir/src/main/main.cc.o: CMakeFiles/lazarus.dir/flags.make
CMakeFiles/lazarus.dir/src/main/main.cc.o: /home/waxymantis/projects/lazarus-ds/src/main/main.cc
CMakeFiles/lazarus.dir/src/main/main.cc.o: CMakeFiles/lazarus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/waxymantis/projects/lazarus-ds/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/lazarus.dir/src/main/main.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lazarus.dir/src/main/main.cc.o -MF CMakeFiles/lazarus.dir/src/main/main.cc.o.d -o CMakeFiles/lazarus.dir/src/main/main.cc.o -c /home/waxymantis/projects/lazarus-ds/src/main/main.cc

CMakeFiles/lazarus.dir/src/main/main.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lazarus.dir/src/main/main.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/waxymantis/projects/lazarus-ds/src/main/main.cc > CMakeFiles/lazarus.dir/src/main/main.cc.i

CMakeFiles/lazarus.dir/src/main/main.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lazarus.dir/src/main/main.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/waxymantis/projects/lazarus-ds/src/main/main.cc -o CMakeFiles/lazarus.dir/src/main/main.cc.s

CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o: CMakeFiles/lazarus.dir/flags.make
CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o: /home/waxymantis/projects/lazarus-ds/src/main/lazarus_data_store.cc
CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o: CMakeFiles/lazarus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/waxymantis/projects/lazarus-ds/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o -MF CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o.d -o CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o -c /home/waxymantis/projects/lazarus-ds/src/main/lazarus_data_store.cc

CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/waxymantis/projects/lazarus-ds/src/main/lazarus_data_store.cc > CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.i

CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/waxymantis/projects/lazarus-ds/src/main/lazarus_data_store.cc -o CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.s

CMakeFiles/lazarus.dir/src/network/server/server.cc.o: CMakeFiles/lazarus.dir/flags.make
CMakeFiles/lazarus.dir/src/network/server/server.cc.o: /home/waxymantis/projects/lazarus-ds/src/network/server/server.cc
CMakeFiles/lazarus.dir/src/network/server/server.cc.o: CMakeFiles/lazarus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/waxymantis/projects/lazarus-ds/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/lazarus.dir/src/network/server/server.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lazarus.dir/src/network/server/server.cc.o -MF CMakeFiles/lazarus.dir/src/network/server/server.cc.o.d -o CMakeFiles/lazarus.dir/src/network/server/server.cc.o -c /home/waxymantis/projects/lazarus-ds/src/network/server/server.cc

CMakeFiles/lazarus.dir/src/network/server/server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lazarus.dir/src/network/server/server.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/waxymantis/projects/lazarus-ds/src/network/server/server.cc > CMakeFiles/lazarus.dir/src/network/server/server.cc.i

CMakeFiles/lazarus.dir/src/network/server/server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lazarus.dir/src/network/server/server.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/waxymantis/projects/lazarus-ds/src/network/server/server.cc -o CMakeFiles/lazarus.dir/src/network/server/server.cc.s

CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o: CMakeFiles/lazarus.dir/flags.make
CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o: /home/waxymantis/projects/lazarus-ds/src/network/endpoints/object_container_endpoint.cc
CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o: CMakeFiles/lazarus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/waxymantis/projects/lazarus-ds/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o -MF CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o.d -o CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o -c /home/waxymantis/projects/lazarus-ds/src/network/endpoints/object_container_endpoint.cc

CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/waxymantis/projects/lazarus-ds/src/network/endpoints/object_container_endpoint.cc > CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.i

CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/waxymantis/projects/lazarus-ds/src/network/endpoints/object_container_endpoint.cc -o CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.s

# Object files for target lazarus
lazarus_OBJECTS = \
"CMakeFiles/lazarus.dir/src/main/main.cc.o" \
"CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o" \
"CMakeFiles/lazarus.dir/src/network/server/server.cc.o" \
"CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o"

# External object files for target lazarus
lazarus_EXTERNAL_OBJECTS =

lazarus: CMakeFiles/lazarus.dir/src/main/main.cc.o
lazarus: CMakeFiles/lazarus.dir/src/main/lazarus_data_store.cc.o
lazarus: CMakeFiles/lazarus.dir/src/network/server/server.cc.o
lazarus: CMakeFiles/lazarus.dir/src/network/endpoints/object_container_endpoint.cc.o
lazarus: CMakeFiles/lazarus.dir/build.make
lazarus: /usr/local/lib/libdrogon.a
lazarus: /usr/local/lib/libtrantor.a
lazarus: /usr/lib/x86_64-linux-gnu/libjsoncpp.so
lazarus: /usr/lib/x86_64-linux-gnu/libuuid.so
lazarus: /usr/lib/x86_64-linux-gnu/libz.so
lazarus: CMakeFiles/lazarus.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/waxymantis/projects/lazarus-ds/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable lazarus"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lazarus.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lazarus.dir/build: lazarus
.PHONY : CMakeFiles/lazarus.dir/build

CMakeFiles/lazarus.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lazarus.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lazarus.dir/clean

CMakeFiles/lazarus.dir/depend:
	cd /home/waxymantis/projects/lazarus-ds/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/waxymantis/projects/lazarus-ds /home/waxymantis/projects/lazarus-ds /home/waxymantis/projects/lazarus-ds/build /home/waxymantis/projects/lazarus-ds/build /home/waxymantis/projects/lazarus-ds/build/CMakeFiles/lazarus.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/lazarus.dir/depend

