cmake_minimum_required(VERSION 3.10)
project(CasinoObserver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #for vim enjoyers

include(FetchContent)

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
#Utils
add_subdirectory(Utils)

#All libraries and subdirectories of the project
set(LIBRARIES sfml-graphics sfml-window
              sfml-system sfml-audio 
              game_states
              tables
              gamblers
              utils)

if(CMAKE_BUILD_TYPE MATCHES "TESTING")
    #setup googletest
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(googletest EXCLUDE_FROM_ALL)
    enable_testing() 
    #Tests
    add_subdirectory(Tests)
    add_executable(${PROJECT_NAME} ${TESTS_SOURCES})
    #link and include
    set(LIBRARIES ${LIBRARIES} gtest_main gmock_main)
    target_include_directories(${PROJECT_NAME} PRIVATE ${gtest_SOURCE_DIR}/include ${gtest_BINARY_DIR})
else()
    #I dont know where i can put them, so since know the called "TOP_LAYER"
    set(TOP_LAYER_SOURCES game_loop.cpp
                      state_manager.cpp)
    #Release/Debug target
    add_executable(${PROJECT_NAME} ${TOP_LAYER_SOURCES})
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})

#Compilation flags
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:DEBUG>:-g3 -O0>
    $<$<CONFIG:TESTING>:-g3 -O0>
    $<$<CONFIG:RELEASE>:-O3 -march=native>
)
