cc_library(
    name = "sudoku-solver",
    srcs = [
            "AllowedValues.cc",
            "Square.cc",
            "Group.cc",
            "Board.cc",
            ],
    hdrs = [
            "AllowedValues.h",
            "Square.h",
            "Group.h",
            "Board.h",
            ],
    deps = [
            "@com_github_google_glog//:glog",
            ],
    visibility = [
            "//src/test:__pkg__",
            ]
)

cc_binary(
    name = "sudoku-solver-test",
    srcs = [
            "main.cc",
            ],
    deps = [
            ":sudoku-solver",
            "@com_github_gflags_gflags//:gflags",
            ],
)

