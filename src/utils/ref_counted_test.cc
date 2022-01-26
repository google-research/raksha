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
#include "src/utils/ref_counted.h"

#include <iostream>
#include <string>

#include "src/common/testing/gtest.h"

namespace raksha {

class RefCountedType : public RefCounted<RefCountedType> {
 public:
  static unsigned GetDestructorCalls() { return destructor_calls_; }
  static void ResetDestructorCalls() { destructor_calls_ = 0; }

  virtual ~RefCountedType() { destructor_calls_++; }

 private:
  static unsigned destructor_calls_;
};

unsigned RefCountedType::destructor_calls_ = 0;

template <>
class RefCountManager<RefCountedType> {
 public:
  static void Retain(RefCountedType* ptr) { ptr->Retain(); }
  static void Release(RefCountedType* ptr) { ptr->Release(); }
};

namespace {

TEST(RefCountedTest, CallsDestructorOnlyWhenRefCountIsZero) {
  RefCountedType* x = new RefCountedType();
  RefCountedType::ResetDestructorCalls();
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  RefCountManager<RefCountedType>::Retain(x);
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  RefCountManager<RefCountedType>::Retain(x);
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  RefCountManager<RefCountedType>::Release(x);
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  RefCountManager<RefCountedType>::Retain(x);
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  RefCountManager<RefCountedType>::Release(x);
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  RefCountManager<RefCountedType>::Release(x);
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 1);
}

TEST(RefCountedTest, FailsWhenReleaseCalledOnZeroRefCount) {
  EXPECT_DEATH(
      {
        RefCountedType x;
        RefCountManager<RefCountedType>::Release(&x);
      },
      "Reference count is already zero.");
}

}  // namespace

}  // namespace raksha
