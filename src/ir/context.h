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
#ifndef SRC_IR_CONTEXT_H_
#define SRC_IR_CONTEXT_H_

#include "absl/container/flat_hash_map.h"
#include "src/ir/operation.h"
#include "src/ir/types/type_factory.h"

namespace raksha::ir {

class Context {
  // TODO: delete unneeded constructors.
 public:
  const Operator *RegisterOperator(std::unique_ptr<Operator> op) {
    auto ins_res = operators_.insert({op->name(), std::move(op)});
    CHECK(ins_res.second) << "Tried to insert second operator with name "
                          << ins_res.first->first;
    return ins_res.first->second.get();
  }

  // Returns the Operator with a particular name. If there is no
  // operator with that name, returns nullptr.
  const Operator *GetOperator(
      absl::string_view operators_name) const {
    auto find_res = operators_.find(operators_name);
    return (find_res != operators_.end()) ? find_res->second.get() : nullptr;
  }

  types::TypeFactory& type_factory() { return type_factory_; }

 private:
  // The particles in the system spec.
  absl::flat_hash_map<std::string, std::unique_ptr<Operator>> operators_;

  types::TypeFactory type_factory_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_CONTEXT_H_
