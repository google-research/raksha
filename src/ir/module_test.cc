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
#include "src/ir/module.h"

#include <string>
#include <vector>

#include "fuzztest/fuzztest.h"
#include "src/common/testing/gtest.h"
#include "src/ir/storage.h"
#include "src/ir/types/type_factory.h"
#include "src/ir/value.h"

namespace raksha::ir {
namespace {

using fuzztest::Arbitrary;
using fuzztest::NonEmpty;
using fuzztest::PrintableAsciiString;
using fuzztest::UniqueElementsVectorOf;
using raksha::ir::types::TypeFactory;
using raksha::ir::value::Any;
using testing::IsEmpty;
using testing::UnorderedElementsAreArray;

TEST(ModuleTest, DefaultConstructorCreatesEmptyBlocks) {
  Module module;
  EXPECT_EQ(module.blocks().size(), 0);
}

TEST(ModuleTest, AddBlockReturnsPassedInBlock) {
  Module module;
  auto block = std::make_unique<Block>();
  const Block* passed_in_block = block.get();
  const Block& returned_block = module.AddBlock(std::move(block));
  EXPECT_EQ(std::addressof(returned_block), passed_in_block);
  EXPECT_EQ(returned_block.parent_module(), &module);
}

TEST(ModuleTest, AddBlockUpdatesBlockList) {
  Module module;
  const Block* block1 =
      std::addressof(module.AddBlock(std::make_unique<Block>()));
  const Block* block2 =
      std::addressof(module.AddBlock(std::make_unique<Block>()));
  EXPECT_NE(block1, block2);
  EXPECT_THAT(module.blocks(), testing::UnorderedElementsAre(
                                   testing::Pointer(testing::Eq(block1)),
                                   testing::Pointer(testing::Eq(block2))));
}

class ModuleStorageTestFixture {
 public:
  explicit ModuleStorageTestFixture(std::vector<std::string> storage_names)
      : module_(), type_factory_() {
    bool first = true;
    for (absl::string_view name : storage_names) {
      if (first) {
        name_not_in_storages_ = name;
        first = false;
        continue;
      }
      created_storages_.push_back(
          &module_.CreateStorage(name, type_factory_.MakePrimitiveType()));
    }
  }
  Module& module() { return module_; }
  TypeFactory& type_factory() { return type_factory_; }
  const std::vector<const Storage*>& created_storages() {
    return created_storages_;
  }
  absl::string_view name_not_in_storages() { return name_not_in_storages_; }

 private:
  Module module_;
  TypeFactory type_factory_;
  std::vector<const Storage*> created_storages_;
  // A name that is not taken by any of the created storages. Useful for a
  // number of tests.
  std::string name_not_in_storages_;
};

std::vector<const Storage*> MakeNamedStorageMapIntoStorageVector(
    const Module::NamedStorageMap& map) {
  std::vector<const Storage*> storage_vector;
  storage_vector.reserve(map.size());
  for (const auto& name_storage_pair : map) {
    CHECK(name_storage_pair.first == name_storage_pair.second->name());
    storage_vector.push_back(name_storage_pair.second.get());
  }
  return storage_vector;
}

void TestCreateStorages(std::vector<std::string> storage_names) {
  ModuleStorageTestFixture fixture(std::move(storage_names));
  EXPECT_THAT(MakeNamedStorageMapIntoStorageVector(
                  fixture.module().named_storage_map()),
              UnorderedElementsAreArray(fixture.created_storages()));
  const Storage& storage = fixture.module().CreateStorage(
      fixture.name_not_in_storages(),
      fixture.type_factory().MakePrimitiveType());
  std::vector<const Storage*> created_storages_plus_new_element;
  created_storages_plus_new_element.push_back(&storage);
  created_storages_plus_new_element.insert(
      created_storages_plus_new_element.end(),
      fixture.created_storages().begin(), fixture.created_storages().end());
  EXPECT_THAT(MakeNamedStorageMapIntoStorageVector(
                  fixture.module().named_storage_map()),
              UnorderedElementsAreArray(created_storages_plus_new_element));
}

FUZZ_TEST(ModuleStorageTest, TestCreateStorages)
    .WithDomains(NonEmpty(UniqueElementsVectorOf(PrintableAsciiString())));

void GetAddedStorageBehavesCorrectly(std::vector<std::string> storage_names) {
  ModuleStorageTestFixture fixture(std::move(storage_names));
  for (const Storage* storage : fixture.created_storages()) {
    EXPECT_EQ(&fixture.module().GetStorage(storage->name()), storage);
  }
}

FUZZ_TEST(ModuleStorageTest, GetAddedStorageBehavesCorrectly)
    .WithDomains(NonEmpty(UniqueElementsVectorOf(PrintableAsciiString())));

void GetNotPresentStorageFails(std::vector<std::string> storage_names) {
  ModuleStorageTestFixture fixture(std::move(storage_names));
  EXPECT_DEATH(fixture.module().GetStorage(fixture.name_not_in_storages()),
               absl::StrCat(".*Could not find Storage with name.*"));
}

FUZZ_TEST(ModuleStorageTest, GetNotPresentStorageFails)
    .WithDomains(NonEmpty(UniqueElementsVectorOf(PrintableAsciiString())));

void CreateStorageSameNameDies(std::vector<std::string> storage_names,
                               uint64_t idx) {
  ModuleStorageTestFixture fixture(std::move(storage_names));
  absl::string_view name_to_duplicate =
      fixture.created_storages()
          .at(idx % fixture.created_storages().size())
          ->name();
  EXPECT_DEATH(
      fixture.module().CreateStorage(
          name_to_duplicate, fixture.type_factory().MakePrimitiveType()),
      ".*Multiple stores with name.*");
}

FUZZ_TEST(ModuleStorageTest, CreateStorageSameNameDies)
    .WithDomains(UniqueElementsVectorOf(PrintableAsciiString()).WithMinSize(2),
                 Arbitrary<uint64_t>());

void AddValueToStorage(std::vector<std::string> storage_names, uint64_t idx) {
  ModuleStorageTestFixture fixture(std::move(storage_names));
  absl::string_view name_to_add_value_to =
      fixture.created_storages()
          .at(idx % fixture.created_storages().size())
          ->name();
  Value value_to_add = Value(Any());
  fixture.module().AddInputValueToStorage(name_to_add_value_to, value_to_add);
  for (const auto& [name, storage] : fixture.module().named_storage_map()) {
    if (name == name_to_add_value_to) {
      EXPECT_THAT(storage->input_values(),
                  UnorderedElementsAreArray({value_to_add}));
    } else {
      EXPECT_THAT(storage->input_values(), IsEmpty());
    }
  }
}

FUZZ_TEST(ModuleStorageTest, AddValueToStorage)
    .WithDomains(UniqueElementsVectorOf(PrintableAsciiString()).WithMinSize(2),
                 Arbitrary<uint64_t>());

}  // namespace
}  // namespace raksha::ir
