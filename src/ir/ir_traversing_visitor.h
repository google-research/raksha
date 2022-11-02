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
template <typename Derived, typename Result = Unit, bool IsConst = true>
class IRTraversingVisitor : public IRVisitor<Derived, Result, IsConst> {
 private:
  template <class ValueType, class Enable = void>
  struct DefaultValueGetter {
    // TODO(cypher1, jopra): See
    // https://github.com/google-research/raksha/issues/618
    static ValueType Get() {
      LOG(FATAL) << "Override required for non-default-constructible type.";
    }
  };

  template <class ValueType>
  struct DefaultValueGetter<
      ValueType, std::enable_if_t<std::is_default_constructible_v<ValueType>>> {
    static ValueType Get() { return ValueType(); }
  };

 public:
  virtual ~IRTraversingVisitor() {}

  // Gives a default value for all 'PreVisit's to start with.
  // Should be over-ridden if the Result is not default constructable.
  virtual Result GetDefaultValue() { return DefaultValueGetter<Result>::Get(); }

  // Used to accumulate child results from the node's children.
  // Should discard or merge `child_result` into the `accumulator`.
  virtual Result FoldResult(Result accumulator, Result child_result) {
    return accumulator;
  }

  // Invoked before all the children of `module` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Module>& module) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `module` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Module>& module,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `block` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Block>& block) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `block` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Block>& block,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `operation` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Operation>& operation) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `operation` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Operation>& operation,
                           Result in_order_result) {
    return in_order_result;
  }

  Result Visit(CopyConst<IsConst, Module>& module) final override {
    Result pre_visit_result = PreVisit(module);
    Result fold_result = common::utils::fold(
        module.blocks(), std::move(pre_visit_result),
        [this](Result acc, CopyConst<IsConst, std::unique_ptr<Block>>& block) {
          return FoldResult(std::move(acc), block->Accept(*this));
        });
    return PostVisit(module, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, Block>& block) final override {
    Result pre_visit_result = PreVisit(block);
    Result fold_result = common::utils::fold(
        block.operations(), std::move(pre_visit_result),
        [this](Result acc,
               CopyConst<IsConst, std::unique_ptr<Operation>>& operation) {
          return FoldResult(std::move(acc), operation->Accept(*this));
        });
    return PostVisit(block, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, Operation>& operation) final override {
    Result pre_visit_result = PreVisit(operation);
    auto* module = operation.impl_module();
    Result result = module == nullptr ? std::move(pre_visit_result)
                                      : FoldResult(std::move(pre_visit_result),
                                                   module->Accept(*this));
    return PostVisit(operation, std::move(result));
  }
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_TRAVERSING_VISITOR_H_
