#include "AllowedValues.h"

#include <algorithm>

bool AllowedValues::allowed(const uint8_t value) const {
  for (auto j=0; j<_size; j++) {
    if (_values[j]==value)
      return true;
  }
  return false;
}

bool AllowedValues::disallow(const uint8_t value) {

  auto j=0;
  while (j<_size) {
    if (_values[j]!=value) {
      j++;
      continue;
    }
    _size--;
    std::swap(_values[j], _values[_size]);
    return true;
  }
  return false;
}

bool AllowedValues::disallow_except(const uint8_t value) {

  if (!allowed(value))
    return false;

  auto j=0;
  while (j<_size) {
    if (_values[j]==value) {
      j++;
      continue;
    }
    _size--;
    std::swap(_values[j], _values[_size]);
  }

  return true;
}
