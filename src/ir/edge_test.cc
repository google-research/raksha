//-----------------------------------------------------------------------------
// Copyright 2021 Google LLC
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

#include "src/ir/edge.h"

#include "src/common/testing/gtest.h"
#include "src/ir/access_path_root.h"
#include "src/ir/noop_instantiator.h"

namespace raksha::ir {

static const AccessPathSelectors x_y_access_path_selectors =
    AccessPathSelectors(
        Selector(FieldSelector("x")),
        AccessPathSelectors(Selector(FieldSelector("y"))));

static const std::tuple<Edge, std::string> edge_todatalog_pairs[] = {
    { Edge(
        AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
            "recipe", "particle", "handle")),
                   AccessPathSelectors(Selector(FieldSelector("field1")))),
        AccessPath(
            AccessPathRoot(HandleConnectionAccessPathRoot(
                "recipe2", "particle2", "handle2")),
            AccessPathSelectors(Selector(FieldSelector("field2"))))),
        "edge(\"recipe.particle.handle.field1\", "
        "\"recipe2.particle2.handle2.field2\")."},
    { Edge(
            AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
                "r", "p", "h")),
                       x_y_access_path_selectors),
            AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
                "r", "p", "h2")),
                       x_y_access_path_selectors)),
      "edge(\"r.p.h.x.y\", \"r.p.h2.x.y\")." },
    { Edge(
          AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
              "pre", "fix", "1")),
                     AccessPathSelectors()),
          AccessPath(AccessPathRoot(HandleConnectionAccessPathRoot(
              "pre", "fix", "2")),
                     AccessPathSelectors())),
      "edge(\"pre.fix.1\", \"pre.fix.2\")."} };

class EdgeToDatalogTest :
    public testing::TestWithParam<std::tuple<Edge, std::string>> {};

TEST_P(EdgeToDatalogTest, EdgeToDatalogTest) {
  const Edge &edge = std::get<0>(GetParam());
  const std::string &expected_to_string = std::get<1>(GetParam());

  EXPECT_EQ(edge.ToDatalog(NoopInstantiator::Get()), expected_to_string);
}

INSTANTIATE_TEST_SUITE_P(EdgeToDatalogTest, EdgeToDatalogTest,
                         testing::ValuesIn(edge_todatalog_pairs));

class EdgeEqTest : public testing::TestWithParam<
    std::tuple<
      std::tuple<AccessPath, AccessPath>,
      std::tuple<AccessPath, AccessPath>>> {};

TEST_P(EdgeEqTest, EdgeEqTest) {
  const std::tuple<AccessPath, AccessPath> &edge_args1 =
      std::get<0>(GetParam());
  const std::tuple<AccessPath, AccessPath> &edge_args2 =
      std::get<1>(GetParam());

  Edge edge1(std::get<0>(edge_args1), std::get<1>(edge_args1));
  Edge edge2(std::get<0>(edge_args2), std::get<1>(edge_args2));

  EXPECT_EQ(edge1 == edge2, edge_args1 == edge_args2);
}

// We only have two access paths here. The reason is that we will use each
// possible pair of access paths to make edges, and use each possible pair of
// edges as test inputs. That means that N entries in this array will create
// N^4 tests. Be very careful adding more access paths, as that's a very steep
// growth curve.
static AccessPath sample_access_paths[] = {
    AccessPath(
        AccessPathRoot(HandleConnectionSpecAccessPathRoot("pspec", "hspec")),
        AccessPathSelectors(Selector(FieldSelector("field1")))),
    AccessPath(
        AccessPathRoot(HandleConnectionAccessPathRoot(
            "recipe", "particle", "handle")),
        AccessPathSelectors(Selector(FieldSelector("field2")))),
};

INSTANTIATE_TEST_SUITE_P(
    EdgeEqTest, EdgeEqTest,
    testing::Combine(
        testing::Combine(testing::ValuesIn(sample_access_paths),
                         testing::ValuesIn(sample_access_paths)),
        testing::Combine(testing::ValuesIn(sample_access_paths),
                         testing::ValuesIn(sample_access_paths))
        ));

}  // namespace raksha::ir
