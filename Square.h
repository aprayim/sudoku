#pragma once

#include "AllowedValues.h"

#include <stdint.h>
#include <array>
#include <iostream>

class Square {
public:
//constructors
  Square() = delete;
  Square(const Square&) = delete;
  Square(Square&&) = delete;
  explicit Square(int r, int c) : _r(r), _c(c), _value(0) {}

//basic getters
  uint8_t r() const {return _r;}
  uint8_t c() const {return _c;}
  uint8_t value() const {return _value;}

//interface to allowed values
  bool allowed(const uint8_t value) const {return _allowed.allowed(value);}
  bool disallow(const uint8_t value) {return _allowed.disallow(value);}
  bool disallow_except(const uint8_t value) {return _allowed.disallow_except(value);}
  const uint8_t number_allowed() const {return _allowed.number_allowed();}
  const uint8_t allowed_at(const uint8_t idx) const {return _allowed.at(idx);}

//others
  bool is_value_set() const { return _value!=0;}
  bool set_value(const uint8_t value); //check if it can disallow except, then set value

//output
  friend std::ostream& operator<<(std::ostream& os, const Square& sq);

private:
  uint8_t _r, _c;
  uint8_t _value;
  AllowedValues _allowed;
};
