file(GLOB_RECURSE GAME_STATES_SOURCES "*.cpp")

add_library(game_states STATIC ${GAME_STATES_SOURCES})
