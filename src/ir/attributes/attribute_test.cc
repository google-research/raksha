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
#include "src/ir/attributes/attribute.h"

#include "src/common/testing/gtest.h"
#include "src/common/utils/intrusive_ptr.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"

namespace raksha::ir {
namespace {

struct TestData {
  TestData(Attribute attribute, AttributeBase::Kind kind,
           absl::string_view string_rep)
      : attribute(attribute), kind(kind), string_rep(string_rep) {}

  Attribute attribute;
  AttributeBase::Kind kind;
  absl::string_view string_rep;
};

class AttributeTest : public testing::TestWithParam<TestData> {};

TEST_P(AttributeTest, ConstructedAttributeReturnsCorrectProperties) {
  TestData p = GetParam();
  EXPECT_EQ(p.attribute.kind(), p.kind);
  EXPECT_EQ(p.attribute.ToString(), p.string_rep);
}

INSTANTIATE_TEST_SUITE_P(
    AttributeTest, AttributeTest,
    testing::Values(TestData(Attribute::Create<Int64Attribute>(10),
                             AttributeBase::Kind::kInt64, "10"),
                    TestData(Attribute::Create<StringAttribute>("Hello World!"),
                             AttributeBase::Kind::kString, "Hello World!")));

}  // namespace
}  // namespace raksha::ir
