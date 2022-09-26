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
#include "src/ir/attributes/float_attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"

namespace raksha::ir {
namespace {
// A method to construct a test attribute of a specific attribute type. This
// method is specialized below for every attribute type that we want to test.
template <typename T>
std::pair<Attribute, std::shared_ptr<const T>> CreateTestAttribute();

template <typename T>
class AttributeTest : public testing::Test {};

using AttributeTypes = ::testing::Types<Int64Attribute, StringAttribute>;
TYPED_TEST_SUITE(AttributeTest, AttributeTypes);

// Example attributes for tests.
//
// All specializations return a pair consisting of the following:
//   - An `Attribute` that wraps the underlying `std::shared_ptr<T>`.
//   - A directly created instance of `std::shared_ptr<T>`, which is used to
//     compare against the value returned by Attribute::As<T>().
//
template <>
std::pair<Attribute, std::shared_ptr<const FloatAttribute>>
CreateTestAttribute() {
  return std::make_pair(Attribute::Create<FloatAttribute>(0.31415),
                        FloatAttribute::Create(0.31415));
}

template <>
std::pair<Attribute, std::shared_ptr<const Int64Attribute>>
CreateTestAttribute() {
  return std::make_pair(Attribute::Create<Int64Attribute>(10),
                        Int64Attribute::Create(10));
}

template <>
std::pair<Attribute, std::shared_ptr<const StringAttribute>>
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
    AttributeWithId(4, Attribute::Create<FloatAttribute>(0.3)),
    AttributeWithId(4, Attribute::Create<FloatAttribute>(0.3)),
    AttributeWithId(5, Attribute::Create<FloatAttribute>(-0.3)),
};

INSTANTIATE_TEST_SUITE_P(
    AttributeEqualityTest, AttributeEqualityTest,
    testing::Combine(testing::ValuesIn(kExampleAttributes),
                     testing::ValuesIn(kExampleAttributes)));

TYPED_TEST(AttributeTest, ConstructorAndAsConversionWorksCorrectly) {
  const auto& [attr, typed_attribute] = CreateTestAttribute<TypeParam>();
  std::shared_ptr<const TypeParam> expected_typed_attribute =
      attr.template GetIf<TypeParam>();
  ASSERT_NE(expected_typed_attribute, nullptr);
  EXPECT_EQ(*expected_typed_attribute, *typed_attribute);
}

TEST(AttributeAsMethodTest, IncorrectTypeReturnsNullptrFloatIsNotString) {
  auto [float_attribute, _] = CreateTestAttribute<FloatAttribute>();
  EXPECT_EQ(float_attribute.GetIf<StringAttribute>(), nullptr);
}

TEST(AttributeAsMethodTest, IncorrectTypeReturnsNullptrIntIsNotString) {
  auto [int_attribute, _] = CreateTestAttribute<Int64Attribute>();
  EXPECT_EQ(int_attribute.GetIf<StringAttribute>(), nullptr);
}

}  // namespace
}  // namespace raksha::ir
