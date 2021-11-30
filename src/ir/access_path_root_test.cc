#include "src/ir/access_path_root.h"

#include "absl/hash/hash_testing.h"
#include "src/common/testing/gtest.h"

namespace raksha::ir {

TEST(HandleConnectionSpecAccessPathRootBehaviorTest,
    HandleConnectionSpecAccessPathRootBehaviorTest) {
  HandleConnectionSpecAccessPathRoot spec_access_path_root
    ("particle_spec_name", "handle_access_path_spec_name");
  AccessPathRoot test_access_path_root(spec_access_path_root);
  EXPECT_DEATH(test_access_path_root.ToString(),
               "Attempted to print out an AccessPath before connecting it "
                  "to a fully-instantiated root!");
}

TEST(HandleConnectionAccessPathRootTest, HandleConnectionAccessPathRootTest) {
  HandleConnectionAccessPathRoot handle_connection_access_path_root(
      "recipe", "particle", "handle");
  AccessPathRoot test_access_path_root(handle_connection_access_path_root);
  EXPECT_EQ(test_access_path_root.ToString(), "recipe.particle.handle");
}

TEST(HandleAccessPathRootTest, HandleAccessPathRootTest) {
  HandleAccessPathRoot handle_connection_access_path_root("recipe", "handle");
  AccessPathRoot test_access_path_root(handle_connection_access_path_root);
  EXPECT_EQ(test_access_path_root.ToString(), "recipe.handle");
}

enum AccessPathRootKind {
  kHandleConnectionSpec,
  kHandleConnection,
  kHandle
};

// An AccessPathRoot, an enum indicating which AccessPathRoot was
// constructed, and a vector of strings used to construct the root. The roots
// should be equal exactly when the enum and arg vectors are equal.
struct AccessPathRootTypeAndArgs {
  AccessPathRoot root;
  AccessPathRootKind kind;
  std::vector<std::string> arg_strings;
};

class AccessPathRootEqTest :
 public testing::TestWithParam<
  std::tuple<AccessPathRootTypeAndArgs, AccessPathRootTypeAndArgs>> {};

TEST_P(AccessPathRootEqTest, AccessPathRootEqTest) {
  const AccessPathRootTypeAndArgs &info1 = std::get<0>(GetParam());
  const AccessPathRootTypeAndArgs &info2 = std::get<1>(GetParam());

  bool kinds_equal = info1.kind == info2.kind;
  bool args_equal = info1.arg_strings == info2.arg_strings;

  ASSERT_EQ(info1.root == info2.root, kinds_equal && args_equal);
}

static AccessPathRootTypeAndArgs root_and_info_array[] = {
    { .root = AccessPathRoot(HandleConnectionSpecAccessPathRoot(
          "particle_spec1", "handle_connection")),
      .kind = kHandleConnectionSpec,
      .arg_strings = {"particle_spec1", "handle_connection"} },
    { .root = AccessPathRoot(HandleConnectionSpecAccessPathRoot(
          "particle_spec1", "handle_connection2")),
      .kind = kHandleConnectionSpec,
      .arg_strings = {"particle_spec1", "handle_connection2"} },
    { .root = AccessPathRoot(HandleConnectionSpecAccessPathRoot(
          "particle_spec2", "handle_connection")),
      .kind = kHandleConnectionSpec,
      .arg_strings = {"particle_spec2", "handle_connection"} },
    { .root = AccessPathRoot(HandleConnectionSpecAccessPathRoot(
          "handle_connection", "particle_spec1")),
      .kind = kHandleConnectionSpec,
      .arg_strings = {"handle_connection", "particle_spec1"} },
    { .root = AccessPathRoot(HandleConnectionAccessPathRoot(
        "recipe", "particle", "handle")),
      .kind = kHandleConnection,
      .arg_strings = {"recipe", "particle", "handle"}  },
    { .root = AccessPathRoot(HandleConnectionAccessPathRoot(
        "recipe2", "particle", "handle")),
      .kind = kHandleConnection,
      .arg_strings = {"recipe2", "particle", "handle"}  },
    { .root = AccessPathRoot(HandleConnectionAccessPathRoot(
        "recipe", "particle2", "handle")),
      .kind = kHandleConnection,
      .arg_strings = {"recipe", "particle2", "handle"}  },
    { .root = AccessPathRoot(HandleConnectionAccessPathRoot(
        "recipe", "particle", "handle2")),
      .kind = kHandleConnection,
      .arg_strings = {"recipe", "particle", "handle2"} },
    { .root = AccessPathRoot(HandleAccessPathRoot("recipe", "handle")),
      .kind = kHandle,
      .arg_strings = {"recipe", "handle"} },
    { .root = AccessPathRoot(HandleAccessPathRoot("recipe2", "handle")),
      .kind = kHandle,
      .arg_strings = {"recipe2", "handle"} },
    { .root = AccessPathRoot(HandleAccessPathRoot("recipe", "handle2")),
      .kind = kHandle,
      .arg_strings = {"recipe", "handle2"} },
};

INSTANTIATE_TEST_SUITE_P(
    AccessPathRootEqTest, AccessPathRootEqTest,
    testing::Combine(testing::ValuesIn(root_and_info_array),
                     testing::ValuesIn(root_and_info_array)));

TEST(AccessPathRootHashTest, AccessPathRootHashTest) {
  std::vector<AccessPathRoot> interesting_roots;
  for (const AccessPathRootTypeAndArgs &info : root_and_info_array) {
    interesting_roots.push_back(info.root);
  }
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly(interesting_roots));
}

}  // namespace raksha::ir
