//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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
#include "src/ir/types/type.h"

#include <algorithm>
#include <memory>
#include <optional>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_split.h"
#include "google/protobuf/text_format.h"
#include "src/common/logging/logging.h"
#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/proto/type.h"
#include "src/ir/types/schema.h"

namespace raksha::ir::types {

// Helper function for making an unnamed schema from a field map.
static const Schema &MakeAnonymousSchema(
    TypeFactory &type_factory,
    absl::flat_hash_map<std::string, Type> field_map) {
  return type_factory.RegisterSchema(std::nullopt, std::move(field_map));
}

static Type MakeEntityTypeWithAnonymousSchema(
    TypeFactory &type_factory,
    absl::flat_hash_map<std::string, Type> field_map) {
  return type_factory.MakeEntityType(
      MakeAnonymousSchema(type_factory, std::move(field_map)));
}

// Given a vector of strings representing access paths to leaves, generate a
// type having all of those access paths and no others.
// The resulting type will not have information about the name of the type;
// this is not presented by the selector access path and thus cannot be
// derived from it.
template <typename Iter>
static Type MakeMinimalTypeFromAccessPathFieldVec(TypeFactory &type_factory,
                                                  Iter begin_iter,
                                                  Iter end_iter,
                                                  uint64_t depth) {
  // We expect that the incoming range is never empty.
  EXPECT_NE(begin_iter, end_iter);
  // If they happen to be equal, though, just bail out with a primitive type.
  if (begin_iter == end_iter) {
    return type_factory.MakePrimitiveType();
  }

  // If the first vector does not have the required depth, we expect that we
  // have a single vector that is out of its depth, consistent with the
  // primitive type case. Note that this is also consistent with the case of
  // an entity type with no fields. With full type information, this
  // ambiguity would be resolved by the structure of the base type, but here
  // we just make a choice.
  if (begin_iter->size() <= depth) {
    EXPECT_EQ(++begin_iter, end_iter);
    return type_factory.MakePrimitiveType();
  }

  // An equality function that compares vectors only by the element at depth.
  // This is used to group the same field at a particular height in the tree.
  auto compare_vecs_at_depth = [depth](const std::vector<std::string> &vec1,
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

  absl::flat_hash_map<std::string, Type> field_map;
  while (begin_iter != end_iter) {
    const std::vector<std::string> &range_start_vec = *begin_iter;
    std::string field_name = range_start_vec.at(depth);
    // Get the iterator past the point where this field is referenced at this
    // depth. The earlier parts of the access path are guaranteed to be the
    // same due to the vector being sorted.
    auto equal_range_end = std::upper_bound(begin_iter, end_iter, *begin_iter,
                                            compare_vecs_at_depth);
    auto insert_result =
        field_map.insert({field_name, MakeMinimalTypeFromAccessPathFieldVec(
                                          type_factory, begin_iter,
                                          equal_range_end, depth + 1)});
    // If the insert did not succeed, then the field name is already in the
    // map, which should not be the case.
    EXPECT_TRUE(insert_result.second);
    // Move the iterator past the range of equal fields.
    begin_iter = equal_range_end;
  }
  return MakeEntityTypeWithAnonymousSchema(type_factory, std::move(field_map));
}

// Given a vector of strings representing access paths to leaves, generate a
// type having all of those access paths and no others.
static Type MakeMinimalTypeFromAccessPathStrings(
    TypeFactory &type_factory, std::vector<std::string> access_path_strs) {
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

  return MakeMinimalTypeFromAccessPathFieldVec(type_factory,
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
    {".b", ".d", ".a.b.c", ".c.b.a", ".a.a.a"}};

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

class RoundTripStrsThroughTypeTest
    : public testing::TestWithParam<std::vector<std::string>> {
 protected:
  TypeFactory type_factory_;
};

// Ensure that when we start with a list of string versions of selector
// access paths, build a type out of that, then get the list of access paths
// out again, that we get the same list of access paths. This ensures that we
// don't lose information in this process (for the given inputs).
TEST_P(RoundTripStrsThroughTypeTest, RoundTripStrsThroughTypeTest) {
  std::vector<std::string> original_strs = GetParam();
  Type generated_type =
      MakeMinimalTypeFromAccessPathStrings(type_factory_, original_strs);
  std::vector<std::string> result_strs =
      GetAccessPathStrVecFromAccessPathSelectorsSet(
          generated_type.GetAccessPathSelectorsSet());

  EXPECT_THAT(result_strs, testing::UnorderedElementsAreArray(original_strs));
}

INSTANTIATE_TEST_SUITE_P(RoundTripStrsThroughTypeTest,
                         RoundTripStrsThroughTypeTest,
                         testing::ValuesIn(sample_access_path_str_vecs));

class TypeProducesAccessPathStrsTest
    : public testing::TestWithParam<
          std::tuple<const TypeBase *, std::vector<std::string>>> {
 public:
  static TypeFactory type_factory;
};

TypeFactory TypeProducesAccessPathStrsTest::type_factory;

TEST_P(TypeProducesAccessPathStrsTest, TypeProducesAccessPathStrsTest) {
  const auto &param_pair = GetParam();
  const TypeBase *type = std::get<0>(param_pair);
  const std::vector<std::string> expected_strs = std::get<1>(param_pair);

  const std::vector<std::string> result_strs =
      GetAccessPathStrVecFromAccessPathSelectorsSet(
          type->GetAccessPathSelectorsSet());

  EXPECT_THAT(result_strs, testing::UnorderedElementsAreArray(expected_strs));
}

static Type kPrimitive =
    TypeProducesAccessPathStrsTest::type_factory.MakePrimitiveType();
static Type kEmptyEntity = MakeEntityTypeWithAnonymousSchema(
    TypeProducesAccessPathStrsTest::type_factory,
    absl::flat_hash_map<std::string, Type>());

// Show that we can have aliasing in subpaths between paths that end in a
// PrimitiveType and paths that end in an empty EntityType. This should be
// disambiguated by the top-level type of the path, but it seems important to
// note in a test.
static std::tuple<const TypeBase *, std::vector<std::string>>
    types_to_access_path_lists[] = {{&kPrimitive.type_base(), {""}},
                                    {&kEmptyEntity.type_base(), {""}}};

INSTANTIATE_TEST_SUITE_P(TypeProducesAccessPathStrsTest,
                         TypeProducesAccessPathStrsTest,
                         testing::ValuesIn(types_to_access_path_lists));

class GetAccessPathSelectorsWithProtoTest
    : public testing::TestWithParam<
          std::tuple<std::string, std::vector<std::string>>> {
 protected:
  TypeFactory type_factory_;
};

TEST_P(GetAccessPathSelectorsWithProtoTest,
       GetAccessPathSelectorsWithProtoTest) {
  const auto &[type_as_textproto, expected_access_path_strs] = GetParam();
  arcs::TypeProto orig_type_proto;
  ASSERT_TRUE(google::protobuf::TextFormat::ParseFromString(type_as_textproto,
                                                            &orig_type_proto))
      << "Failed to parse text proto!";
  Type type = proto::Decode(type_factory_, orig_type_proto);
  std::vector<std::string> access_path_str_vec =
      GetAccessPathStrVecFromAccessPathSelectorsSet(
          type.GetAccessPathSelectorsSet());
  EXPECT_THAT(access_path_str_vec,
              testing::UnorderedElementsAreArray(expected_access_path_strs));
}

const std::tuple<std::string, std::vector<std::string>>
    type_proto_and_access_path_strings[] = {
        // Simple primitive type.
        {"primitive: TEXT", {""}},
        // Entity with no fields.
        {"entity: { schema: { } }", {""}},
        // Entity with one primitive field, field1.
        {R"(
entity: {
  schema: {
    fields [ { key: "field1", value: { primitive: TEXT } } ]} })",
         {".field1"}},
        // Entity with one primitive field and a named schema
        {R"(
entity: {
  schema: {
    names: ["my_schema"]
      fields: [ { key: "field1", value: { primitive: TEXT } } ] } })",
         {".field1"}},
        // Entity with multiple primitive fields.
        {R"(
entity: {
  schema: {
    fields: [
      { key: "field1", value: { primitive: TEXT } },
      { key: "x", value: { primitive: TEXT } },
      { key: "hello", value: { primitive: TEXT } } ] } })",
         {".field1", ".x", ".hello"}},
        // Entity with sub entities and primitive fields.
        {R"(
entity: {
  schema: {
    fields: [
      { key: "field1", value: { primitive: TEXT } },
      { key: "x",
        value: {
          entity: { schema: { names: ["embedded"], fields: [
            { key: "sub_field1", value: { primitive: TEXT } },
            { key: "sub_field2", value: { primitive: TEXT } }
           ]}}}},
      { key: "hello", value: { primitive: TEXT } } ] } })",
         {".field1", ".x.sub_field1", ".x.sub_field2", ".hello"}},
};

INSTANTIATE_TEST_SUITE_P(GetAccessPathSelectorsWithProtoTest,
                         GetAccessPathSelectorsWithProtoTest,
                         testing::ValuesIn(type_proto_and_access_path_strings));

// TODO(#122): This test should be moved to appropriate file while refactoring.
TEST(EntityTypeTest, KindReturnsCorrectKind) {
  TypeFactory type_factory;
  EntityType entity_type(MakeAnonymousSchema(type_factory, {}));
  EXPECT_EQ(entity_type.kind(), TypeBase::Kind::kEntity);
}

}  // namespace raksha::ir::types
