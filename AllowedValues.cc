#include "AllowedValues.h"

#include <glog/logging.h>

#include <algorithm>

bool AllowedValues::allow(const uint8_t value) {

  LOG_IF(FATAL, value==0 || value>9) << " bad value: " << value;

  auto j=0;
  while (j<_size) {
    if (_values[j]<value)
      j++;
    else if (_values[j]==value)
      return false;
    else
      break;
  }
  auto k=_size;
  while (k>j) {
    _values[k] = _values[k-1];
    k--;
  }
  _values[k] = value;
  _size++;
  return true;
}

bool AllowedValues::allowed(const uint8_t value) const {

  LOG_IF(FATAL, value==0 || value>9) << " bad value: " << value;

  for (auto j=0; j<_size; j++) {
    if (_values[j]==value)
      return true;
  }
  return false;
}

bool AllowedValues::disallow(const uint8_t value) {

  LOG_IF(FATAL, value==0 || value>9) << " bad value: " << value;

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

  LOG_IF(FATAL, value==0 || value>9) << " bad value: " << value;

  if (!allowed(value))
    LOG(FATAL) << "disallow_except: " << value << " not allowed.";

  //because no change
  if (_size==1)
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
  
  LOG_IF(FATAL, value1==0 || value1>9 || value2==0 || value2>9 ) << " bad values: " << value1 << " " << value2;

  if (!allowed(value1) || !allowed(value2))
    return false;

  if (_size==2)
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


std::ostream& operator<<(std::ostream& os, const AllowedValues& allowed) {
  for (auto j=0; j<allowed._size; j++)
    os << (unsigned)allowed._values[j] << " ";
  return os;
}
