#include "Square.h"

#include <glog/logging.h>

#include <iostream>

bool Square::set_value(const uint8_t value) {

  if (!_allowed.disallow_except(value)) {
    LOG(ERROR) << "Square: " << _r << "," << _c << "unable to set value to: " << (unsigned)_value << std::endl;
    return false;
  }

  _value = value;

  return true;
}

bool Square::same_allowed(const Square& sq) {
  if (number_allowed()!=sq.number_allowed())
    return false;
  for (auto j=0; j<number_allowed(); j++) {
    if (allowed_at(j)!=sq.allowed_at(j))
      return false;
  }
  return true;
}

bool operator==(const Square& sq1, const Square& sq2) {
  return sq1._r==sq2._r && sq1._c==sq2._c;
}

std::ostream& operator<<(std::ostream& os, const Square& sq) {
  os << "r" << (unsigned)sq._r+1 << "c" << (unsigned)sq._c+1 << "h" << (unsigned)sq._h+1;
  return os;
}
