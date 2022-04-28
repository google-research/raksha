#include "src/ir/access_path_selectors_set.h"

#include "absl/container/btree_set.h"
#include "absl/container/flat_hash_set.h"
#include "absl/strings/str_cat.h"
#include "src/common/testing/gtest.h"
#include "src/ir/field_selector.h"
#include "src/ir/selector.h"

namespace raksha::ir {

// Sample Selectors for use in tests.
static const Selector kFooFieldSelector = Selector(FieldSelector("foo"));
static const Selector kBarFieldSelector = Selector(FieldSelector("bar"));
static const Selector kBazFieldSelector = Selector(FieldSelector("baz"));

// Single element AccessPathSelectors for use in tests.
static const AccessPathSelectors kFooFieldSelectorPath(kFooFieldSelector);
static const AccessPathSelectors kBarFieldSelectorPath(kBarFieldSelector);
static const AccessPathSelectors kBazFieldSelectorPath(kBazFieldSelector);

// An array of selectors to use as test inputs.
static const Selector sample_selectors[] = {
    kFooFieldSelector, kBarFieldSelector, kBarFieldSelector};

// Vectors of AccessPathSelectors to use as test inputs.
static const std::vector<AccessPathSelectors> sample_path_vecs[] = {
    {},
    {kFooFieldSelectorPath},
    {kFooFieldSelectorPath, kFooFieldSelectorPath},
    {kFooFieldSelectorPath, kBarFieldSelectorPath, kBazFieldSelectorPath,
     kBarFieldSelectorPath, kBazFieldSelectorPath, kFooFieldSelectorPath},
    {AccessPathSelectors(kFooFieldSelector, kBazFieldSelectorPath),
     kBarFieldSelectorPath},
    {AccessPathSelectors(kFooFieldSelector, kBazFieldSelectorPath),
     AccessPathSelectors(kBarFieldSelector, kBarFieldSelectorPath)}};

class CreateAbslSetTest
    : public ::testing::TestWithParam<std::vector<AccessPathSelectors>> {};

// It should be the case that creating an AccessPathSelectorsSet from a group
// of AccessPathSelectors and then making an absl set from that should be
// equivalent to just making the absl set from that group of
// AccessPathSelectors in the first place.
TEST_P(CreateAbslSetTest, CreateAbslSetTest) {
  std::vector<AccessPathSelectors> param = GetParam();
  ASSERT_EQ(
      AccessPathSelectorsSet::CreateAbslSet(AccessPathSelectorsSet(param)),
      absl::flat_hash_set<AccessPathSelectors>(param.begin(), param.end()));
}

INSTANTIATE_TEST_SUITE_P(CreateAbslSetTest, CreateAbslSetTest,
                         testing::ValuesIn(sample_path_vecs));

class AddParentTest
    : public ::testing::TestWithParam<
          std::tuple<Selector, std::vector<AccessPathSelectors>>> {};

// Adding a parent selector to a set should be equivalent to prepending the
// parent selector's string representation to the front of the string
// representation of all access paths in that set.
TEST_P(AddParentTest, AddParentTest) {
  std::tuple<Selector, std::vector<AccessPathSelectors>> info = GetParam();
  Selector new_parent_selector = std::get<0>(info);
  AccessPathSelectorsSet original_set =
      AccessPathSelectorsSet(std::get<1>(info));
  absl::flat_hash_set<AccessPathSelectors> absl_original_set =
      AccessPathSelectorsSet::CreateAbslSet(original_set);

  AccessPathSelectorsSet set_with_parent =
      AccessPathSelectorsSet::AddParentToAll(new_parent_selector, original_set);
  absl::flat_hash_set<AccessPathSelectors> absl_set_with_parent =
      AccessPathSelectorsSet::CreateAbslSet(set_with_parent);

  // Adding a parent should not affect the size.
  ASSERT_EQ(absl_original_set.size(), absl_set_with_parent.size());

  // For each string generated from AccessPathSelectors in the original set,
  // prepending the selector string should produce a string in the new set.
  absl::flat_hash_set<std::string> set_with_parent_strings;
  for (const AccessPathSelectors &path : absl_set_with_parent) {
    set_with_parent_strings.insert(path.ToString());
  }

  for (const AccessPathSelectors &orig_path : absl_original_set) {
    std::string expected_parent_str =
        absl::StrCat(new_parent_selector.ToString(), orig_path.ToString());
    ASSERT_TRUE(set_with_parent_strings.contains(expected_parent_str));
  }
}

INSTANTIATE_TEST_SUITE_P(AddParentTest, AddParentTest,
                         testing::Combine(testing::ValuesIn(sample_selectors),
                                          testing::ValuesIn(sample_path_vecs)));

class UnionTest
    : public ::testing::TestWithParam<std::tuple<
          std::vector<AccessPathSelectors>, std::vector<AccessPathSelectors>>> {
};

absl::btree_set<std::string> MakeOrderedStrSet(
    AccessPathSelectorsSet orig_set) {
  absl::flat_hash_set<AccessPathSelectors> absl_hash_set =
      AccessPathSelectorsSet::CreateAbslSet(std::move(orig_set));
  absl::btree_set<std::string> absl_ordered_str_set;
  for (const AccessPathSelectors &path : absl_hash_set) {
    absl_ordered_str_set.insert(path.ToString());
  }

  return absl_ordered_str_set;
}

// Unioning two sets and calling ToString on each element should produce the
// same set of string representations result as creating tree sets of
// the string representations of the contents of the two sets and merging
// them.
TEST_P(UnionTest, UnionTest) {
  std::tuple<std::vector<AccessPathSelectors>, std::vector<AccessPathSelectors>>
      info = GetParam();
  AccessPathSelectorsSet set1 = AccessPathSelectorsSet(std::get<0>(info));
  AccessPathSelectorsSet set2 = AccessPathSelectorsSet(std::get<1>(info));

  absl::btree_set<std::string> str_set1 = MakeOrderedStrSet(set1);
  absl::btree_set<std::string> str_set2 = MakeOrderedStrSet(set2);
  absl::btree_set<std::string> unioned_str_set = MakeOrderedStrSet(
      AccessPathSelectorsSet::Union(std::move(set1), std::move(set2)));

  str_set1.merge(str_set2);
  ASSERT_EQ(unioned_str_set, str_set1);
}

INSTANTIATE_TEST_SUITE_P(UnionTestSuite, UnionTest,
                         testing::Combine(testing::ValuesIn(sample_path_vecs),
                                          testing::ValuesIn(sample_path_vecs)));

class IntersectionTest
    : public ::testing::TestWithParam<std::tuple<
          std::vector<AccessPathSelectors>, std::vector<AccessPathSelectors>>> {
};

// Intersecting two sets and calling ToString on each element should produce the
// same set of string representations result as creating tree sets of
// the string representations of the contents of the two sets and intersecting
// them.
TEST_P(IntersectionTest, IntersectionTest) {
  std::tuple<std::vector<AccessPathSelectors>, std::vector<AccessPathSelectors>>
      info = GetParam();

  AccessPathSelectorsSet set1 = AccessPathSelectorsSet(std::get<0>(info));
  AccessPathSelectorsSet set2 = AccessPathSelectorsSet(std::get<1>(info));

  absl::btree_set<std::string> str_set1 = MakeOrderedStrSet(set1);
  absl::btree_set<std::string> str_set2 = MakeOrderedStrSet(set2);
  absl::btree_set<std::string> intersected_str_set =
      MakeOrderedStrSet(AccessPathSelectorsSet::Intersect(set1, set2));

  absl::btree_set<std::string> std_set_intersection_result;
  std::set_intersection(str_set1.begin(), str_set1.end(), str_set2.begin(),
                        str_set2.end(),
                        std::inserter(std_set_intersection_result,
                                      std_set_intersection_result.begin()));

  ASSERT_EQ(intersected_str_set, std_set_intersection_result);
}

INSTANTIATE_TEST_SUITE_P(IntersectionTestSuite, IntersectionTest,
                         testing::Combine(testing::ValuesIn(sample_path_vecs),
                                          testing::ValuesIn(sample_path_vecs)));

}  // namespace raksha::ir
