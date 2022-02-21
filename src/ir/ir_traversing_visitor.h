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
//----------------------------------------------------------------------------
#ifndef SRC_IR_IR_TRAVERSING_VISITOR_H_
#define SRC_IR_IR_TRAVERSING_VISITOR_H_

#include "src/ir/ir_visitor.h"
#include "src/ir/module.h"

namespace raksha::ir {

// A visitor that also traverses the children of a node and allows performing
// different actions before (PreVisit) and after (PostVisit) the children are
// visited. Override any of the `PreVisit` and `PostVisit` methods as needed.
template <typename Derived>
class IRTraversingVisitor : public IRVisitor<Derived> {
 public:
  virtual ~IRTraversingVisitor() {}

  // Invoked before all the children of `module` is visited.
  virtual void PreVisit(const Module& module) {}
  // Invoked after all the children of `module` is visited.
  virtual void PostVisit(const Module& module) {}
  // Invoked before all the children of `block` is visited.
  virtual void PreVisit(const Block& block) {}
  // Invoked after all the children of `block` is visited.
  virtual void PostVisit(const Block& block) {}
  // Invoked before all the children of `operation` is visited.
  virtual void PreVisit(const Operation& operation) {}
  // Invoked after all the children of `operation` is visited.
  virtual void PostVisit(const Operation& operation) {}

  void Visit(const Module& module) final override {
    PreVisit(module);
    for (const std::unique_ptr<Block>& block : module.blocks()) {
      block->Accept(*this);
    }
    PostVisit(module);
  }

  void Visit(const Block& block) final override {
    PreVisit(block);
    for (const std::unique_ptr<Operation>& operation : block.operations()) {
      operation->Accept(*this);
    }
    PostVisit(block);
  }

  void Visit(const Operation& operation) final override {
    PreVisit(operation);
    const Module* module = operation.impl_module();
    if (module != nullptr) {
      module->Accept(*this);
    }
    PostVisit(operation);
  }
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_TRAVERSING_VISITOR_H_
