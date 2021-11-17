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

#include "src/xform_to_datalog/manifest_datalog_facts.h"

#include <google/protobuf/text_format.h>

#include "absl/strings/substitute.h"
#include "src/common/testing/gtest.h"
#include "src/ir/datalog_print_context.h"
#include "src/ir/proto/system_spec.h"
#include "src/ir/single-use-arena-and-predicate.h"

namespace raksha::xform_to_datalog {

namespace ir = raksha::ir;

class ManifestDatalogFactsToDatalogTest :
   public testing::TestWithParam<std::tuple<ManifestDatalogFacts, std::string>>
   {};

TEST_P(ManifestDatalogFactsToDatalogTest, ManifestDatalogFactsToDatalogTest) {
  const ManifestDatalogFacts &datalog_facts = std::get<0>(GetParam());
  const std::string &expected_result_string = std::get<1>(GetParam());

  ir::DatalogPrintContext ctxt;
  EXPECT_EQ(datalog_facts.ToDatalog(ctxt), expected_result_string);
}

static const ir::SingleUseArenaAndTagPresence kTag2Presence("tag2");

static const ir::AccessPath kHandleH1AccessPath(
    ir::AccessPathRoot(ir::HandleAccessPathRoot("recipe", "h1")),
    ir::AccessPathSelectors());

static const ir::AccessPath kHandleH2AccessPath(
    ir::AccessPathRoot(ir::HandleAccessPathRoot("recipe", "h2")),
    ir::AccessPathSelectors());

static const ir::AccessPath kHandleConnectionInAccessPath(
    ir::AccessPathRoot(ir::HandleConnectionAccessPathRoot(
        "recipe", "particle", "in")),
    ir::AccessPathSelectors());

static const ir::AccessPath kHandleConnectionOutAccessPath(
    ir::AccessPathRoot(ir::HandleConnectionAccessPathRoot(
        "recipe", "particle", "out")),
    ir::AccessPathSelectors());

static std::tuple<ManifestDatalogFacts, std::string>
  datalog_facts_and_output_strings[] = {
    { ManifestDatalogFacts({}, {}, {}),
      R"(
// Claims:


// Checks:


// Edges:

)" },
    { ManifestDatalogFacts(
        { ir::TagClaim(
            "particle", kHandleConnectionOutAccessPath, true, "tag") },
        { ir::TagCheck(
            kHandleConnectionInAccessPath, *kTag2Presence.predicate()) },
        { ir::Edge(kHandleH1AccessPath, kHandleConnectionInAccessPath),
          ir::Edge(kHandleConnectionInAccessPath,
                   kHandleConnectionOutAccessPath),
           ir::Edge(kHandleConnectionOutAccessPath, kHandleH2AccessPath)}),
      R"(
// Claims:
says_hasTag("particle", "recipe.particle.out", owner, "tag") :- ownsAccessPath(owner, "recipe.particle.out").

// Checks:
isCheck("check_num_0", "recipe.particle.in"). check("check_num_0", owner, "recipe.particle.in") :-
  ownsAccessPath(owner, "recipe.particle.in"), mayHaveTag("recipe.particle.in", owner, "tag2").

// Edges:
edge("recipe.h1", "recipe.particle.in").
edge("recipe.particle.in", "recipe.particle.out").
edge("recipe.particle.out", "recipe.h2").
)"
    }
};

INSTANTIATE_TEST_SUITE_P(
    ManifestDatalogFactsToDatalogTest, ManifestDatalogFactsToDatalogTest,
    testing::ValuesIn(datalog_facts_and_output_strings));

