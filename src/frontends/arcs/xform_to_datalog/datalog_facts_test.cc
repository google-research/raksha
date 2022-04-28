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
#include "src/frontends/arcs/xform_to_datalog/datalog_facts.h"

#include "src/common/testing/gtest.h"
#include "src/common/utils/test/utils.h"
#include "src/frontends/arcs/datalog_print_context.h"
#include "src/frontends/arcs/xform_to_datalog/authorization_logic_datalog_facts.h"
#include "src/frontends/arcs/xform_to_datalog/manifest_datalog_facts.h"

namespace raksha::xform_to_datalog {

namespace arcs = frontends::arcs;

// If authorization logic is disabled, just run a single, always-true dummy
// test.
#ifdef DISABLE_AUTHORIZATION_LOGIC

TEST(Dummy, Dummy) { ASSERT_EQ(1, 1); }

#else

const std::string path = "src/frontends/arcs/xform_to_datalog/testdata";

class DatalogFactsTest
    : public testing::TestWithParam<std::tuple<
          ManifestDatalogFacts, AuthorizationLogicDatalogFacts, std::string> > {
};

TEST_P(DatalogFactsTest, IncludesManifestFactsWithCorrectPrefixAndSuffix) {
  const auto &[manifest_datalog_facts, auth_logic_datalog_facts,
               expected_string] = GetParam();
  DatalogFacts datalog_facts(manifest_datalog_facts, auth_logic_datalog_facts);
  arcs::DatalogPrintContext ctxt;
  EXPECT_EQ(datalog_facts.ToDatalog(ctxt), expected_string);
}

static std::unique_ptr<arcs::ParticleSpec> particle_spec(
    arcs::ParticleSpec::Create(
        "particle",
        /*checks=*/{},
        /*tag_claims=*/
        {arcs::TagClaim(
            "particle",
            arcs::AccessPath(
                arcs::AccessPathRoot(arcs::HandleConnectionAccessPathRoot(
                    "recipe", "particle", "out")),
                ir::AccessPathSelectors()),
            true, "tag")},
        /*derives_from_claims=*/{},
        /*edges=*/{}));

INSTANTIATE_TEST_SUITE_P(
    DatalogFactsTest, DatalogFactsTest,
    testing::Values(
        std::make_tuple(ManifestDatalogFacts(),
                        *(AuthorizationLogicDatalogFacts::create(
                            utils::test::GetTestDataDir(path),
                            "empty_auth_logic")),
                        R"(// GENERATED FILE, DO NOT EDIT!

#include "src/analysis/souffle/taint.dl"
#include "src/analysis/souffle/may_will.dl"

// Rules for detecting policy failures.
.decl testFails(check_index: symbol)
.output testFails(IO=stdout)
.decl allTests(check_index: symbol)
.output allTests(IO=stdout)
.decl duplicateTestCaseNames(testAspectName: symbol)
.output duplicateTestCaseNames(IO=stdout)
.output disallowedUsage(IO=stdout)

.decl isCheck(check_index: symbol, path: AccessPath)
.decl check(check_index: symbol, owner: Principal, path: AccessPath)

allTests(check_index) :- isCheck(check_index, _).
testFails(cat(check_index, "-", owner, "-", path)) :-
  isCheck(check_index, path), ownsAccessPath(owner, path),
  !check(check_index, owner, path).

testFails("may_will") :- disallowedUsage(_, _, _, _).

.decl says_may(speaker: Principal, actor: Principal, usage: Usage, tag: Tag)
.decl says_will(speaker: Principal, usage: Usage, path: AccessPath)
saysMay(w, x, y, z) :- says_may(w, x, y, z).
saysWill(w, x, y) :- says_will(w, x, y).

// Manifest
// Claims:

// Checks:

// Edges:


// Authorization Logic
.type DummyType <: symbol
.decl grounded_dummy(dummy_param: DummyType)
.decl says_canActAs(speaker: Principal, p1: Principal, p2: Principal)
.decl says_isNumber(speaker: Principal, x: number)
grounded_dummy("dummy_var").

)"),
        std::make_tuple(ManifestDatalogFacts({ManifestDatalogFacts::Particle(
                            particle_spec.get(), {}, {})}),
                        *(AuthorizationLogicDatalogFacts::create(
                            utils::test::GetTestDataDir(path),
                            "simple_auth_logic")),
                        R"(// GENERATED FILE, DO NOT EDIT!

#include "src/analysis/souffle/taint.dl"
#include "src/analysis/souffle/may_will.dl"

// Rules for detecting policy failures.
.decl testFails(check_index: symbol)
.output testFails(IO=stdout)
.decl allTests(check_index: symbol)
.output allTests(IO=stdout)
.decl duplicateTestCaseNames(testAspectName: symbol)
.output duplicateTestCaseNames(IO=stdout)
.output disallowedUsage(IO=stdout)

.decl isCheck(check_index: symbol, path: AccessPath)
.decl check(check_index: symbol, owner: Principal, path: AccessPath)

allTests(check_index) :- isCheck(check_index, _).
testFails(cat(check_index, "-", owner, "-", path)) :-
  isCheck(check_index, path), ownsAccessPath(owner, path),
  !check(check_index, owner, path).

testFails("may_will") :- disallowedUsage(_, _, _, _).

.decl says_may(speaker: Principal, actor: Principal, usage: Usage, tag: Tag)
.decl says_will(speaker: Principal, usage: Usage, path: AccessPath)
saysMay(w, x, y, z) :- says_may(w, x, y, z).
saysWill(w, x, y) :- says_will(w, x, y).

// Manifest
// Claims:
says_hasTag("particle", "recipe.particle.out", owner, "tag") :- ownsAccessPath(owner, "recipe.particle.out").

// Checks:

// Edges:


// Authorization Logic
.type DummyType <: symbol
.type SomeType <: symbol
.decl grounded_dummy(dummy_param: DummyType)
.decl says_canActAs(speaker: Principal, p1: Principal, p2: Principal)
.decl says_cond1(speaker: Principal, someType: SomeType)
.decl says_fact1(speaker: Principal, someType: SomeType)
.decl says_isNumber(speaker: Principal, x: number)
.decl says_isSomeType(speaker: Principal, x: SomeType)
grounded_dummy("dummy_var").
says_cond1("prin1", "foo").
says_fact1("prin1", thing_x) :- says_cond1("prin1", thing_x), says_isSomeType("prin1", thing_x).
says_isPrincipal("prin1", "prin1").
says_isPrincipal("prin1", "prin1").
says_isSomeType("prin1", "foo").
says_isSomeType("prin1", "foo").

)")));

#endif

}  // namespace raksha::xform_to_datalog
