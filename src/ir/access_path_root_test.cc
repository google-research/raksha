#include "src/ir/access_path_root.h"

#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"

namespace raksha::ir {

TEST(HandleConnectionSpecAccessPathRootBehaviorTest,
    HandleConnectionSpecAccessPathRootBehaviorTest) {
  HandleConnectionSpecAccessPathRoot spec_access_path_root
    ("particle_spec_name", "handle_access_path_spec_name");
  AccessPathRoot test_access_path_root(spec_access_path_root);
  EXPECT_FALSE(test_access_path_root.IsInstantiated());
  EXPECT_DEATH(test_access_path_root.ToString(),
               "Attempted to print out an AccessPath before connecting it "
                  "to a fully-instantiated root!");
}

TEST(HandleConnectionAccessPathRootTest, HandleConnectionAccessPathRootTest) {
  HandleConnectionAccessPathRoot handle_connection_access_path_root(
      "recipe", "particle", "handle");
  AccessPathRoot test_access_path_root(handle_connection_access_path_root);
  EXPECT_TRUE(test_access_path_root.IsInstantiated());
  EXPECT_EQ(test_access_path_root.ToString(), "recipe.particle.handle");
}

class ConnectionSpecRootProtoTest :
    public testing::TestWithParam<
      std::tuple<std::string, std::string, std::string>>{};

TEST_P(ConnectionSpecRootProtoTest, ConnectionSpecRootProtoTest) {
  std::string textproto;
  std::string expected_particle_spec;
  std::string expected_handle_connection;
  std::tie(textproto, expected_particle_spec, expected_handle_connection) =
      GetParam();

  arcs::AccessPathProto_HandleRoot orig_handle_root_proto;
  google::protobuf::TextFormat::ParseFromString(
      textproto, &orig_handle_root_proto);

  HandleConnectionSpecAccessPathRoot connection_spec_root =
      HandleConnectionSpecAccessPathRoot::CreateFromProto(
          orig_handle_root_proto);

  EXPECT_EQ(connection_spec_root.particle_spec_name(), expected_particle_spec);
  EXPECT_EQ(connection_spec_root.handle_connection_spec_name(),
            expected_handle_connection);

  arcs::AccessPathProto_HandleRoot result_handle_root_proto =
      connection_spec_root.MakeProto();

  EXPECT_TRUE(google::protobuf::util::MessageDifferencer::Equals
    (orig_handle_root_proto, result_handle_root_proto));
}

static std::tuple<std::string, std::string, std::string>
  textproto_particle_spec_and_handle_connections[] = {
    { "particle_spec: \"ps\" handle_connection: \"hc\"", "ps", "hc" },
    { "particle_spec: \"p_spec\" handle_connection: \"handle\"",
      "p_spec", "handle" },
    {  "particle_spec: \"a\" handle_connection: \"b\"", "a", "b"}
};

INSTANTIATE_TEST_SUITE_P(
    ConnectionSpecRootProtoTest, ConnectionSpecRootProtoTest,
    testing::ValuesIn(textproto_particle_spec_and_handle_connections));

enum AccessPathRootKind {
  kHandleConnectionSpec,
  kHandleConnection
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
      .arg_strings = {"recipe", "particle", "handle2"}  },
};

INSTANTIATE_TEST_SUITE_P(
    AccessPathRootEqTest, AccessPathRootEqTest,
    testing::Combine(testing::ValuesIn(root_and_info_array),
                     testing::ValuesIn(root_and_info_array)));

}  // namespace raksha::ir
