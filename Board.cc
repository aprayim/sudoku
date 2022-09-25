#include "Board.h"

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

Board::Board(std::ifstream& is) {

  for (auto r=0; r<9; r++) {
    for (auto c=0; c<9; c++) {
      _squares[r*9+c] = std::make_shared<Square>(r, c, rc_to_h(r, c));
    }
  }

  //std::cout << "created squares" << std::endl;
  std::string line;
  std::vector<size_t> squares_to_process;
  auto r=0;
  while (getline(is, line)) {

    //std::cout << "line: " << line << std::endl;

    if (r>=9)
      throw std::length_error("too many rows");
    
    auto c=0;
    for (auto x : line)
    {
      if (c>=9)
        throw std::length_error("too many columns");
      
      uint8_t value = (uint8_t)(x-'0');
      if (value) {
        //std::cout << "r: " << r << " c: " << c << std::endl;
        const auto idx = r*9+c;
        _squares[idx]->set_value(value);
        squares_to_process.push_back(idx);
      }
      c++;
    }
    r++;
  }

  //std::cout << "setting up rows" << std::endl;
  //rows
  std::array<std::shared_ptr<Square>, 9> buffer;
  for (auto r=0; r<9; r++) {
    for (auto c=0; c<9; c++) {
      buffer[c] = _squares[r*9+c];
    } 
    _rows[r] = std::make_shared<Group>(buffer, Group::Type::ROW, r);
  } 
  //std::cout << "setting up columns" << std::endl;
  //columns
  for (auto c=0; c<9; c++) {
    for (auto r=0; r<9; r++) {
      buffer[r] = _squares[r*9+c];
    }
    _columns[c] = std::make_shared<Group>(buffer, Group::Type::COLUMN, c);
  }
  //std::cout << "setting up houses" << std::endl;
  //houses
  for (auto xr=0; xr<3; xr++) {
    for (auto xc=0; xc<3; xc++) {
      auto idx = 0;
      for (auto r=xr*3; r<(xr+1)*3; r++) {
        for (auto c=xc*3; c<(xc+1)*3; c++) {
          //std::cout << r << " " << c << " " << idx << std::endl;
          buffer[idx] = _squares[r*9+c];
          idx++;
        }
      }
      //std::cout << "building house " << xr << " " << xc << std::endl;
      _houses[xr*3+xc] = std::make_shared<Group>(buffer, Group::Type::HOUSE, xr*3+xc);
      //std::cout << "house built." << std::endl;
    }
  }

  //std::cout << "created groups" << std::endl;

  for (auto idx : squares_to_process) {
    //std::cout << "processing square: " << idx << std::endl;
    adjust_from_square(_squares[idx]);
  }
}

bool Board::solve() {

  bool activity=true;

  while (activity) {

    if (process_all_squares(&Board::naked_single_helper)) {
      continue;
    }
    if (process_all_groups(&Board::hidden_single_helper)) {
      continue;
    }
    if (process_all_groups(&Board::naked_pair_helper)) {
      continue;
    }
    if (process_all_groups(&Board::locked_candidate_helper)) {
      continue;
    }
    if (process_all_groups(&Board::pointing_tuple_helper)) {
      continue;
    }

    activity = false;
  }

  //here I need to check if all squares are filled up
  for (auto sq : _squares) {
    if (!sq->is_value_set())
      return false;
  }
  return true;
}

bool Board::adjust_from_square(std::shared_ptr<Square> square_to_process) {

  bool activity = false;

  auto r = square_to_process->r();
  auto c = square_to_process->c();
  auto value = square_to_process->value();

  auto row = _rows[r];
  auto column = _columns[c];
  auto house = _houses[rc_to_h(r, c)];

  std::array<std::shared_ptr<Group>, 3> groups = {row, column, house};
  for (auto group : groups) {
    auto squares = group->squares();
    for (auto square : squares) {
      activity |= square->disallow(value);
    }
  }

  return activity;
}

//check each square if it has only a single option
bool Board::naked_single_helper(std::shared_ptr<Square> square) {
  if (square->is_value_set())
    return false;
  if (square->number_allowed() > 1)
    return false;
  auto value = square->allowed_at(0);
  if (!square->set_value(value))
    return false;
  std::cout << "naked single: " << *square << std::endl;
  return adjust_from_square(square);
}

bool Board::hidden_single_helper(std::shared_ptr<Group> group) {
  
  bool activity = false;
  auto squares = group->squares();
  std::array<std::shared_ptr<Square>, 9> result = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
  std::array<uint8_t, 9> counts = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (auto square : squares) {
    for (auto j=0; j<square->number_allowed(); j++) {
      auto idx = square->allowed_at(j)-1;
      counts[idx]++;
      if (counts[idx]==1)
        result[idx] = square;
      else
        result[idx] = nullptr;
    }
  }
  for (auto j=0; j<9; j++) {
    if (!result[j])
      continue;
    auto value = j+1;
    auto square = result[j];
    square->set_value(value);
    std::cout << "hidden single: " << *square << std::endl;
    activity |= adjust_from_square(square);
  }
  return activity;
}

