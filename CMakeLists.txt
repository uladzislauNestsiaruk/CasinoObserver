cmake_minimum_required(VERSION 3.10)
project(CasinoObserver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #for vim enjoyers

#SFML linkage
include_directories(${CMAKE_SOURCE_DIR}/SFML_ARM/include)
link_directories(${CMAKE_SOURCE_DIR}/SFML_ARM/lib)

#add_includes
include_directories(${CMAKE_SOURCE_DIR}/utils)

#GameStates parsing
add_subdirectory(GameStates)
#Tables logic
add_subdirectory(Tables)
#Gamblers
add_subdirectory(Gamblers)

#All libraries and subdirectories of the project
set(LIBRARIES sfml-graphics sfml-window
              sfml-system sfml-audio
              game_states
              tables
              gamblers)

#I dont know where i can put them, so since know the called "TOP_LAYER"
set(TOP_LAYER_SOURCES game_loop.cpp
                      state_manager.cpp
                      textures_loader.cpp)

#Release target
add_executable(${PROJECT_NAME} ${TOP_LAYER_SOURCES})
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})

#Compilation flags
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:DEBUG>:-g3 -O0>
    $<$<CONFIG:RELEASE>:-O3 -march=native>
)
