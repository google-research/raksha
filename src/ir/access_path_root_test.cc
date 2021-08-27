#include "src/ir/access_path_root.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {

TEST(SpecAccessPathRootBehaviorTest, SpecAccessPathRootBehaviorTest) {
  SpecAccessPathRoot spec_access_path_root;
  AccessPathRoot test_access_path_root(spec_access_path_root);
  EXPECT_FALSE(test_access_path_root.IsInstantiated());
  EXPECT_DEATH(test_access_path_root.ToString(),
               "Attempted to print out an AccessPath before connecting it "
                  "to a fully-instantiated root!");
}

class ConcreteAccessPathRootTest :
    public testing::TestWithParam<std::string> {};

TEST_P(ConcreteAccessPathRootTest, ConcreteAccessPathRootTest) {
  const std::string &root_string = GetParam();
  ConcreteAccessPathRoot concrete_access_path_root(root_string);
  AccessPathRoot test_access_path_root(concrete_access_path_root);
  EXPECT_TRUE(test_access_path_root.IsInstantiated());
  EXPECT_EQ(test_access_path_root.ToString(), root_string);
}

INSTANTIATE_TEST_SUITE_P(
    ConcreteAccessPathRootTest, ConcreteAccessPathRootTest,
    testing::Values("recipe.particle", "recipe.handle", "arbitrary"));

}  // namespace raksha::ir
