# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/harry/workspace/sylar/sylar_learn

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/harry/workspace/sylar/sylar_learn/build

# Include any dependencies generated for this target.
include CMakeFiles/sylar.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/sylar.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/sylar.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sylar.dir/flags.make

CMakeFiles/sylar.dir/codegen:
.PHONY : CMakeFiles/sylar.dir/codegen

CMakeFiles/sylar.dir/tests/test_daemon.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/tests/test_daemon.cpp.o: /home/harry/workspace/sylar/sylar_learn/tests/test_daemon.cpp
CMakeFiles/sylar.dir/tests/test_daemon.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sylar.dir/tests/test_daemon.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/tests/test_daemon.cpp.o -MF CMakeFiles/sylar.dir/tests/test_daemon.cpp.o.d -o CMakeFiles/sylar.dir/tests/test_daemon.cpp.o -c /home/harry/workspace/sylar/sylar_learn/tests/test_daemon.cpp

CMakeFiles/sylar.dir/tests/test_daemon.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/tests/test_daemon.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/tests/test_daemon.cpp > CMakeFiles/sylar.dir/tests/test_daemon.cpp.i

CMakeFiles/sylar.dir/tests/test_daemon.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/tests/test_daemon.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/tests/test_daemon.cpp -o CMakeFiles/sylar.dir/tests/test_daemon.cpp.s

CMakeFiles/sylar.dir/src/log/logger.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/log/logger.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/log/logger.cpp
CMakeFiles/sylar.dir/src/log/logger.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/sylar.dir/src/log/logger.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/log/logger.cpp.o -MF CMakeFiles/sylar.dir/src/log/logger.cpp.o.d -o CMakeFiles/sylar.dir/src/log/logger.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/log/logger.cpp

CMakeFiles/sylar.dir/src/log/logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/log/logger.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/log/logger.cpp > CMakeFiles/sylar.dir/src/log/logger.cpp.i

CMakeFiles/sylar.dir/src/log/logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/log/logger.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/log/logger.cpp -o CMakeFiles/sylar.dir/src/log/logger.cpp.s

CMakeFiles/sylar.dir/src/util/util.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/util/util.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/util/util.cpp
CMakeFiles/sylar.dir/src/util/util.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/sylar.dir/src/util/util.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/util/util.cpp.o -MF CMakeFiles/sylar.dir/src/util/util.cpp.o.d -o CMakeFiles/sylar.dir/src/util/util.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/util/util.cpp

CMakeFiles/sylar.dir/src/util/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/util/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/util/util.cpp > CMakeFiles/sylar.dir/src/util/util.cpp.i

CMakeFiles/sylar.dir/src/util/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/util/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/util/util.cpp -o CMakeFiles/sylar.dir/src/util/util.cpp.s

CMakeFiles/sylar.dir/src/util/hook.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/util/hook.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/util/hook.cpp
CMakeFiles/sylar.dir/src/util/hook.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/sylar.dir/src/util/hook.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/util/hook.cpp.o -MF CMakeFiles/sylar.dir/src/util/hook.cpp.o.d -o CMakeFiles/sylar.dir/src/util/hook.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/util/hook.cpp

CMakeFiles/sylar.dir/src/util/hook.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/util/hook.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/util/hook.cpp > CMakeFiles/sylar.dir/src/util/hook.cpp.i

CMakeFiles/sylar.dir/src/util/hook.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/util/hook.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/util/hook.cpp -o CMakeFiles/sylar.dir/src/util/hook.cpp.s

CMakeFiles/sylar.dir/src/util/env.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/util/env.cc.o: /home/harry/workspace/sylar/sylar_learn/src/util/env.cc
CMakeFiles/sylar.dir/src/util/env.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/sylar.dir/src/util/env.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/util/env.cc.o -MF CMakeFiles/sylar.dir/src/util/env.cc.o.d -o CMakeFiles/sylar.dir/src/util/env.cc.o -c /home/harry/workspace/sylar/sylar_learn/src/util/env.cc

CMakeFiles/sylar.dir/src/util/env.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/util/env.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/util/env.cc > CMakeFiles/sylar.dir/src/util/env.cc.i

CMakeFiles/sylar.dir/src/util/env.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/util/env.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/util/env.cc -o CMakeFiles/sylar.dir/src/util/env.cc.s

CMakeFiles/sylar.dir/src/util/daemon.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/util/daemon.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/util/daemon.cpp
CMakeFiles/sylar.dir/src/util/daemon.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/sylar.dir/src/util/daemon.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/util/daemon.cpp.o -MF CMakeFiles/sylar.dir/src/util/daemon.cpp.o.d -o CMakeFiles/sylar.dir/src/util/daemon.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/util/daemon.cpp

