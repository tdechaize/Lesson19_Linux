# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

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
CMAKE_SOURCE_DIR = /home/thierry/Sources/Opengl/Lesson19

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug

# Include any dependencies generated for this target.
include CMakeFiles/lesson19.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/lesson19.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/lesson19.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lesson19.dir/flags.make

CMakeFiles/lesson19.dir/src/lesson19.c.o: CMakeFiles/lesson19.dir/flags.make
CMakeFiles/lesson19.dir/src/lesson19.c.o: /home/thierry/Sources/Opengl/Lesson19/src/lesson19.c
CMakeFiles/lesson19.dir/src/lesson19.c.o: CMakeFiles/lesson19.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/lesson19.dir/src/lesson19.c.o"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/lesson19.dir/src/lesson19.c.o -MF CMakeFiles/lesson19.dir/src/lesson19.c.o.d -o CMakeFiles/lesson19.dir/src/lesson19.c.o -c /home/thierry/Sources/Opengl/Lesson19/src/lesson19.c

CMakeFiles/lesson19.dir/src/lesson19.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lesson19.dir/src/lesson19.c.i"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thierry/Sources/Opengl/Lesson19/src/lesson19.c > CMakeFiles/lesson19.dir/src/lesson19.c.i

CMakeFiles/lesson19.dir/src/lesson19.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lesson19.dir/src/lesson19.c.s"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thierry/Sources/Opengl/Lesson19/src/lesson19.c -o CMakeFiles/lesson19.dir/src/lesson19.c.s

CMakeFiles/lesson19.dir/src/logger.c.o: CMakeFiles/lesson19.dir/flags.make
CMakeFiles/lesson19.dir/src/logger.c.o: /home/thierry/Sources/Opengl/Lesson19/src/logger.c
CMakeFiles/lesson19.dir/src/logger.c.o: CMakeFiles/lesson19.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/lesson19.dir/src/logger.c.o"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/lesson19.dir/src/logger.c.o -MF CMakeFiles/lesson19.dir/src/logger.c.o.d -o CMakeFiles/lesson19.dir/src/logger.c.o -c /home/thierry/Sources/Opengl/Lesson19/src/logger.c

CMakeFiles/lesson19.dir/src/logger.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lesson19.dir/src/logger.c.i"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/thierry/Sources/Opengl/Lesson19/src/logger.c > CMakeFiles/lesson19.dir/src/logger.c.i

CMakeFiles/lesson19.dir/src/logger.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lesson19.dir/src/logger.c.s"
	/usr/bin/clang $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/thierry/Sources/Opengl/Lesson19/src/logger.c -o CMakeFiles/lesson19.dir/src/logger.c.s

# Object files for target lesson19
lesson19_OBJECTS = \
"CMakeFiles/lesson19.dir/src/lesson19.c.o" \
"CMakeFiles/lesson19.dir/src/logger.c.o"

# External object files for target lesson19
lesson19_EXTERNAL_OBJECTS =

/home/thierry/Sources/Opengl/Lesson19/binclang32/Debug/lesson19.exe: CMakeFiles/lesson19.dir/src/lesson19.c.o
/home/thierry/Sources/Opengl/Lesson19/binclang32/Debug/lesson19.exe: CMakeFiles/lesson19.dir/src/logger.c.o
/home/thierry/Sources/Opengl/Lesson19/binclang32/Debug/lesson19.exe: CMakeFiles/lesson19.dir/build.make
/home/thierry/Sources/Opengl/Lesson19/binclang32/Debug/lesson19.exe: CMakeFiles/lesson19.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable /home/thierry/Sources/Opengl/Lesson19/binclang32/Debug/lesson19.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lesson19.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lesson19.dir/build: /home/thierry/Sources/Opengl/Lesson19/binclang32/Debug/lesson19.exe
.PHONY : CMakeFiles/lesson19.dir/build

CMakeFiles/lesson19.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lesson19.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lesson19.dir/clean

CMakeFiles/lesson19.dir/depend:
	cd /home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/thierry/Sources/Opengl/Lesson19 /home/thierry/Sources/Opengl/Lesson19 /home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug /home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug /home/thierry/Sources/Opengl/Lesson19/build.cmake/clang32/Debug/CMakeFiles/lesson19.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/lesson19.dir/depend

