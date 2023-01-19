//-----------------------------------------------------------------------------
// Copyright 2023 Google LLC
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

// This file describes AST nodes for SQLite to be used in the Raksha
// frontend for SQLite.

#ifndef SRC_FRONTENDS_SQLITE_AST_H_
#define SRC_FRONTENDS_SQLITE_AST_H_

#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "absl/log/check.h"
#include "absl/strings/string_view.h"

namespace raksha::frontends::sqlite {

class LiteralValue;
class MergeExpr;
class Expr;
class Star;
class ResultColumn;
class Select;
class SelectStmt;
class Stmt;
class Program;

// Base class for the different kinds of expressions.
class Expr {
 public:
  // This destructor needs to be public because it gets placed in
  // a vector of unique_ptrs and as a result the destructor
  // needs to be callable not just from the sublasses of Expr,
  // but also from within unique_ptr.
  virtual ~Expr() = default;

 protected:
  Expr(const Expr&) = default;
  Expr(Expr&&) = default;
  Expr& operator=(const Expr&) = default;
  Expr& operator=(Expr&&) = default;
  Expr() = default;
};

/// Another partial solution to MergeExpr that is to be deleted
// if the other one is accepted.
class MergeExpr : public Expr {
 public:
  explicit MergeExpr(std::vector<std::unique_ptr<Expr>> sub_exprs)
      : sub_exprs_(std::move(sub_exprs)) {}

  explicit MergeExpr() = default;

  const Expr& operator[](size_t index) const { return *sub_exprs_.at(index); }

 private:
  std::vector<std::unique_ptr<Expr>> sub_exprs_;
};

// This is a literal expression. Currently, all the kinds of literal values
// are mapped to a single type of AST node which just holds the raw text
// that was parsed for this literal.
class LiteralValue : public Expr {
 public:
  explicit LiteralValue(absl::string_view value) : value_(value) {}
  absl::string_view value() const { return value_; }

 private:
  std::string value_;
};

// ResultColumn can either be: * (star), table_name.*, or an expression
// optionally with an alias.
class ResultColumn {
 public:
  // This destructor needs to be public because it gets placed in
  // a vector of unique_ptrs and as a result the destructor
  // needs to be callable not just from the sublasses of Expr,
  // but also from within unique_ptr.
  virtual ~ResultColumn() = default;

 protected:
  ResultColumn(const ResultColumn&) = default;
  ResultColumn(ResultColumn&&) = default;
  ResultColumn& operator=(const ResultColumn&) = default;
  ResultColumn& operator=(ResultColumn&&) = default;
  ResultColumn() = default;
};

// This class represents either "*" (any) in a ResultColumn
// or it represents "table_name.*". When the table_name is
// empty, it represents '*', and it represents the other
// grammar alternative if it is non-empty.
// From the antlr grammar, it corresponds to two of the alternatives
// for `result_column`.
class TableStar : public ResultColumn {
 public:
  TableStar(std::optional<absl::string_view> table_name)
      : table_name_(table_name) {}
  std::optional<absl::string_view> table_name() const { return table_name_; }

 private:
  std::optional<std::string> table_name_;
};

// Base class for different kinds of Scan operations (e.g. SELECT, WHERE, ...)
class Scan {
 public:
  // The destructor needs to be public to allow it to be called
  // from within unique_ptr
  virtual ~Scan() = default;

 protected:
  Scan(const Scan&) = default;
  Scan(Scan&&) = default;
  Scan& operator=(const Scan&) = default;
  Scan& operator=(Scan&&) = default;
  Scan() = default;
};

class TableName : public Scan {
 public:
  explicit TableName(absl::string_view table_name) : table_name_(table_name) {}
  absl::string_view table_name() const { return table_name_; }

 private:
  std::string table_name_;
};

// TODO add other subclasses of SelectOperation to implement
// WHERE, GROUP_BY, HAVING, and other components as we need them

// This is the AST node for a SELECT statment
class Select : public Scan {
 public:
  explicit Select(bool is_distinct,
                  std::vector<std::unique_ptr<ResultColumn>> result_columns,
                  std::unique_ptr<Scan> input_scan)
      : is_distinct_(is_distinct),
        result_columns_(std::move(result_columns)),
        input_scan_(std::move(input_scan)) {}
  bool is_distinct() const { return is_distinct_; }
  bool is_all() const { return !is_distinct_; }
  const ResultColumn& operator[](size_t index) const {
    return *result_columns_.at(index);
  }

  const Scan& input_scan() const { return *input_scan_; }

 private:
  bool is_distinct_;
  std::vector<std::unique_ptr<ResultColumn>> result_columns_;
  std::unique_ptr<Scan> input_scan_;
};

// Note: In the Antlr grammar for SQLite, programs are actually
// lists of stmt_lists which are lists of stmts. In this AST node,
// the lists of stmt_lists are just flattened into one list of stmts.
class Program {
 public:
  explicit Program(std::vector<std::unique_ptr<Scan>> stmt_list)
      : stmt_list_(std::move(stmt_list)) {}
  explicit Program() = default;

  const Scan& operator[](size_t index) const {
    CHECK(index < stmt_list_.size())
        << "Statement index in a program is out of bounds";
    return *stmt_list_.at(index);
  }

 private:
  std::vector<std::unique_ptr<Scan>> stmt_list_;
};

}  // namespace raksha::frontends::sqlite

#endif  // SRC_FRONTENDS_SQLITE_AST_H_
