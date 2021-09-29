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

#include "src/xform_to_datalog/arcs_manifest_tree/particle_spec.h"

#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"
#include "src/ir/instantiator.h"
#include "src/ir/map_instantiator.h"

namespace raksha::xform_to_datalog::arcs_manifest_tree {

namespace ir = raksha::ir;

struct ParticleSpecProtoAndExpectedInfo {
  std::string textproto;
  std::string expected_name;
  std::vector<ir::TagClaim> expected_claims;
  std::vector<ir::TagCheck> expected_checks;
  std::vector<ir::Edge> expected_edges;
};

class ParticleSpecFromProtoTest :
   public testing::TestWithParam<ParticleSpecProtoAndExpectedInfo> {};

TEST_P(ParticleSpecFromProtoTest, ParticleSpecFromProtoTest) {
  const ParticleSpecProtoAndExpectedInfo &param = GetParam();
  arcs::ParticleSpecProto particle_spec_proto;
  google::protobuf::TextFormat::ParseFromString(
      param.textproto, &particle_spec_proto);
  ParticleSpec particle_spec =
      ParticleSpec::CreateFromProto(particle_spec_proto);

  EXPECT_EQ(particle_spec.name(), param.expected_name);
  EXPECT_THAT(
      particle_spec.tag_claims(),
      testing::UnorderedElementsAreArray(param.expected_claims));
  EXPECT_THAT(
      particle_spec.checks(),
      testing::UnorderedElementsAreArray(param.expected_checks));
  EXPECT_THAT(
      particle_spec.edges(),
      testing::UnorderedElementsAreArray(param.expected_edges));
}

// Constant to reduce wordiness of test expected output.
static const ir::AccessPathRoot kPs1InHandleRoot(
    ir::HandleConnectionSpecAccessPathRoot("PS1", "in_handle"));
static const ir::AccessPathRoot kPs1OutHandleRoot(
    ir::HandleConnectionSpecAccessPathRoot("PS1", "out_handle"));
static const ir::AccessPathRoot kPs1InOutHandleRoot(
    ir::HandleConnectionSpecAccessPathRoot("PS1", "in_out_handle"));
static const ir::AccessPathRoot kPs2HcHandleRoot(
    ir::HandleConnectionSpecAccessPathRoot("PS2", "hc"));
static const ir::AccessPathRoot kPs2Hc2HandleRoot(
    ir::HandleConnectionSpecAccessPathRoot("PS2", "hc2"));

// Helper function to cut down on expected output wordiness.
static ir::AccessPathSelectors MakeSingleFieldSelectors(
    std::string field_name) {
  return ir::AccessPathSelectors(ir::Selector(ir::FieldSelector(
      std::move(field_name))));
}

static ParticleSpecProtoAndExpectedInfo spec_proto_and_expected_info[] = {
    { .textproto = R"(name: "p_spec")", .expected_name = "p_spec",
      .expected_claims = { }, .expected_checks = { },  .expected_edges = { } },
    { .textproto = R"(
name: "PS1" connections: [
  {
    name: "out_handle" direction: WRITES
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field1", value: { primitive: TEXT } },
            { key: "field2", value: { primitive: TEXT } },
            { key: "field3", value: { primitive: TEXT } }] } } } },
  {
    name: "in_handle" direction: READS
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field2", value: { primitive: TEXT } },
            { key: "world", value: { primitive: TEXT } } ] } } } } ])",
    .expected_name = "PS1", .expected_claims = { }, .expected_checks = { },
    .expected_edges = {
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
    } },
        { .textproto = R"(
name: "PS1"
connections: [
  {
    name: "out_handle" direction: WRITES
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field1", value: { primitive: TEXT } },
            { key: "field2", value: { primitive: TEXT } },
            { key: "field3", value: { primitive: TEXT } } ] } } } },
  {
    name: "in_handle" direction: READS
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field2", value: { primitive: TEXT } },
            { key: "world", value: { primitive: TEXT } } ] } } } } ],
claims: [
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field2" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "field2" } } } } ])",
    .expected_name = "PS1", .expected_claims = { }, .expected_checks = { },
    .expected_edges = {
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
    } },
    { .textproto = R"(
name: "PS1"
connections: [
  {
    name: "out_handle" direction: WRITES
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field1", value: { primitive: TEXT } },
            { key: "field2", value: { primitive: TEXT } },
            { key: "field3", value: { primitive: TEXT } } ] } } } },
  {
    name: "in_handle" direction: READS
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field2", value: { primitive: TEXT } },
            { key: "world", value: { primitive: TEXT } } ] } } } } ],
