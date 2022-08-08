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
#include "src/ir/attributes/attribute.h"

#include "src/common/testing/gtest.h"
#include "src/common/utils/intrusive_ptr.h"
#include "src/ir/attributes/double_attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"

namespace raksha::ir {
namespace {
// A method to construct a test attribute of a specific attribute type. This
// method is specialized below for every attribute type that we want to test.
template <typename T>
std::pair<Attribute, intrusive_ptr<const T>> CreateTestAttribute();

template <typename T>
class AttributeTest : public testing::Test {};

using AttributeTypes = ::testing::Types<Int64Attribute, StringAttribute>;
TYPED_TEST_SUITE(AttributeTest, AttributeTypes);

// Example attributes for tests.
//
// All specializations return a pair consisting of the following:
//   - An `Attribute` that wraps the underlying `intrusive_ptr<T>`.
//   - A directly created instance of `intrusive_ptr<T>`, which is used to
//     compare against the value returned by Attribute::As<T>().
//
template <>
std::pair<Attribute, intrusive_ptr<const DoubleAttribute>>
CreateTestAttribute() {
  return std::make_pair(Attribute::Create<DoubleAttribute>(0.31415),
                        DoubleAttribute::Create(0.31415));
}

template <>
std::pair<Attribute, intrusive_ptr<const Int64Attribute>>
CreateTestAttribute() {
  return std::make_pair(Attribute::Create<Int64Attribute>(10),
                        Int64Attribute::Create(10));
}

template <>
std::pair<Attribute, intrusive_ptr<const StringAttribute>>
CreateTestAttribute() {
  return std::make_pair(Attribute::Create<StringAttribute>("Hello World!"),
                        StringAttribute::Create("Hello World!"));
}

struct AttributeWithId {
  AttributeWithId(int id, Attribute attribute) : id(id), attribute(attribute) {}
  int id;
  Attribute attribute;
};

class AttributeEqualityTest
    : public testing::TestWithParam<
          std::tuple<AttributeWithId, AttributeWithId>> {};

TEST_P(AttributeEqualityTest, EqualityReturnsCorrectValue) {
  const auto& [lhs, rhs] = GetParam();
  EXPECT_EQ(lhs.id == rhs.id, lhs.attribute == rhs.attribute);
}

AttributeWithId kExampleAttributes[] = {
    AttributeWithId(0, Attribute::Create<StringAttribute>("Hello")),
    AttributeWithId(0, Attribute::Create<StringAttribute>("Hello")),
    AttributeWithId(1, Attribute::Create<Int64Attribute>(10)),
    AttributeWithId(1, Attribute::Create<Int64Attribute>(10)),
    AttributeWithId(2, Attribute::Create<StringAttribute>("World")),
    AttributeWithId(3, Attribute::Create<Int64Attribute>(30)),
    AttributeWithId(4, Attribute::Create<DoubleAttribute>(0.3)),
    AttributeWithId(4, Attribute::Create<DoubleAttribute>(0.3)),
    AttributeWithId(5, Attribute::Create<DoubleAttribute>(-0.3)),
};

INSTANTIATE_TEST_SUITE_P(
    AttributeEqualityTest, AttributeEqualityTest,
    testing::Combine(testing::ValuesIn(kExampleAttributes),
                     testing::ValuesIn(kExampleAttributes)));

TYPED_TEST(AttributeTest, ConstructorAndAsConversionWorksCorrectly) {
  const auto& [attr, typed_attribute] = CreateTestAttribute<TypeParam>();
  intrusive_ptr<const TypeParam> expected_typed_attribute =
      attr.template GetIf<TypeParam>();
  ASSERT_NE(expected_typed_attribute, nullptr);
  EXPECT_EQ(*expected_typed_attribute, *typed_attribute);
}

TEST(AttributeAsMethodTest, IncorrectTypeReturnsNullptrDoubleIsNotString) {
  auto [double_attribute, _] = CreateTestAttribute<DoubleAttribute>();
  EXPECT_EQ(double_attribute.GetIf<StringAttribute>(), nullptr);
}

TEST(AttributeAsMethodTest, IncorrectTypeReturnsNullptrIntIsNotString) {
  auto [int_attribute, _] = CreateTestAttribute<Int64Attribute>();
  EXPECT_EQ(int_attribute.GetIf<StringAttribute>(), nullptr);
}

}  // namespace
}  // namespace raksha::ir
