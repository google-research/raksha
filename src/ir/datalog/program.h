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

// This file contains the datalog IR (DLIR) which makes the translation from
// this authorization logic into datalog simpler.

#ifndef SRC_IR_DATALOG_PROGRAM_H_
#define SRC_IR_DATALOG_PROGRAM_H_

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "src/ir/ast_node.h"

namespace raksha::ir::datalog {

// Used to represent whether a predicate is negated or not
enum Sign { kNegated, kPositive };

// Predicate corresponds to a predicate of the form
// <pred_name>(arg_1, ..., arg_n), and it may or may not be negated
class Predicate : public AstNode {
 public:
  Predicate(std::string name, std::vector<std::string> args, Sign sign)
      : name_(std::move(name)),
        args_(std::move(args)),
        // TODO move surpresses an unused private field error about sign_
        // for now, get rid of this eventually
        sign_(std::move(sign)) {}

  virtual ~Predicate() = default;

  const std::string& name() const { return name_; }
  const std::vector<std::string>& args() const { return args_; }
  Sign sign() const { return sign_; }

  template <typename H>
  friend H AbslHashValue(H h, const Predicate& p) {
    return H::combine(std::move(h), p.name(), p.args(), p.sign());
  }
  // Equality is also needed to use a Predicate in a flat_hash_set
  bool operator==(const Predicate& otherPredicate) const {
    return this->name() == otherPredicate.name() &&
           this->sign() == otherPredicate.sign() &&
           this->args() == otherPredicate.args();
  }

