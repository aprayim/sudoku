#include "Square.h"

#include <iostream>

bool Square::set_value(const uint8_t value) {

  if (!_allowed.disallow_except(value)) {
    //std::cout << "unable to set value to: " << (unsigned)_value << std::endl;
    return false;
  }

  _value = value;
  //std::cout << "setting value to: " << (unsigned)_value << std::endl;

  return true;
}

std::ostream& operator<<(std::ostream& os, const Square& sq) {
  os << "r: " << (unsigned)sq._r << " c: " << (unsigned)sq._c << " value: " << (unsigned)sq._value;
  return os;
}
