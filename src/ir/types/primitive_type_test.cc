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
#include "src/ir/types/primitive_type.h"

#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"

namespace raksha::ir::types {

namespace ir = raksha::ir;

TEST(PrimitiveTypeTest, KindReturnsCorrectKind) {
  PrimitiveType primitive_type;
  EXPECT_EQ(primitive_type.kind(), TypeBase::Kind::kPrimitive);
}

TEST(TestGetAccessPaths, TestGetAccessPaths) {
  PrimitiveType primitive_type;
  absl::flat_hash_set<ir::AccessPathSelectors>
      access_path_selectors_set =
        ir::AccessPathSelectorsSet::CreateAbslSet(
            primitive_type.GetAccessPathSelectorsSet());
  ASSERT_EQ(access_path_selectors_set.size(), 1);
  ASSERT_EQ(*access_path_selectors_set.begin(), ir::AccessPathSelectors());
}

}  // namespace raksha::ir::types