CMakeFiles/sylar.dir/src/util/daemon.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/util/daemon.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/util/daemon.cpp > CMakeFiles/sylar.dir/src/util/daemon.cpp.i

CMakeFiles/sylar.dir/src/util/daemon.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/util/daemon.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/util/daemon.cpp -o CMakeFiles/sylar.dir/src/util/daemon.cpp.s

CMakeFiles/sylar.dir/src/config/config.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/config/config.cc.o: /home/harry/workspace/sylar/sylar_learn/src/config/config.cc
CMakeFiles/sylar.dir/src/config/config.cc.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/sylar.dir/src/config/config.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/config/config.cc.o -MF CMakeFiles/sylar.dir/src/config/config.cc.o.d -o CMakeFiles/sylar.dir/src/config/config.cc.o -c /home/harry/workspace/sylar/sylar_learn/src/config/config.cc

CMakeFiles/sylar.dir/src/config/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/config/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/config/config.cc > CMakeFiles/sylar.dir/src/config/config.cc.i

CMakeFiles/sylar.dir/src/config/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/config/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/config/config.cc -o CMakeFiles/sylar.dir/src/config/config.cc.s

CMakeFiles/sylar.dir/src/thread/thread.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/thread/thread.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/thread/thread.cpp
CMakeFiles/sylar.dir/src/thread/thread.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/sylar.dir/src/thread/thread.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/thread/thread.cpp.o -MF CMakeFiles/sylar.dir/src/thread/thread.cpp.o.d -o CMakeFiles/sylar.dir/src/thread/thread.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/thread/thread.cpp

CMakeFiles/sylar.dir/src/thread/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/thread/thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/thread/thread.cpp > CMakeFiles/sylar.dir/src/thread/thread.cpp.i

CMakeFiles/sylar.dir/src/thread/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/thread/thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/thread/thread.cpp -o CMakeFiles/sylar.dir/src/thread/thread.cpp.s

CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/fiber/fiber.cpp
CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o -MF CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o.d -o CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/fiber/fiber.cpp

CMakeFiles/sylar.dir/src/fiber/fiber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/fiber/fiber.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/fiber/fiber.cpp > CMakeFiles/sylar.dir/src/fiber/fiber.cpp.i

CMakeFiles/sylar.dir/src/fiber/fiber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/fiber/fiber.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/fiber/fiber.cpp -o CMakeFiles/sylar.dir/src/fiber/fiber.cpp.s

CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/fiber/scheduler.cpp
CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o -MF CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o.d -o CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/fiber/scheduler.cpp

CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/fiber/scheduler.cpp > CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.i

CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/fiber/scheduler.cpp -o CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.s

CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/eventpoller/eventpoller.cpp
CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o -MF CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o.d -o CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/eventpoller/eventpoller.cpp

CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/eventpoller/eventpoller.cpp > CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.i

CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/eventpoller/eventpoller.cpp -o CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.s

CMakeFiles/sylar.dir/src/timer/timer.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/timer/timer.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/timer/timer.cpp
CMakeFiles/sylar.dir/src/timer/timer.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/sylar.dir/src/timer/timer.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/timer/timer.cpp.o -MF CMakeFiles/sylar.dir/src/timer/timer.cpp.o.d -o CMakeFiles/sylar.dir/src/timer/timer.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/timer/timer.cpp

CMakeFiles/sylar.dir/src/timer/timer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/timer/timer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/timer/timer.cpp > CMakeFiles/sylar.dir/src/timer/timer.cpp.i

CMakeFiles/sylar.dir/src/timer/timer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/timer/timer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/timer/timer.cpp -o CMakeFiles/sylar.dir/src/timer/timer.cpp.s

CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/socket/fdManager.cpp
CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o -MF CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o.d -o CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/socket/fdManager.cpp

CMakeFiles/sylar.dir/src/socket/fdManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/socket/fdManager.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/socket/fdManager.cpp > CMakeFiles/sylar.dir/src/socket/fdManager.cpp.i

CMakeFiles/sylar.dir/src/socket/fdManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/socket/fdManager.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/socket/fdManager.cpp -o CMakeFiles/sylar.dir/src/socket/fdManager.cpp.s

CMakeFiles/sylar.dir/src/socket/address.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/socket/address.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/socket/address.cpp
CMakeFiles/sylar.dir/src/socket/address.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/sylar.dir/src/socket/address.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/socket/address.cpp.o -MF CMakeFiles/sylar.dir/src/socket/address.cpp.o.d -o CMakeFiles/sylar.dir/src/socket/address.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/socket/address.cpp

CMakeFiles/sylar.dir/src/socket/address.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/socket/address.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/socket/address.cpp > CMakeFiles/sylar.dir/src/socket/address.cpp.i

CMakeFiles/sylar.dir/src/socket/address.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/socket/address.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/socket/address.cpp -o CMakeFiles/sylar.dir/src/socket/address.cpp.s

