cmake_minimum_required(VERSION 3.10)

project(avram_sudoku VERSION 0.9)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(gflags)
add_subdirectory(glog)
include_directories (${gflags_INCLUDE_DIR})
include_directories (${glog_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(test)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)
enable_testing()