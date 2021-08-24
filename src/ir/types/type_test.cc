#include "src/ir/types/type.h"

#include <algorithm>
#include <memory>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_split.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/common/logging/logging.h"
#include "src/common/testing/gtest.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/primitive_type.h"
#include "src/ir/types/schema.h"

namespace raksha::ir::types {

// Helper function for making an unnamed schema from a field map.
static Schema MakeAnonymousSchema(
    absl::flat_hash_map<std::string, std::unique_ptr<Type>> field_map) {
  return Schema(absl::optional<std::string>(), std::move(field_map));
}

// Helper function for making an entity type with an unnamed schema from a
// field map.
static EntityType MakeEntityTypeWithAnonymousSchema(
    absl::flat_hash_map<std::string, std::unique_ptr<Type>> field_map) {
  return EntityType(MakeAnonymousSchema(std::move(field_map)));
}

static std::unique_ptr<EntityType> MakeUniquePtrEntityTypeWithAnonymousSchema(
    absl::flat_hash_map<std::string, std::unique_ptr<Type>> field_map) {
  return std::make_unique<EntityType>(
      MakeAnonymousSchema(std::move(field_map)));
}

// Given a vector of strings representing access paths to leaves, generate a
// type having all of those access paths and no others.
// The resulting type will not have information about the name of the type;
// this is not presented by the selector access path and thus cannot be
// derived from it.
template<typename Iter>
static std::unique_ptr<Type> MakeMinimalTypeFromAccessPathFieldVec(
    Iter begin_iter, Iter end_iter, uint64_t depth) {

  // We expect that the incoming range is never empty.
  EXPECT_NE(begin_iter, end_iter);
  // If they happen to be equal, though, just bail out with a primitive type.
  if (begin_iter == end_iter) {
    return std::make_unique<PrimitiveType>();
  }

  // If the first vector does not have the required depth, we expect that we
  // have a single vector that is out of its depth, consistent with the
  // primitive type case. Note that this is also consistent with the case of
  // an entity type with no fields. With full type information, this
  // ambiguity would be resolved by the structure of the base type, but here
  // we just make a choice.
  if (begin_iter->size() <= depth) {
    EXPECT_EQ(++begin_iter, end_iter);
    return std::make_unique<PrimitiveType>();
  }

  // An equality function that compares vectors only by the element at depth.
  // This is used to group the same field at a particular height in the tree.
  auto compare_vecs_at_depth =
      [depth](
          const std::vector<std::string> &vec1,
          const std::vector<std::string> &vec2) {
    bool vec1_too_shallow = vec1.size() <= depth;
    bool vec2_too_shallow = vec2.size() <= depth;

    // vec1 is less than vec2 if it is too shallow and vec2 is not. In other
    // "too shallow" cases, vec1 is not less than vec2.
    if (vec1_too_shallow && !vec2_too_shallow) {
      return true;
    } else if (vec1_too_shallow || vec2_too_shallow) {
      return false;
    }

    // Compare the elements of the two vectors at the depth.
    return vec1.at(depth) < vec2.at(depth);
  };

  absl::flat_hash_map<std::string, std::unique_ptr<Type>> field_map;
  while (begin_iter != end_iter) {
    const std::vector<std::string> &range_start_vec = *begin_iter;
    std::string field_name = range_start_vec.at(depth);
    // Get the iterator past the point where this field is referenced at this
    // depth. The earlier parts of the access path are guaranteed to be the
    // same due to the vector being sorted.
    auto equal_range_end = std::upper_bound(
        begin_iter, end_iter, *begin_iter, compare_vecs_at_depth);
    auto insert_result = field_map.insert({
      field_name,
      MakeMinimalTypeFromAccessPathFieldVec(
            begin_iter, equal_range_end, depth + 1) });
    // If the insert did not succeed, then the field name is already in the
    // map, which should not be the case.
    EXPECT_TRUE(insert_result.second);
    // Move the iterator past the range of equal fields.
    begin_iter = equal_range_end;
  }
  return MakeUniquePtrEntityTypeWithAnonymousSchema(std::move(field_map));
}

// Given a vector of strings representing access paths to leaves, generate a
// type having all of those access paths and no others.
static std::unique_ptr<Type> MakeMinimalTypeFromAccessPathStrings(
    std::vector<std::string> access_path_strs) {
  std::vector<std::vector<std::string>> access_path_field_vecs;
  for (std::string str : access_path_strs) {
    access_path_field_vecs.push_back(
        absl::StrSplit(std::move(str), '.', absl::SkipEmpty()));
  }
  // Sort the vectors by their natural order.
  std::sort(access_path_field_vecs.begin(), access_path_field_vecs.end());
  // Test input check: if any vector in the sorted order is a prefix of the one
  // before it, then we are describing a path to an inner node instead of to
  // a leaf field, which is not appropriate for this test.
  for (auto iter = access_path_field_vecs.begin();
    iter + 1 != access_path_field_vecs.end(); ++iter) {
    const std::vector<std::string> &prev_vec = *iter;
    const std::vector<std::string> &next_vec = *(iter + 1);
     // Can't be a prefix if it's longer.
     if (prev_vec.size() > next_vec.size()) {
       continue;
     }
     bool found_difference = false;
     for (uint64_t i = 0; i < prev_vec.size(); ++i) {
       if (prev_vec.at(i) != next_vec.at(i)) {
         found_difference = true;
         break;
       }
     }
     CHECK(found_difference)
      << "One path in test was prefix of another; all test paths required to "
      << "be leaf paths.";
  }

  return MakeMinimalTypeFromAccessPathFieldVec(
      access_path_field_vecs.begin(),
      access_path_field_vecs.end(),
      /*depth=*/0);
}

// A list of collections of access path strings, describing types with
// particular nesting structures. All strings should describe selector access
// paths extending to a leaf field.
static std::vector<std::string> sample_access_path_str_vecs[] = {
    {""},
    {".field1"},
    {".field1.field2"},
    {".field1.field2.field3"},
    {".field1.field2.field3.field4"},
    {".field1.child1", ".field1.child2"},
    {".field1.child1", ".field2.child2"},
    {".field1.a", ".field1.b", ".field1.c", ".field1.d", ".f.e", ".f.d"},
    {".b", ".d", ".a.b.c", ".c.b.a", ".a.a.a"}
};

static std::vector<std::string> GetAccessPathStrVecFromAccessPathSelectorsSet(
    raksha::ir::AccessPathSelectorsSet access_path_set) {
  absl::flat_hash_set<raksha::ir::AccessPathSelectors> absl_access_path_set =
      raksha::ir::AccessPathSelectorsSet::CreateAbslSet(
          std::move(access_path_set));
  std::vector<std::string> result_strs;
  for (raksha::ir::AccessPathSelectors path : absl_access_path_set) {
    result_strs.push_back(path.ToString());
  }
  return result_strs;
}

class RoundTripStrsThroughTypeTest :
    public testing::TestWithParam<std::vector<std::string>> {};

// Ensure that when we start with a list of string versions of selector
// access paths, build a type out of that, then get the list of access paths
// out again, that we get the same list of access paths. This ensures that we
// don't lose information in this process (for the given inputs).
TEST_P(RoundTripStrsThroughTypeTest, RoundTripStrsThroughTypeTest) {
  std::vector<std::string> original_strs = GetParam();
  std::unique_ptr<Type> generated_type =
      MakeMinimalTypeFromAccessPathStrings(original_strs);
  std::vector<std::string> result_strs =
      GetAccessPathStrVecFromAccessPathSelectorsSet(
          generated_type->GetAccessPaths());

  EXPECT_THAT(result_strs, testing::UnorderedElementsAreArray(original_strs));
}

INSTANTIATE_TEST_SUITE_P(
    RoundTripStrsThroughTypeTest, RoundTripStrsThroughTypeTest,
    testing::ValuesIn(sample_access_path_str_vecs));


class TypeProducesAccessPathStrsTest :
 public testing::TestWithParam<
  std::tuple<const Type *, std::vector<std::string>>> {};

TEST_P(TypeProducesAccessPathStrsTest, TypeProducesAccessPathStrsTest) {
  const auto &param_pair = GetParam();
  const Type * type = std::get<0>(param_pair);
  const std::vector<std::string> expected_strs = std::get<1>(param_pair);

  const std::vector<std::string> result_strs =
      GetAccessPathStrVecFromAccessPathSelectorsSet(type->GetAccessPaths());

  EXPECT_THAT(result_strs, testing::UnorderedElementsAreArray(expected_strs));
}

static PrimitiveType kPrimitive;
static EntityType kEmptyEntity(MakeEntityTypeWithAnonymousSchema(
    absl::flat_hash_map<std::string, std::unique_ptr<Type>>{}));

// Show that we can have aliasing in subpaths between paths that end in a
// PrimitiveType and paths that end in an empty EntityType. This should be
// disambiguated by the top-level type of the path, but it seems important to
// note in a test.
static std::tuple<const Type *, std::vector<std::string>>
types_to_access_path_lists[] = {
    { &kPrimitive, {""} },
    { &kEmptyEntity, {""} }
};

INSTANTIATE_TEST_SUITE_P(
    TypeProducesAccessPathStrsTest,
    TypeProducesAccessPathStrsTest,
    testing::ValuesIn(types_to_access_path_lists));

}  // namespace raksha::transform::types
