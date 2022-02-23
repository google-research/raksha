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
#ifndef SRC_IR_IR_CONTEXT_H_
#define SRC_IR_IR_CONTEXT_H_

#include <memory>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "src/ir/operator.h"
#include "src/ir/types/type_factory.h"
#include "src/ir/module.h"

namespace raksha::ir {

class IRContext {
 public:
  IRContext() = default;
  // Disable copy (and move) semantics.
  IRContext(const IRContext &) = delete;
  IRContext &operator=(const IRContext &) = delete;

  types::TypeFactory &type_factory() { return type_factory_; }

  // Register an operator and return the stored operator instances.
  const Operator &RegisterOperator(std::unique_ptr<Operator> op) {
    return operators_.RegisterNode(std::move(op), "operator");
  }

  // Returns the Operator with a particular name. If there is no
  // operator with that name, fail.
  const Operator &GetOperator(absl::string_view operators_name) const {
    return operators_.GetNode(operators_name, "operator");
  }

  // Returns true if the operator is registered with this context.
  bool IsRegisteredOperator(absl::string_view operators_name) const {
    return operators_.IsRegisteredNode(operators_name);
  }

    // Register an operator and return the stored operator instances.
  const Storage &RegisterStorage(std::unique_ptr<Storage> op) {
    return storages_.RegisterNode(std::move(op), "storage");
  }

  // Returns the Storage with a particular name. If there is no
  // operator with that name, fail.
  const Storage &GetStorage(absl::string_view operators_name) const {
    return storages_.GetNode(operators_name, "storage");
  }

  // Returns true if the operator is registered with this context.
  bool IsRegisteredStorage(absl::string_view operators_name) const {
    return storages_.IsRegisteredNode(operators_name);
  }

 private:
  template<class Node>
  class NamedIRNodeRegistry {
   public:
      // Register a node and return the stored instance.
    const Node &RegisterNode(std::unique_ptr<Node> node,
                                    absl::string_view node_kind_name) {
        // Note: using node->name() here is ok because C++ list initialization
        // guarantees that evaluation of initializers earlier in the list
        // are sequenced before initializers later in the list.
        auto ins_res = nodes_.insert(
            {std::string(node->name()), std::move(node)});
        CHECK(ins_res.second)
          << "Cannot register duplicate " << node_kind_name << " with name '"
          << ins_res.first->first << "'.";
        return *ins_res.first->second.get();
    }

    // Returns the node with a particular name. If there is no
    // node with that name, fail.
    const Node &GetNode(
        absl::string_view node_name, absl::string_view node_kind_name) const {
      auto find_res = nodes_.find(node_name);
      CHECK(find_res != nodes_.end())
          << "Looking up an unregistered " << node_kind_name
          <<  " '" << node_name << "'.";
      return *find_res->second;
    }

    // Returns true if the node is registered with this context.
    bool IsRegisteredNode(absl::string_view node_name) const {
      return nodes_.find(node_name) != nodes_.end();
    }
   private:
    absl::flat_hash_map<std::string, std::unique_ptr<Node>> nodes_;
  };

  // List of registered operators.
  NamedIRNodeRegistry<Operator> operators_;
  // List of registered storages.
  NamedIRNodeRegistry<Storage> storages_;
  // TypeFactory for this context.
  types::TypeFactory type_factory_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_CONTEXT_H_
