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
#ifndef SRC_IR_IR_PRINTER_H_
#define SRC_IR_IR_PRINTER_H_

#include <memory>

#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "src/ir/ir_traversing_visitor.h"
#include "src/ir/module.h"
#include "src/ir/ssa_names.h"
#include "src/ir/value.h"

namespace raksha::ir {

// A visitor that prints the IR.
class IRPrinter : public IRTraversingVisitor<IRPrinter> {
 public:
  template <typename T>
  static void ToString(std::ostream& out, const T& entity, SsaNames& ssa_names) {
    IRPrinter printer(out, ssa_names);
    entity.Accept(printer);
  }

  template <typename T>
  static void ToString(std::ostream& out, const T& entity) {
    SsaNames ssa_names;
    IRPrinter printer(out, ssa_names);
    entity.Accept(printer);
  }

  template <typename T>
  static std::string ToString(const T& entity, SsaNames& ssa_names) {
    std::ostringstream out;
    ToString(out, entity, ssa_names);
    return out.str();
  }

  template <typename T>
  static std::string ToString(const T& entity) {
    std::ostringstream out;
    ToString(out, entity);
    return out.str();
  }

  Unit PreVisit(const Module& module) override {
    out_ << Indent()
         << absl::StreamFormat("module %s {\n",
                               ssa_names_.GetOrCreateID(module));
    IncreaseIndent();
    return Unit();
  }

  Unit PostVisit(const Module& module, Unit result) override {
    DecreaseIndent();
    out_ << Indent()
         << absl::StreamFormat("}  // module %s\n",
                               ssa_names_.GetOrCreateID(module));
    return result;
  }

  Unit PreVisit(const Block& block) override {
    out_ << Indent()
         << absl::StreamFormat("block %s {\n", ssa_names_.GetOrCreateID(block));
    IncreaseIndent();
    return Unit();
  }

  Unit PostVisit(const Block& block, Unit result) override {
    DecreaseIndent();
    out_ << Indent()
         << absl::StreamFormat("}  // block %s\n",
                               ssa_names_.GetOrCreateID(block));
    return result;
  }

  Unit PreVisit(const Operation& operation) override {
    constexpr absl::string_view kOperationFormat = "%s = %s [%s](%s)";
    SsaNames::ID this_ssa_name = ssa_names_.GetOrCreateID(operation);

    // We want the attribute names to print in a stable order. This means that
    // we cannot just print from the attribute map directly. Gather the names
    // into a vector and sort that, then use the order in that vector to print
    // the attributes.
    std::string attributes_string = PrintNamedMapInNameOrder(
        operation.attributes(),
        [](const Attribute& attr) { return attr.ToString(); });

    std::string inputs_string = absl::StrJoin(
        operation.inputs(), ", ", [this](std::string* out, const Value& value) {
          absl::StrAppend(out, value.ToString(ssa_names_));
        });
    out_ << Indent()
         << absl::StreamFormat(kOperationFormat, this_ssa_name,
                               operation.op().name(), attributes_string,
                               inputs_string);

    const Value& v = Value(value::OperationResult(operation, "out"));
    ssa_names_.GetOrCreateID(v);

    if (operation.impl_module()) {
      out_ << " {\n";
      IncreaseIndent();
    } else {
      out_ << "\n";
    }
    return Unit();
  }

  Unit PostVisit(const Operation& operation, Unit result) override {
    if (operation.impl_module()) {
      DecreaseIndent();
      out_ << Indent() << "}\n";
    }
    return result;
  }

  // Returns a pretty-printed map where entries are sorted by the key.
  template <class T, class F>
  static std::string PrintNamedMapInNameOrder(
      const absl::flat_hash_map<std::string, T>& map_to_print,
      F value_pretty_printer) {
    std::vector<absl::string_view> names;
    names.reserve(map_to_print.size());
    for (auto& key_value_pair : map_to_print) {
      names.push_back(key_value_pair.first);
    }
    std::sort(names.begin(), names.end());
    return absl::StrJoin(
        names, ", ", [&](std::string* out, const absl::string_view name) {
          absl::StrAppend(out, name, ": ",
                          value_pretty_printer(map_to_print.at(name)));
        });
  }

 private:
  std::string Indent() { return std::string(indent_ * 2, ' '); }
  void IncreaseIndent() { ++indent_; }
  void DecreaseIndent() { --indent_; }

  IRPrinter(std::ostream& out, SsaNames& ssa_names)
      : out_(out), indent_(0), ssa_names_(ssa_names) {}

  std::ostream& out_;
  int indent_;
  // TODO(#615): Make ssa_names_ const.
  SsaNames& ssa_names_;
};

inline std::string Value::ToString(SsaNames& ssa_names) const {
  return std::visit(
      raksha::utils::overloaded{
          [&ssa_names](const value::Any& any) {
            return any.ToString(ssa_names);
          },
          [&ssa_names](const value::StoredValue& stored_value) {
            return stored_value.ToString(ssa_names);
          },
          [this, &ssa_names](const value::BlockArgument& block_argument) {
            return absl::StrFormat("%s", ssa_names.GetOrCreateID(*this));
          },
          [this, &ssa_names](const value::OperationResult& operation_result) {
            return absl::StrFormat("%s", ssa_names.GetOrCreateID(*this));
          }},
      Value::value_);
}

inline std::ostream& operator<<(std::ostream& out, const Operation& operation) {
  IRPrinter::ToString(out, operation);
  return out;
}

inline std::ostream& operator<<(std::ostream& out, const Block& block) {
  IRPrinter::ToString(out, block);
  return out;
}
inline std::ostream& operator<<(std::ostream& out, const Module& module) {
  IRPrinter::ToString(out, module);
  return out;
}
}  // namespace raksha::ir

#endif  // SRC_IR_IR_PRINTER_H_
