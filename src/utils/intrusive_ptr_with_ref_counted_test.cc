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
#include <iostream>
#include <string>

#include "src/common/testing/gtest.h"
#include "src/utils/intrusive_ptr.h"
#include "src/utils/ref_counted.h"

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

using RefCountedTypePtr = intrusive_ptr<RefCountedType>;

namespace {

TEST(IntrusivePtrWithRefCountedTest, CallsDestructorOnlyWhenRefCountIsZero) {
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  {
    RefCountedTypePtr copy;
    EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
    {
      RefCountedTypePtr ptr = make_intrusive_ptr<RefCountedType>();
      EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
      copy = ptr;
      // `ptr` is destroyed at the end of this scope, but `copy` is alive.
    }
    // `copy` is destroyed at the end of this scope.
    EXPECT_EQ(RefCountedType::GetDestructorCalls(), 0);
  }
  EXPECT_EQ(RefCountedType::GetDestructorCalls(), 1);
}

}  // namespace

}  // namespace raksha