  bool operator==(const AstNode& rhs) const override {
    try {
      const Predicate& rhsPredicate= dynamic_cast<const Predicate&>(rhs);
      return *this == rhsPredicate;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  // < operator is needed for btree_set, which is only used for declarations.
  // Since declarations are uniquely defined by the name of the predicate,
  // this implementation that just uses < on the predicate names should be
  // sufficent in the context where it is used.
  bool operator<(const Predicate& otherPredicate) const {
    return this->name() < otherPredicate.name();
  }


  // A potentially ugly print of the state in this class
  // for debugging only
  std::string DebugPrint() const override {
    return absl::StrCat(sign_, name_, absl::StrJoin(args_, ", "));
  }

 private:
  std::string name_;
  std::vector<std::string> args_;
  Sign sign_;
};

// Relation declaration types are of 3 forms
// 1. NumberType
// 2. PrincipalType
// 3. User defined CustomType (string to store name of the type)
class ArgumentType : public AstNode {
 public:
  enum class Kind { kNumber, kPrincipal, kCustom };
  explicit ArgumentType(Kind kind, absl::string_view name)
      : kind_(kind), name_(name) {}
  virtual ~ArgumentType() = default;
  Kind kind() const { return kind_; }
  absl::string_view name() const { return name_; }

  bool operator==(const ArgumentType& otherType) const {
    return this->kind_ == otherType.kind() &&
      this->kind_ == Kind::kCustom ? this->name_ == otherType.name() : true;
  }

  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const ArgumentType& rhsArgType= dynamic_cast<const ArgumentType&>(rhs);
      return *this == rhsArgType;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  // A potentially ugly print of the state in this class
  // for debugging only
  std::string DebugPrint() const override { return absl::StrCat(kind_, name_); }

 private:
  Kind kind_;
  std::string name_;
};

class Argument : public AstNode {
 public:
  explicit Argument(std::string_view argument_name, ArgumentType argument_type)
      : argument_name_(argument_name),
        argument_type_(std::move(argument_type)) {}
  virtual ~Argument() = default;
  absl::string_view argument_name() const { return argument_name_; }
  ArgumentType argument_type() const { return argument_type_; }

  bool operator==(const Argument& otherArgument) const {
    return this->argument_name_ == otherArgument.argument_name() &&
    this->argument_type_ == otherArgument.argument_type();
  }

  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Argument& rhsArgument= dynamic_cast<const Argument&>(rhs);
      return *this == rhsArgument;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  // A potentially ugly print of the state in this class
  // for debugging only
  std::string DebugPrint() const override {
    return absl::StrCat(argument_name_, " : ", argument_type_.DebugPrint());
  }

 private:
  std::string argument_name_;
  ArgumentType argument_type_;
};

class RelationDeclaration : public AstNode {
 public:
  explicit RelationDeclaration(absl::string_view relation_name,
                               bool is_attribute,
                               std::vector<Argument> arguments)
      : relation_name_(relation_name),
        is_attribute_(is_attribute),
        arguments_(std::move(arguments)) {}
  virtual ~RelationDeclaration() = default;
  absl::string_view relation_name() const { return relation_name_; }
  bool is_attribute() const { return is_attribute_; }
  const std::vector<Argument>& arguments() const { return arguments_; }
  
  bool operator==(const RelationDeclaration& otherDeclaration) const {
    return this->relation_name_ == otherDeclaration.relation_name() &&
    this->is_attribute_ == otherDeclaration.is_attribute() &&
    this->arguments_ == otherDeclaration.arguments();
  }

  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const RelationDeclaration& rhsRelDecl = 
        dynamic_cast<const RelationDeclaration&>(rhs);
      return *this == rhsRelDecl;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  // A potentially ugly print of the state in this class
  // for debugging only
  std::string DebugPrint() const override {
    std::vector<std::string> arg_strings;
    arg_strings.reserve(arguments_.size());
    for (const Argument& arg : arguments_) {
      arg_strings.push_back(arg.DebugPrint());
    }
    return absl::StrCat(".decl ", relation_name_, is_attribute_,
                        absl::StrJoin(arg_strings, ", "));
  }

 private:
  std::string relation_name_;
  bool is_attribute_;
  std::vector<Argument> arguments_;
};

// A conditional datalog assertion with a left hand side and/or a right hand
// side. A Rule is either:
//    - an unconditional fact which is a predicate
//    - a conditional assertion
class Rule : AstNode {
 public:
  explicit Rule(Predicate lhs, std::vector<Predicate> rhs)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
  explicit Rule(Predicate lhs) : lhs_(std::move(lhs)) {}
  virtual ~Rule() = default;
  const Predicate& lhs() const { return lhs_; }
  const std::vector<Predicate>& rhs() const { return rhs_; }

  bool operator==(const Rule& otherRule) const {
    return this->lhs_ == otherRule.lhs() &&
    this->rhs_ == otherRule.rhs();
  }

  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Rule& rhsRule = dynamic_cast<const Rule&>(rhs);
      return *this == rhsRule;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  // A potentially ugly print of the state in this class
  // for debugging only
  std::string DebugPrint() const override {
    std::vector<std::string> rhs_strings;
    rhs_strings.reserve(rhs_.size());
    for (const Predicate& pred: rhs_) {
      rhs_strings.push_back(pred.DebugPrint());
    }
    return absl::StrCat(lhs_.DebugPrint(), ":-",
                        absl::StrJoin(rhs_strings, ", "));
  }

 private:
  Predicate lhs_;
  std::vector<Predicate> rhs_;
};

class Program : public AstNode {
 public:
  Program(std::vector<RelationDeclaration> relation_declarations,
          std::vector<Rule> rules, std::vector<std::string> outputs)
      : relation_declarations_(std::move(relation_declarations)),
        rules_(std::move(rules)),
        outputs_(std::move(outputs)) {}
  virtual ~Program() = default;
  const std::vector<RelationDeclaration>& relation_declarations() const {
    return relation_declarations_;
  }
  const std::vector<Rule>& rules() const { return rules_; }
  const std::vector<std::string>& outputs() const { return outputs_; }

  bool operator==(const Program& otherProgram) const {
    return this->relation_declarations_ == 
      otherProgram.relation_declarations() &&
      this->rules_ == otherProgram.rules() &&
      this->outputs_ == otherProgram.outputs();
  }

  virtual bool operator==(const AstNode& rhs) const override {
    try {
      const Program& rhsProgram= dynamic_cast<const Program&>(rhs);
      return *this == rhsProgram;
    } catch(const std::bad_cast& e) {
      return false;
    }
  }

  // A potentially ugly print of the state in this class
  // for debugging only
  std::string DebugPrint() const override {
    std::vector<std::string> relation_decl_strings;
    relation_decl_strings.reserve(relation_declarations_.size());
    for (const RelationDeclaration& rel_decl :
         relation_declarations_) {
      relation_decl_strings.push_back(rel_decl.DebugPrint());
    }
    std::vector<std::string> rule_strings;
    rule_strings.reserve(rules_.size());
    for (const Rule& rule: rules_) {
      rule_strings.push_back(rule.DebugPrint());
    }
    return absl::StrCat("Program(\n",
                        absl::StrJoin(relation_decl_strings, "\n"),
                        absl::StrJoin(outputs_, "\n"),
                        absl::StrJoin(rule_strings, "\n"), ")");
  }

 private:
  std::vector<RelationDeclaration> relation_declarations_;
  std::vector<Rule> rules_;
  std::vector<std::string> outputs_;
};

}  // namespace raksha::ir::datalog

#endif  // SRC_IR_DATALOG_PROGRAM_H_
