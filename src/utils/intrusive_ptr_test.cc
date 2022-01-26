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
//-----------------------------------------------------------------------------
#include "src/utils/intrusive_ptr.h"

#include <iostream>
#include <string>

#include "src/common/testing/gtest.h"

namespace raksha {
namespace {

class RefCountedType {
 public:
  RefCountedType(std::string id = "<unknown>", unsigned int initial_count = 0)
      : id_(id), count_(initial_count) {}

  unsigned int count() const { return count_; }
  void set_id(std::string id) { std::swap(id_, id); }
  const std::string& id() const { return id_; }
  void Retain() { count_++; }
  void Release() { count_--; }

 private:
  std::string id_;
  unsigned int count_;
};

using RefCountedTypePtr = intrusive_ptr<RefCountedType>;

TEST(IntrusivePtrTest, DefaultConstructorInitializesToNullptr) {
  RefCountedTypePtr null;
  EXPECT_EQ(null.get(), nullptr);
}

TEST(IntrusivePtrTest, ConstructorIncreasesCountAndSavesPtr) {
  auto value = std::make_unique<RefCountedType>();
  EXPECT_EQ(value->count(), 0);

  RefCountedTypePtr ptr(value.get());
  EXPECT_EQ(value->count(), 1);

  EXPECT_EQ(ptr.get(), value.get());
}

TEST(IntrusivePtrTest, CopyConstructorIncreasesCountAndSharesPtr) {
  auto value = std::make_unique<RefCountedType>();
  EXPECT_EQ(value->count(), 0);

  RefCountedTypePtr ptr(value.get());
  EXPECT_EQ(value->count(), 1);

  RefCountedTypePtr copy = ptr;
  EXPECT_EQ(value->count(), 2);
  EXPECT_EQ(ptr.get(), copy.get());
}

TEST(IntrusivePtrTest, MoveConstructorMaintainsCountAndNullifiesOther) {
  auto value = std::make_unique<RefCountedType>();
  EXPECT_EQ(value->count(), 0);

  RefCountedTypePtr ptr(value.get());
  EXPECT_EQ(value->count(), 1);

  RefCountedTypePtr moved = std::move(ptr);
  EXPECT_EQ(value->count(), 1);
  EXPECT_EQ(ptr.get(), nullptr);
}

TEST(IntrusivePtrTest, AssignmentIncreasesCountAndSharesPtr) {
  auto value = std::make_unique<RefCountedType>();
  EXPECT_EQ(value->count(), 0);

  RefCountedTypePtr ptr(value.get());
  EXPECT_EQ(value->count(), 1);

  RefCountedTypePtr assigned;
  EXPECT_EQ(assigned.get(), nullptr);

  assigned = ptr;
  EXPECT_EQ(value->count(), 2);
  EXPECT_EQ(ptr.get(), assigned.get());
}

TEST(IntrusivePtrTest, GetReturnsSavedPointer) {
  auto value = std::make_unique<RefCountedType>();
  RefCountedTypePtr ptr(value.get());
  EXPECT_EQ(ptr.get(), value.get());
}

TEST(IntrusivePtrTest, DereferenceReturnsSavedObject) {
  auto value = std::make_unique<RefCountedType>();
  RefCountedTypePtr ptr(value.get());
  EXPECT_EQ(&(*ptr), value.get());
}

TEST(IntrusivePtrTest, DereferencingNullCausesFailure) {
  EXPECT_DEATH(
      {
        RefCountedTypePtr null;
        EXPECT_EQ((*null).count(), 0);
      },
      "Dereferencing a nullptr!");
}

TEST(IntrusivePtrTest, ArrowOperatorProvidesAccessToSavedPtr) {
  auto value = std::make_unique<RefCountedType>("NewValue");
  RefCountedTypePtr ptr(value.get());
  EXPECT_EQ(value->id(), "NewValue");
  ptr->set_id("UpdatedValue");
  EXPECT_EQ(value->id(), "UpdatedValue");
}

TEST(IntrusivePtrTest, FactoryConstructionWorks) {
  auto default_obj = make_intrusive_ptr<RefCountedType>();
  EXPECT_EQ(default_obj->count(), 1);
  EXPECT_EQ(default_obj->id(), "<unknown>");

  auto ptr = make_intrusive_ptr<RefCountedType>("NewValue");
  EXPECT_EQ(ptr->count(), 1);
  EXPECT_EQ(ptr->id(), "NewValue");

  auto another_ptr = make_intrusive_ptr<RefCountedType>("AnotherValue", 10);
  EXPECT_EQ(another_ptr->count(), 11);
  EXPECT_EQ(another_ptr->id(), "AnotherValue");
  EXPECT_NE(ptr.get(), another_ptr.get());
}

TEST(IntrusivePtrTest, DestructorReducesCount) {
  auto value = std::make_unique<RefCountedType>();
  EXPECT_EQ(value->count(), 0);

  {
    RefCountedTypePtr ptr(value.get());
    EXPECT_EQ(value->count(), 1);
    // RefCountedTypePtr is destroyed at the end of this scope.
  }
  EXPECT_EQ(value->count(), 0);
}

}  // namespace
}  // namespace raksha