claims: [
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field2" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "field2" } } } },
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field1" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "world" } } } }
 ])",
    .expected_name = "PS1", .expected_claims = { }, .expected_checks = { },
    .expected_edges = {
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
    } },
    { .textproto = R"(
name: "PS1"
connections: [
  {
    name: "out_handle" direction: WRITES
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field1", value: { primitive: TEXT } },
            { key: "field2", value: { primitive: TEXT } },
            { key: "field3", value: { primitive: TEXT } } ] } } } },
  {
    name: "in_handle" direction: READS
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field2", value: { primitive: TEXT } },
            { key: "world", value: { primitive: TEXT } } ] } } } } ],
claims: [
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field2" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "field2" } } } },
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field2" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "world" } } } },
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field1" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "world" } } } }
 ])",
    .expected_name = "PS1", .expected_claims = { }, .expected_checks = { },
    .expected_edges = {
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
    } },
    { .textproto = R"(
name: "PS1"
connections: [
  {
    name: "out_handle" direction: WRITES
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field1", value: { primitive: TEXT } },
            { key: "field2", value: { primitive: TEXT } },
            { key: "field3", value: { primitive: TEXT } } ] } } } },
  {
    name: "in_handle" direction: READS
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field2", value: { primitive: TEXT } },
            { key: "world", value: { primitive: TEXT } } ] } } } } ],
claims: [
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field2" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "field2" } } } },
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field1" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "world" } } } },
  { derives_from: {
      target: {
        handle: { particle_spec: "PS1" handle_connection: "out_handle" }
        selectors: { field: "field3" } }
      source: {
        handle: { particle_spec: "PS1" handle_connection: "in_handle" }
        selectors: { field: "field2" } } } }
 ])",
    .expected_name = "PS1", .expected_claims = { }, .expected_checks = { },
    .expected_edges = {
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
    } },
    { .textproto = R"(
name: "PS1" connections: [
  {
    name: "out_handle" direction: WRITES
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field1", value: { primitive: TEXT } } ] } } } },
  {
    name: "in_handle" direction: READS
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field2", value: { primitive: TEXT } } ] } } } },
  {
    name: "in_out_handle" direction: READS_WRITES
      type: {
        entity: {
          schema: {
            fields: [
              { key: "field3", value: { primitive: TEXT } } ] } } } } ] )",
    .expected_name = "PS1", .expected_claims = { }, .expected_checks = { },
    .expected_edges = {
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
            ir::AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(
                kPs1InOutHandleRoot, MakeSingleFieldSelectors("field3"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InOutHandleRoot, MakeSingleFieldSelectors("field3")),
          ir::AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
          ir::AccessPath(
              kPs1InOutHandleRoot, MakeSingleFieldSelectors("field3")),
          ir::AccessPath(
              kPs1InOutHandleRoot, MakeSingleFieldSelectors("field3"))),
    } },
    {
      .textproto = R"(
name: "PS2"
checks: [ {
  access_path: {
    handle: {
      particle_spec: "PS2",
      handle_connection: "hc" },
    selectors: { field: "field1" } },
  predicate: { label: { semantic_tag: "tag1"} } },
  {
   access_path: {
     handle: {
       particle_spec: "PS2",
       handle_connection: "hc2" },
     selectors: { field: "field2" } },
   predicate: { label: { semantic_tag: "tag2"} } } ])",
      .expected_name = "PS2",
      .expected_claims = { },
      .expected_checks = {
          ir::TagCheck(
              ir::TagAnnotationOnAccessPath(
                  ir::AccessPath(
                      kPs2HcHandleRoot, MakeSingleFieldSelectors("field1")),
                      "tag1")
              ),
          ir::TagCheck(
              ir::TagAnnotationOnAccessPath(
                  ir::AccessPath(
                      kPs2Hc2HandleRoot, MakeSingleFieldSelectors("field2")),
                      "tag2")
              )
      },
      .expected_edges = { }
    },
    {
      .textproto = R"(
name: "PS2"
claims: [ { assume: {
  access_path: {
    handle: {
      particle_spec: "PS2",
      handle_connection: "hc" },
    selectors: { field: "field1" } },
  predicate: { label: { semantic_tag: "tag1"} } } },
{ assume: {
    access_path: {
      handle: {
        particle_spec: "PS2",
        handle_connection: "hc2" },
      selectors: { field: "field2" } },
    predicate: { label: { semantic_tag: "tag2"} } } } ])",
      .expected_name = "PS2",
      .expected_claims = {
          ir::TagClaim(
              "PS2",
              ir::AccessPath(
                  kPs2HcHandleRoot, MakeSingleFieldSelectors("field1")),
              true,
              "tag1"),
          ir::TagClaim(
              "PS2",
              ir::AccessPath(
                  kPs2Hc2HandleRoot, MakeSingleFieldSelectors("field2")),
              true,
              "tag2")
      },
      .expected_checks = { },
      .expected_edges = { }
    }
};

INSTANTIATE_TEST_SUITE_P(
    ParticleSpecFromProtoTest, ParticleSpecFromProtoTest,
    testing::ValuesIn(spec_proto_and_expected_info));

