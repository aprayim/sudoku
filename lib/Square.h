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
  explicit Square(uint8_t r, uint8_t c, uint8_t h) : _r(r), _c(c), _h(h), _value(0) {}

//basic getters
  uint8_t r() const {return _r;}
  uint8_t c() const {return _c;}
  uint8_t h() const {return _h;}
  uint8_t value() const {return _value;}
  AllowedValues allowed_values() const {return _allowed;}

//interface to allowed values
  bool allow(const uint8_t value) {return _allowed.allow(value);}
  bool allowed(const uint8_t value) const {return _allowed.allowed(value);}
  bool disallow(const uint8_t value) {return _allowed.disallow(value);}
  bool disallow_except(std::initializer_list<uint8_t> values) {return _allowed.disallow_except(values);}
  const uint8_t number_allowed() const {return _allowed.number_allowed();}
  const uint8_t allowed_at(const uint8_t idx) const {return _allowed.at(idx);}

//comparison to other squares
  bool same_allowed(const Square& sq);

//others
  bool is_value_set() const { return _value!=0;}
  bool set_value(const uint8_t value); //check if it can disallow except, then set value
  bool unset();

//friends
  friend bool operator==(const Square& sq1, const Square& sq2);
  friend std::ostream& operator<<(std::ostream& os, const Square& sq);

private:
  uint8_t _r, _c, _h;
  uint8_t _value;
  AllowedValues _allowed;
};
