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
#ifndef SRC_IR_AUTH_LOGIC_AST_TRAVERSING_VISITOR_H_
#define SRC_IR_AUTH_LOGIC_AST_TRAVERSING_VISITOR_H_

#include <glog/logging.h>

#include "src/common/utils/fold.h"
#include "src/common/utils/overloaded.h"
#include "src/common/utils/types.h"
#include "src/ir/auth_logic/ast.h"
#include "src/ir/auth_logic/auth_logic_ast_visitor.h"
#include "src/ir/datalog/program.h"

// The implementation of this visitor over the AST nodes of authorizaiton logic
// directly follows the one for the IR in /src/ir/ir_traversing_visitor.h

namespace raksha::ir::auth_logic {

// A visitor that also traverses the children of a node and allows performing
// different actions before (PreVisit) and after (PostVisit) the children are
// visited. Override any of the `PreVisit` and `PostVisit` methods as needed.
template <typename Derived, typename Result = Unit, bool IsConst = true>
class AuthLogicAstTraversingVisitor
    : public AuthLogicAstVisitor<Derived, Result, IsConst> {
 private:
  template <class ValueType, class Enable = void>
  struct DefaultValueGetter {
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
  virtual ~AuthLogicAstTraversingVisitor() {}

  // Gives a default value for all 'PreVisit's to start with.
  // Should be over-ridden if the Result is not default constructable.
  virtual Result GetDefaultValue() { return DefaultValueGetter<Result>::Get(); }

  // Used to accumulate child results from the node's children.
  // Should discard or merge `child_result` into the `accumulator`.
  virtual Result FoldResult(Result accumulator, Result child_result) {
    return accumulator;
  }
  // Invoked before all the children of `principal` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Principal>& principal) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `principal` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Principal>& principal,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `attribute` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Attribute>& attribute) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `attribute` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Attribute>& attribute,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `canActAs` are visited.
  virtual Result PreVisit(CopyConst<IsConst, CanActAs>& canActAs) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `canActAs` are visited.
  virtual Result PostVisit(CopyConst<IsConst, CanActAs>& canActAs,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `baseFact` are visited.
  virtual Result PreVisit(CopyConst<IsConst, BaseFact>& baseFact) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `baseFact` are visited.
  virtual Result PostVisit(CopyConst<IsConst, BaseFact>& baseFact,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `fact` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Fact>& fact) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `fact` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Fact>& fact,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `conditionalAssertion` are visited.
  virtual Result PreVisit(
      CopyConst<IsConst, ConditionalAssertion>& conditionalAssertion) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `conditionalAssertion` are visited.
  virtual Result PostVisit(
      CopyConst<IsConst, ConditionalAssertion>& conditionalAssertion,
      Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `assertion` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Assertion>& assertion) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `assertion` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Assertion>& assertion,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `saysAssertion` are visited.
  virtual Result PreVisit(CopyConst<IsConst, SaysAssertion>& saysAssertion) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `saysAssertion` are visited.
  virtual Result PostVisit(CopyConst<IsConst, SaysAssertion>& saysAssertion,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `query` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Query>& query) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `query` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Query>& query,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `program` are visited.
  virtual Result PreVisit(CopyConst<IsConst, Program>& program) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `program` are visited.
  virtual Result PostVisit(CopyConst<IsConst, Program>& program,
                           Result in_order_result) {
    return in_order_result;
  }

  // TODO (#644) aferr
  // The Visits for the Datalog IR classes (RelationDeclaration, Predciate)
  // are here temporarily until these AST classes are refactored out
  // of the Datalog IR.

  virtual Result Visit(
      CopyConst<IsConst, datalog::RelationDeclaration>& relationDeclaration) {
    return GetDefaultValue();
  }

  virtual Result Visit(CopyConst<IsConst, datalog::Predicate>& predicate) {
    return GetDefaultValue();
  }

  // The remaining Visits are meant to follow the convention

  Result Visit(CopyConst<IsConst, Principal>& principal) final override {
    Result pre_visit_result = PreVisit(principal);
    return PostVisit(principal, std::move(pre_visit_result));
  }

  Result Visit(CopyConst<IsConst, Attribute>& attribute) final override {
    Result pre_visit_result = PreVisit(attribute);
    Result fold_result =
        FoldResult(FoldResult(std::move(pre_visit_result),
                              attribute.principal().Accept(*this)),
                   // TODO(#644 aferr): fix this to use predicate().Accept once
                   // predicate has been refactored into ast.h
                   Visit(attribute.predicate()));
    return PostVisit(attribute, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, CanActAs>& canActAs) final override {
    Result pre_visit_result = PreVisit(canActAs);
    Result fold_result =
        FoldResult(FoldResult(std::move(pre_visit_result),
                              canActAs.left_principal().Accept(*this)),
                   canActAs.right_principal().Accept(*this));
    return PostVisit(canActAs, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, BaseFact>& baseFact) final override {
    Result pre_visit_result = PreVisit(baseFact);
    Result variant_visit_result = std::visit(
        raksha::utils::overloaded{
            [this](const datalog::Predicate& pred) {
              return VariantVisit(pred);
            },
            [this](const Attribute& attrib) { return VariantVisit(attrib); },
            [this](const CanActAs& canActAs) {
              return VariantVisit(canActAs);
            }},
        baseFact.GetValue());
    Result fold_result = FoldResult(std::move(pre_visit_result),
                                    std::move(variant_visit_result));
    return PostVisit(baseFact, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, Fact>& fact) final override {
    Result pre_visit_result = PreVisit(fact);
    Result base_fact_result =
        FoldResult(std::move(pre_visit_result), fact.base_fact().Accept(*this));
    Result fold_result = common::utils::fold(
        fact.delegation_chain(), std::move(base_fact_result),
        [this](Result acc, CopyConst<IsConst, Principal> principal) {
          return FoldResult(std::move(acc), principal.Accept(*this));
        });
    return PostVisit(fact, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, ConditionalAssertion>& conditionalAssertion)
      final override {
    Result pre_visit_result = PreVisit(conditionalAssertion);
    Result lhs_result = FoldResult(std::move(pre_visit_result),
                                   conditionalAssertion.lhs().Accept(*this));
    Result fold_result = common::utils::fold(
        conditionalAssertion.rhs(), std::move(lhs_result),
        [this](Result acc, CopyConst<IsConst, BaseFact> baseFact) {
          return FoldResult(std::move(acc), baseFact.Accept(*this));
        });
    return PostVisit(conditionalAssertion, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, Assertion>& assertion) final override {
    Result pre_visit_result = PreVisit(assertion);
    Result variant_visit_result =
        std::visit(raksha::utils::overloaded{
                       [this](const Fact& fact) { return VariantVisit(fact); },
                       [this](const ConditionalAssertion& condAssertion) {
                         return VariantVisit(condAssertion);
                       }},
                   assertion.GetValue());
    Result fold_result = FoldResult(std::move(pre_visit_result),
                                    std::move(variant_visit_result));
    return PostVisit(assertion, std::move(fold_result));
  }

  Result Visit(
      CopyConst<IsConst, SaysAssertion>& saysAssertion) final override {
    Result pre_visit_result = PreVisit(saysAssertion);
    Result principal_result = FoldResult(
        std::move(pre_visit_result), saysAssertion.principal().Accept(*this));
    Result fold_result = common::utils::fold(
        saysAssertion.assertions(), std::move(principal_result),
        [this](Result acc, CopyConst<IsConst, Assertion> assertion) {
          return FoldResult(std::move(acc), assertion.Accept(*this));
        });
    return PostVisit(saysAssertion, fold_result);
  }

  Result Visit(CopyConst<IsConst, Query>& query) final override {
    Result pre_visit_result = PreVisit(query);
    Result fold_result = FoldResult(std::move(pre_visit_result),
                                    FoldResult(query.principal().Accept(*this),
                                               query.fact().Accept(*this)));
    return PostVisit(query, fold_result);
  }

  Result Visit(CopyConst<IsConst, Program>& program) final override {
    Result pre_visit_result = PreVisit(program);
    Result declarations_result = common::utils::fold(
        program.relation_declarations(), std::move(pre_visit_result),
        [this](Result acc, CopyConst<IsConst, datalog::RelationDeclaration>
                               relationDeclaration) {
          // TODO(#644 aferr) Fix this to accept once once relationDeclaration
          // has been refactored into ast.h
          return FoldResult(std::move(acc), Visit(relationDeclaration));
        });
    Result says_assertions_result = common::utils::fold(
        program.says_assertions(), std::move(declarations_result),
        [this](Result acc, CopyConst<IsConst, SaysAssertion> saysAssertion) {
          return FoldResult(std::move(acc), saysAssertion.Accept(*this));
        });
    Result queries_result = common::utils::fold(
        program.queries(), std::move(says_assertions_result),
        [this](Result acc, CopyConst<IsConst, Query> query) {
          return FoldResult(std::move(acc), query.Accept(*this));
        });
    return PostVisit(program, queries_result);
  }

  // The VariantVisit methods use overloading to help visit
  // the alternatives for the underlying std::variants in the AST

  // For BaseFactVariantType
  Result VariantVisit(datalog::Predicate predicate) {
    // TODO(#644 aferr) once a separate predicate has been added to ast.h
    // this should use predicate.Accept(*this);
    return Visit(predicate);
  }
  Result VariantVisit(Attribute attribute) { return attribute.Accept(*this); }
  Result VariantVisit(CanActAs canActAs) { return canActAs.Accept(*this); }

  // For AssertionVariantType
  Result VariantVisit(Fact fact) { return fact.Accept(*this); }
  Result VariantVisit(ConditionalAssertion conditionalAssertion) {
    return conditionalAssertion.Accept(*this);
  }

 private:
};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_TRAVERSING_VISITOR_H_
