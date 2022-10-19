macro(add_app name)
    add_executable(${name} ${name}.cc)
    target_link_libraries(${name} board gflags glog)
endmacro()

add_app(main)
add_app(puzzle_generator)
add_app(valid_board)