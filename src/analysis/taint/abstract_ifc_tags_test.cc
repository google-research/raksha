//-----------------------------------------------------------------------------
// Copyright 2023 Google LLC
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
//-----------------------------------------------------------------------------
#include "src/analysis/taint/abstract_ifc_tags.h"

#include <cstdint>
#include <tuple>
#include <utility>

#include "src/common/testing/gtest.h"

namespace raksha::analysis::taint {
namespace {

using ::testing::Combine;
using ::testing::Eq;
using ::testing::ValuesIn;

// IfcTags tests
using IfcTagsConstructionTest =
    ::testing::TestWithParam<std::tuple<TagIdSet, TagIdSet>>;

const TagIdSet kTagSets[] = {{}, {3}, {0, 1}, {0, 1, 2}};
INSTANTIATE_TEST_SUITE_P(IfcTagsConstructionTests, IfcTagsConstructionTest,
                         Combine(ValuesIn(kTagSets), ValuesIn(kTagSets)));

TEST_P(IfcTagsConstructionTest, FactoryMethodSetsTheTagsCorrectly) {
  const auto& [secrecy, integrity] = GetParam();
  IfcTagsPtr tags = IfcTags::Create(secrecy, integrity);
  EXPECT_THAT(tags->secrecy_tags(), secrecy);
  EXPECT_THAT(tags->integrity_tags(), integrity);
}

// AbstractIfcTags tests

// A struct to define examples for testing manipulation of tag ids in the sets.
struct TagIdManipulationTestCase {
  TagIdSet tags;            // Original tags.
  TagId tag;                // Tag to be added or removed.
  TagIdSet added_result;    // Result of adding a tag.
  TagIdSet cleared_result;  // Result of clearing a tag.
};

const TagIdManipulationTestCase kTagIdManipulationExamples[] = {
    {.tags = {}, .tag = 0, .added_result = {0}, .cleared_result = {}},
    {.tags = {3}, .tag = 3, .added_result = {3}, .cleared_result = {}},
    {.tags = {3}, .tag = 2, .added_result = {2, 3}, .cleared_result = {3}},
    {.tags = {0, 1}, .tag = 1, .added_result = {0, 1}, .cleared_result = {0}},
    {.tags = {0, 1},
     .tag = 2,
     .added_result = {0, 1, 2},
     .cleared_result = {0, 1}},
    {.tags = {0, 1, 2},
     .tag = 2,
     .added_result = {0, 1, 2},
     .cleared_result = {0, 1}},
    {.tags = {0, 1, 2},
     .tag = 5,
     .added_result = {0, 1, 2, 5},
     .cleared_result = {0, 1, 2}}};

using AbstractIfcTagsTest = ::testing::TestWithParam<
    std::tuple<TagIdManipulationTestCase, TagIdManipulationTestCase>>;

INSTANTIATE_TEST_SUITE_P(AbstractIfcTagsTests, AbstractIfcTagsTest,
                         Combine(ValuesIn(kTagIdManipulationExamples),
                                 ValuesIn(kTagIdManipulationExamples)));

TEST(AbstractIfcTagsTest, IsBottomBehavesCorrectly) {
  EXPECT_TRUE(AbstractIfcTags::Bottom().IsBottom());
  EXPECT_FALSE(AbstractIfcTags({}, {}).IsBottom());
  EXPECT_FALSE(AbstractIfcTags({0, 2}, {}).IsBottom());
  EXPECT_FALSE(AbstractIfcTags({}, {1, 2}).IsBottom());
  EXPECT_FALSE(AbstractIfcTags({0, 2}, {1, 2}).IsBottom());
}

TEST(AbstractIfcTagsTest, HasNoSecrecyTests) {
  EXPECT_TRUE(AbstractIfcTags({}, {}).HasNoSecrecy());
  EXPECT_TRUE(AbstractIfcTags({}, {0}).HasNoSecrecy());
  EXPECT_TRUE(AbstractIfcTags({}, {0, 1}).HasNoSecrecy());
  EXPECT_FALSE(AbstractIfcTags({0}, {}).HasNoSecrecy());
  EXPECT_FALSE(AbstractIfcTags({0}, {0}).HasNoSecrecy());
  EXPECT_FALSE(AbstractIfcTags({0}, {0, 1}).HasNoSecrecy());
  EXPECT_FALSE(AbstractIfcTags({0, 1}, {}).HasNoSecrecy());
  EXPECT_FALSE(AbstractIfcTags({0, 1}, {0}).HasNoSecrecy());
  EXPECT_FALSE(AbstractIfcTags({0, 1}, {0, 1}).HasNoSecrecy());
}

TEST(AbstractIfcTagsTest, HasIntegrityTests) {
  EXPECT_FALSE(AbstractIfcTags({}, {}).HasIntegrity(0));
  EXPECT_TRUE(AbstractIfcTags({}, {0}).HasIntegrity(0));
  EXPECT_FALSE(AbstractIfcTags({}, {0}).HasIntegrity(1));
  EXPECT_TRUE(AbstractIfcTags({}, {0, 1}).HasIntegrity(0));
  EXPECT_TRUE(AbstractIfcTags({}, {0, 1}).HasIntegrity(1));
  EXPECT_FALSE(AbstractIfcTags({}, {0, 1}).HasIntegrity(2));
  EXPECT_FALSE(AbstractIfcTags({0}, {}).HasIntegrity(0));
  EXPECT_TRUE(AbstractIfcTags({0}, {0}).HasIntegrity(0));
  EXPECT_FALSE(AbstractIfcTags({0, 1}, {}).HasIntegrity(0));
  EXPECT_FALSE(AbstractIfcTags({0, 1}, {}).HasIntegrity(1));
  EXPECT_TRUE(AbstractIfcTags({0, 1}, {0, 1}).HasIntegrity(0));
  EXPECT_TRUE(AbstractIfcTags({0, 1}, {0, 1}).HasIntegrity(1));
  EXPECT_FALSE(AbstractIfcTags({0, 1}, {0, 1}).HasIntegrity(2));
}

TEST_P(AbstractIfcTagsTest, ConstructionWorksCorrectly) {
  const auto& [secrecy, integrity] = GetParam();
  AbstractIfcTags ifc_tags(secrecy.tags, integrity.tags);
  EXPECT_THAT(ifc_tags.secrecy_tags(), Eq(secrecy.tags));
  EXPECT_THAT(ifc_tags.integrity_tags(), Eq(integrity.tags));
}

TEST(AbstractIfcTagsDeathTest, AccessingSecrecyTagsOnBottomFails) {
  EXPECT_DEATH(AbstractIfcTags::Bottom().secrecy_tags(),
               "Cannot return secrecy tags for bottom.");
}
TEST(AbstractIfcTagsDeathTest, AccessingIntegrityTagsOnBottomFails) {
  EXPECT_DEATH(AbstractIfcTags::Bottom().integrity_tags(),
               "Cannot return integrity tags for bottom.");
}

TEST(AbstractIfcTagsTest, SetSecrecyOnBottomReturnsBottom) {
  AbstractIfcTags bottom = AbstractIfcTags::Bottom();
  EXPECT_TRUE(bottom.SetSecrecy(0).IsBottom());
  EXPECT_TRUE(bottom.SetSecrecy(1).IsBottom());
}

TEST_P(AbstractIfcTagsTest, SetSecrecyUpdatesSecrecyTagsOnly) {
  const auto& [secrecy_param, integrity_param] = GetParam();
  AbstractIfcTags ifc_tags(secrecy_param.tags, integrity_param.tags);
  AbstractIfcTags result = ifc_tags.SetSecrecy(secrecy_param.tag);
  EXPECT_THAT(result.secrecy_tags(), Eq(secrecy_param.added_result));
  EXPECT_THAT(result.integrity_tags(), Eq(integrity_param.tags));
}

TEST(AbstractIfcTagsTest, ClearSecrecyOnBottomReturnsBottom) {
  AbstractIfcTags bottom = AbstractIfcTags::Bottom();
  EXPECT_TRUE(bottom.ClearSecrecy(0).IsBottom());
  EXPECT_TRUE(bottom.ClearSecrecy(1).IsBottom());
}

TEST_P(AbstractIfcTagsTest, ClearSecrecyUpdatesSecrecyTagsOnly) {
  const auto& [secrecy_param, integrity_param] = GetParam();
  AbstractIfcTags ifc_tags(secrecy_param.tags, integrity_param.tags);
  AbstractIfcTags result = ifc_tags.ClearSecrecy(secrecy_param.tag);
  EXPECT_THAT(result.secrecy_tags(), Eq(secrecy_param.cleared_result));
  EXPECT_THAT(result.integrity_tags(), Eq(integrity_param.tags));
}

TEST(AbstractIfcTagsTest, SetIntegrityOnBottomReturnsBottom) {
  AbstractIfcTags bottom = AbstractIfcTags::Bottom();
  EXPECT_TRUE(bottom.SetIntegrity(0).IsBottom());
  EXPECT_TRUE(bottom.SetIntegrity(1).IsBottom());
}

TEST_P(AbstractIfcTagsTest, SetIntegrityUpdatesIntegrityTagsOnly) {
  const auto& [secrecy_param, integrity_param] = GetParam();
  AbstractIfcTags ifc_tags(secrecy_param.tags, integrity_param.tags);
  AbstractIfcTags result = ifc_tags.SetIntegrity(integrity_param.tag);
  EXPECT_THAT(result.secrecy_tags(), Eq(secrecy_param.tags));
  EXPECT_THAT(result.integrity_tags(), Eq(integrity_param.added_result));
}

TEST(AbstractIfcTagsTest, ClearIntegrityOnBottomReturnsBottom) {
  AbstractIfcTags bottom = AbstractIfcTags::Bottom();
  EXPECT_TRUE(bottom.ClearIntegrity(0).IsBottom());
  EXPECT_TRUE(bottom.ClearIntegrity(1).IsBottom());
}

TEST_P(AbstractIfcTagsTest, ClearIntegrityUpdatesIntegrityTagsOnly) {
  const auto& [secrecy_param, integrity_param] = GetParam();
  AbstractIfcTags ifc_tags(secrecy_param.tags, integrity_param.tags);
  AbstractIfcTags result = ifc_tags.ClearIntegrity(integrity_param.tag);
  EXPECT_THAT(result.secrecy_tags(), Eq(secrecy_param.tags));
  EXPECT_THAT(result.integrity_tags(), Eq(integrity_param.cleared_result));
}

using AbstractIfcTagsTestCase = std::pair<uint64_t, AbstractIfcTags>;
using AbstractIfcTagsIsEquivalentToTest = ::testing::TestWithParam<
    std::tuple<AbstractIfcTagsTestCase, AbstractIfcTagsTestCase>>;

const AbstractIfcTagsTestCase kAbstractIfcTagsTestCases[] = {
    std::make_pair(0, AbstractIfcTags::Bottom()),
    std::make_pair(1, AbstractIfcTags({}, {})),
    std::make_pair(2, AbstractIfcTags({}, {0})),
    std::make_pair(3, AbstractIfcTags({}, {0, 1})),
    std::make_pair(4, AbstractIfcTags({0}, {})),
    std::make_pair(5, AbstractIfcTags({0}, {0})),
    std::make_pair(6, AbstractIfcTags({0}, {0, 1})),
    std::make_pair(7, AbstractIfcTags({0, 1}, {})),
    std::make_pair(8, AbstractIfcTags({0, 1}, {0})),
    std::make_pair(9, AbstractIfcTags({0, 1}, {0, 1}))};

INSTANTIATE_TEST_SUITE_P(AbstractIfcTagsTests,
                         AbstractIfcTagsIsEquivalentToTest,
                         Combine(ValuesIn(kAbstractIfcTagsTestCases),
                                 ValuesIn(kAbstractIfcTagsTestCases)));

TEST_P(AbstractIfcTagsIsEquivalentToTest, IsEquivalentToWorksCorrectly) {
  const auto& [lhs_param, rhs_param] = GetParam();
  const auto& [lhs_id, lhs_value] = lhs_param;
  const auto& [rhs_id, rhs_value] = rhs_param;
  EXPECT_EQ(lhs_id == rhs_id, lhs_value.IsEquivalentTo(rhs_value));
}

// Test case for joins.
struct AbstractIfcTagsJoinTestCase {
  TagIdSet arg0;
  TagIdSet arg1;
  TagIdSet union_result;
  TagIdSet intersect_result;
};
const AbstractIfcTagsJoinTestCase kAbstractIfcTagsJoinTestCases[] = {
    // arg0 = {}
    {.arg0 = {}, .arg1 = {}, .union_result = {}, .intersect_result = {}},
    {.arg0 = {}, .arg1 = {0}, .union_result = {0}, .intersect_result = {}},
    {.arg0 = {},
     .arg1 = {0, 1},
     .union_result = {0, 1},
     .intersect_result = {}},
    {.arg0 = {},
     .arg1 = {2, 3},
     .union_result = {2, 3},
     .intersect_result = {}},
    // arg0 {0}
    {.arg0 = {0}, .arg1 = {}, .union_result = {0}, .intersect_result = {}},
    {.arg0 = {0}, .arg1 = {0}, .union_result = {0}, .intersect_result = {0}},
    {.arg0 = {0},
     .arg1 = {0, 1},
     .union_result = {0, 1},
     .intersect_result = {0}},
    {.arg0 = {0},
     .arg1 = {2, 3},
     .union_result = {0, 2, 3},
     .intersect_result = {}},
    // arg0 {0, 1}
    {.arg0 = {0, 1},
     .arg1 = {},
     .union_result = {0, 1},
     .intersect_result = {}},
    {.arg0 = {0, 1},
     .arg1 = {0},
     .union_result = {0, 1},
     .intersect_result = {0}},
    {.arg0 = {0, 1},
     .arg1 = {0, 1},
     .union_result = {0, 1},
     .intersect_result = {0, 1}},
    {.arg0 = {0, 1},
     .arg1 = {2, 3},
     .union_result = {0, 1, 2, 3},
     .intersect_result = {}},
    // arg0 {2, 3}
    {.arg0 = {2, 3},
     .arg1 = {},
     .union_result = {2, 3},
     .intersect_result = {}},
    {.arg0 = {2, 3},
     .arg1 = {0},
     .union_result = {0, 2, 3},
     .intersect_result = {}},
    {.arg0 = {2, 3},
     .arg1 = {0, 1},
     .union_result = {0, 1, 2, 3},
     .intersect_result = {}},
    {.arg0 = {2, 3},
     .arg1 = {2, 3},
     .union_result = {2, 3},
     .intersect_result = {2, 3}},
};

using AbstractIfcTagsJoinBottomTest =
    ::testing::TestWithParam<AbstractIfcTagsJoinTestCase>;

INSTANTIATE_TEST_SUITE_P(AbstractIfcTagsJoinTests,
                         AbstractIfcTagsJoinBottomTest,
                         ValuesIn(kAbstractIfcTagsJoinTestCases));

TEST_P(AbstractIfcTagsJoinBottomTest, BottomActsAsIdentity) {
  const auto& [secrecy, integrity, _union, _intersect] = GetParam();
  AbstractIfcTags ifc_tags(secrecy, integrity);
  EXPECT_TRUE(
      ifc_tags.Join(AbstractIfcTags::Bottom()).IsEquivalentTo(ifc_tags));
  EXPECT_TRUE(
      AbstractIfcTags::Bottom().Join(ifc_tags).IsEquivalentTo(ifc_tags));
}

using AbstractIfcTagsJoinTest = ::testing::TestWithParam<
    std::tuple<AbstractIfcTagsJoinTestCase, AbstractIfcTagsJoinTestCase>>;

INSTANTIATE_TEST_SUITE_P(AbstractIfcTagsJoinTests, AbstractIfcTagsJoinTest,
                         Combine(ValuesIn(kAbstractIfcTagsJoinTestCases),
                                 ValuesIn(kAbstractIfcTagsJoinTestCases)));

TEST_P(AbstractIfcTagsJoinTest, JoinWorksCorrectly) {
  const auto& [secrecy_param, integrity_param] = GetParam();
  AbstractIfcTags arg0(secrecy_param.arg0, integrity_param.arg0);
  AbstractIfcTags arg1(secrecy_param.arg1, integrity_param.arg1);
  AbstractIfcTags result(secrecy_param.union_result,
                         integrity_param.intersect_result);
  EXPECT_TRUE(arg0.Join(arg1).IsEquivalentTo(result));
}

}  // namespace
}  // namespace raksha::analysis::taint
