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

#include "src/common/utils/fold.h"
#include "src/common/utils/types.h"
#include "src/ir/ir_visitor.h"
#include "src/ir/module.h"

namespace raksha::ir {

// A visitor that also traverses the children of a node and allows performing
// different actions before (PreVisit) and after (PostVisit) the children are
// visited. Override any of the `PreVisit` and `PostVisit` methods as needed.
template <typename Derived, typename Result = Unit>
class IRTraversingVisitor : public IRVisitor<Derived, Result> {
 public:
  virtual ~IRTraversingVisitor() {}

  // Gives a default value for all 'PreVisit's to start with.
  // Should be over-ridden if the Result is not default constructable.
  virtual Result GetDefaultValue() { return Result(); }

  // Invoked before all the children of `module` is visited.
  virtual Result PreVisit(const Module& module) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `module` is visited.
  virtual Result PostVisit(const Module& module, Result in_order_result) {
    return in_order_result;
  }
  // Used to accumulate child results from the Module's children.
  // Should discard or merge `child_result` into the `accumulator`.
  virtual Result FoldResult(Result accumulator, const Module& parent,
                            Result child_result) {
    return accumulator;
  }
  // Invoked before all the children of `block` is visited.
  virtual Result PreVisit(const Block& block) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `block` is visited.
  virtual Result PostVisit(const Block& block, Result in_order_result) {
    return in_order_result;
  }
  // Used to accumulate child results from the Block's children.
  // Should discard or merge `child_result` into the `accumulator`.
  virtual Result FoldResult(Result accumulator, const Block& parent,
                            Result child_result) {
    return accumulator;
  }
  // Invoked before all the children of `operation` is visited.
  virtual Result PreVisit(const Operation& operation) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `operation` is visited.
  virtual Result PostVisit(const Operation& operation, Result in_order_result) {
    return in_order_result;
  }
  // Used to accumulate child results from the Operation's children.
  // Should discard or merge `child_result` into the `accumulator`.
  virtual Result FoldResult(Result accumulator, const Operation& parent,
                            Result child_result) {
    return accumulator;
  }

  Result Visit(const Module& module) final override {
    Result pre_visit_result = PreVisit(module);
    Result fold_result = common::utils::fold(
        module.blocks(), std::move(pre_visit_result),
        [this, &module](Result acc, const std::unique_ptr<Block>& block) {
          return FoldResult(acc, module,
                                      block->Accept(*this));
        });
    return PostVisit(module, fold_result);
  }

  Result Visit(const Block& block) final override {
    Result pre_visit_result = PreVisit(block);
    Result fold_result = common::utils::fold(
        block.operations(), std::move(pre_visit_result),
        [this, &block](Result acc, const std::unique_ptr<Operation>& operation) {
          return FoldResult(acc, block,
                         operation->Accept(*this));
        });
    return PostVisit(block, fold_result);
  }

  Result Visit(const Operation& operation) final override {
    Result result = PreVisit(operation);
    const Module* module = operation.impl_module();
    if (module != nullptr) {
      result = FoldResult(std::move(result), operation, module->Accept<Derived, Result>(*this));
    }
    return PostVisit(operation, result);
  }
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_TRAVERSING_VISITOR_H_
