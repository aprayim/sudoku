#pragma once

#include <stdint.h>
#include <array>

class AllowedValues {

public:
  AllowedValues()=default;
  AllowedValues(const AllowedValues&)=delete;
  AllowedValues(AllowedValues&&)=delete;

  bool allowed(const uint8_t value) const; // check if value is allowed
  bool disallow(const uint8_t value); //disallow value, return false if already disallowed
  bool disallow_except(const uint8_t value); // disallow all except value, return false is value already disallowed
  bool disallow_except(const uint8_t value, const uint8_t value2); // disallow all except values, return false is value already disallowed
  const uint8_t number_allowed() const {return _size;}
  const uint8_t at(const uint8_t idx) const {return _values[idx];}

private:
  std::array<uint8_t, 9> _values = {1,2,3,4,5,6,7,8,9};
  uint8_t _size{9};

};
