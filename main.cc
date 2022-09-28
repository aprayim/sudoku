#include "Board.h"
#include "Group.h"
#include "AllowedValues.h"

#include <glog/logging.h>
#include <gflags/gflags.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

DEFINE_string(file, "", "file to read sudoku from");
DEFINE_string(action, "", "proposed action, comma separated values with additional variables");
DEFINE_bool(allowed_values_test, false, "test allowed values");

int main(int argc, char* argv[]) {

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  if (FLAGS_allowed_values_test) {
    AllowedValues a;
    std::cout << a << std::endl;
    a.disallow_except(5);
    std::cout << a << std::endl;
    a.allow(3);
    std::cout << a << std::endl;
    a.disallow(5);
    std::cout << a << std::endl;
    a.allow(5);
    std::cout << a << std::endl;
    a.allow(7);
    std::cout << a << std::endl;
    a.allow(1);
    std::cout << a << std::endl;
  }

  try
  {
    std::ifstream file(FLAGS_file);

    Board board(file);

    std::cout << "init board: " << std::endl;
    std::cout << board << std::endl;

    if (FLAGS_action.empty()) {
      auto solved = board.solve();
      std::cout << (solved ? "solved board: " : "unable to solve: ")  << std::endl;
      std::cout << board << std::endl;
    }
    else {
      auto split_to_vector = [](const std::string& s, char delim=',') {
        std::vector<std::string> out;
        std::stringstream ss;
        ss << s;
        std::string dum;
        while (std::getline(ss, dum, delim))
          out.push_back(dum);
        return out;
      };

      auto action = split_to_vector(FLAGS_action);
      bool solved = false;
      if (action[0]=="hidden_pair" || action[0]=="hidden_pair_split")
        solved = board.solve_hidden_pair(std::stoi(action[1]), Group::string_to_group_type(action[2]));
      else if (action[0]=="naked_triple")
        solved = board.solve_naked_triple(std::stoi(action[1]), Group::string_to_group_type(action[2]));
      else if (action[0]=="xwing")
        solved = board.solve_xwing();
      else if (action[0]=="brute") {
        solved = board.solve_brute_force();
        std::cout << board << std::endl;
      }
      else
        LOG(FATAL) << "unknown action: " << action[0];

      if (!solved)
        LOG(ERROR) << "Unable to find action: " << action[0];
    }
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
  } 

  return 1;
}