// Create a manifest textproto to test constructing ManifestDatalogFacts from
// a ManifestProto. The ParticleSpecs will be pretty simple, as we have
// tested creating ParticleSpecs from ParticleSpecProtos in more depth
// elsewhere. We're focusing more on connecting these to particles here.
//
// TODO(#107): This is currently quite complex. After the MVP, we should break
// this up into smaller, more focused tests.
static const std::string kManifestTextproto = R"(
    particle_specs: [
    { name: "PS1" connections: [
      {
        name: "out_handle" direction: WRITES
        type: {
        entity: {
          schema: {
            fields: [
              { key: "field1" value: { primitive: TEXT } }
        ]
     } } } },
      {
        name: "in_handle" direction: READS
        type: {
        entity: {
          schema: {
            fields: [
              { key: "field1" value: { primitive: TEXT } }
        ]
     } } } } ],
     claims: [
       { assume: {
           access_path: {
             handle: {
               particle_spec: "PS1",
               handle_connection: "out_handle" },
             selectors: { field: "field1" } }
           predicate: { label: { semantic_tag: "tag1"} } } }
     ],
     checks: [
       {
         access_path: {
             handle: {
               particle_spec: "PS1",
               handle_connection: "in_handle" },
             selectors: { field: "field1" } }
         predicate: { label: { semantic_tag: "tag2"} } }
     ]
    },
    { name: "PS2" connections: [
      {
        name: "out_handle" direction: WRITES
        type: {
        entity: {
          schema: {
            fields: [
              { key: "field1" value: { primitive: TEXT } }
        ]
     } } } },
      {
        name: "in_handle" direction: READS
        type: {
        entity: {
          schema: {
            fields: [
              { key: "field1" value: { primitive: TEXT } }
        ]
     } } } } ]
     claims: [
       { assume: {
           access_path: {
             handle: {
               particle_spec: "PS2",
               handle_connection: "out_handle" },
             selectors: { field: "field1" } }
           predicate: { label: { semantic_tag: "tag3"} } } }
     ],
     checks: [
       {
         access_path: {
           handle: {
             particle_spec: "PS2", handle_connection: "in_handle" },
           selectors: { field: "field1" } }
         predicate: { label: { semantic_tag: "tag4"} } }
     ]
    }
    ]
    recipes: [
     { name: "NamedR"
       particles: [
         { spec_name: "PS1"
           connections: [
             { name: "in_handle" handle: "h1" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT } },
                     { key: "field2", value: { primitive: TEXT }}]}}}},
             { name: "out_handle" handle: "h2" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT }}]}}}}]},
         { spec_name: "PS1"
           connections: [
             { name: "in_handle" handle: "h2" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT }}]}}}},
             { name: "out_handle" handle: "h3" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT } }]}}}}]},
         { spec_name: "PS2"
           connections: [
             { name: "in_handle" handle: "h3" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT }}]}}}},
             { name: "out_handle" handle: "h4" type: {
               entity: {
                 schema: {
                   fields: [
                  { key: "field1", value: { primitive: TEXT } } ]}}}}]}]},
     {
       particles: [
         { spec_name: "PS1"
           connections: [
             { name: "in_handle" handle: "h1" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT } }]}}}},
             { name: "out_handle" handle: "h2" type: {
               entity: {
                 schema: {
                   fields: [
                    { key: "field1", value: { primitive: TEXT } } ]}}}}]},
         { spec_name: "PS2"
           connections: [
             { name: "in_handle" handle: "h2" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT }}]}}}},
             { name: "out_handle" handle: "h3" type: {
               entity: {
                 schema: {
                   fields: [
                    { key: "field1", value: { primitive: TEXT } } ]}}}}]},
         { spec_name: "PS2"
           connections: [
             { name: "in_handle" handle: "h3" type: {
               entity: {
                 schema: {
                   fields: [
                     { key: "field1", value: { primitive: TEXT }}]}}}},
             { name: "out_handle" handle: "h4" type: {
               entity: {
                 schema: {
                   fields: [
                   { key: "field1", value: { primitive: TEXT } } ]}}}}]}]}]
)";

class ParseBigManifestTest : public testing::Test {
 public:
  ParseBigManifestTest() {
    arcs::ManifestProto manifest_proto;
    google::protobuf::TextFormat::ParseFromString(
        kManifestTextproto, &manifest_proto);
    system_spec_ = ir::proto::Decode(manifest_proto);
    datalog_facts_ = ManifestDatalogFacts::CreateFromManifestProto(
        system_spec_.get(), manifest_proto);
  }

