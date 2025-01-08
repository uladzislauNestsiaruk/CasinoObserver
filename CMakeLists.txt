cmake_minimum_required(VERSION 3.10)
project(CasinoObserver)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #for vim enjoyers

include(FetchContent)

#All Sources
set(SOURCES "")
#All include headers
set(HEADERS "")
#All libraries
set(LIBRARIES "")

#SFML linkage
set(HEADERS ${HEADERS} ${CMAKE_SOURCE_DIR}/SFML_ARM/include)
link_directories(${CMAKE_SOURCE_DIR}/SFML_ARM/lib)

#add_includes
set(HEADERS ${HEADERS} ${CMAKE_SOURCE_DIR}/utils)

#GameStates parsing
add_subdirectory(GameStates)
#Tables logic
add_subdirectory(Tables)
#Gamblers
add_subdirectory(Gamblers)
#Utils
add_subdirectory(Utils)

#All libraries and subdirectories of the project
set(LIBRARIES ${LIBRARIES} sfml-graphics sfml-window
              sfml-system sfml-audio)

if(CMAKE_BUILD_TYPE MATCHES "TESTING")
    #setup googletest
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_subdirectory(googletest EXCLUDE_FROM_ALL)
    enable_testing() 
    #Tests
    add_subdirectory(Tests)
    #link and include
    set(LIBRARIES ${LIBRARIES} gtest_main gmock_main)
    set(HEADERS ${HEADERS} ${gtest_SOURCE_DIR}/include ${gtest_BINARY_DIR})
    #Compile flags
    add_compile_options(-g3 -O0)
else()
    #I dont know where i can put them, so since know the called "TOP_LAYER"
    set(SOURCES ${SOURCES} game_loop.cpp)
    #Release/Debug target
    #Compilation flags
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:DEBUG>:-g3 -O0>
        $<$<CONFIG:RELEASE>:-O3 -march=native>
    )
endif()

#Compile sources
add_executable(${PROJECT_NAME} ${SOURCES})
#Include headers
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS})
#Link all
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBRARIES})
