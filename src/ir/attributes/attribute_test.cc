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

// Constructor for different attribute types.
template <typename A>
intrusive_ptr<const A> MakeAttribute();

template <>
intrusive_ptr<const StringAttribute> MakeAttribute() {
  return StringAttribute::Create("Hello");
}
template <>
intrusive_ptr<const Int64Attribute> MakeAttribute() {
  return Int64Attribute::Create(10);
}

template <typename T>
class AttributeTest : public testing::Test {};

using AttributeTypes = testing::Types<Int64Attribute, StringAttribute>;
TYPED_TEST_SUITE(AttributeTest, AttributeTypes);

TYPED_TEST(AttributeTest, ConstructedAttributeReturnsTheCorrectProperties) {
  intrusive_ptr<const TypeParam> typed_attribute = MakeAttribute<TypeParam>();
  Attribute attribute = typed_attribute;
  EXPECT_EQ(attribute.kind(), typed_attribute->kind());
  EXPECT_EQ(attribute.ToString(), typed_attribute->ToString());
}

}  // namespace
}  // namespace raksha::ir
