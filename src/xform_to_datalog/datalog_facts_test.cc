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

#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"

namespace raksha::xform_to_datalog {

namespace ir = raksha::ir;

class DatalogFactsToStringTest :
   public testing::TestWithParam<std::tuple<DatalogFacts, std::string>> {};

TEST_P(DatalogFactsToStringTest, DatalogFactsToStringTest) {
  const DatalogFacts &datalog_facts = std::get<0>(GetParam());
  const std::string &expected_result_string = std::get<1>(GetParam());

  EXPECT_EQ(datalog_facts.ToString(), expected_result_string);
}

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

static std::tuple<DatalogFacts, std::string>
  datalog_facts_and_output_strings[] = {
    { DatalogFacts({}, {}, {}), "// Claims:\n\n// Checks:\n\n// Edges:\n" },
    { DatalogFacts(
        { ir::TagClaim(ir::TagAnnotationOnAccessPath(
            kHandleConnectionOutAccessPath, "tag")) },
        { ir::TagCheck(ir::TagAnnotationOnAccessPath(
            kHandleConnectionInAccessPath, "tag2")) },
        { ir::Edge(kHandleH1AccessPath, kHandleConnectionInAccessPath),
          ir::Edge(kHandleConnectionInAccessPath,
                   kHandleConnectionOutAccessPath),
           ir::Edge(kHandleConnectionOutAccessPath, kHandleH2AccessPath)}),
      "// Claims:\nclaimHasTag(\"recipe.particle.out\", \"tag\").\n"
      "\n// Checks:\ncheckHasTag(\"recipe.particle.in\", \"tag2\").\n"
      "\n// Edges:\nedge(\"recipe.h1\", \"recipe.particle.in\").\n"
      "edge(\"recipe.particle.in\", \"recipe.particle.out\").\n"
      "edge(\"recipe.particle.out\", \"recipe.h2\").\n" }
};

INSTANTIATE_TEST_SUITE_P(DatalogFactsToStringTest, DatalogFactsToStringTest,
                         testing::ValuesIn(datalog_facts_and_output_strings));

// Create a manifest textproto to test constructing DatalogFacts from
// a ManifestProto. The ParticleSpecs will be pretty simple, as we have
// tested creating ParticleSpecs from ParticleSpecProtos in more depth
// elsewhere. We're focusing more on connecting these to particles here.
//
// TODO(#107): This is currently quite complex. After the MVP, we should break
// this up into smaller, more focused tests.
static const std::string kManifestTextproto =
    "particle_specs: [\n"
    "{ name: \"PS1\" connections: [\n"
    "  {\n"
    "    name: \"out_handle\" direction: WRITES \n"
    "    type: {\n"
    "    entity: { \n"
    "      schema: { \n"
    "        fields: [ \n"
    "          { key: \"field1\" value: { primitive: TEXT } }\n"
    "    ]\n"
    " } } } },\n"
    "  {\n"
    "    name: \"in_handle\" direction: READS \n"
    "    type: {\n"
    "    entity: { \n"
    "      schema: { \n"
    "        fields: [ \n"
    "          { key: \"field1\" value: { primitive: TEXT } }\n"
    "    ]\n"
    " } } } } ],\n"
    " claims: [\n"
    "   { assume: {\n"
    "       access_path: {\n"
    "         handle: {\n"
    "           particle_spec: \"PS1\",\n"
    "           handle_connection: \"out_handle\" },\n"
    "         selectors: { field: \"field1\" } }"
    "       predicate: { label: { semantic_tag: \"tag1\"} } } }"
    " ],\n"
    " checks: [\n"
    "   {\n"
    "     access_path: {\n"
    "         handle: {\n"
    "           particle_spec: \"PS1\",\n"
    "           handle_connection: \"in_handle\" },\n"
    "         selectors: { field: \"field1\" } }"
    "     predicate: { label: { semantic_tag: \"tag2\"} } }"
    " ]\n"
    "},\n"
    "{ name: \"PS2\" connections: [\n"
    "  {\n"
    "    name: \"out_handle\" direction: WRITES \n"
    "    type: {\n"
    "    entity: { \n"
    "      schema: { \n"
    "        fields: [ \n"
    "          { key: \"field1\" value: { primitive: TEXT } }\n"
    "    ]\n"
    " } } } },\n"
    "  {\n"
    "    name: \"in_handle\" direction: READS \n"
    "    type: {\n"
    "    entity: { \n"
    "      schema: { \n"
    "        fields: [ \n"
    "          { key: \"field1\" value: { primitive: TEXT } }\n"
    "    ]\n"
    " } } } } ],"
    " claims: [\n"
    "   { assume: {\n"
    "       access_path: {\n"
    "         handle: {\n"
    "           particle_spec: \"PS2\", \n"
    "           handle_connection: \"out_handle\" },\n"
    "         selectors: { field: \"field1\" } }\n"
    "       predicate: { label: { semantic_tag: \"tag3\"} } } }\n"
    " ],\n"
    " checks: [\n"
    "   {\n"
    "     access_path: {\n"
    "       handle: {\n"
    "         particle_spec: \"PS2\", handle_connection: \"in_handle\" },\n"
    "       selectors: { field: \"field1\" } }"
    "     predicate: { label: { semantic_tag: \"tag4\"} } }\n"
    " ]\n"
    "}\n"
    "] \n"
    "recipes: [\n"
    " { name: \"NamedR\"\n"
    "   particles: [\n"
    "     { spec_name: \"PS1\"\n"
    "       connections: [\n"
    "         { name: \"in_handle\" handle: \"h1\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                 { key: \"field1\", value: { primitive: TEXT } },\n"
    "                 { key: \"field2\", value: { primitive: TEXT }}]}}}},\n"
    "         { name: \"out_handle\" handle: \"h2\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                 { key: \"field1\", value: { primitive: TEXT }}]}}}}]},\n"
    "     { spec_name: \"PS1\"\n"
    "       connections: [\n"
    "         { name: \"in_handle\" handle: \"h2\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                 { key: \"field1\", value: { primitive: TEXT }}]}}}},\n"
    "         { name: \"out_handle\" handle: \"h3\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                 { key: \"field1\", value: { primitive: TEXT } }]}}}}]},\n"
    "     { spec_name: \"PS2\"\n"
    "       connections: [\n"
    "         { name: \"in_handle\" handle: \"h3\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [\n"
    "                 { key: \"field1\", value: { primitive: TEXT }}]}}}},\n"
    "         { name: \"out_handle\" handle: \"h4\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "              { key: \"field1\", value: { primitive: TEXT } } ]}}}}]}]},\n"
    " {\n"
    "   particles: [\n"
    "     { spec_name: \"PS1\"\n"
    "       connections: [\n"
    "         { name: \"in_handle\" handle: \"h1\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                 { key: \"field1\", value: { primitive: TEXT } }]}}}},\n"
    "         { name: \"out_handle\" handle: \"h2\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                { key: \"field1\", value: { primitive: TEXT } } ]}}}}]},\n"
    "     { spec_name: \"PS2\"\n"
    "       connections: [\n"
    "         { name: \"in_handle\" handle: \"h2\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                 { key: \"field1\", value: { primitive: TEXT }}]}}}},\n"
    "         { name: \"out_handle\" handle: \"h3\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                { key: \"field1\", value: { primitive: TEXT } } ]}}}}]},\n"
    "     { spec_name: \"PS2\"\n"
    "       connections: [\n"
    "         { name: \"in_handle\" handle: \"h3\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "                 { key: \"field1\", value: { primitive: TEXT }}]}}}},\n"
    "         { name: \"out_handle\" handle: \"h4\" type: {\n"
    "           entity: { \n"
    "             schema: { \n"
    "               fields: [ \n"
    "               { key: \"field1\", value: { primitive: TEXT } } ]}}}}]}]}\n"
    "]";

// These are a bunch of static constants to make building the expected facts
// a little easier to understand.

// Access path component strings.
static const std::string kNamedRecipeName = "NamedR";
static const std::string kH1HandleName = "h1";
static const std::string kH2HandleName = "h2";
static const std::string kH3HandleName = "h3";
static const std::string kH4HandleName = "h4";
static const std::string kField1Name = "field1";
static const std::string kField2Name = "field2";
static const std::string kPs1Inst0Name = "PS1#0";
static const std::string kPs1Inst1Name = "PS1#1";
static const std::string kPs2Inst1Name = "PS2#1";
static const std::string kPs2Inst2Name = "PS2#2";
static const std::string kInHcName = "in_handle";
static const std::string kOutHcName = "out_handle";

// Access path strings.
static const std::string kNamedRecipeH1Field1 =
    absl::StrJoin({kNamedRecipeName, kH1HandleName, kField1Name}, ".");
static const std::string kNamedRecipeH1Field2 =
    absl::StrJoin({kNamedRecipeName, kH1HandleName, kField2Name}, ".");

static const std::string kNamedRecipeH2Field1 =
    absl::StrJoin({kNamedRecipeName, kH2HandleName, kField1Name}, ".");

static const std::string kNamedRecipeH3Field1 =
  absl::StrJoin({kNamedRecipeName, kH3HandleName, kField1Name}, ".");

static const std::string kNamedRecipeH4Field1 =
    absl::StrJoin({kNamedRecipeName, kH4HandleName, kField1Name}, ".");

static const std::string kNamedRecipePs1Inst0InHandleField1 =
    absl::StrJoin({kNamedRecipeName, kPs1Inst0Name, kInHcName, kField1Name},
                  ".");

static const std::string kNamedRecipePs1Inst0InHandleField2 =
        absl::StrJoin({kNamedRecipeName, kPs1Inst0Name, kInHcName, kField2Name},
                  ".");
static const std::string kNamedRecipePs1Inst0OutHandleField1 =
    absl::StrJoin({kNamedRecipeName, kPs1Inst0Name, kOutHcName, kField1Name},
          ".");

static const std::string kNamedRecipePs1Inst1InHandleField1 =
    absl::StrJoin({kNamedRecipeName, kPs1Inst1Name, kInHcName, kField1Name },
                  ".");

static const std::string kNamedRecipePs1Inst1InHandleField2 =
    absl::StrJoin({kNamedRecipeName, kPs1Inst1Name, kInHcName, kField2Name },
              ".");

static const std::string kNamedRecipePs1Inst1OutHandleField1 =
    absl::StrJoin({kNamedRecipeName, kPs1Inst1Name, kOutHcName, kField1Name},
                  ".");

static const std::string kNamedRecipePs2Inst2InHandleField1 =
    absl::StrJoin({kNamedRecipeName, kPs2Inst2Name, kInHcName, kField1Name},
                  ".");

static const std::string kNamedRecipePs2Inst2OutHandleField1 =
    absl::StrJoin({kNamedRecipeName, kPs2Inst2Name, kOutHcName, kField1Name},
                  ".");

static const std::string kGenRecipeName = "GENERATED_RECIPE_NAME0";

static const std::string kGenRecipeH1Field1 =
    absl::StrJoin({kGenRecipeName, kH1HandleName, kField1Name}, ".");

static const std::string kGenRecipeH2Field1 =
    absl::StrJoin({kGenRecipeName, kH2HandleName, kField1Name}, ".");

static const std::string kGenRecipeH3Field1 =
    absl::StrJoin({kGenRecipeName, kH3HandleName, kField1Name}, ".");

static const std::string kGenRecipeH4Field1 =
    absl::StrJoin({kGenRecipeName, kH4HandleName, kField1Name}, ".");

static const std::string kGenRecipePs1Inst0InHandleField1 =
    absl::StrJoin({kGenRecipeName, kPs1Inst0Name, kInHcName, kField1Name}, ".");

static const std::string kGenRecipePs1Inst0InHandleField2 =
    absl::StrJoin({kGenRecipeName, kPs1Inst0Name, kInHcName, kField2Name}, ".");
static const std::string kGenRecipePs1Inst0OutHandleField1 =
    absl::StrJoin({kGenRecipeName, kPs1Inst0Name, kOutHcName, kField1Name},
                  ".");

static const std::string kGenRecipePs2Inst1InHandleField1 =
    absl::StrJoin({kGenRecipeName, kPs2Inst1Name, kInHcName, kField1Name}, ".");

static const std::string kGenRecipePs2Inst1OutHandleField1 =
    absl::StrJoin({kGenRecipeName, kPs2Inst1Name, kOutHcName, kField1Name},
                  ".");

static const std::string kGenRecipePs2Inst2InHandleField1 =
    absl::StrJoin({kGenRecipeName, kPs2Inst2Name, kInHcName, kField1Name}, ".");

static const std::string kGenRecipePs2Inst2OutHandleField1 =
    absl::StrJoin({kGenRecipeName, kPs2Inst2Name, kOutHcName, kField1Name},
                  ".");

static std::string MakeClaimStr(std::string access_path_str, std::string tag) {
  return absl::StrCat(
      "claimHasTag(\"", access_path_str, "\", \"", tag, "\").\n");
}

static const std::string kExpectedClaimStrings[] = {
    MakeClaimStr(kNamedRecipePs1Inst0OutHandleField1, "tag1"),
    MakeClaimStr(kNamedRecipePs1Inst1OutHandleField1, "tag1"),
    MakeClaimStr(kNamedRecipePs2Inst2OutHandleField1, "tag3"),
    MakeClaimStr(kGenRecipePs1Inst0OutHandleField1, "tag1"),
    MakeClaimStr(kGenRecipePs2Inst1OutHandleField1, "tag3"),
    MakeClaimStr(kGenRecipePs2Inst2OutHandleField1, "tag3"),
};

static std::string MakeCheckStr(std::string access_path_str, std::string tag) {
  return absl::StrCat(
      "checkHasTag(\"", access_path_str, "\", \"", tag, "\").\n");
}

static std::string kExpectedCheckStrings[] = {
    MakeCheckStr(kNamedRecipePs1Inst0InHandleField1, "tag2"),
    MakeCheckStr(kNamedRecipePs1Inst1InHandleField1, "tag2"),
    MakeCheckStr(kNamedRecipePs2Inst2InHandleField1, "tag4"),
    MakeCheckStr(kGenRecipePs1Inst0InHandleField1, "tag2"),
    MakeCheckStr(kGenRecipePs2Inst1InHandleField1, "tag4"),
    MakeCheckStr(kGenRecipePs2Inst2InHandleField1, "tag4"),
};

static std::string MakeEdgeStr(std::string ap1, std::string ap2) {
  return absl::StrCat("edge(\"", ap1, "\", \"", ap2, "\").\n");
}

static const std::string kExpectedEdgeStrings[] = {
    // Named recipe edges:
    // Edges connecting h1 to NamedR.PS1#0 for fields {field1, field2}.
    MakeEdgeStr(kNamedRecipeH1Field1, kNamedRecipePs1Inst0InHandleField1),
    MakeEdgeStr(kNamedRecipePs1Inst0InHandleField1, kNamedRecipeH1Field1),
    MakeEdgeStr(kNamedRecipeH1Field2, kNamedRecipePs1Inst0InHandleField2),
    MakeEdgeStr(kNamedRecipePs1Inst0InHandleField2, kNamedRecipeH1Field2),

    // Edges connecting h2 to NamedR.PS1#0 for field1
    MakeEdgeStr(kNamedRecipeH2Field1, kNamedRecipePs1Inst0OutHandleField1),
    MakeEdgeStr(kNamedRecipePs1Inst0OutHandleField1, kNamedRecipeH2Field1),

    // Intra-particle connection
    MakeEdgeStr(kNamedRecipePs1Inst0InHandleField1,
             kNamedRecipePs1Inst0OutHandleField1),

    // Edges connecting h2 to NamedR.PS1#1 for field1.
    MakeEdgeStr(kNamedRecipeH2Field1, kNamedRecipePs1Inst1InHandleField1),
    MakeEdgeStr(kNamedRecipePs1Inst1InHandleField1, kNamedRecipeH2Field1),

    // Edges connecting h3 to NamedR.PS1#1 for field1
    MakeEdgeStr(kNamedRecipeH3Field1, kNamedRecipePs1Inst1OutHandleField1),
    MakeEdgeStr(kNamedRecipePs1Inst1OutHandleField1, kNamedRecipeH3Field1),

    // Intra-particle connection
    MakeEdgeStr(kNamedRecipePs1Inst1InHandleField1,
             kNamedRecipePs1Inst1OutHandleField1),

    // Edges connecting h3 to NamedR.PS2#2 for field1
    MakeEdgeStr(kNamedRecipeH3Field1, kNamedRecipePs2Inst2InHandleField1),
    MakeEdgeStr(kNamedRecipePs2Inst2InHandleField1, kNamedRecipeH3Field1),

    // Edges connecting h4 to NamedR.Ps2#2 for field1
    MakeEdgeStr(kNamedRecipeH4Field1, kNamedRecipePs2Inst2OutHandleField1),
    MakeEdgeStr(kNamedRecipePs2Inst2OutHandleField1, kNamedRecipeH4Field1),

    // Intra-particle connection

    MakeEdgeStr(kNamedRecipePs2Inst2InHandleField1,
                kNamedRecipePs2Inst2OutHandleField1),

    // Unnamed recipe edges:
    MakeEdgeStr(kGenRecipeH1Field1, kGenRecipePs1Inst0InHandleField1),
    MakeEdgeStr(kGenRecipePs1Inst0InHandleField1, kGenRecipeH1Field1),

    // Edges connecting h2 to NamedR.PS1#0 for field1
    MakeEdgeStr(kGenRecipeH2Field1, kGenRecipePs1Inst0OutHandleField1),
    MakeEdgeStr(kGenRecipePs1Inst0OutHandleField1, kGenRecipeH2Field1),

    // Intra-particle connection
    MakeEdgeStr(kGenRecipePs1Inst0InHandleField1,
             kGenRecipePs1Inst0OutHandleField1),

    // Edges connecting h2 to NamedR.PS2#1 for field1.
    MakeEdgeStr(kGenRecipeH2Field1, kGenRecipePs2Inst1InHandleField1),
    MakeEdgeStr(kGenRecipePs2Inst1InHandleField1, kGenRecipeH2Field1),

    // Edges connecting h3 to NamedR.PS2#1 for field1
    MakeEdgeStr(kGenRecipeH3Field1, kGenRecipePs2Inst1OutHandleField1),
    MakeEdgeStr(kGenRecipePs2Inst1OutHandleField1, kGenRecipeH3Field1),

    // Intra-particle connection
    MakeEdgeStr(kGenRecipePs2Inst1InHandleField1,
             kGenRecipePs2Inst1OutHandleField1),

    // Edges connecting h3 to NamedR.PS2#2 for field1
    MakeEdgeStr(kGenRecipeH3Field1, kGenRecipePs2Inst2InHandleField1),
    MakeEdgeStr(kGenRecipePs2Inst2InHandleField1, kGenRecipeH3Field1),

    // Edges connecting h4 to NamedR.PS2#2 for field1
    MakeEdgeStr(kGenRecipeH4Field1, kGenRecipePs2Inst2OutHandleField1),
    MakeEdgeStr(kGenRecipePs2Inst2OutHandleField1, kGenRecipeH4Field1),

    // Intra-particle connection
    MakeEdgeStr(kGenRecipePs2Inst2InHandleField1,
                kGenRecipePs2Inst2OutHandleField1)
};

TEST(CreateFactsFromManifestProtoTest, CreateFactsFromManifestProtoTest) {
  arcs::ManifestProto manifest_proto;
  google::protobuf::TextFormat::ParseFromString(
      kManifestTextproto, &manifest_proto);

  DatalogFacts datalog_facts =
      DatalogFacts::CreateFromManifestProto(manifest_proto);

  // Go through all of the facts, convert them to strings, and compare them
  // against our expected strings. Strings are much easier to read on a test
  // failure than structured datalog facts.

  {
    std::vector<std::string> claim_datalog_strings;
    for (const ir::TagClaim &claim : datalog_facts.claims()) {
      claim_datalog_strings.push_back(claim.ToString());
    }
    EXPECT_THAT(claim_datalog_strings,
                testing::UnorderedElementsAreArray(kExpectedClaimStrings));
  }

  {
    std::vector<std::string> check_datalog_strings;
    for (const ir::TagCheck &check : datalog_facts.checks()) {
      check_datalog_strings.push_back(check.ToString());
    }
    EXPECT_THAT(check_datalog_strings,
                testing::UnorderedElementsAreArray(kExpectedCheckStrings));
  }

  std::vector<std::string> edge_datalog_strings;
  for (const ir::Edge &edge : datalog_facts.edges()) {
    edge_datalog_strings.push_back(edge.ToString());
  }
  EXPECT_THAT(
      edge_datalog_strings,
      testing::UnorderedElementsAreArray(kExpectedEdgeStrings));
}

}  // namespace raksha::xform_to_datalog
