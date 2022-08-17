//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//----------------------------------------------------------------------------
#include "src/ir/field_selector.h"

#include <string>

#include "absl/hash/hash_testing.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir {

// A list of "interesting" field names to feed into various tests below.
static const std::string sample_field_names[] = {
    "foo", "bar", "foobar", "baz", "k", "CapsCase",
};

class FieldSelectorTest : public ::testing::TestWithParam<std::string> {};

// Each FieldSelector should have a string representation that is just the
// field name from which it was constructed with a '.' prepended.
TEST_P(FieldSelectorTest, ToStringJustDotPlusFieldName) {
  const std::string field_name = GetParam();
  FieldSelector field_selector(field_name);
  const std::string field_selector_to_string = field_selector.ToString();
  ASSERT_TRUE(field_selector_to_string.at(0) == '.');
  ASSERT_TRUE(field_selector_to_string.substr(1) == field_name);
}

INSTANTIATE_TEST_SUITE_P(FieldSelectorTestSuite, FieldSelectorTest,
                         testing::ValuesIn(sample_field_names));

// Check that the hash function works as expected for at least the field
// names in sample_field_names.
TEST(FieldSelectorHashTest, FieldSelectorHashTest) {
  std::vector<FieldSelector> field_selectors_to_check;
  for (std::string field_name : sample_field_names) {
    field_selectors_to_check.push_back(FieldSelector(field_name));
  }
  EXPECT_TRUE(
      absl::VerifyTypeImplementsAbslHashCorrectly(field_selectors_to_check));
}

class FieldSelectorPairTest
    : public ::testing::TestWithParam<::std::tuple<std::string, std::string>> {
};

// For each pair of field names from sample_field_names, ensure that the two
// FieldSelectors constructed from those field names are equal only when the
// original field names are equal.
TEST_P(FieldSelectorPairTest, FieldSelectorEqualsTest) {
  std::string field_name1;
  std::string field_name2;
  std::tie(field_name1, field_name2) = GetParam();
  ASSERT_EQ(FieldSelector(field_name1) == FieldSelector(field_name2),
            field_name1 == field_name2);
}

INSTANTIATE_TEST_SUITE_P(
    FieldSelectorPairTestSuite, FieldSelectorPairTest,
    testing::Combine(testing::ValuesIn(sample_field_names),
                     testing::ValuesIn(sample_field_names)));

}  // namespace raksha::ir
