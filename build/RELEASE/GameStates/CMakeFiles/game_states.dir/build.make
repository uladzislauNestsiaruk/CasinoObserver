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
include GameStates/CMakeFiles/game_states.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include GameStates/CMakeFiles/game_states.dir/compiler_depend.make

# Include the progress variables for this target.
include GameStates/CMakeFiles/game_states.dir/progress.make

# Include the compile flags for this target's objects.
include GameStates/CMakeFiles/game_states.dir/flags.make

GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.o: GameStates/CMakeFiles/game_states.dir/flags.make
GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.o: /Users/uladzislaunestsiaruk/Projects/CasinoObserver/GameStates/blackjack_close.cpp
GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.o: GameStates/CMakeFiles/game_states.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.o"
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.o -MF CMakeFiles/game_states.dir/blackjack_close.cpp.o.d -o CMakeFiles/game_states.dir/blackjack_close.cpp.o -c /Users/uladzislaunestsiaruk/Projects/CasinoObserver/GameStates/blackjack_close.cpp

GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/game_states.dir/blackjack_close.cpp.i"
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/uladzislaunestsiaruk/Projects/CasinoObserver/GameStates/blackjack_close.cpp > CMakeFiles/game_states.dir/blackjack_close.cpp.i

GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/game_states.dir/blackjack_close.cpp.s"
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/uladzislaunestsiaruk/Projects/CasinoObserver/GameStates/blackjack_close.cpp -o CMakeFiles/game_states.dir/blackjack_close.cpp.s

# Object files for target game_states
game_states_OBJECTS = \
"CMakeFiles/game_states.dir/blackjack_close.cpp.o"

# External object files for target game_states
game_states_EXTERNAL_OBJECTS =

GameStates/libgame_states.a: GameStates/CMakeFiles/game_states.dir/blackjack_close.cpp.o
GameStates/libgame_states.a: GameStates/CMakeFiles/game_states.dir/build.make
GameStates/libgame_states.a: GameStates/CMakeFiles/game_states.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libgame_states.a"
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates && $(CMAKE_COMMAND) -P CMakeFiles/game_states.dir/cmake_clean_target.cmake
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/game_states.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
GameStates/CMakeFiles/game_states.dir/build: GameStates/libgame_states.a
.PHONY : GameStates/CMakeFiles/game_states.dir/build

GameStates/CMakeFiles/game_states.dir/clean:
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates && $(CMAKE_COMMAND) -P CMakeFiles/game_states.dir/cmake_clean.cmake
.PHONY : GameStates/CMakeFiles/game_states.dir/clean

GameStates/CMakeFiles/game_states.dir/depend:
	cd /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/uladzislaunestsiaruk/Projects/CasinoObserver /Users/uladzislaunestsiaruk/Projects/CasinoObserver/GameStates /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates /Users/uladzislaunestsiaruk/Projects/CasinoObserver/build/RELEASE/GameStates/CMakeFiles/game_states.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : GameStates/CMakeFiles/game_states.dir/depend

