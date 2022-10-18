
#include <iostream>
#include <fstream>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "lib/Board.h"

DEFINE_string(valid_board, "", "filepath to valid board, can't be empty");
DEFINE_string(output_base, "", "output to filename, if empty outputs to stdout");
DEFINE_int32(n, 1, "number of puzzles generated");

int main(int argc, char* argv[]) {

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  LOG_IF(FATAL, FLAGS_valid_board.empty()) << "--valid_board can't be empty";
  LOG_IF(FATAL, FLAGS_output_base.empty()) << "--output_base can't be empty";
 
  try {
    std::random_device rd;
    std::mt19937 g(rd());
    auto valid_board = Board::createFromSimpleFile(FLAGS_valid_board);
    for (auto j=1; j<=FLAGS_n; j++) {
      auto puzzle = Board::createPuzzleFromValidBoard(*valid_board, g);
      std::ofstream ofs(FLAGS_output_base+std::to_string(j)+".sudoku");
      ofs << *puzzle;
      LOG(INFO) << "created puzzle " << j << std::endl;
    }
  }
  catch (std::exception& e) {
    LOG(FATAL) << "caught exception: " << e.what();
    return -1;
  }
  return 1;
} 
