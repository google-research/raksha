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

#include "src/ir/particle_spec.h"

#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"
#include "src/common/logging/logging.h"
#include "src/ir/proto/particle_spec.h"
#include "src/ir/predicate_arena.h"
#include "src/ir/single-use-arena-and-predicate.h"

namespace raksha::ir {

struct ParticleSpecProtoAndExpectedInfo {
  std::string textproto;
  std::string expected_name;
  std::vector<TagClaim> expected_claims;
  std::vector<TagCheck> expected_checks;
  std::vector<Edge> expected_edges;
};

class ParticleSpecFromProtoTest :
   public testing::TestWithParam<ParticleSpecProtoAndExpectedInfo> {
 protected:
  ParticleSpecFromProtoTest() : particle_spec_(CreateParticleSpec()) {}

  static std::unique_ptr<ParticleSpec> CreateParticleSpec() {
    arcs::ParticleSpecProto particle_spec_proto;
    std::string textproto = GetParam().textproto;
    CHECK(google::protobuf::TextFormat::ParseFromString(
        textproto, &particle_spec_proto))
        << "Particle spec textproto did not parse correctly.";
    return proto::Decode(std::make_unique<PredicateArena>(),
                         particle_spec_proto);
  }

  std::unique_ptr<ParticleSpec> particle_spec_;
};

TEST_P(ParticleSpecFromProtoTest, ParticleSpecFromProtoTest) {
  const ParticleSpecProtoAndExpectedInfo &param = GetParam();

  EXPECT_EQ(particle_spec_->name(), param.expected_name);
  EXPECT_THAT(
      param.expected_claims,
      testing::UnorderedElementsAreArray(particle_spec_->tag_claims()));
  EXPECT_THAT(
      param.expected_checks,
      testing::UnorderedElementsAreArray(particle_spec_->checks()));
  EXPECT_THAT(
      param.expected_edges,
      testing::UnorderedElementsAreArray(particle_spec_->edges()));
}

// Constant to reduce wordiness of test expected output.
static const AccessPathRoot kPs1InHandleRoot(
    HandleConnectionSpecAccessPathRoot("PS1", "in_handle"));
static const AccessPathRoot kPs1OutHandleRoot(
    HandleConnectionSpecAccessPathRoot("PS1", "out_handle"));
static const AccessPathRoot kPs1InOutHandleRoot(
    HandleConnectionSpecAccessPathRoot("PS1", "in_out_handle"));
static const AccessPathRoot kPs2HcHandleRoot(
    HandleConnectionSpecAccessPathRoot("PS2", "hc"));
static const AccessPathRoot kPs2Hc2HandleRoot(
    HandleConnectionSpecAccessPathRoot("PS2", "hc2"));

// Helper function to cut down on expected output wordiness.
static AccessPathSelectors MakeSingleFieldSelectors(
    std::string field_name) {
  return AccessPathSelectors(Selector(FieldSelector(
      std::move(field_name))));
}

// Tag presence predicates for constructing checks.
static const ir::SingleUseArenaAndTagPresence kTag1Present("tag1");
static const ir::SingleUseArenaAndTagPresence kTag2Present("tag2");
static const ir::SingleUseArenaAndTagPresence kTag3Present("tag3");
static const ir::SingleUseArenaAndTagPresence kTag4Present("tag4");

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
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          AccessPath(
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
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          AccessPath(
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
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          AccessPath(
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
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
          AccessPath(
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
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field2"))),
        Edge(
          AccessPath(
              kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
          AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field3"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("world")),
            AccessPath(
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
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
            AccessPath(
                kPs1InHandleRoot, MakeSingleFieldSelectors("field2")),
            AccessPath(
                kPs1InOutHandleRoot, MakeSingleFieldSelectors("field3"))),
        Edge(
          AccessPath(
              kPs1InOutHandleRoot, MakeSingleFieldSelectors("field3")),
          AccessPath(
              kPs1OutHandleRoot, MakeSingleFieldSelectors("field1"))),
        Edge(
          AccessPath(
              kPs1InOutHandleRoot, MakeSingleFieldSelectors("field3")),
          AccessPath(
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
             ir::AccessPath(
                 kPs2HcHandleRoot, MakeSingleFieldSelectors("field1")),
             *kTag1Present.predicate()),
         ir::TagCheck(
             ir::AccessPath(
                 kPs2Hc2HandleRoot, MakeSingleFieldSelectors("field2")),
             *kTag2Present.predicate()),
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
          TagClaim(
              "PS2",
              AccessPath(
                  kPs2HcHandleRoot, MakeSingleFieldSelectors("field1")),
              true,
              "tag1"),
          TagClaim(
              "PS2",
              AccessPath(
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

TEST(BulkInstantiateTest, BulkInstantiateTest) {
  arcs::ParticleSpecProto particle_spec_proto;
  google::protobuf::TextFormat::ParseFromString(
      kTextprotoWithAllFacts, &particle_spec_proto);
  std::unique_ptr<ParticleSpec> particle_spec =
      proto::Decode(std::make_unique<PredicateArena>(), particle_spec_proto);

  const AccessPathRoot p1_in_impl(
      HandleConnectionAccessPathRoot("recipe", "P1", "in_impl"));
  const AccessPathRoot p1_out_impl(
      HandleConnectionAccessPathRoot("recipe", "P1", "out_impl"));
  const AccessPathRoot p1_in_out_impl(
      HandleConnectionAccessPathRoot("recipe", "P1", "in_out_impl"));

  const absl::flat_hash_map<AccessPathRoot, AccessPathRoot>
      instantiation_map {
    { kPs1InHandleRoot, p1_in_impl }, { kPs1OutHandleRoot, p1_out_impl },
    { kPs1InOutHandleRoot, p1_in_out_impl } };

  InstantiatedParticleSpecFacts instantiated_facts =
      particle_spec->BulkInstantiate(instantiation_map);

  ASSERT_THAT(
      instantiated_facts.tag_claims,
      testing::UnorderedElementsAreArray({
        TagClaim(
            "PS1",
            AccessPath(p1_out_impl, MakeSingleFieldSelectors("field1")),
            true,
            "tag1"),
        TagClaim(
            "PS1",
            AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field2")),
            true,
            "tag2")}));

  ASSERT_THAT(
      instantiated_facts.checks,
      testing::UnorderedElementsAreArray({
        ir::TagCheck(
            ir::AccessPath(p1_in_impl, MakeSingleFieldSelectors("field1")),
            *kTag3Present.predicate()),
        ir::TagCheck(
            ir::AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field2")),
            *kTag4Present.predicate())}));

  ASSERT_THAT(
      instantiated_facts.edges,
      testing::UnorderedElementsAreArray({
        Edge(
            AccessPath(p1_in_impl, MakeSingleFieldSelectors("field2")),
            AccessPath(p1_out_impl, MakeSingleFieldSelectors("field1"))),
        Edge(
            AccessPath(p1_in_impl, MakeSingleFieldSelectors("field2")),
            AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3"))),
        Edge(
            AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3")),
            AccessPath(p1_out_impl, MakeSingleFieldSelectors("field1"))),
        Edge(
          AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3")),
          AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3"))),
      }));
}

}  // namespace raksha::ir
