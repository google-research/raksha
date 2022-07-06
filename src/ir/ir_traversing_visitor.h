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

#include "src/common/utils/types.h"
#include "src/ir/ir_visitor.h"
#include "src/ir/module.h"
#include "src/common/utils/types.h"

namespace raksha::ir {

// A visitor that also traverses the children of a node and allows performing
// different actions before (PreVisit) and after (PostVisit) the children are
// visited. Override any of the `PreVisit` and `PostVisit` methods as needed.
template <typename Derived, bool IsConst=true, typename Result=std::monostate>
class IRTraversingVisitor : public IRVisitor<Derived, IsConst, Result> {
 public:
  virtual ~IRTraversingVisitor() {}

  // Invoked before all the children of `module` is visited.
  virtual Result PreVisit(CopyConst<IsConst, Module>& module) {
    return Result();
  }
  // Invoked after all the children of `module` is visited.
  virtual Result PostVisit(CopyConst<IsConst, Module& module, Result result) {
    return result;
  }
  virtual Result FoldResult(CopyConst<IsConst, Module& module, Result child_result,
                            Result result) {
    return result;
  }
  // Invoked before all the children of `block` is visited.
  virtual Result PreVisit(CopyConst<IsConst, Block& block) {
    return Result();
  }
  // Invoked after all the children of `block` is visited.
  virtual Result PostVisit(CopyConst<IsConst, Block& block, Result result) {
    return result;
  }
  virtual Result FoldResult(CopyConst<IsConst, Block& block, Result child_result,
                            Result result) {
    return result;
  }
  // Invoked before all the children of `operation` is visited.
  virtual Result PreVisit(CopyConst<IsConst, Operation& operation) {
    return Result();
  }
  // Invoked after all the children of `operation` is visited.
  virtual Result PostVisit(CopyConst<IsConst, Operation& operation, Result result) {
    return result;
  }
  virtual Result FoldResult(CopyConst<IsConst, Operation& operation, Result child_result,
                            Result result) {
    return result;
  }

  Result Visit(CopyConst<IsConst, Module>& module) final override {
    Result result = PreVisit(module);
    for (CopyConst<IsConst, std::unique_ptr<Block>>& block : module.blocks()) {
      result =
          FoldResult(*block, block->Accept<Derived, Result>(*this), std::move(result));
    }
    return PostVisit(module, std::move(result));
  }

  Result Visit(CopyConst<IsConst, Block>& block) final override {
    Result result = PreVisit(block);
    for (CopyConst<IsConst, std::unique_ptr<Operation>& operation : block.operations()) {
      result = FoldResult(*operation, operation->Accept<Derived, Result>(*this),
                          std::move(result));
    }
    return PostVisit(block, std::move(result));
  }

  Result Visit(CopyConst<IsConst, Operation>& operation) final override {
    Result result = PreVisit(operation);
    CopyConst<IsConst, Module* module = operation.impl_module();
    if (module != nullptr) {
      result = FoldResult(operation, module->Accept<Derived, Result>(*this),
                          std::move(result));
    }
    return PostVisit(operation, std::move(result));
  }
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_TRAVERSING_VISITOR_H_
