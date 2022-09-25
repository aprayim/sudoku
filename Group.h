#pragma once

#include "AllowedValues.h"
#include "Square.h"
#include <stdint.h>
#include <array>

class Group {

public:
  enum Type {
    ROW=1,
    COLUMN=2,
    HOUSE=4,
  };

public:
//constuctors
  Group(const std::array<std::shared_ptr<Square>, 9>& squares, Type type, uint8_t idx) : _squares(squares), _type(type), _idx(idx) {}
  Group()=delete;
  Group(const Group&)=delete;
  Group(Group&&)=delete;

//utilities
  const std::array<std::shared_ptr<Square>, 9>& squares() const {return _squares;}
  const Type& type() const {return _type;}
  const uint8_t& idx() const {return _idx;}

private:
  std::array<std::shared_ptr<Square>, 9> _squares;
  Type _type;
  uint8_t _idx;
};