CMakeFiles/sylar.dir/src/socket/socket.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/socket/socket.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/socket/socket.cpp
CMakeFiles/sylar.dir/src/socket/socket.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/sylar.dir/src/socket/socket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/socket/socket.cpp.o -MF CMakeFiles/sylar.dir/src/socket/socket.cpp.o.d -o CMakeFiles/sylar.dir/src/socket/socket.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/socket/socket.cpp

CMakeFiles/sylar.dir/src/socket/socket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/socket/socket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/socket/socket.cpp > CMakeFiles/sylar.dir/src/socket/socket.cpp.i

CMakeFiles/sylar.dir/src/socket/socket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/socket/socket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/socket/socket.cpp -o CMakeFiles/sylar.dir/src/socket/socket.cpp.s

CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/socket/bytearray.cpp
CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Building CXX object CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o -MF CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o.d -o CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/socket/bytearray.cpp

CMakeFiles/sylar.dir/src/socket/bytearray.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/socket/bytearray.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/socket/bytearray.cpp > CMakeFiles/sylar.dir/src/socket/bytearray.cpp.i

CMakeFiles/sylar.dir/src/socket/bytearray.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/socket/bytearray.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/socket/bytearray.cpp -o CMakeFiles/sylar.dir/src/socket/bytearray.cpp.s

CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o: /home/harry/workspace/sylar/sylar_learn/src/server/TCPserver.cpp
CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o: CMakeFiles/sylar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_17) "Building CXX object CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o -MF CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o.d -o CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o -c /home/harry/workspace/sylar/sylar_learn/src/server/TCPserver.cpp

CMakeFiles/sylar.dir/src/server/TCPserver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/sylar.dir/src/server/TCPserver.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/harry/workspace/sylar/sylar_learn/src/server/TCPserver.cpp > CMakeFiles/sylar.dir/src/server/TCPserver.cpp.i

CMakeFiles/sylar.dir/src/server/TCPserver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/src/server/TCPserver.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/harry/workspace/sylar/sylar_learn/src/server/TCPserver.cpp -o CMakeFiles/sylar.dir/src/server/TCPserver.cpp.s

# Object files for target sylar
sylar_OBJECTS = \
"CMakeFiles/sylar.dir/tests/test_daemon.cpp.o" \
"CMakeFiles/sylar.dir/src/log/logger.cpp.o" \
"CMakeFiles/sylar.dir/src/util/util.cpp.o" \
"CMakeFiles/sylar.dir/src/util/hook.cpp.o" \
"CMakeFiles/sylar.dir/src/util/env.cc.o" \
"CMakeFiles/sylar.dir/src/util/daemon.cpp.o" \
"CMakeFiles/sylar.dir/src/config/config.cc.o" \
"CMakeFiles/sylar.dir/src/thread/thread.cpp.o" \
"CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o" \
"CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o" \
"CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o" \
"CMakeFiles/sylar.dir/src/timer/timer.cpp.o" \
"CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o" \
"CMakeFiles/sylar.dir/src/socket/address.cpp.o" \
"CMakeFiles/sylar.dir/src/socket/socket.cpp.o" \
"CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o" \
"CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o"

# External object files for target sylar
sylar_EXTERNAL_OBJECTS =

/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/tests/test_daemon.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/log/logger.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/util/util.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/util/hook.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/util/env.cc.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/util/daemon.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/config/config.cc.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/thread/thread.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/fiber/fiber.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/fiber/scheduler.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/eventpoller/eventpoller.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/timer/timer.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/socket/fdManager.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/socket/address.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/socket/socket.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/socket/bytearray.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/src/server/TCPserver.cpp.o
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/build.make
/home/harry/workspace/sylar/sylar_learn/bin/sylar: /usr/lib/x86_64-linux-gnu/libyaml-cpp.so.0.7.0
/home/harry/workspace/sylar/sylar_learn/bin/sylar: CMakeFiles/sylar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/harry/workspace/sylar/sylar_learn/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_18) "Linking CXX executable /home/harry/workspace/sylar/sylar_learn/bin/sylar"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sylar.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sylar.dir/build: /home/harry/workspace/sylar/sylar_learn/bin/sylar
.PHONY : CMakeFiles/sylar.dir/build

CMakeFiles/sylar.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sylar.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sylar.dir/clean

CMakeFiles/sylar.dir/depend:
	cd /home/harry/workspace/sylar/sylar_learn/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/harry/workspace/sylar/sylar_learn /home/harry/workspace/sylar/sylar_learn /home/harry/workspace/sylar/sylar_learn/build /home/harry/workspace/sylar/sylar_learn/build /home/harry/workspace/sylar/sylar_learn/build/CMakeFiles/sylar.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/sylar.dir/depend

