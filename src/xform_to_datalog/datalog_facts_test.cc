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
//-----------------------------------------------------------------------------
#include "src/xform_to_datalog/datalog_facts.h"

#include "src/common/testing/gtest.h"
#include "src/xform_to_datalog/authorization_logic_test_utils.h"
#include "src/xform_to_datalog/authorization_logic_datalog_facts.h"
#include "src/xform_to_datalog/manifest_datalog_facts.h"

namespace raksha::xform_to_datalog {

class DatalogFactsTest : public testing::TestWithParam<
  std::tuple<ManifestDatalogFacts, AuthorizationLogicDatalogFacts, std::string>>
{};

TEST_P(DatalogFactsTest, IncludesManifestFactsWithCorrectPrefixAndSuffix) {
  const auto& [manifest_datalog_facts,
               auth_logic_datalog_facts,
               expected_string] = GetParam();
  DatalogFacts datalog_facts(manifest_datalog_facts, auth_logic_datalog_facts);
  EXPECT_EQ(datalog_facts.ToDatalog(), expected_string);
}

INSTANTIATE_TEST_SUITE_P(
    DatalogFactsTest, DatalogFactsTest,
    testing::Values(
        std::make_tuple(ManifestDatalogFacts({}, {}, {}),
                        *(AuthorizationLogicDatalogFacts::create(
                          AuthorizationLogicTest::GetTestDataDir().c_str(), "empty_auth_logic")),
                       R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"
.output checkHasTag

// Claims:


// Checks:


// Edges:

.decl grounded_dummy(x0: symbol)
grounded_dummy("dummy_var").
)"),
        std::make_tuple(
            ManifestDatalogFacts(
                {ir::TagClaim(
                    "particle",
                      ir::AccessPath(
                          ir::AccessPathRoot(
                              ir::HandleConnectionAccessPathRoot(
                                  "recipe", "particle", "out")),
                      ir::AccessPathSelectors()),
                      true,
                      "tag")},
                {}, {}),
            *(AuthorizationLogicDatalogFacts::create(
              AuthorizationLogicTest::GetTestDataDir().c_str(), "simple_auth_logic")),
            R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"
.output checkHasTag

// Claims:
claimHasTag("particle", "recipe.particle.out", "tag").

// Checks:


// Edges:

.decl grounded_dummy(x0: symbol)
.decl says_cond1(x0: symbol, x1: symbol)
.decl says_fact1(x0: symbol, x1: symbol)
says_fact1("prin1", thing_x) :- says_cond1("prin1", thing_x).
says_cond1("prin1", "foo").
grounded_dummy("dummy_var").
)")));

}  // namespace raksha::xform_to_datalog
