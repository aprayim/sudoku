#include "Board.h"

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

    auto solved = board.solve();

    std::cout << (solved ? "solved board: " : "unable to solve: ")  << std::endl;
    std::cout << board << std::endl;
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
  } 

  return 1;
}
