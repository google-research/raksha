#include "src/frontends/arcs/access_path.h"

#include "absl/hash/hash_testing.h"
#include "src/common/testing/gtest.h"

namespace raksha::frontends::arcs {

static ir::Selector MakeFieldSelector(std::string field_name) {
  return ir::Selector(ir::FieldSelector(field_name));
}

static const std::tuple<AccessPath, std::string>
    access_path_and_expected_tostring_pairs[] = {
        {AccessPath(
             AccessPathRoot(HandleConnectionAccessPathRoot("a", "b", "c")),
             ir::AccessPathSelectors(
                 MakeFieldSelector("d"),
                 ir::AccessPathSelectors(MakeFieldSelector("e"),
                                         ir::AccessPathSelectors()))),
         "a.b.c.d.e"},
        {AccessPath(AccessPathRoot(
                        HandleConnectionAccessPathRoot("foo", "bar", "baz")),
                    ir::AccessPathSelectors()),
         "foo.bar.baz"},
        {AccessPath(AccessPathRoot(
                        HandleConnectionAccessPathRoot("foo", "bar", "baz")),
                    ir::AccessPathSelectors(MakeFieldSelector("field"),
                                            ir::AccessPathSelectors())),
         "foo.bar.baz.field"}};

class AccessPathToStringTest
    : public testing::TestWithParam<std::tuple<AccessPath, std::string>> {};

TEST_P(AccessPathToStringTest, AccessPathToStringTest) {
  const AccessPath &access_path = std::get<0>(GetParam());
  const std::string &expected_to_string = std::get<1>(GetParam());
  EXPECT_EQ(access_path.ToString(), expected_to_string);
}

INSTANTIATE_TEST_SUITE_P(
    AccessPathToStringTest, AccessPathToStringTest,
    testing::ValuesIn(access_path_and_expected_tostring_pairs));

class AccessPathEqualsTest
    : public testing::TestWithParam<
          std::tuple<std::tuple<AccessPathRoot, ir::AccessPathSelectors>,
                     std::tuple<AccessPathRoot, ir::AccessPathSelectors>>> {};

TEST_P(AccessPathEqualsTest, AccessPathEqualsTest) {
  const std::tuple<AccessPathRoot, ir::AccessPathSelectors> access_path_args1 =
      std::get<0>(GetParam());
  const std::tuple<AccessPathRoot, ir::AccessPathSelectors> access_path_args2 =
      std::get<1>(GetParam());

  AccessPath access_path1(std::get<0>(access_path_args1),
                          std::get<1>(access_path_args1));
  AccessPath access_path2(std::get<0>(access_path_args2),
                          std::get<1>(access_path_args2));

  EXPECT_EQ(access_path1 == access_path2,
            access_path_args1 == access_path_args2);
}

// Note: be careful adding elements to sample_roots and sample_selectors. We
// create an access path for each combination of elements in
// (sample_roots, sample_selectors) and then compare each of those access
// paths against each other. That means that we generate a number of tests
// equal to the square of the size of sample_roots * sample_selectors.
static AccessPathRoot sample_roots[] = {
    AccessPathRoot(HandleConnectionSpecAccessPathRoot("spec1", "handle_spec1")),
    AccessPathRoot(
        HandleConnectionAccessPathRoot("recipe", "particle", "handle")),
    AccessPathRoot(HandleAccessPathRoot("recipe", "handle"))};

static ir::AccessPathSelectors sample_selectors[] = {
    ir::AccessPathSelectors(),
    ir::AccessPathSelectors(ir::Selector(ir::FieldSelector("field1"))),
    ir::AccessPathSelectors(
        ir::Selector(ir::FieldSelector("x")),
        ir::AccessPathSelectors(ir::Selector(ir::FieldSelector("y"))))};

INSTANTIATE_TEST_SUITE_P(
    AccessPathEqualsTest, AccessPathEqualsTest,
    testing::Combine(testing::Combine(testing::ValuesIn(sample_roots),
                                      testing::ValuesIn(sample_selectors)),
                     testing::Combine(testing::ValuesIn(sample_roots),
                                      testing::ValuesIn(sample_selectors))));

TEST(AccessPathHashTest, AccessPathHashTest) {
  std::vector<AccessPath> test_access_paths;
  for (const AccessPathRoot &access_path_root : sample_roots) {
    for (const ir::AccessPathSelectors &access_path_selectors :
         sample_selectors) {
      test_access_paths.push_back(
          AccessPath(access_path_root, access_path_selectors));
    }
  }
  EXPECT_TRUE(absl::VerifyTypeImplementsAbslHashCorrectly(test_access_paths));
}

}  // namespace raksha::frontends::arcs
