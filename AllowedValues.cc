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
    if (_values[j]!=value)
      j++;
    else
      break;
  }

  if (j==_size)
    return false;

  while (j+1<_size) {
    std::swap(_values[j], _values[j+1]);
    j++;
  }
  _size--;
  return true;

}

bool AllowedValues::disallow_except(const uint8_t value) {

  if (!allowed(value))
    return false;

  auto j=0;
  while (j<_size) {
    if (_values[j]==value)
      break;
    else
      j++;
  }

  std::swap(_values[j], _values[0]);
  _size=1;

  return true;
}


bool AllowedValues::disallow_except(const uint8_t value1, const uint8_t value2) {
  if (!allowed(value1) || !allowed(value2))
    return false;

  auto j=0, k=0;
  while (j<_size) {
    if (_values[j]==value1 || _values[j]==value2) {
      std::swap(_values[j], _values[k]);
      k++;
      if (k==2)
        break;
    }
    j++;
  }
  _size=2;
  return true;
}
