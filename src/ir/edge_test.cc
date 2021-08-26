#include "src/ir/edge.h"

#include "src/common/testing/gtest.h"

namespace raksha::ir {

static const AccessPathSelectors x_y_access_path_selectors =
    AccessPathSelectors(
        Selector(FieldSelector("x")),
        AccessPathSelectors(Selector(FieldSelector("y"))));

static const std::tuple<Edge, std::string> edge_tostring_pairs[] = {
    { Edge(
        AccessPath("recipe.handle",
                   AccessPathSelectors(Selector(FieldSelector("field1")))),
        AccessPath("particle.connection",
                  AccessPathSelectors(Selector(FieldSelector("field2"))))),
        "edge(\"recipe.handle.field1\", \"particle.connection.field2\").\n"},
    { Edge(
            AccessPath("r.h", x_y_access_path_selectors),
            AccessPath("r.p", x_y_access_path_selectors)),
      "edge(\"r.h.x.y\", \"r.p.x.y\").\n" },
    { Edge(
          AccessPath("prefix1", AccessPathSelectors()),
          AccessPath("prefix2", AccessPathSelectors())),
      "edge(\"prefix1\", \"prefix2\").\n"} };

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
