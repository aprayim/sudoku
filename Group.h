#pragma once

#include "AllowedValues.h"
#include "Square.h"
#include <stdint.h>
#include <array>

class Group {

public:
  enum Type {
    ROW,
    COLUMN,
    HOUSE
  };

public:
//constuctors
  Group(const std::array<std::shared_ptr<Square>, 9>& squares, Type type) : _squares(squares), _type(type) {}
  Group()=delete;
  Group(const Group&)=delete;
  Group(Group&&)=delete;

//utilities
  const std::array<std::shared_ptr<Square>, 9>& squares() const {return _squares;}

private:
  std::array<std::shared_ptr<Square>, 9> _squares;
  Type _type;
};
