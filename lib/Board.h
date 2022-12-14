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
#include <filesystem>
#include <random>

class Board {

//static functions
public:
  static std::unique_ptr<Board> createFromSimpleFile(const std::filesystem::path& path);
  static std::unique_ptr<Board> createValidBoard();
  static std::unique_ptr<Board> createEmptyBoard();
  static std::unique_ptr<Board> createPuzzleFromValidBoard(const Board& valid_board, std::mt19937& g);
  static std::unique_ptr<Board> createPuzzle();

private:
  bool create_valid_board_helper(std::mt19937& g, const uint8_t idx=0);

private: //constructors
  Board();
  Board(const Board&);
  Board(Board&&) = delete;

  void build_internals();

public:
  ~Board() {};

public:
  void find_brute_force_solution(size_t& num_solutions_found, const size_t stop_at, const uint8_t sq_idx=0);

public: //solver
  bool solve_brute_force(uint8_t sq_idx=0);//finds the first solution
  bool solve();
  bool solve_hidden_pair(uint8_t idx, Group::Type type);
  bool solve_naked_triple(uint8_t idx, Group::Type type);
  bool solve_xwing();

public: //output
  friend std::ostream& operator<<(std::ostream& os, const Board& board);

private:
  std::array<std::shared_ptr<Group>, 9> _rows, _columns, _houses;
  std::array<std::shared_ptr<Square>, 81> _squares;

private: //solve functions
  bool adjust_from_square(std::shared_ptr<Square> sq, std::shared_ptr<std::vector<std::shared_ptr<Square>>> modified_squares=nullptr);
  //process all functions
  bool process_all_squares(bool (Board::*func)(std::shared_ptr<Square>));
  bool process_all_groups(bool (Board::*func)(std::shared_ptr<Group>));
  //individual processing
  bool naked_single_helper(std::shared_ptr<Square> square);
  bool hidden_single_helper(std::shared_ptr<Group> group);
  bool naked_pair_helper(std::shared_ptr<Group> group);
  bool locked_candidate_helper(std::shared_ptr<Group> group);
  bool pointing_tuple_helper(std::shared_ptr<Group> group);
  bool hidden_pair_helper(std::shared_ptr<Group> group);
  bool naked_triple_helper(std::shared_ptr<Group> group);
  bool xwing_helper();

private: //utilities
  static uint8_t rc_to_h(uint8_t r, uint8_t c) {return (r/3)*3+c/3;}
  std::shared_ptr<Group> get_group(uint8_t idx, Group::Type type);
};
