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
CMAKE_COMMAND = /opt/homebrew/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build

# Utility rule file for app.

# Include any custom commands dependencies for this target.
include CMakeFiles/app.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/app.dir/progress.make

CMakeFiles/app:

CMakeFiles/app.dir/codegen:
.PHONY : CMakeFiles/app.dir/codegen

app: CMakeFiles/app
app: CMakeFiles/app.dir/build.make
.PHONY : app

# Rule to build all files generated by this target.
CMakeFiles/app.dir/build: app
.PHONY : CMakeFiles/app.dir/build

CMakeFiles/app.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/app.dir/cmake_clean.cmake
.PHONY : CMakeFiles/app.dir/clean

CMakeFiles/app.dir/depend:
	cd /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2 /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2 /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build/CMakeFiles/app.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/app.dir/depend

