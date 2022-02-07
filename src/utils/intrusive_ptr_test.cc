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
  RefCountedType(std::string id = "<unknown>", bool some_flag = false)
      : id_(id), some_flag_(some_flag), count_(0) {}

  unsigned int count() const { return count_; }
  bool some_flag() const { return some_flag_; }
  void set_id(std::string id) { std::swap(id_, id); }
  const std::string& id() const { return id_; }
  void Retain() { count_++; }
  void Release() {
    count_--;
    if (count_ == 0) {
      delete this;
    }
  }

 private:
  std::string id_;
  bool some_flag_;
  unsigned int count_;
};

using RefCountedTypePtr = intrusive_ptr<RefCountedType>;

// We create a raw pointer for test. However, in practice, we should avoid
// creating raw pointers and use `make_intrusive_ptr` factory method to create
// and warp the object immediately.
RefCountedType* MakeRefCountedTypeInstance(std::string id = "<unknown>",
                                           bool some_flag = false) {
  return new RefCountedType(id, some_flag);
}

TEST(IntrusivePtrTest, DefaultConstructorInitializesToNullptr) {
  RefCountedTypePtr null;
  EXPECT_EQ(null.get(), nullptr);
}

TEST(IntrusivePtrTest, ConstructorIncreasesCountAndSavesPtr) {
  RefCountedType* value = MakeRefCountedTypeInstance();
  EXPECT_EQ(value->count(), 0);
  RefCountedTypePtr ptr(value);
  EXPECT_EQ(value->count(), 1);

  EXPECT_EQ(ptr.get(), value);
  // When `ptr` is destroyed, object pointed to by `value` gets destroyed.
}

TEST(IntrusivePtrTest, CopyConstructorIncreasesCountAndSharesPtr) {
  RefCountedType* value = MakeRefCountedTypeInstance();
  EXPECT_EQ(value->count(), 0);
  RefCountedTypePtr ptr(value);
  EXPECT_EQ(value->count(), 1);

  RefCountedTypePtr copy = ptr;
  EXPECT_EQ(value->count(), 2);
  EXPECT_EQ(ptr.get(), copy.get());
  // When `ptr` and `object` are destroyed, object pointed to by `value` gets
  // destroyed.
}

TEST(IntrusivePtrTest, MoveConstructorMaintainsCountAndNullifiesOther) {
  RefCountedType* value = MakeRefCountedTypeInstance();
  EXPECT_EQ(value->count(), 0);
  RefCountedTypePtr ptr(value);
  EXPECT_EQ(value->count(), 1);

  RefCountedTypePtr moved = std::move(ptr);
  EXPECT_EQ(value->count(), 1);
  EXPECT_EQ(ptr.get(), nullptr);
  // When `moved` is destroyed, object pointed to by `value` gets destroyed.
}

TEST(IntrusivePtrTest, AssignmentIncreasesCountAndSharesPtr) {
  RefCountedType* value = MakeRefCountedTypeInstance();
  EXPECT_EQ(value->count(), 0);
  RefCountedTypePtr ptr(value);
  EXPECT_EQ(value->count(), 1);

  RefCountedTypePtr assigned;
  EXPECT_EQ(assigned.get(), nullptr);

  assigned = ptr;
  EXPECT_EQ(value->count(), 2);
  EXPECT_EQ(ptr.get(), assigned.get());
  // When `ptr` and `assigned` are destroyed, the object pointed to by `value`
  // gets destroyed.
}

TEST(IntrusivePtrTest, GetReturnsSavedPointer) {
  auto value = MakeRefCountedTypeInstance();
  RefCountedTypePtr ptr(value);
  EXPECT_EQ(ptr.get(), value);
  // When `ptr` is destroyed, the object pointed to by `value` is destroyed.
}

TEST(IntrusivePtrTest, DereferenceReturnsSavedObject) {
  auto value = MakeRefCountedTypeInstance();
  RefCountedTypePtr ptr(value);
  EXPECT_EQ(&(*ptr), value);
  // When `ptr` is destroyed, the object pointed to by `value` is destroyed.
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
  auto value = MakeRefCountedTypeInstance("NewValue");
  RefCountedTypePtr ptr(value);
  EXPECT_EQ(value->id(), "NewValue");
  ptr->set_id("UpdatedValue");
  EXPECT_EQ(value->id(), "UpdatedValue");
  // When `ptr` is destroyed, the object pointed to by `value` is destroyed.
}

TEST(IntrusivePtrTest, FactoryConstructionWorks) {
  auto default_obj = make_intrusive_ptr<RefCountedType>();
  EXPECT_EQ(default_obj->count(), 1);
  EXPECT_EQ(default_obj->id(), "<unknown>");
  EXPECT_EQ(default_obj->some_flag(), false);

  auto ptr = make_intrusive_ptr<RefCountedType>("NewValue");
  EXPECT_EQ(ptr->count(), 1);
  EXPECT_EQ(ptr->id(), "NewValue");
  EXPECT_EQ(ptr->some_flag(), false);

  auto another_ptr = make_intrusive_ptr<RefCountedType>("AnotherValue", true);
  EXPECT_EQ(another_ptr->count(), 1);
  EXPECT_EQ(another_ptr->id(), "AnotherValue");
  EXPECT_EQ(another_ptr->some_flag(), true);
  EXPECT_NE(ptr.get(), another_ptr.get());
  // All objects will be destroyed when the intrusive_pointers go out of scope.
}

TEST(IntrusivePtrTest, DestructorReducesCount) {
  RefCountedTypePtr ptr = make_intrusive_ptr<RefCountedType>();
  EXPECT_EQ(ptr->count(), 1);
  {
    RefCountedTypePtr copy = ptr;
    EXPECT_EQ(ptr->count(), 2);
    {
      RefCountedTypePtr another_copy = copy;
      EXPECT_EQ(ptr->count(), 3);
      // `another_copy` is destroyed at the end of this scope, but `copy` is
      // alive.
    }
    EXPECT_EQ(ptr->count(), 2);
    // `copy` is destroyed at the end of this scope.
  }
  EXPECT_EQ(ptr->count(), 1);
}

}  // namespace
}  // namespace raksha
