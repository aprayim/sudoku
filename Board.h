#pragma once

#include "Square.h"
#include "Group.h"
#include "Board.h"

#include <iostream>
#include <fstream>
#include <memory>
#include <queue>
#include <stdint.h>
#include <array>

class Board {

public: //constructors
  Board() = delete;
  Board(const Board&) = delete;
  Board(Board&&) = delete;

  explicit Board(std::ifstream&);
  ~Board() {};

public: //solver
  bool solve();

public: //output
  friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
  std::array<std::shared_ptr<Group>, 9> _rows, _columns, _houses;
  std::array<std::shared_ptr<Square>, 81> _squares;

private: //solve functions
  bool process_square(std::shared_ptr<Square> sq);
  // all others go here like find singles, doubles etc etc
  bool naked_single();
  bool hidden_single();
  bool hidden_single_helper(std::shared_ptr<Group> group);

private: //utilities
  std::shared_ptr<Group> group_from_index(const uint8_t r, const uint8_t c, const Group::Type type);
  
};
