#file(GLOB HEADERS "*.h")
add_library(board SHARED 
    AllowedValues.cc
    Square.cc
    Group.cc
    Board.cc
)
target_link_libraries(board gflags glog)