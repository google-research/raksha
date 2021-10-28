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
#include "src/ir/datalog_print_context.h"
#include "src/xform_to_datalog/authorization_logic_datalog_facts.h"
#include "src/xform_to_datalog/authorization_logic_test_utils.h"
#include "src/xform_to_datalog/manifest_datalog_facts.h"

namespace raksha::xform_to_datalog {

class DatalogFactsTest
    : public testing::TestWithParam<std::tuple<
          ManifestDatalogFacts, AuthorizationLogicDatalogFacts, std::string>> {
};

TEST_P(DatalogFactsTest, IncludesManifestFactsWithCorrectPrefixAndSuffix) {
  const auto &[manifest_datalog_facts, auth_logic_datalog_facts,
               expected_string] = GetParam();
  DatalogFacts datalog_facts(manifest_datalog_facts, auth_logic_datalog_facts);
  ir::DatalogPrintContext ctxt;
  EXPECT_EQ(datalog_facts.ToDatalog(ctxt), expected_string);
}

INSTANTIATE_TEST_SUITE_P(
    DatalogFactsTest, DatalogFactsTest,
    testing::Values(
        std::make_tuple(ManifestDatalogFacts({}, {}, {}),
                        *(AuthorizationLogicDatalogFacts::create(
                            AuthorizationLogicTest::GetTestDataDir(),
                            "empty_auth_logic")),
                        R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"

// Rules for detecting policy failures.
.decl testFails(check_index: symbol)
.output testFails
.decl allTests(check_index: symbol)
.output allTests
.decl duplicateTestCaseNames(testAspectName: symbol)
.output duplicateTestCaseNames

.decl isCheck(check_index: symbol)
.decl check(check_index: symbol)

allTests(check_index) :- isCheck(check_index).
testFails(check_index) :-
  isCheck(check_index), !check(check_index).

// Rules for linking generated relations with the ones in the dl program.
.decl says_ownsTag(speaker: Principal, owner: Principal, tag: Tag)
saysOwnsTag(x, y, z) :- says_ownsTag(x, y, z).

.decl says_hasTag(speaker: Principal, path: AccessPath, tag: Tag)
saysHasTag(x, y, z) :- says_hasTag(x, y, z).

.decl says_canSay_hasTag(
  speaker: Principal,
  delegatee: Principal,
  accessPath: AccessPath,
  tag: Tag)
saysCanSayHasTag(w, x, y, z) :- says_canSay_hasTag(w, x, y, z).

.decl says_canSay_downgrades(
    speaker: Principal, delegatee: Principal, path: AccessPath, tag: Tag)
saysCanSayDowngrades(w, x, y, z) :- says_canSay_downgrades(w, x, y, z).

.decl says_downgrades(speaker: Principal, path: AccessPath, tag: Tag)
saysDowngrades(x, y, z) :- says_downgrades(x, y, z).

// Manifest

// Claims:


// Checks:


// Edges:


// Authorization Logic
.decl grounded_dummy(x0: symbol)
grounded_dummy("dummy_var").

)"),
        std::make_tuple(
            ManifestDatalogFacts(
                {ir::TagClaim(
                    "particle",
                    ir::AccessPath(
                        ir::AccessPathRoot(ir::HandleConnectionAccessPathRoot(
                            "recipe", "particle", "out")),
                        ir::AccessPathSelectors()),
                    true, "tag")},
                {}, {}),
            *(AuthorizationLogicDatalogFacts::create(
                AuthorizationLogicTest::GetTestDataDir(),
                "simple_auth_logic")),
            R"(// GENERATED FILE, DO NOT EDIT!

#include "taint.dl"

// Rules for detecting policy failures.
.decl testFails(check_index: symbol)
.output testFails
.decl allTests(check_index: symbol)
.output allTests
.decl duplicateTestCaseNames(testAspectName: symbol)
.output duplicateTestCaseNames

.decl isCheck(check_index: symbol)
.decl check(check_index: symbol)

allTests(check_index) :- isCheck(check_index).
testFails(check_index) :-
  isCheck(check_index), !check(check_index).

// Rules for linking generated relations with the ones in the dl program.
.decl says_ownsTag(speaker: Principal, owner: Principal, tag: Tag)
saysOwnsTag(x, y, z) :- says_ownsTag(x, y, z).

.decl says_hasTag(speaker: Principal, path: AccessPath, tag: Tag)
saysHasTag(x, y, z) :- says_hasTag(x, y, z).

.decl says_canSay_hasTag(
  speaker: Principal,
  delegatee: Principal,
  accessPath: AccessPath,
  tag: Tag)
saysCanSayHasTag(w, x, y, z) :- says_canSay_hasTag(w, x, y, z).

.decl says_canSay_downgrades(
    speaker: Principal, delegatee: Principal, path: AccessPath, tag: Tag)
saysCanSayDowngrades(w, x, y, z) :- says_canSay_downgrades(w, x, y, z).

.decl says_downgrades(speaker: Principal, path: AccessPath, tag: Tag)
saysDowngrades(x, y, z) :- says_downgrades(x, y, z).

// Manifest

// Claims:
claimHasTag("particle", "recipe.particle.out", "tag").

// Checks:


// Edges:


// Authorization Logic
.decl grounded_dummy(x0: symbol)
.decl says_cond1(x0: symbol, x1: symbol)
.decl says_fact1(x0: symbol, x1: symbol)
says_fact1("prin1", thing_x) :- says_cond1("prin1", thing_x).
says_cond1("prin1", "foo").
grounded_dummy("dummy_var").

)")));

}  // namespace raksha::xform_to_datalog
