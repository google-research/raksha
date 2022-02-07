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

#include "google/protobuf/text_format.h"

#include "src/common/logging/logging.h"
#include "src/common/testing/gtest.h"
#include "src/ir/proto/particle_spec.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir {

struct ParticleSpecProtoAndExpectedInfo {
  std::string textproto;
  std::string expected_name;
  std::vector<TagClaim> expected_claims;
  std::vector<const TagCheck*> expected_checks;
  std::vector<Edge> expected_edges;
};

class ParticleSpecFromProtoTest :
   public testing::TestWithParam<ParticleSpecProtoAndExpectedInfo> {
 protected:
  ParticleSpecFromProtoTest()
      : particle_spec_(CreateParticleSpec(type_factory_)) {}

  static std::unique_ptr<ParticleSpec> CreateParticleSpec(
      types::TypeFactory& type_factory) {
    arcs::ParticleSpecProto particle_spec_proto;
    std::string textproto = GetParam().textproto;
    CHECK(google::protobuf::TextFormat::ParseFromString(
        textproto, &particle_spec_proto))
        << "Particle spec textproto did not parse correctly.";
    return proto::Decode(type_factory, particle_spec_proto);
  }

  types::TypeFactory type_factory_;
  std::unique_ptr<ParticleSpec> particle_spec_;
};

// Locally defined operator== for use in test below.
bool operator ==(const ir::TagCheck& check1,
                 const ir::TagCheck* check2) {
  if (check2 == nullptr) return false;
  return check1 == *check2;
}

TEST_P(ParticleSpecFromProtoTest, ParticleSpecFromProtoTest) {
  const ParticleSpecProtoAndExpectedInfo &param = GetParam();

  EXPECT_EQ(particle_spec_->name(), param.expected_name);
  EXPECT_THAT(
      particle_spec_->tag_claims(),
      testing::UnorderedElementsAreArray(param.expected_claims));
  EXPECT_THAT(
      particle_spec_->checks(),
      testing::UnorderedElementsAreArray(param.expected_checks));
  EXPECT_THAT(
      particle_spec_->edges(),
      testing::UnorderedElementsAreArray(param.expected_edges));
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

// Tags for constructing checks.
static const std::unique_ptr<TagCheck> kTag1Check = std::make_unique<TagCheck>(
    ir::AccessPath(kPs2HcHandleRoot, MakeSingleFieldSelectors("field1")),
    std::make_unique<TagPresence>("tag1"));
static const std::unique_ptr<TagCheck> kTag2Check = std::make_unique<TagCheck>(
    ir::AccessPath(kPs2Hc2HandleRoot, MakeSingleFieldSelectors("field2")),
    std::make_unique<TagPresence>("tag2"));

static ParticleSpecProtoAndExpectedInfo spec_proto_and_expected_info[] = {
    {.textproto = R"(name: "p_spec")",
     .expected_name = "p_spec",
     .expected_claims = {},
     .expected_checks = {},
     .expected_edges = {}},
    {.textproto = R"(
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
     .expected_name = "PS1",
     .expected_claims = {},
     .expected_checks = {},
     .expected_edges =
         {
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field2"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field2"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
         }},
    {.textproto = R"(
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
     .expected_name = "PS1",
     .expected_claims = {},
     .expected_checks = {},
     .expected_edges =
         {
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field2"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
         }},
    {.textproto = R"(
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
     .expected_name = "PS1",
     .expected_claims = {},
     .expected_checks = {},
     .expected_edges =
         {
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field2"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
         }},
    {.textproto = R"(
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
     .expected_name = "PS1",
     .expected_claims = {},
     .expected_checks = {},
     .expected_edges =
         {
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field2"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field2"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
         }},
    {.textproto = R"(
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
     .expected_name = "PS1",
     .expected_claims = {},
     .expected_checks = {},
     .expected_edges =
         {
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field2"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("world")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
         }},
    {.textproto = R"(
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
     .expected_name = "PS1",
     .expected_claims = {},
     .expected_checks = {},
     .expected_edges =
         {
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InHandleRoot,
                             MakeSingleFieldSelectors("field2")),
                  AccessPath(kPs1InOutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
             Edge(AccessPath(kPs1InOutHandleRoot,
                             MakeSingleFieldSelectors("field3")),
                  AccessPath(kPs1OutHandleRoot,
                             MakeSingleFieldSelectors("field1"))),
             Edge(AccessPath(kPs1InOutHandleRoot,
                             MakeSingleFieldSelectors("field3")),
                  AccessPath(kPs1InOutHandleRoot,
                             MakeSingleFieldSelectors("field3"))),
         }},
    {.textproto = R"(
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
     .expected_claims = {},
     .expected_checks = { kTag1Check.get(), kTag2Check.get() },
     .expected_edges = {}},
    {.textproto = R"(
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
     .expected_claims =
         {TagClaim(
              "PS2",
              AccessPath(kPs2HcHandleRoot, MakeSingleFieldSelectors("field1")),
              true, "tag1"),
          TagClaim(
              "PS2",
              AccessPath(kPs2Hc2HandleRoot, MakeSingleFieldSelectors("field2")),
              true, "tag2")},
     .expected_checks = {},
     .expected_edges = {}}};

INSTANTIATE_TEST_SUITE_P(
    ParticleSpecFromProtoTest, ParticleSpecFromProtoTest,
    testing::ValuesIn(spec_proto_and_expected_info));

}  // namespace raksha::ir