bool Board::naked_pair_helper(std::shared_ptr<Group> group) {

  bool activity = false;
  auto squares = group->squares();
  for (auto j=0; j<squares.size(); j++) {
    if (squares[j]->number_allowed()!=2)
      continue;
    for (auto k=j+1; k<squares.size(); k++) {
      if (!squares[j]->same_allowed(*(squares[k])))
        continue;

      std::cout << "Naked pair: " << *(squares[j]) << " " << *(squares[k]) << std::endl;
      
      for (auto p=0; p<9; p++) {
        if (p==j || p==k)
          continue;
        activity |= squares[p]->disallow(squares[j]->allowed_at(0));
        activity |= squares[p]->disallow(squares[j]->allowed_at(1));
      }
    }
  }
  return activity;
}

bool Board::locked_candidate_helper(std::shared_ptr<Group> group) {

  if (group->type()==Group::Type::HOUSE)
    return false;

  bool activity = false;
  std::array<uint8_t, 9> house_indices;
  house_indices.fill(9);
  for (auto sq : group->squares()) {
    if (sq->is_value_set())
      continue;
    for (auto j=0; j<sq->number_allowed(); j++) {
      auto value = sq->allowed_at(j);
      auto idx = value-1;
      if (house_indices[idx]==9)
        house_indices[idx] = sq->h();
      else if (house_indices[idx] != sq->h())
        house_indices[idx] = 10;
    }
  }

  for (auto idx=0; idx<9; idx++) {
    if (house_indices[idx]>=9)
      continue;
    auto value = idx+1;
    auto house = _houses[house_indices[idx]];
    for (auto sq : house->squares()) {
      if (group->type()==Group::Type::ROW && group->idx()==sq->r())
        continue;
      if (group->type()==Group::Type::COLUMN && group->idx()==sq->c())
        continue;
      if (!sq->disallow(value))
        continue;
      activity = true;
      std::cout << "locked candidate: removed " << value << " from " << *sq << std::endl;
    }
  }

  return activity;
}

bool Board::pointing_tuple_helper(std::shared_ptr<Group> group) {

  if (group->type() != Group::Type::HOUSE)
    return false;

  auto update = [](uint8_t idx, uint8_t j, std::array<uint8_t, 9>& indices) {
    if (indices[idx]==9)
      indices[idx]=j;
    else if (indices[idx]!=j)
      indices[idx] = 10;
  };

  bool activity = false;

  std::array<uint8_t, 9> column_indices, row_indices;
  column_indices.fill(9);
  row_indices.fill(9);
  for (auto sq : group->squares()) {
    if (sq->is_value_set())
      continue;
    for (auto j=0; j<sq->number_allowed(); j++) {
      auto value = sq->allowed_at(j);
      auto idx = value-1;
      update(idx, sq->r(), row_indices);
      update(idx, sq->c(), column_indices);
    }
  }

  //for rows
  for (auto idx=0; idx<9; idx++) {
    if (row_indices[idx]>=9)
      continue;
    auto value = idx+1;
    auto row = _rows[row_indices[idx]];
    for (auto sq : row->squares()) {
      if (sq->h()==group->idx())
        continue;
      if (!sq->disallow(value))
        continue;
      std::cout << "Pointing Tuple: removed "  << value << " from " << *sq << std::endl;
      activity = true;
    }
  }

  //for columns
  for (auto idx=0; idx<9; idx++) {
    if (column_indices[idx]>=9)
      continue;
    auto value = idx+1;
    auto column = _columns[column_indices[idx]];
    for (auto sq : column->squares()) {
      if (sq->h()==group->idx())
        continue;
      if (!sq->disallow(value))
        continue;
      std::cout << "Pointing Tuple: removed " << value << " from " << *sq << std::endl;
      activity = true;
    }
  }
  return activity;
}

bool Board::process_all_squares(bool (Board::*helper)(std::shared_ptr<Square>)) {
  bool activity = false;
  for (auto square : _squares)
    activity |= (this->*helper)(square);
  return activity;
}

//check each row/column/house to look for singles
bool Board::process_all_groups(bool (Board::*helper)(std::shared_ptr<Group>)) {
  bool activity = false; 
  for (auto row : _rows) { 
    activity |= (this->*helper)(row);
  }
  for (auto column : _columns) {
    activity |= (this->*helper)(column);
  }
  for (auto house : _houses) {
    activity |= (this->*helper)(house);
  }
  return activity;
}

std::ostream& operator<<(std::ostream& os, const Board& board)
{
  for (auto r=0; r<9; r++) {
    auto delim="";
    for (auto c=0; c<9; c++) {
      const unsigned val = board._squares[r*9+c]->value();
      if (val)
        os << delim << val;
      else
        os << delim << "_";
      delim=",";
    }
    os << std::endl;
  }
  return os;
}