 protected:
  std::unique_ptr<ir::SystemSpec> system_spec_;
  ir::DatalogPrintContext ctxt_;
  ManifestDatalogFacts datalog_facts_;
};

static const std::string kExpectedClaimStrings[] = {
    R"(says_hasTag("PS1", "NamedR.PS1#0.out_handle.field1", owner, "tag1") :- ownsAccessPath(owner, "NamedR.PS1#0.out_handle.field1").)",
    R"(says_hasTag("PS1", "NamedR.PS1#1.out_handle.field1", owner, "tag1") :- ownsAccessPath(owner, "NamedR.PS1#1.out_handle.field1").)",
    R"(says_hasTag("PS2", "NamedR.PS2#2.out_handle.field1", owner, "tag3") :- ownsAccessPath(owner, "NamedR.PS2#2.out_handle.field1").)",
    R"(says_hasTag("PS1", "GENERATED_RECIPE_NAME0.PS1#0.out_handle.field1", owner, "tag1") :- ownsAccessPath(owner, "GENERATED_RECIPE_NAME0.PS1#0.out_handle.field1").)",
    R"(says_hasTag("PS2", "GENERATED_RECIPE_NAME0.PS2#1.out_handle.field1", owner, "tag3") :- ownsAccessPath(owner, "GENERATED_RECIPE_NAME0.PS2#1.out_handle.field1").)",
    R"(says_hasTag("PS2", "GENERATED_RECIPE_NAME0.PS2#2.out_handle.field1", owner, "tag3") :- ownsAccessPath(owner, "GENERATED_RECIPE_NAME0.PS2#2.out_handle.field1").)",
};

TEST_F(ParseBigManifestTest, ManifestProtoClaimsTest) {
  // Go through all of the facts, convert them to strings, and compare them
  // against our expected strings. Strings are much easier to read on a test
  // failure than structured datalog facts.
  std::vector<std::string> claim_datalog_strings;
  for (const ir::TagClaim &claim : datalog_facts_.claims()) {
    claim_datalog_strings.push_back(claim.ToDatalog(ctxt_));
  }
  EXPECT_THAT(claim_datalog_strings,
              testing::UnorderedElementsAreArray(kExpectedClaimStrings));
}

static constexpr absl::string_view kPattern =
    R"(isCheck("$0", "$1"). check("$0", owner, "$1") :-
  ownsAccessPath(owner, "$1"), mayHaveTag("$1", owner, "$2").)";

static std::string kExpectedCheckStrings[] = {
    absl::Substitute(kPattern, "check_num_0", "NamedR.PS1#0.in_handle.field1",
                     "tag2"),
    absl::Substitute(kPattern, "check_num_1", "NamedR.PS1#1.in_handle.field1",
                     "tag2"),
    absl::Substitute(kPattern, "check_num_2", "NamedR.PS2#2.in_handle.field1",
                     "tag4"),
    absl::Substitute(kPattern, "check_num_3",
                     "GENERATED_RECIPE_NAME0.PS1#0.in_handle.field1", "tag2"),
    absl::Substitute(kPattern, "check_num_4",
                     "GENERATED_RECIPE_NAME0.PS2#1.in_handle.field1", "tag4"),
    absl::Substitute(kPattern, "check_num_5",
                     "GENERATED_RECIPE_NAME0.PS2#2.in_handle.field1", "tag4"),
};

TEST_F(ParseBigManifestTest, ManifestProtoChecksTest) {
  // Go through all of the facts, convert them to strings, and compare them
  // against our expected strings. Strings are much easier to read on a test
  // failure than structured datalog facts.
  std::vector<std::string> check_datalog_strings;
  for (const ir::TagCheck &check : datalog_facts_.checks()) {
    check_datalog_strings.push_back(check.ToDatalog(ctxt_));
  }
  EXPECT_THAT(check_datalog_strings,
              testing::UnorderedElementsAreArray(kExpectedCheckStrings));
}

