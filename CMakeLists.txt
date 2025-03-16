cmake_minimum_required(VERSION 3.10)
project(casino_observer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #for vim enjoyers and vs code too :(

include(FetchContent)

# All sources
set(SOURCES "")
# All tests sources
set(TESTS_SOURCES "")
# All include headers
set(HEADERS "")
# All libraries
set(LIBRARIES "")
# Compile options
set(OPTIONS -g3 -O0)

# SFML linkage
set(HEADERS ${HEADERS} ${CMAKE_SOURCE_DIR}/SFML_ARM/include)
link_directories(${CMAKE_SOURCE_DIR}/SFML_ARM/lib)

# Add_includes
set(HEADERS ${HEADERS} ${CMAKE_SOURCE_DIR}/utils)

# GameStates parsing
add_subdirectory(game_states)
# Tables logic
add_subdirectory(tables)
# Gamblers
add_subdirectory(gamblers)
# Utils
add_subdirectory(utils)
# Game object
add_subdirectory(game_object)

# All libraries and subdirectories of the project
set(LIBRARIES ${LIBRARIES} sfml-graphics sfml-window
              sfml-system sfml-audio)

if(CMAKE_BUILD_TYPE MATCHES "RELEASE")
    set(OPTIONS -O3 -march=native)
else()
    # Setup googletest
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(googletest EXCLUDE_FROM_ALL)
    enable_testing() 

    # Tests
    add_subdirectory(tests)

    #link and include gtest/gmock
    set(GTEST_LIBRARIES gtest_main gmock_main)
    set(GTEST_HEADERS ${gtest_SOURCE_DIR}/include ${gtest_BINARY_DIR})

    # Add heavy and light tests
    add_executable(light_tests ${SOURCES} ${LIGHT_TESTS_SOURCES})
    add_executable(heavy_tests ${SOURCES} ${HEAVY_TESTS_SOURCES})

    target_include_directories(light_tests PRIVATE ${HEADERS} ${GTEST_HEADERS})
    target_include_directories(heavy_tests PRIVATE ${HEADERS} ${GTEST_HEADERS})
    target_link_libraries(light_tests PRIVATE ${LIBRARIES} ${GTEST_LIBRARIES})
    target_link_libraries(heavy_tests PRIVATE ${LIBRARIES} ${GTEST_LIBRARIES})

    # Run light tests
    add_test(NAME LightTests COMMAND light_tests)
    add_custom_target(run_light_tests ALL
        COMMAND env GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND} --output-on-failure -VV
        DEPENDS light_tests)
endif()

# Targets
add_executable(${PROJECT_NAME} ${SOURCES} game_loop.cpp)

# Objects previewer
add_executable(preview ${SOURCES} game_object_preview.cpp)

# Link all
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
target_link_libraries(preview PRIVATE ${LIBRARIES})

# Include headers
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})
target_include_directories(preview PRIVATE ${HEADERS})

# Compile options
add_compile_options(${OPTIONS})
