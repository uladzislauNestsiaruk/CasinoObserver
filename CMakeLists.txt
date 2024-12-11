cmake_minimum_required(VERSION 3.10)
project(CasinoObserver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/SFML_ARM/include)
link_directories(${CMAKE_SOURCE_DIR}/SFML_ARM/lib)

add_executable(${PROJECT_NAME} game_loop.cpp
                                textures_loader.cpp)
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window
                                      sfml-system sfml-audio)
