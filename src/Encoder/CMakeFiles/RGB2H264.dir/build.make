# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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
CMAKE_SOURCE_DIR = /mass/workspace/rtp_connect/src/Encoder

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mass/workspace/rtp_connect/src/Encoder

# Include any dependencies generated for this target.
include CMakeFiles/RGB2H264.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/RGB2H264.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RGB2H264.dir/flags.make

CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o: CMakeFiles/RGB2H264.dir/flags.make
CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o: rgbToH264.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /mass/workspace/rtp_connect/src/Encoder/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o -c /mass/workspace/rtp_connect/src/Encoder/rgbToH264.cpp

CMakeFiles/RGB2H264.dir/rgbToH264.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RGB2H264.dir/rgbToH264.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /mass/workspace/rtp_connect/src/Encoder/rgbToH264.cpp > CMakeFiles/RGB2H264.dir/rgbToH264.cpp.i

CMakeFiles/RGB2H264.dir/rgbToH264.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RGB2H264.dir/rgbToH264.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /mass/workspace/rtp_connect/src/Encoder/rgbToH264.cpp -o CMakeFiles/RGB2H264.dir/rgbToH264.cpp.s

CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.requires:
.PHONY : CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.requires

CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.provides: CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.requires
	$(MAKE) -f CMakeFiles/RGB2H264.dir/build.make CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.provides.build
.PHONY : CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.provides

CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.provides.build: CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o

# Object files for target RGB2H264
RGB2H264_OBJECTS = \
"CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o"

# External object files for target RGB2H264
RGB2H264_EXTERNAL_OBJECTS =

RGB2H264: CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o
RGB2H264: CMakeFiles/RGB2H264.dir/build.make
RGB2H264: CMakeFiles/RGB2H264.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable RGB2H264"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RGB2H264.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RGB2H264.dir/build: RGB2H264
.PHONY : CMakeFiles/RGB2H264.dir/build

CMakeFiles/RGB2H264.dir/requires: CMakeFiles/RGB2H264.dir/rgbToH264.cpp.o.requires
.PHONY : CMakeFiles/RGB2H264.dir/requires

CMakeFiles/RGB2H264.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RGB2H264.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RGB2H264.dir/clean

CMakeFiles/RGB2H264.dir/depend:
	cd /mass/workspace/rtp_connect/src/Encoder && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mass/workspace/rtp_connect/src/Encoder /mass/workspace/rtp_connect/src/Encoder /mass/workspace/rtp_connect/src/Encoder /mass/workspace/rtp_connect/src/Encoder /mass/workspace/rtp_connect/src/Encoder/CMakeFiles/RGB2H264.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/RGB2H264.dir/depend

