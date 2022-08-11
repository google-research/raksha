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
template <typename Derived, typename Result = Unit, AstNodeMutability IsConst = AstNodeMutability::Immutable>
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

  // Used to combine two `Result`s into one result while visiting a node
  virtual Result CombineResult(Result left_result, Result right_result) {
    return left_result;
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
  // Invoked before all the children of `can_act_as` are visited.
  virtual Result PreVisit(CopyConst<IsConst, CanActAs>& can_act_as) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `canActAs` are visited.
  virtual Result PostVisit(CopyConst<IsConst, CanActAs>& can_act_as,
                           Result in_order_result) {
    return in_order_result;
  }
  // Invoked before all the children of `baseFact` are visited.
  virtual Result PreVisit(CopyConst<IsConst, BaseFact>& base_fact) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `baseFact` are visited.
  virtual Result PostVisit(CopyConst<IsConst, BaseFact>& base_fact,
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
      CopyConst<IsConst, ConditionalAssertion>& conditional_assertion) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `conditionalAssertion` are visited.
  virtual Result PostVisit(
      CopyConst<IsConst, ConditionalAssertion>& conditional_assertion,
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
  virtual Result PreVisit(CopyConst<IsConst, SaysAssertion>& says_assertion) {
    return GetDefaultValue();
  }
  // Invoked after all the children of `saysAssertion` are visited.
  virtual Result PostVisit(CopyConst<IsConst, SaysAssertion>& says_assertion,
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
      CopyConst<IsConst, datalog::RelationDeclaration>& relation_declaration) {
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
        CombineResult(CombineResult(std::move(pre_visit_result),
                              attribute.principal().Accept(*this)),
                   // TODO(#644 aferr): fix this to use predicate().Accept once
                   // predicate has been refactored into ast.h
                   Visit(attribute.predicate()));
    return PostVisit(attribute, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, CanActAs>& can_act_as) final override {
    Result pre_visit_result = PreVisit(can_act_as);
    Result fold_result =
        CombineResult(CombineResult(std::move(pre_visit_result),
                              can_act_as.left_principal().Accept(*this)),
                   can_act_as.right_principal().Accept(*this));
    return PostVisit(can_act_as, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, BaseFact>& base_fact) final override {
    Result pre_visit_result = PreVisit(base_fact);
    Result variant_visit_result = std::visit(
        raksha::utils::overloaded{
            [this](const datalog::Predicate& pred) {
              return VariantVisit(pred);
            },
            [this](const Attribute& attrib) { return VariantVisit(attrib); },
            [this](const CanActAs& can_act_as) {
              return VariantVisit(can_act_as);
            }},
        base_fact.GetValue());
    Result fold_result = CombineResult(std::move(pre_visit_result),
                                    std::move(variant_visit_result));
    return PostVisit(base_fact, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, Fact>& fact) final override {
    Result pre_visit_result = PreVisit(fact);
    Result deleg_result = FoldAccept<Principal, std::forward_list<Principal>>(
      fact.delegation_chain(), pre_visit_result);
    Result base_fact_result =
        CombineResult(std::move(deleg_result), fact.base_fact().Accept(*this));
    return PostVisit(fact, std::move(base_fact_result));
  }

  Result Visit(CopyConst<IsConst, ConditionalAssertion>& conditional_assertion)
      final override {
    Result pre_visit_result = PreVisit(conditional_assertion);
    Result lhs_result = CombineResult(std::move(pre_visit_result),
                                   conditional_assertion.lhs().Accept(*this));
    Result fold_result = FoldAccept<BaseFact, std::vector<BaseFact>>(
        conditional_assertion.rhs(), lhs_result);
    return PostVisit(conditional_assertion, std::move(fold_result));
  }

  Result Visit(CopyConst<IsConst, Assertion>& assertion) final override {
    Result pre_visit_result = PreVisit(assertion);
    Result variant_visit_result =
        std::visit(raksha::utils::overloaded{
                       [this](const Fact& fact) { return VariantVisit(fact); },
                       [this](const ConditionalAssertion& cond_assertion) {
                         return VariantVisit(cond_assertion);
                       }},
                   assertion.GetValue());
    Result fold_result = CombineResult(std::move(pre_visit_result),
                                    std::move(variant_visit_result));
    return PostVisit(assertion, std::move(fold_result));
  }

  Result Visit(
      CopyConst<IsConst, SaysAssertion>& says_assertion) final override {
    Result pre_visit_result = PreVisit(says_assertion);
    Result principal_result = CombineResult(
        std::move(pre_visit_result), says_assertion.principal().Accept(*this));
    Result fold_result = FoldAccept<Assertion, std::vector<Assertion>>
      (says_assertion.assertions(), principal_result);
    return PostVisit(says_assertion, fold_result);
  }

  Result Visit(CopyConst<IsConst, Query>& query) final override {
    Result pre_visit_result = PreVisit(query);
    Result fold_result = CombineResult(std::move(pre_visit_result),
                                    CombineResult(query.principal().Accept(*this),
                                               query.fact().Accept(*this)));
    return PostVisit(query, fold_result);
  }

  Result Visit(CopyConst<IsConst, Program>& program) final override {
    Result pre_visit_result = PreVisit(program);
    Result declarations_result = common::utils::fold(
        program.relation_declarations(), std::move(pre_visit_result),
        [this](Result acc, CopyConst<IsConst, datalog::RelationDeclaration>
                               relation_declaration) {
          // TODO(#644 aferr) Fix this to accept once once relationDeclaration
          // has been refactored into ast.h
          return CombineResult(std::move(acc), Visit(relation_declaration));
        });
    Result says_assertions_result = FoldAccept<SaysAssertion,
      std::vector<SaysAssertion>>(program.says_assertions(), 
      declarations_result);
    Result queries_result = FoldAccept<Query, std::vector<Query>>(
        program.queries(), says_assertions_result);
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
  Result VariantVisit(CanActAs can_act_as) { return can_act_as.Accept(*this); }

  // For AssertionVariantType
  Result VariantVisit(Fact fact) { return fact.Accept(*this); }
  Result VariantVisit(ConditionalAssertion conditional_assertion) {
    return conditional_assertion.Accept(*this);
  }

 private:
  template<class Element, class Container>
  Result FoldAccept(Container container, Result initial) {
    return common::utils::fold(container, std::move(initial),
      [this](Result acc, CopyConst<IsConst, Element> element) {
        return CombineResult(std::move(acc), element.Accept(*this));
    });
  }

};

}  // namespace raksha::ir::auth_logic

#endif  // SRC_IR_AUTH_LOGIC_AST_TRAVERSING_VISITOR_H_
