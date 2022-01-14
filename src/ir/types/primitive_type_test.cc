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
