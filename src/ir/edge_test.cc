#include "src/ir/edge.h"

#include "src/common/testing/gtest.h"
#include "src/ir/access_path_root.h"

namespace raksha::ir {

static const AccessPathSelectors x_y_access_path_selectors =
    AccessPathSelectors(
        Selector(FieldSelector("x")),
        AccessPathSelectors(Selector(FieldSelector("y"))));

static const std::tuple<Edge, std::string> edge_tostring_pairs[] = {
    { Edge(
        AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
            "recipe", "particle", "handle")),
                   AccessPathSelectors(Selector(FieldSelector("field1")))),
        AccessPath(
            AccessPathRoot(HandleConnectionAccessPathRoot(
                "recipe2", "particle2", "handle2")),
            AccessPathSelectors(Selector(FieldSelector("field2"))))),
        "edge(\"recipe.particle.handle.field1\", "
        "\"recipe2.particle2.handle2.field2\").\n"},
    { Edge(
            AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
                "r", "p", "h")),
                       x_y_access_path_selectors),
            AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
                "r", "p", "h2")),
                       x_y_access_path_selectors)),
      "edge(\"r.p.h.x.y\", \"r.p.h2.x.y\").\n" },
    { Edge(
          AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
              "pre", "fix", "1")),
                     AccessPathSelectors()),
          AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
              "pre", "fix", "2")),
                     AccessPathSelectors())),
      "edge(\"pre.fix.1\", \"pre.fix.2\").\n"} };

class EdgeToStringTest :
    public testing::TestWithParam<std::tuple<Edge, std::string>> {};

TEST_P(EdgeToStringTest, EdgeToStringTest) {
  const Edge &edge = std::get<0>(GetParam());
  const std::string &expected_to_string = std::get<1>(GetParam());

  EXPECT_EQ(edge.ToString(), expected_to_string);
}

INSTANTIATE_TEST_SUITE_P(EdgeToStringTest, EdgeToStringTest,
                         testing::ValuesIn(edge_tostring_pairs));


}  // namespace raksha::ir
