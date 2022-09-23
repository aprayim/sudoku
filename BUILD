cc_library(
    name = "sudoku-solver",
    srcs = [
            "AllowedValues.cc",
            "Board.cc",
            "Square.cc",
            ],
    hdrs = [
            "AllowedValues.h",
            "Board.h",
            "Group.h",
            "Square.h",
            ],
)

cc_binary(
    name = "sudoku-solver-test",
    srcs = [
            "main.cc",
            ],
    deps = [
            ":sudoku-solver"
            ],
)
