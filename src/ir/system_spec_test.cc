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
#include "src/ir/system_spec.h"

#include "src/common/logging/logging.h"
#include "src/common/testing/gtest.h"
#include "src/ir/particle_spec.h"


namespace raksha::ir {

TEST(AddParticleSpec, StoresAndReturnsParticleSpec) {
  SystemSpec spec;
  std::unique_ptr<ParticleSpec> particle_spec =
      ParticleSpec::Create("MyParticle", {}, {}, {}, {}, nullptr);
  const ParticleSpec* created_spec = particle_spec.get();
  const ParticleSpec* stored_spec =
      spec.AddParticleSpec(std::move(particle_spec));
  EXPECT_EQ(created_spec, stored_spec);
  EXPECT_EQ(stored_spec->name(), "MyParticle");
}

TEST(AddParticleSpec, FailsWhenDuplicateSpecsAreStored) {
  SystemSpec spec;
  const ParticleSpec* particle_spec = spec.AddParticleSpec(
      ParticleSpec::Create("MyParticle", {}, {}, {}, {}, nullptr));
  EXPECT_EQ(particle_spec->name(), "MyParticle");
  EXPECT_DEATH( spec.AddParticleSpec(
      ParticleSpec::Create("MyParticle", {}, {}, {}, {}, nullptr)),
                "Tried to insert second particle spec with name MyParticle");
}

TEST(GetParticleSpec, ReturnsStoredParticleSpec) {
  SystemSpec spec;
  spec.AddParticleSpec(
      ParticleSpec::Create("MyParticle", {}, {}, {}, {}, nullptr));
  const ParticleSpec& stored_spec = spec.GetParticleSpec("MyParticle");
  EXPECT_EQ(stored_spec.name(), "MyParticle");
}

TEST(GetParticleSpec, FailsIfParticleSpecNotFound) {
  SystemSpec spec;
  spec.AddParticleSpec(
      ParticleSpec::Create("MyParticle", {}, {}, {}, {}, nullptr));
  EXPECT_DEATH(spec.GetParticleSpec("MyPart"),
               "No particle spec with name MyPart");
}



}  // namespace raksha::ir
