#include "src/transform/ir_proto_to_datalog/arcs_manifest_tree/primitive_type.h"

#include "src/common/testing/gtest.h"
#include "src/ir/access_path_selectors.h"
#include "src/ir/access_path_selectors_set.h"

namespace raksha::transform::arcs_manifest_tree {

namespace ir = raksha::ir;

TEST(TestGetAccessPaths, TestGetAccessPaths) {
  PrimitiveType primitive_type;
  absl::flat_hash_set<ir::AccessPathSelectors>
      access_path_selectors_set =
        ir::AccessPathSelectorsSet::CreateAbslSet(
            primitive_type.GetAccessPaths());
  ASSERT_EQ(access_path_selectors_set.size(), 1);
  ASSERT_EQ(*access_path_selectors_set.begin(), ir::AccessPathSelectors());
}

}  // namespace raksha::transform::ir_proto_to_datalog
