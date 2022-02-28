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

#ifndef SRC_IR_PROTO_PARTICLE_SPEC_H_
#define SRC_IR_PROTO_PARTICLE_SPEC_H_

#include <memory>

#include "src/ir/block_builder.h"
#include "src/ir/ir_context.h"
#include "src/ir/module.h"
#include "src/ir/particle_spec.h"
#include "src/ir/types/type_factory.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {

std::unique_ptr<ParticleSpec> Decode(types::TypeFactory& type_factory,
                                     const arcs::ParticleSpecProto& proto);

const Operation& Decode(IRContext& ir_context, BlockBuilder& block_builer,
                        const arcs::ParticleSpecProto& proto);

}  // namespace raksha::ir::proto

#endif  // SRC_IR_PROTO_PARTICLE_SPEC_H_
