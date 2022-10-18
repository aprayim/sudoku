cmake_minimum_required(VERSION 3.10)

project(avram_sudoku VERSION 0.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(gflags)
add_subdirectory(glog)
include_directories (${gflags_INCLUDE_DIR})
include_directories (${glog_INCLUDE_DIR})

add_subdirectory(lib)
add_subdirectory(app)