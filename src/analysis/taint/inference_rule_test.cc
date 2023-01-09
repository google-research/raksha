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
#include "src/analysis/taint/inference_rule.h"

#include <variant>

#include "src/common/testing/gtest.h"

namespace raksha::analysis::taint {
namespace {

using TypesUnderTest =
    std::variant<InputHasIntegrity, CopyInput, MergeIntegrityTags, ModifyTag>;

class EqualityTest : public testing::TestWithParam<
                         std::tuple<std::pair<uint64_t, TypesUnderTest>,
                                    std::pair<uint64_t, TypesUnderTest>>> {};

TEST_P(EqualityTest, EqualAndNotEqualWorkCorrectly) {
  const auto& [lhs, rhs] = GetParam();
  const auto& [lhs_eq_class, lhs_value] = lhs;
  const auto& [rhs_eq_class, rhs_value] = rhs;
  EXPECT_EQ(lhs_eq_class == rhs_eq_class, lhs_value == rhs_value);
  EXPECT_EQ(lhs_eq_class != rhs_eq_class, lhs_value != rhs_value);
}

std::pair<uint64_t, TypesUnderTest> kSampleValues[] = {
    {0, InputHasIntegrity{.input_index = 0, .tag = 1}},
    {1, InputHasIntegrity{.input_index = 0, .tag = 2}},
    {2, InputHasIntegrity{.input_index = 2, .tag = 1}},
    {3, InputHasIntegrity{.input_index = 2, .tag = 2}},
    {4, CopyInput{.input_index = 1}},
    {5, CopyInput{.input_index = 2}},
    {6, MergeIntegrityTags{.kind = MergeIntegrityTags::Kind::kIntersect,
                           .input_indices = {0, 1, 2}}},
    {7, MergeIntegrityTags{.kind = MergeIntegrityTags::Kind::kIntersect,
                           .input_indices = {0, 2}}},
    {8, MergeIntegrityTags{.kind = MergeIntegrityTags::Kind::kUnion,
                           .input_indices = {0, 1, 2}}},
    {9, MergeIntegrityTags{.kind = MergeIntegrityTags::Kind::kUnion,
                           .input_indices = {0, 2}}},
    {10, ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity, .tag = 1}},
    {11, ModifyTag{.kind = ModifyTag::Kind::kAddIntegrity, .tag = 2}},
    {12, ModifyTag{.kind = ModifyTag::Kind::kAddSecrecy, .tag = 1}},
    {13, ModifyTag{.kind = ModifyTag::Kind::kAddSecrecy, .tag = 2}},
    {14, ModifyTag{.kind = ModifyTag::Kind::kRemoveSecrecy, .tag = 1}},
    {15, ModifyTag{.kind = ModifyTag::Kind::kRemoveSecrecy, .tag = 2}}};

INSTANTIATE_TEST_SUITE_P(EqualityTests, EqualityTest,
                         testing::Combine(testing::ValuesIn(kSampleValues),
                                          testing::ValuesIn(kSampleValues)));

}  // namespace
}  // namespace raksha::analysis::taint
