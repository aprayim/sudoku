#include "AllowedValues.h"

#include <glog/logging.h>

#include <algorithm>

bool AllowedValues::allow(const uint8_t value) {

  LOG_IF(FATAL, value==0 || value>9) << " bad value: " << (unsigned)value;

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

  LOG_IF(FATAL, value==0 || value>9) << " bad value: " << (unsigned)value;

  for (auto j=0; j<_size; j++) {
    if (_values[j]==value)
      return true;
  }
  return false;
}

bool AllowedValues::disallow(const uint8_t value) {

  LOG_IF(FATAL, value==0 || value>9) << " bad value: " << (unsigned)value;

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

bool AllowedValues::disallow_except(std::initializer_list<uint8_t> values) {
  
  for (auto value : values)
    LOG_IF(FATAL, value==0 || value>9) << " bad value: " << (unsigned)value;

  for (auto value : values) {
    if (!allowed(value))
      return false;
  }

  if (_size == values.size())
    return false;//because nothing changes

  auto j=0, k=0;
  while (j<_size && k<values.size()) {
    auto k_th_element = *(values.begin()+k);
    if (_values[j] == k_th_element) {
      std::swap(_values[j], _values[k]);
      k++;
    }
    j++;
  }

  _size=k;
  return true;
}

std::ostream& operator<<(std::ostream& os, const AllowedValues& allowed) {
  for (auto j=0; j<allowed._size; j++)
    os << (unsigned)allowed._values[j] << " ";
  return os;
}
