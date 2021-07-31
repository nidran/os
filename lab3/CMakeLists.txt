cmake_minimum_required(VERSION 3.3)
project(testClion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main2.cpp
        ../lab4/data.cpp
        ../lab4/pte.cpp
        ../lab4/data.cpp
        ../lab4/helper.cpp)
add_executable(testClion ${SOURCE_FILES})