static const std::string kExpectedEdgeStrings[] = {
    // Named recipe edges:
    // Edges connecting h1 to NamedR.PS1#0 for fields {field1, field2}.
    R"(edge("NamedR.h1.field1", "NamedR.PS1#0.in_handle.field1").)",
    R"(edge("NamedR.h1.field2", "NamedR.PS1#0.in_handle.field2").)",

    // Edges connecting h2 to NamedR.PS1#0 for field1
     R"(edge("NamedR.PS1#0.out_handle.field1", "NamedR.h2.field1").)",

    // Intra-particle connection
    R"(edge("NamedR.PS1#0.in_handle.field1", "NamedR.PS1#0.out_handle.field1").)",

    // Edges connecting h2 to NamedR.PS1#1 for field1.
    R"(edge("NamedR.h2.field1", "NamedR.PS1#1.in_handle.field1").)",

    // Edges connecting h3 to NamedR.PS1#1 for field1
    R"(edge("NamedR.PS1#1.out_handle.field1", "NamedR.h3.field1").)",

    // Intra-particle connection
    R"(edge("NamedR.PS1#1.in_handle.field1", "NamedR.PS1#1.out_handle.field1").)",

    // Edges connecting h3 to NamedR.PS2#2 for field1
    R"(edge("NamedR.h3.field1", "NamedR.PS2#2.in_handle.field1").)",

    // Edges connecting h4 to NamedR.Ps2#2 for field1
    R"(edge("NamedR.PS2#2.out_handle.field1", "NamedR.h4.field1").)",

    // Intra-particle connection
    R"(edge("NamedR.PS2#2.in_handle.field1", "NamedR.PS2#2.out_handle.field1").)",

    // Unnamed recipe edges:
    R"(edge("GENERATED_RECIPE_NAME0.h1.field1", "GENERATED_RECIPE_NAME0.PS1#0.in_handle.field1").)",

    // Edges connecting h2 to NamedR.PS1#0 for field1
    R"(edge("GENERATED_RECIPE_NAME0.PS1#0.out_handle.field1", "GENERATED_RECIPE_NAME0.h2.field1").)",

    // Intra-particle connection
    R"(edge("GENERATED_RECIPE_NAME0.PS1#0.in_handle.field1", "GENERATED_RECIPE_NAME0.PS1#0.out_handle.field1").)",

    // Edges connecting h2 to NamedR.PS2#1 for field1.
    R"(edge("GENERATED_RECIPE_NAME0.h2.field1", "GENERATED_RECIPE_NAME0.PS2#1.in_handle.field1").)",

    // Edges connecting h3 to NamedR.PS2#1 for field1
    R"(edge("GENERATED_RECIPE_NAME0.PS2#1.out_handle.field1", "GENERATED_RECIPE_NAME0.h3.field1").)",

    // Intra-particle connection
    R"(edge("GENERATED_RECIPE_NAME0.PS2#1.in_handle.field1", "GENERATED_RECIPE_NAME0.PS2#1.out_handle.field1").)",

    // Edges connecting h3 to NamedR.PS2#2 for field1
    R"(edge("GENERATED_RECIPE_NAME0.h3.field1", "GENERATED_RECIPE_NAME0.PS2#2.in_handle.field1").)",

    // Edges connecting h4 to NamedR.PS2#2 for field1
    R"(edge("GENERATED_RECIPE_NAME0.PS2#2.out_handle.field1", "GENERATED_RECIPE_NAME0.h4.field1").)",

    // Intra-particle connection
    R"(edge("GENERATED_RECIPE_NAME0.PS2#2.in_handle.field1", "GENERATED_RECIPE_NAME0.PS2#2.out_handle.field1").)"
};

TEST_F(ParseBigManifestTest, ManifestProtoEdgesTest) {
  // Go through all of the facts, convert them to strings, and compare them
  // against our expected strings. Strings are much easier to read on a test
  // failure than structured datalog facts.
  std::vector<std::string> edge_datalog_strings;
  for (const ir::Edge &edge : datalog_facts_.edges()) {
    edge_datalog_strings.push_back(edge.ToDatalog(ctxt_));
  }
  EXPECT_THAT(edge_datalog_strings,
              testing::UnorderedElementsAreArray(kExpectedEdgeStrings));
}

}  // namespace raksha::xform_to_datalog
