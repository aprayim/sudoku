#include <iostream>
#include <fstream>

#include <glog/logging.h>
#include <gflags/gflags.h>

#include "lib/Board.h"

DEFINE_string(output_file, "", "output to filename, if empty outputs to stdout");

int main(int argc, char* argv[]) {

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  try {

    auto board = Board::createValidBoard(); 
    if (FLAGS_output_file.empty())
      std::cout << *board << std::endl;
    else {
      std::ofstream ofs(FLAGS_output_file);
      ofs << *board;
    }
    return 0;
  }
  catch (std::exception& e) {
    LOG(ERROR) << "caught exception: " << e.what();
    return -1;
  }

}
