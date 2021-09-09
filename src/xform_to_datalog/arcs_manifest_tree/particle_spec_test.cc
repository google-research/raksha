#include "src/xform_to_datalog/arcs_manifest_tree/particle_spec.h"

#include <google/protobuf/text_format.h>

#include "src/common/testing/gtest.h"

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
      particle_spec.claims(),
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
              ir::TagAnnotationOnAccessPath(
                  ir::AccessPath(
                      kPs2HcHandleRoot, MakeSingleFieldSelectors("field1")),
                      "tag1")
              ),
          ir::TagClaim(
              ir::TagAnnotationOnAccessPath(
                  ir::AccessPath(
                      kPs2Hc2HandleRoot, MakeSingleFieldSelectors("field2")),
                      "tag2")
              )
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

  InstantiatedParticleSpecFacts instantiated_facts =
      particle_spec.BulkInstantiate(instantiation_map);

  ASSERT_THAT(
      instantiated_facts.claims,
      testing::UnorderedElementsAreArray({
        ir::TagClaim(ir::TagAnnotationOnAccessPath(ir::AccessPath(
          p1_out_impl, MakeSingleFieldSelectors("field1")), "tag1")),
        ir::TagClaim(ir::TagAnnotationOnAccessPath(ir::AccessPath(
            p1_in_out_impl, MakeSingleFieldSelectors("field2")), "tag2"))}));

  ASSERT_THAT(
      instantiated_facts.checks,
      testing::UnorderedElementsAreArray({
        ir::TagCheck(ir::TagAnnotationOnAccessPath(ir::AccessPath(
            p1_in_impl, MakeSingleFieldSelectors("field1")), "tag3")),
        ir::TagCheck(ir::TagAnnotationOnAccessPath(ir::AccessPath(
            p1_in_out_impl, MakeSingleFieldSelectors("field2")), "tag4"))}));

  ASSERT_THAT(
      instantiated_facts.edges,
      testing::UnorderedElementsAreArray({
        ir::Edge(
            ir::AccessPath(p1_in_impl, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(p1_out_impl, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
            ir::AccessPath(p1_in_impl, MakeSingleFieldSelectors("field2")),
            ir::AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3"))),
        ir::Edge(
            ir::AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3")),
            ir::AccessPath(p1_out_impl, MakeSingleFieldSelectors("field1"))),
        ir::Edge(
          ir::AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3")),
          ir::AccessPath(p1_in_out_impl, MakeSingleFieldSelectors("field3"))),
      }));
}

}  // namespace raksha::xform_to_datalog::arcs_manifest_tree
