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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.28.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.28.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/uladzislaunestsiaruk/Projects/CasinoObserver

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE

# Include any dependencies generated for this target.
include CMakeFiles/CasinoObserver.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/CasinoObserver.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/CasinoObserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CasinoObserver.dir/flags.make

CMakeFiles/CasinoObserver.dir/game_loop.cpp.o: CMakeFiles/CasinoObserver.dir/flags.make
CMakeFiles/CasinoObserver.dir/game_loop.cpp.o: /Users/uladzislaunestsiaruk/Projects/CasinoObserver/game_loop.cpp
CMakeFiles/CasinoObserver.dir/game_loop.cpp.o: CMakeFiles/CasinoObserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/CasinoObserver.dir/game_loop.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CasinoObserver.dir/game_loop.cpp.o -MF CMakeFiles/CasinoObserver.dir/game_loop.cpp.o.d -o CMakeFiles/CasinoObserver.dir/game_loop.cpp.o -c /Users/uladzislaunestsiaruk/Projects/CasinoObserver/game_loop.cpp

CMakeFiles/CasinoObserver.dir/game_loop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CasinoObserver.dir/game_loop.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/uladzislaunestsiaruk/Projects/CasinoObserver/game_loop.cpp > CMakeFiles/CasinoObserver.dir/game_loop.cpp.i

CMakeFiles/CasinoObserver.dir/game_loop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CasinoObserver.dir/game_loop.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/uladzislaunestsiaruk/Projects/CasinoObserver/game_loop.cpp -o CMakeFiles/CasinoObserver.dir/game_loop.cpp.s

CMakeFiles/CasinoObserver.dir/state_manager.cpp.o: CMakeFiles/CasinoObserver.dir/flags.make
CMakeFiles/CasinoObserver.dir/state_manager.cpp.o: /Users/uladzislaunestsiaruk/Projects/CasinoObserver/state_manager.cpp
CMakeFiles/CasinoObserver.dir/state_manager.cpp.o: CMakeFiles/CasinoObserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/CasinoObserver.dir/state_manager.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CasinoObserver.dir/state_manager.cpp.o -MF CMakeFiles/CasinoObserver.dir/state_manager.cpp.o.d -o CMakeFiles/CasinoObserver.dir/state_manager.cpp.o -c /Users/uladzislaunestsiaruk/Projects/CasinoObserver/state_manager.cpp

CMakeFiles/CasinoObserver.dir/state_manager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CasinoObserver.dir/state_manager.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/uladzislaunestsiaruk/Projects/CasinoObserver/state_manager.cpp > CMakeFiles/CasinoObserver.dir/state_manager.cpp.i

CMakeFiles/CasinoObserver.dir/state_manager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CasinoObserver.dir/state_manager.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/uladzislaunestsiaruk/Projects/CasinoObserver/state_manager.cpp -o CMakeFiles/CasinoObserver.dir/state_manager.cpp.s

CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o: CMakeFiles/CasinoObserver.dir/flags.make
CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o: /Users/uladzislaunestsiaruk/Projects/CasinoObserver/textures_loader.cpp
CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o: CMakeFiles/CasinoObserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o -MF CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o.d -o CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o -c /Users/uladzislaunestsiaruk/Projects/CasinoObserver/textures_loader.cpp

CMakeFiles/CasinoObserver.dir/textures_loader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/CasinoObserver.dir/textures_loader.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/uladzislaunestsiaruk/Projects/CasinoObserver/textures_loader.cpp > CMakeFiles/CasinoObserver.dir/textures_loader.cpp.i

CMakeFiles/CasinoObserver.dir/textures_loader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/CasinoObserver.dir/textures_loader.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/uladzislaunestsiaruk/Projects/CasinoObserver/textures_loader.cpp -o CMakeFiles/CasinoObserver.dir/textures_loader.cpp.s

# Object files for target CasinoObserver
CasinoObserver_OBJECTS = \
"CMakeFiles/CasinoObserver.dir/game_loop.cpp.o" \
"CMakeFiles/CasinoObserver.dir/state_manager.cpp.o" \
"CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o"

# External object files for target CasinoObserver
CasinoObserver_EXTERNAL_OBJECTS =

CasinoObserver: CMakeFiles/CasinoObserver.dir/game_loop.cpp.o
CasinoObserver: CMakeFiles/CasinoObserver.dir/state_manager.cpp.o
CasinoObserver: CMakeFiles/CasinoObserver.dir/textures_loader.cpp.o
CasinoObserver: CMakeFiles/CasinoObserver.dir/build.make
CasinoObserver: GameStates/libgame_states.a
CasinoObserver: CMakeFiles/CasinoObserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable CasinoObserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CasinoObserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CasinoObserver.dir/build: CasinoObserver
.PHONY : CMakeFiles/CasinoObserver.dir/build

CMakeFiles/CasinoObserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CasinoObserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CasinoObserver.dir/clean

CMakeFiles/CasinoObserver.dir/depend:
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/uladzislaunestsiaruk/Projects/CasinoObserver /Users/uladzislaunestsiaruk/Projects/CasinoObserver /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/CMakeFiles/CasinoObserver.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/CasinoObserver.dir/depend

