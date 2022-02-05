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
#include "src/ir/data_decl.h"

#include "src/common/testing/gtest.h"
#include "src/ir/types/entity_type.h"
#include "src/ir/types/type.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir {
namespace {

class DataDeclTestBase : public testing::Test {
 protected:
  types::TypeFactory type_factory_;

  static types::Type MakeTestEntityType(types::TypeFactory& factory,
                                        absl::string_view name) {
    const types::Schema& schema = factory.RegisterSchema(std::string(name), {});
    return factory.MakeEntityType(schema);
  }
};

class DataDeclTest : public DataDeclTestBase {};

TEST_F(DataDeclTest, PrimitiveTypeDeclConstruction) {
  DataDecl primitive_decl("count", type_factory_.MakePrimitiveType());
  EXPECT_EQ(primitive_decl.name(), "count");
}

TEST_F(DataDeclTest, EntityTypeDeclConstruction) {
  DataDecl entity_decl(
      "person", DataDeclTest::MakeTestEntityType(type_factory_, "TestPerson"));
  EXPECT_EQ(entity_decl.name(), "person");
  const types::TypeBase& type = entity_decl.type().type_base();
  EXPECT_EQ(type.kind(), types::TypeBase::Kind::kEntity);
  EXPECT_EQ(static_cast<const types::EntityType&>(type).schema().name(),
            "TestPerson");
}

class DataDeclCollectionTest : public DataDeclTestBase {
 public:
  DataDeclCollection collection_;
};

TEST_F(DataDeclCollectionTest, AddDeclReturnsAddedDecl) {
  const DataDecl& decl = collection_.AddDecl(
      "count", DataDeclTest::MakeTestEntityType(type_factory_, "TestCount"));
  EXPECT_EQ(decl.name(), "count");
  const types::TypeBase& type = decl.type().type_base();
  EXPECT_EQ(type.kind(), types::TypeBase::Kind::kEntity);
  EXPECT_EQ(static_cast<const types::EntityType&>(type).schema().name(),
            "TestCount");
}

TEST_F(DataDeclCollectionTest, DuplicateAddDeclCausesFaiure) {
  collection_.AddDecl("count", type_factory_.MakePrimitiveType());
  EXPECT_DEATH(
      { collection_.AddDecl("count", type_factory_.MakePrimitiveType()); },
      "Adding a duplicate declaration for `count`.");
}

TEST_F(DataDeclCollectionTest, FindDeclReturnsExisingDecl) {
  collection_.AddDecl(
      "city", DataDeclTest::MakeTestEntityType(type_factory_, "TestCity"));

  const DataDecl* decl = collection_.FindDecl("city");
  ASSERT_NE(decl, nullptr);
  const types::TypeBase& type = decl->type().type_base();
  EXPECT_EQ(type.kind(), types::TypeBase::Kind::kEntity);
  EXPECT_EQ(static_cast<const types::EntityType&>(type).schema().name(),
            "TestCity");
}

TEST_F(DataDeclCollectionTest, FindDeclReturnsNullIfNotFound) {
  EXPECT_EQ(collection_.FindDecl("count"), nullptr);
}

}  // namespace
}  // namespace raksha::ir
