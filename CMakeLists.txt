
# Set CMake Stuff
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}CMakeModules)

###
# Set project properties
project(WaveMatch)

###
# Perform build configuration of dependencies
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
set (LIBS ${LIBS} ${OpenCV_LIBS})

###
# Add the src directory so that we can make the project
add_subdirectory(src)

###
# Unit testing stuff
option(Riotest_UNIT_TESTS "Generate Riotest unit tests" OFF)
if(Riotest_UNIT_TESTS)
    add_subdirectory(Libraries/GTest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
    add_subdirectory(test)



endif()
