//-----------------------------------------------------------------------------
// Copyright 2022 Google LLC
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

#include "src/ir/proto/particle_spec.h"

#include "src/common/testing/gtest.h"
#include "src/ir/ir_printer.h"
#include "src/ir/ir_visitor.h"
#include "src/ir/proto/operators.h"
#include "src/ir/proto/type.h"
#include "src/test_utils/utils.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {
namespace {

class DatalogPrinter : public IRVisitor<DatalogPrinter> {};

TEST(ParticleSpecTest, TestSomething) {
  auto test_data =
      test_utils::GetTestDataDir("src/ir/proto/test_arcs_proto.binarypb");
  // Parse manifest stream and dump as datalog program.
  std::ifstream manifest_proto_stream(test_data);
  if (!manifest_proto_stream) {
    LOG(ERROR) << "Error reading manifest proto file " << test_data << ":"
               << strerror(errno);
    return;
  }

  arcs::ManifestProto manifest_proto;
  if (!manifest_proto.ParseFromIstream(&manifest_proto_stream)) {
    LOG(ERROR) << "Error parsing the manifest proto " << test_data;
  }
  IRContext context;
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kParticleSpec));
  context.RegisterOperator(std::make_unique<Operator>(arcs::operators::kClaim));
  context.RegisterOperator(std::make_unique<Operator>(arcs::operators::kCheck));
  context.RegisterOperator(std::make_unique<Operator>(arcs::operators::kMerge));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kReadAccessPath));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kUpdateAccessPath));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kParticle));

  Module global_module;
  BlockBuilder builder;
  for (const arcs::ParticleSpecProto &particle_spec_proto :
       manifest_proto.particle_specs()) {
    const auto &operation =
        ir::proto::Decode(context, builder, particle_spec_proto);
    LOG(WARNING) << operation;
  }
  for (const arcs::RecipeProto &recipe_proto : manifest_proto.recipes()) {
    // const std::string &recipe_name =
    //     recipe_proto.name().empty()
    //         ? absl::StrCat(generated_recipe_name_prefix, recipe_num++)
    //         : recipe_proto.name();

    // For each Particle, generate the relevant facts for that particle.
    // These fall into two categories: facts that lie within the ParticleSpec
    // that just need to be instantiated for this particular Particle, and
    // edges that connect this Particle to input/output Handles.
    // uint64_t particle_num = 0;
    for (const arcs::HandleProto &handle_proto : recipe_proto.handles()) {
      auto type =
          types::proto::Decode(context.type_factory(), handle_proto.type());
      context.RegisterStorage(
          std::make_unique<Storage>(handle_proto.name(), type));
    }
    for (const arcs::ParticleProto &particle_proto : recipe_proto.particles()) {
      NamedValueMap inputs;
      for (const auto &handle_connection_proto : particle_proto.connections()) {
        inputs.insert({handle_connection_proto.name(),
                       Value(value::StoredValue(context.GetStorage(
                           handle_connection_proto.handle())))});
      }
      builder.AddOperation(
          context.GetOperator(arcs::operators::kParticle),
          {{"name", StringAttribute::Create(particle_proto.spec_name())}},
          inputs);
    }
    global_module.AddBlock(builder.build());
  }
  LOG(WARNING) << global_module;
}

}  // namespace
}  // namespace raksha::ir::proto
