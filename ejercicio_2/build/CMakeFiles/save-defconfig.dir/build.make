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

# Utility rule file for save-defconfig.

# Include any custom commands dependencies for this target.
include CMakeFiles/save-defconfig.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/save-defconfig.dir/progress.make

CMakeFiles/save-defconfig:
	/Users/sofiacastelli/esp/idf-tools/python_env/idf5.5_py3.13_env/bin/python /Users/sofiacastelli/esp/idf/esp-idf/tools/kconfig_new/prepare_kconfig_files.py --list-separator=semicolon --env-file /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build/config.env
	/Users/sofiacastelli/esp/idf-tools/python_env/idf5.5_py3.13_env/bin/python -m kconfgen --list-separator=semicolon --kconfig /Users/sofiacastelli/esp/idf/esp-idf/Kconfig --sdkconfig-rename /Users/sofiacastelli/esp/idf/esp-idf/sdkconfig.rename --config /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/sdkconfig --env IDF_MINIMAL_BUILD=n --env-file /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build/config.env --dont-write-deprecated --output savedefconfig /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/sdkconfig.defaults

CMakeFiles/save-defconfig.dir/codegen:
.PHONY : CMakeFiles/save-defconfig.dir/codegen

save-defconfig: CMakeFiles/save-defconfig
save-defconfig: CMakeFiles/save-defconfig.dir/build.make
.PHONY : save-defconfig

# Rule to build all files generated by this target.
CMakeFiles/save-defconfig.dir/build: save-defconfig
.PHONY : CMakeFiles/save-defconfig.dir/build

CMakeFiles/save-defconfig.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/save-defconfig.dir/cmake_clean.cmake
.PHONY : CMakeFiles/save-defconfig.dir/clean

CMakeFiles/save-defconfig.dir/depend:
	cd /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2 /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2 /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build /Users/sofiacastelli/Documents/GitHub/Rep_SE_Lab2_Poch_Castelli/ejercicio_2/build/CMakeFiles/save-defconfig.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/save-defconfig.dir/depend

