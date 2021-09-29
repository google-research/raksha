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
//----------------------------------------------------------------------------

#ifndef SRC_IR_INSTANCE_FACT_H_
#define SRC_IR_INSTANCE_FACT_H_

#include "src/ir/instantiator.h"
#include "src/ir/noop_instantiator.h"

// An InstanceFact is an instantation of some other fact that may be
// initially not fully instantiated.
//
// Consider, for instance, the case of checks and claims on a ParticleSpec.
// Checks and claims describe the check or claim they wish to perform on
// AccessPaths rooted at the ParticleSpec. At some point, however, they must be
// printed out as performing those claims and checks upon the access paths of
// an actual instantiated particle.
//
// To do this, we create an InstanceFact upon the check or claim (or any
// other instantiatable fact). The InstanceFact points at the original,
// uninstantiated fact, and an Instantiator, which is just an object that
// will give us a map from uninstantiated AccessPath roots to instantiated
// AccessPath roots. Using the mapping provided by the instantiator, we can
// transform details of the uninstantiated fact to be instantiated on the fly.
// This prevents having to copy the data initially on the uninstantiated
// fact, sidestepping the pointer ownership quandries and performance
// penalties we may otherwise have to deal with.

namespace raksha::ir {

// This class represents an instantiation of a fact that was originally
// stated about some uninstantiated root (such as a HandleConnectionSpec)
// using an Instantiator.
template<class T>
class InstantiatedInstanceFact {
 public:
  InstantiatedInstanceFact(
      const T &original_fact, const Instantiator &instantiator)
      : original_fact_(&original_fact), instantiator_(&instantiator) {}

  std::string ToDatalog() const {
    return original_fact_->ToDatalog(*instantiator_);
  }

 private:
  const T *original_fact_;
  const Instantiator *instantiator_;
};

// This class represents a fact that was instantiated at its creation, and
// thus is directly embedded into the InstanceFact. Because it is already
// instantiated, we always use the NoopInstantiator when calling methods upon
// it.
template<class T>
class ImmediateInstanceFact {
 public:
  ImmediateInstanceFact(T fact) : inner_fact_(std::move(fact)) { }

  std::string ToDatalog() const {
    return inner_fact_.ToDatalog(NoopInstantiator::Get());
  }
 private:
  T inner_fact_;
};

// The InstanceFact wraps a std::variant containing both of the above cases
// and acts as if it were an interface-style abstract class.
template<class T>
class InstanceFact {
 public:
  using SpecificFactInstantiation =
      std::variant<InstantiatedInstanceFact<T>, ImmediateInstanceFact<T>>;

  explicit InstanceFact(SpecificFactInstantiation specific_fact_instantiation)
    : specific_fact_instantiation_(std::move(specific_fact_instantiation)) {}

  std::string ToDatalog() const {
    return std::visit(
        [](const auto &specific_fact_instantiation) {
          return specific_fact_instantiation.ToDatalog();
        }, specific_fact_instantiation_);
  }

 private:
  SpecificFactInstantiation specific_fact_instantiation_;
};

// We define these factory functions outside of the InstanceFact class to
// allow for less boilerplate when calling the function.

// A factory helper function for creating an ImmediateInstanceFact.
template<class F>
static InstanceFact<F> CreateImmediateInstanceFact(F fact) {
  return InstanceFact<F>(ImmediateInstanceFact(fact));
}

// A factory helper function for creating an InstantiatedInstanceFact.
template<class F>
static InstanceFact<F> CreateInstantiatedInstanceFact(
    const F &original_fact, const Instantiator &instantiator) {
  return InstanceFact<F>(InstantiatedInstanceFact(original_fact, instantiator));
}

}  // namespace raksha::ir

#endif  // SRC_IR_INSTANCE_FACT_H_
