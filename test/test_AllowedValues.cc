#include "src/AllowedValues.h"

#include <gtest/gtest.h>

TEST(AllowedValue, Constructor) {
  const AllowedValues a;
  ASSERT_EQ(a.number_allowed(),9);
  for (auto j=0; j<9; j++) {
    ASSERT_EQ(a.allowed(j+1), true);
    ASSERT_EQ(a.at(j),j+1);
  }
}

TEST(AllowedValue, disallow_allow_basic) {
  AllowedValues a;
  for (auto j=1; j<9; j++) {
    ASSERT_EQ(a.allow(j), false);
    ASSERT_EQ(a.disallow(j), true);
    ASSERT_EQ(a.allowed(j), false);
    ASSERT_EQ(a.disallow(j), false);
    ASSERT_EQ(a.allow(j), true);
    ASSERT_EQ(a.allowed(j), true);
  }
}
