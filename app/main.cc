#include "lib/Board.h"
#include "lib/Group.h"

#include <glog/logging.h>
#include <gflags/gflags.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

DEFINE_bool(brute, false, "find solutions brute force");
DEFINE_bool(valid, false, "generate valid board");
DEFINE_bool(puzzle, false, "generate puzzle");
DEFINE_string(file, "", "file to read sudoku from");
DEFINE_string(action, "", "proposed action, comma separated values with additional variables");

int main(int argc, char* argv[]) {

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  try
  {
    if (FLAGS_puzzle) {
      auto board = Board::createPuzzle();
      std::cout << *board << std::endl;
      if (!board->solve()) {
        LOG(INFO) << "Unable to solve.";
      }
      else {
        std::cout << "solved\n" << *board << std::endl;
      }
    }
    if (FLAGS_brute) {
      auto board = Board::createEmptyBoard();
      std::cout << *board << std::endl;
      size_t num_solutions_found = 0;
      board->find_brute_force_solution(num_solutions_found, 5);
      std::cout << num_solutions_found << std::endl;
    }
    if (FLAGS_valid) {
      auto board = Board::createValidBoard();
      if (!board)
        LOG(ERROR) << "unable to create";
      else {
        LOG(INFO) << "created board";
        std::cout << *board << std::endl;
      }
    }
    if (!FLAGS_file.empty()) {
      auto board = Board::createFromSimpleFile(FLAGS_file);

      std::cout << "init board: " << std::endl;
      std::cout << *board << std::endl;

      if (FLAGS_action.empty()) {
        auto solved = board->solve();
        if (!solved) {
          size_t num_solutions_found=0;
          std::cout << "unable to solve, trying brute force";
          board = Board::createFromSimpleFile(FLAGS_file);
          board->find_brute_force_solution(num_solutions_found, 1);
          std::cout << "solution:\n" << *board << std::endl;
        } else {
          std::cout << "solved!\n" << *board << std::endl;
        }
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
          solved = board->solve_hidden_pair(std::stoi(action[1]), Group::string_to_group_type(action[2]));
        else if (action[0]=="naked_triple")
          solved = board->solve_naked_triple(std::stoi(action[1]), Group::string_to_group_type(action[2]));
        else if (action[0]=="xwing")
          solved = board->solve_xwing();
        else if (action[0]=="brute") {
          solved = board->solve_brute_force();
          std::cout << *board << std::endl;
        }
        else
          LOG(FATAL) << "unknown action: " << action[0];

        if (!solved)
          LOG(ERROR) << "Unable to find action: " << action[0];
      }
    }
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
  } 

  return 1;
}
