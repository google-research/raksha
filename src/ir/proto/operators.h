/* Copyright 2022 Google LLC.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "absl/strings/string_view.h"

namespace arcs {
constexpr absl::string_view kDefaultInput = "input";
constexpr absl::string_view kDefaultResult = "result";
constexpr absl::string_view kParticleNameAttribute = "name";

namespace operators {
constexpr absl::string_view kParticleSpec = "arcs.ParticleSpec";
constexpr absl::string_view kClaim = "arcs.Claim";
constexpr absl::string_view kCheck = "arcs.Check";
constexpr absl::string_view kMerge = "arcs.Merge";
constexpr absl::string_view kReadAccessPath = "arcs.ReadAccessPath";
constexpr absl::string_view kUpdateAccessPath = "arcs.UpdateAccessPath";
constexpr absl::string_view kParticle = "arcs.Particle";
constexpr absl::string_view kWriteStorage = "arcs.WriteStorage";
constexpr absl::string_view kReadStorage = "arcs.ReadStorage";
}  // namespace operators

}  // namespace arcs