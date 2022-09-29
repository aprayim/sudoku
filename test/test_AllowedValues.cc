#include "lib/AllowedValues.h"

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

TEST(AllowedValue, disallow_except) {
  AllowedValues a;
  ASSERT_EQ(a.disallow_except({3, 6}), true);
  ASSERT_EQ(a.disallow_except({2}), false);
  ASSERT_EQ(a.number_allowed(), 2);
  ASSERT_EQ(a.at(0), 3);
  ASSERT_EQ(a.at(1), 6);
}

TEST(AllowedValue, allow_order_preservation) {
  AllowedValues a;
  a.disallow_except({3, 6});
  ASSERT_EQ(a.allow(3), false);
  ASSERT_EQ(a.allow(1), true);
  ASSERT_EQ(a.number_allowed(), 3);
  ASSERT_EQ(a.at(0), 1);
  ASSERT_EQ(a.at(1), 3);
  ASSERT_EQ(a.allow(5), true);
  ASSERT_EQ(a.at(2), 5);
  ASSERT_EQ(a.at(3), 6);
  ASSERT_EQ(a.allow(9), true);
  ASSERT_EQ(a.at(4), 9);
  ASSERT_EQ(a.number_allowed(), 5);
}
