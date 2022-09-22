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
#include "src/ir/ssa_names.h"

#include "src/common/testing/gtest.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::ir {
namespace {

template <typename T>
class SsaNamesTest : public ::testing::Test {
 protected:
  T first_entity_;
  T second_entity_;
};

// Specialize class for Value as it has no default constructor to
// initialize `first_entity_` and `second_entity_`.
template <>
class SsaNamesTest<Value> : public ::testing::Test {
 protected:
  SsaNamesTest()
      : dummy_test_operation1_(nullptr, Operator("first"), {}, {}),
        dummy_test_operation2_(nullptr, Operator("second"), {}, {}),
        first_entity_(
            Value::MakeDefaultOperationResultValue(dummy_test_operation1_)),
        second_entity_(
            Value::MakeDefaultOperationResultValue(dummy_test_operation2_)) {}

  Operation dummy_test_operation1_;
  Operation dummy_test_operation2_;
  Value first_entity_;
  Value second_entity_;
};

using SsaNamesTestTypes = ::testing::Types<Value, Block, Module>;
TYPED_TEST_SUITE(SsaNamesTest, SsaNamesTestTypes);

TYPED_TEST(SsaNamesTest, GetOrCreateIDReturnsUniqueIDs) {
  SsaNames names;
  SsaNames::ID first_id = names.GetOrCreateID(this->first_entity_);
  SsaNames::ID second_id = names.GetOrCreateID(this->second_entity_);

  EXPECT_NE(first_id, second_id);
  EXPECT_EQ(names.GetOrCreateID(this->first_entity_), first_id);
  EXPECT_EQ(names.GetOrCreateID(this->second_entity_), second_id);
}

TYPED_TEST(SsaNamesTest, AddIDReturnsCorrectIDs) {
  SsaNames names;
  SsaNames::ID first_id = names.GetOrCreateID(this->first_entity_);
  SsaNames::ID second_id = names.AddID(this->second_entity_, "something");

  EXPECT_NE(first_id, second_id);
  EXPECT_DEATH(names.AddID(this->first_entity_, "another_thing"),
               "another_thing");
  EXPECT_EQ(names.GetOrCreateID(this->second_entity_), "something");
}

}  // namespace
}  // namespace raksha::ir
