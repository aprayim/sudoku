#pragma once

#include <stdint.h>
#include <array>
#include <iostream>

class AllowedValues {

public:
  AllowedValues()=default;
  AllowedValues(const AllowedValues&)=delete;
  AllowedValues(AllowedValues&&)=delete;

  bool allow(const uint8_t value);//return false if value is already allowed
  bool allowed(const uint8_t value) const; // check if value is allowed
  bool disallow(const uint8_t value); //disallow value, return false if already disallowed
  bool disallow_except(std::initializer_list<uint8_t> values); // disallow all except value, return false if any of the values are already disallowed, values should be in increasing order
  const uint8_t number_allowed() const {return _size;}
  const uint8_t at(const uint8_t idx) const {return _values[idx];}

  friend std::ostream& operator<<(std::ostream& os, const AllowedValues& allowed);

private:
  std::array<uint8_t, 9> _values = {1,2,3,4,5,6,7,8,9};
  uint8_t _size{9};

};
