#include "Board.h"

#include <glog/logging.h>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <numeric>

Board::Board(std::ifstream& is) {

  for (auto r=0; r<9; r++) {
    for (auto c=0; c<9; c++) {
      _squares[r*9+c] = std::make_shared<Square>(r, c, rc_to_h(r, c));
    }
  }

  std::string line;
  std::vector<size_t> squares_to_process;
  auto r=0;
  while (getline(is, line)) {

    if (r>=9)
      throw std::length_error("too many rows");
    
    auto c=0;
    for (auto x : line)
    {
      if (c>=9)
        throw std::length_error("too many columns");
      
      uint8_t value = (uint8_t)(x-'0');
      if (value) {
        const auto idx = r*9+c;
        _squares[idx]->set_value(value);
        squares_to_process.push_back(idx);
      }
      c++;
    }
    r++;
  }

  //rows
  std::array<std::shared_ptr<Square>, 9> buffer;
  for (auto r=0; r<9; r++) {
    for (auto c=0; c<9; c++) {
      buffer[c] = _squares[r*9+c];
    } 
    _rows[r] = std::make_shared<Group>(buffer, Group::Type::ROW, r);
  } 
  //columns
  for (auto c=0; c<9; c++) {
    for (auto r=0; r<9; r++) {
      buffer[r] = _squares[r*9+c];
    }
    _columns[c] = std::make_shared<Group>(buffer, Group::Type::COLUMN, c);
  }
  //houses
  for (auto xr=0; xr<3; xr++) {
    for (auto xc=0; xc<3; xc++) {
      auto idx = 0;
      for (auto r=xr*3; r<(xr+1)*3; r++) {
        for (auto c=xc*3; c<(xc+1)*3; c++) {
          buffer[idx] = _squares[r*9+c];
          idx++;
        }
      }
      _houses[xr*3+xc] = std::make_shared<Group>(buffer, Group::Type::HOUSE, xr*3+xc);
    }
  }


  for (auto idx : squares_to_process) {
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
    if (process_all_groups(&Board::hidden_pair_helper)) {
      continue;
    }
    if (process_all_groups(&Board::naked_triple_helper)) {
      continue;
    }
    if (xwing_helper()) {
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

bool Board::solve_hidden_pair(uint8_t idx, Group::Type type) {
  return hidden_pair_helper(get_group(idx, type));
}

bool Board::solve_naked_triple(uint8_t idx, Group::Type type) {
  return naked_triple_helper(get_group(idx, type));
}

bool Board::solve_xwing() {
  return xwing_helper();
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
  LOG(INFO) << "naked single: " << *square << " value: " << (unsigned)value;
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
    LOG(INFO) << "hidden single: " << *square << " value: " << (unsigned)value;
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

      auto value1 = squares[j]->allowed_at(0);
      auto value2 = squares[j]->allowed_at(1);

      bool cur_activity=false;
      for (auto p=0; p<9; p++) {
        if (p==j || p==k)
          continue;
        cur_activity |= squares[p]->disallow(value1);
        cur_activity |= squares[p]->disallow(value2);
      }

      if (!cur_activity)
        continue;

      LOG(INFO) << (squares[j]->h()==squares[k]->h() ? "Naked pair: " : "Naked pair (split): ") << *(squares[j]) << " " << *(squares[k]) << " values: " << (unsigned)value1 << " " << (unsigned)value2;

      activity = true;
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
    bool cur_activity = false;
    auto value = idx+1;
    auto house = _houses[house_indices[idx]];
    for (auto sq : house->squares()) {
      if (group->type()==Group::Type::ROW && group->idx()==sq->r())
        continue;
      if (group->type()==Group::Type::COLUMN && group->idx()==sq->c())
        continue;
      if (!sq->disallow(value))
        continue;
      cur_activity = true;
    }
    if (!cur_activity)
      continue;
    activity = true;
    LOG(INFO) << "Locked candidate: " << *group << " " << *house << " value: " << (unsigned)value;
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

  auto process = [](std::array<std::shared_ptr<Group>, 9>& relevant_groups, uint8_t house_index,  std::array<uint8_t, 9>& indices) {
    bool activity = false;
    for (auto idx=0; idx<9; idx++) {
      if (indices[idx]>=9)
        continue;
      auto value = idx+1;
      auto group = relevant_groups[indices[idx]];
      bool cur_activity = false;
      for (auto sq : group->squares()) {
        if (sq->h()==house_index)
          continue;
        if (!sq->disallow(value))
          continue;
        cur_activity = true;
      }
      if (!cur_activity)
        continue;
      activity = true;
      LOG(INFO) << "Pointing Tuple: House " << (unsigned)house_index+1 << " " << *group << " value: " << (unsigned)value;
    }
    return activity;
  };

  activity |= process(_rows, group->idx(), row_indices);
  activity |= process(_columns, group->idx(), column_indices);
  return activity;
}

bool Board::hidden_pair_helper(std::shared_ptr<Group> group) {

  std::array<uint8_t, 9> first, second;
  first.fill(9);
  second.fill(9);

  for (auto k=0; k<9; k++) {
    auto sq = group->squares()[k];
    for (auto j=0; j<sq->number_allowed(); j++) {
      auto value = sq->allowed_at(j);
      auto idx = value-1;
      if (first[idx]==9)
        first[idx] = k;
      else if (second[idx]==9)
        second[idx] = k;
      else {
        first[idx]=10;
      }
    }
  }

  bool activity = false;
  for (auto p=0; p<9; p++) {
    if (first[p]>=9 || second[p]>=9)
      continue;
    for (auto q=p+1; q<9; q++) {
      if (first[p]!=first[q])
        continue;
      if (second[p]!=second[q])
        continue;

      bool cur_activity = false;
      auto value1=p+1;
      auto value2=q+1;
      auto sq1 = group->squares()[first[p]];
      auto sq2 = group->squares()[second[p]];

      cur_activity |= sq1->disallow_except(value1, value2);
      cur_activity |= sq2->disallow_except(value1, value2);

      auto disallow_from_group = [](std::shared_ptr<Group> g, const Square& sq1, const Square& sq2, uint8_t value1, uint8_t value2) {
        bool activity = false;
        for (auto sq : g->squares()) {
          if (*sq==sq1 || *sq==sq2)
            continue;
          activity |= sq->disallow(value1);
          activity |= sq->disallow(value2);
        }
        return activity;
      };
      
      //row
      if (sq1->r()==sq2->r()) {
        cur_activity |= disallow_from_group(_rows[sq1->r()], *sq1, *sq2, value1, value2);
      }
      //column
      if (sq1->c()==sq2->c()) {
        cur_activity |= disallow_from_group(_columns[sq1->c()], *sq1, *sq2, value1, value2);
      }
      //house
      if (sq1->h()==sq2->h()) {
        cur_activity |= disallow_from_group(_houses[sq1->h()], *sq1, *sq2, value1, value2);
      }
      
      if (!cur_activity)
        continue;
      activity = true;
      LOG(INFO) << (sq1->h()==sq2->h() ? "Hidden pair: " : "Hidden pair (split): ") << *sq1 << " " << *sq2 << ", values: " << value1 << "," << value2;
    }
  }
  return activity;
}

bool Board::naked_triple_helper(std::shared_ptr<Group> group) {

  auto update = [](std::array<uint8_t, 9>& dum, std::shared_ptr<Square> sq) {
    for (auto j=0; j<sq->number_allowed(); j++)
      dum[sq->allowed_at(j)-1]=1;
  };
  bool activity = false;
  auto squares = group->squares();
  for (auto j=0; j<9; j++) {
    if (squares[j]->is_value_set() || squares[j]->number_allowed()>3)
      continue;
    for (auto k=j+1; k<9; k++) {
      if (squares[k]->is_value_set() || squares[k]->number_allowed()>3)
        continue;
      for (auto l=k+1; l<9; l++) {
        if (squares[l]->is_value_set() || squares[l]->number_allowed()>3)
          continue;
        std::array<uint8_t, 9> dum;
        dum.fill(0);
        update(dum, squares[j]);
        update(dum, squares[k]);
        update(dum, squares[l]);
        if (std::accumulate(dum.begin(), dum.end(), 0)>3)
          continue;
        bool cur_activity = false;
        std::array<uint8_t, 3> values;
        auto vidx=0;
        for (auto idx=0; idx<9; idx++) {
          if (!dum[idx])
            continue;
          auto value = idx+1;
          values[vidx++] = value;
          for (auto x=0; x<9; x++) {
            if (x==j || x==k || x==l)
              continue;
            cur_activity |= squares[x]->disallow(value);
          }
        }
        if (!cur_activity)
          continue;
        activity = true;
        LOG(INFO) << "Naked Triple: " << *squares[j] << " " << *squares[k] << " " << *squares[l] << " values: " << (unsigned)values[0] << " " << (unsigned)values[1] << " " << (unsigned)values[2];
      }
    }
  }
  return activity;
}

//this is on the whole board
bool Board::xwing_helper() {

  auto fill_places = [](const uint8_t value, std::shared_ptr<Group> group, std::array<uint8_t, 9>& place) {
    place.fill(0);
    for (auto j=0; j<9; j++) {
      auto sq = group->squares()[j];
      if (sq->is_value_set())
        continue;
      if (!sq->allowed(value))
        continue;
      place[j] = 1;
    }
  };
  
  auto get_common_2 = [](const std::array<uint8_t, 9>& x, const std::array<uint8_t, 9>& y, std::array<uint8_t, 2>& indices) {
    auto good=0;
    for (auto j=0; j<9; j++) {
      if (x[j]==1 && y[j]==1) {
        if (good==2)
          return false;
        indices[good++] = j;
      }
      else if (x[j]==1 || y[j]==1)
        return false;
    }
    return good==2;
  };

  auto process = [&fill_places,&get_common_2](std::array<std::shared_ptr<Group>, 9> primary, std::array<std::shared_ptr<Group>, 9> secondary, bool primary_is_rows) {
    bool activity = false;
    for (auto value=1; value<=9; value++) {
      std::array<std::array<uint8_t, 9>, 9> places;
      for (auto x=0; x<9; x++)
        fill_places(value, primary[x], places[x]);
      for (auto j=0; j<9; j++) {
        for (auto k=j+1; k<9; k++) {
          std::array<uint8_t, 2> indices;
          if (!get_common_2(places[j], places[k], indices))
            continue;

          bool current_activity = false;
          for (auto i : indices) {
            auto g = primary_is_rows ? primary[j]->squares()[i]->c() : primary[j]->squares()[i]->r();
            auto group = secondary[g];
            for (auto p=0; p<9; p++) {
              if (p==j || p==k)
                continue;
              if (!group->squares()[p]->disallow(value))
                continue;
              current_activity = true;
            }
          }
          if (!current_activity)
            continue;
          LOG(INFO) << "X-Wing:" << (primary_is_rows ? " rows " : " columns ") << (unsigned)(j+1) << "," << (unsigned)(k+1) << (primary_is_rows ? " columns " : " rows ") <<  (unsigned)(indices[0]+1) << "," << (unsigned)(indices[1]+1) << " value " << value;
          activity = true;
        }
      }
    }
    return activity;
  };

  bool activity = false;
  activity |= process(_rows, _columns, /*primary_is_rows*/true);
  activity |= process(_columns, _rows, /*primary_is_rows*/false);
  return activity;
}

std::shared_ptr<Group> Board::get_group(uint8_t idx, Group::Type type) {
  idx--;
  switch (type) {
  case Group::Type::ROW:
    return _rows[idx];
  case Group::Type::COLUMN:
    return _columns[idx];
  case Group::Type::HOUSE:
    return _houses[idx];
  default:
    break;
  }
  return nullptr;
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

