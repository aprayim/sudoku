#include "Board.h"
#include "Group.h"

#include <fstream>
#include <iostream>

int main(const int argc, const char* argv[]) {

  if (argc==1) {
    std::cout << "no sudoku file given" << std::endl;
  }

  try
  {
    std::ifstream file(argv[1]);

    Board board(file);

    std::cout << "init board: " << std::endl;
    std::cout << board << std::endl;

    if (argc==3) {
      std::string action = argv[2];
      if (action=="hidden_pair")
      {
        auto solved = board.solve_hidden_pair(4, Group::Type::HOUSE);
        if (!solved)
          std::cout << "unable to find " << action << std::endl;
      }
      else if (action=="hidden_pair_split")
      {
        auto solved = board.solve_hidden_pair(5, Group::Type::COLUMN);
        if (!solved)
          std:: cout << "unable to find " << action << std::endl;
      }
    }
    else {
      auto solved = board.solve();
      std::cout << (solved ? "solved board: " : "unable to solve: ")  << std::endl;
      std::cout << board << std::endl;
    }
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
  } 

  return 1;
}
