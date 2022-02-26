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
#include "src/ir/proto/operators.h"
#include "src/test_utils/utils.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {

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

  for (const arcs::ParticleSpecProto &particle_spec_proto :
       manifest_proto.particle_specs()) {
    auto operation = ir::proto::Decode(context, particle_spec_proto);
    LOG(WARNING) << IRPrinter::ToString(*operation);
  }
}

}  // namespace raksha::ir::proto
