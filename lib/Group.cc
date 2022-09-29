#include "Group.h"

#include <glog/logging.h>

Group::Type Group::string_to_group_type(const std::string& s) {
  if (s=="HOUSE")
    return Group::Type::HOUSE;
  else if (s=="COLUMN")
    return Group::Type::COLUMN;
  else if (s=="ROW")
    return Group::Type::ROW;
  else
    LOG(FATAL) << "Unknown type name: " << s;
  return Group::Type::HOUSE;
}

std::ostream& operator<<(std::ostream& os, const Group::Type& type) {
  switch (type) {
  case Group::Type::ROW:
    os << "Row";
    break;
  case Group::Type::COLUMN:
    os << "Column";
    break;
  case Group::Type::HOUSE:
    os << "House";
    break;
  default:
    os << "UNKNOWN GROUP";
    break;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Group& group) {
  os << group._type << " " << (unsigned)(group._idx+1);
  return os;
}