// This example textproto contains checks, claims, and connections. We're
// going to use it to show that BulkInstantiate will successfully give us the
// instantiated versions of all of these facts.
static const std::string kTextprotoWithAllFacts = R"(
name: "PS1" claims: [ { assume: {
  access_path: {
    handle: {
      particle_spec: "PS1",
      handle_connection: "out_handle" },
    selectors: { field: "field1" } },
  predicate: { label: { semantic_tag: "tag1"} } } },
{ assume: {
  access_path: {
    handle: {
      particle_spec: "PS1",
      handle_connection: "in_out_handle" },
    selectors: { field: "field2" } },
  predicate: { label: { semantic_tag: "tag2"} } } } ]
checks: [ {
  access_path: {
    handle: {
      particle_spec: "PS1",
        handle_connection: "in_handle" },
    selectors: { field: "field1" } },
  predicate: { label: { semantic_tag: "tag3"} } },
{
  access_path: {
    handle: {
      particle_spec: "PS1",
      handle_connection: "in_out_handle" },
    selectors: { field: "field2" } },
  predicate: { label: { semantic_tag: "tag4"} } } ]
connections: [
  {
    name: "out_handle" direction: WRITES
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field1", value: { primitive: TEXT } } ] } } } },
  {
    name: "in_handle" direction: READS
    type: {
      entity: {
        schema: {
          fields: [
            { key: "field2", value: { primitive: TEXT } } ] } } } },
{
  name: "in_out_handle" direction: READS_WRITES
  type: {
    entity: {
      schema: {
        fields: [
          { key: "field3", value: { primitive: TEXT } } ] } } } } ])";

TEST(InstantiateTest, InstantiateTest) {
  arcs::ParticleSpecProto particle_spec_proto;
  google::protobuf::TextFormat::ParseFromString(
      kTextprotoWithAllFacts, &particle_spec_proto);
  ParticleSpec particle_spec =
      ParticleSpec::CreateFromProto(particle_spec_proto);

  const ir::AccessPathRoot p1_in_impl(
      ir::HandleConnectionAccessPathRoot("recipe", "P1", "in_impl"));
  const ir::AccessPathRoot p1_out_impl(
      ir::HandleConnectionAccessPathRoot("recipe", "P1", "out_impl"));
  const ir::AccessPathRoot p1_in_out_impl(
      ir::HandleConnectionAccessPathRoot("recipe", "P1", "in_out_impl"));

  const absl::flat_hash_map<ir::AccessPathRoot, ir::AccessPathRoot>
      instantiation_map {
    { kPs1InHandleRoot, p1_in_impl }, { kPs1OutHandleRoot, p1_out_impl },
    { kPs1InOutHandleRoot, p1_in_out_impl } };

  ir::MapInstantiator map_instantiator(instantiation_map);
  InstantiatedParticleSpecFacts instantiated_facts =
      particle_spec.Instantiate(map_instantiator);

  std::vector<std::string> claims_as_datalog_strings;
  for (const auto &claim : instantiated_facts.tag_claims) {
    claims_as_datalog_strings.push_back(claim.ToDatalog());
  }
  ASSERT_THAT(
      claims_as_datalog_strings,
      testing::UnorderedElementsAreArray({
        R"(claimHasTag("PS1", "recipe.P1.out_impl.field1", "tag1").)",
        R"(claimHasTag("PS1", "recipe.P1.in_out_impl.field2", "tag2").)"}));

  std::vector<std::string> checks_as_datalog_strings;
  for (const auto &check : instantiated_facts.checks) {
    checks_as_datalog_strings.push_back(check.ToDatalog());
  }
  ASSERT_THAT(
      checks_as_datalog_strings,
      testing::UnorderedElementsAreArray({
        R"(checkHasTag("recipe.P1.in_impl.field1", "tag3") :- mayHaveTag("recipe.P1.in_impl.field1", "tag3").)",
        R"(checkHasTag("recipe.P1.in_out_impl.field2", "tag4") :- mayHaveTag("recipe.P1.in_out_impl.field2", "tag4").)"
      }));

  std::vector<std::string> edges_as_datalog_strings;
  for (const auto &edge : instantiated_facts.edges) {
    edges_as_datalog_strings.push_back(edge.ToDatalog());
  }
  ASSERT_THAT(
      edges_as_datalog_strings,
      testing::UnorderedElementsAreArray({
        R"(edge("recipe.P1.in_impl.field2", "recipe.P1.out_impl.field1").)",
        R"(edge("recipe.P1.in_impl.field2", "recipe.P1.in_out_impl.field3").)",
        R"(edge("recipe.P1.in_out_impl.field3", "recipe.P1.out_impl.field1").)",
        R"(edge("recipe.P1.in_out_impl.field3", "recipe.P1.in_out_impl.field3").)",
      }));
}

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree
