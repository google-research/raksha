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
#include "src/ir/value.h"

namespace raksha::ir {

// A visitor that prints the IR.
class IRPrinter : public IRTraversingVisitor<IRPrinter> {
 public:
  template <typename T>
  static std::string ToString(const T& entity) {
    IRPrinter printer;
    entity.Accept(printer);
    return printer.out.str();
  }

  void PreVisit(const Module& module) override {
    out << Indent()
        << absl::StreamFormat("module m%d {\n",
                              ssa_names_.GetOrCreateID(module));
    IncreaseIndent();
  }

  void PostVisit(const Module& module) override {
    DecreaseIndent();
    out << Indent()
        << absl::StreamFormat("}  // module m%d\n",
                              ssa_names_.GetOrCreateID(module));
  }

  void PreVisit(const Block& block) override {
    out << Indent()
        << absl::StreamFormat("block b%d {\n", ssa_names_.GetOrCreateID(block));
    IncreaseIndent();
  }

  void PostVisit(const Block& block) override {
    DecreaseIndent();
    out << Indent()
        << absl::StreamFormat("}  // block b%d\n",
                              ssa_names_.GetOrCreateID(block));
  }

  void PreVisit(const Operation& operation) override {
    constexpr absl::string_view kOperationFormat = "%%%d = %s [%s](%s)";
    SsaNames::ID this_ssa_name = ssa_names_.GetOrCreateID(operation);

    // We want the attribute names to print in a stable order. This means that
    // we cannot just print from the attribute map directly. Gather the names
    // into a vector and sort that, then use the order in that vector to print
    // the attributes.
    std::string attributes_string = PrintNamedMapInNameOrder(
        operation.attributes(),
        [](const Attribute& attr) { return attr->ToString(); });

    std::string inputs_string = PrintNamedMapInNameOrder(
        operation.inputs(),
        [&](const Value& val) { return val.ToString(ssa_names_); });

    out << Indent()
        << absl::StreamFormat(kOperationFormat, this_ssa_name,
                              operation.op().name(), attributes_string,
                              inputs_string);
    if (operation.impl_module()) {
      out << " {\n";
      IncreaseIndent();
    } else {
      out << "\n";
    }
  }

  void PostVisit(const Operation& operation) override {
    if (operation.impl_module()) {
      DecreaseIndent();
      out << Indent() << "}\n";
    }
  }

 private:
  std::string Indent() { return std::string(indent_ << 1, ' '); }
  void IncreaseIndent() { ++indent_; }
  void DecreaseIndent() { --indent_; }

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

  IRPrinter() : indent_(0) {}

  std::ostringstream out;
  int indent_;
  SsaNames ssa_names_;
};

}  // namespace raksha::ir

#endif  // SRC_IR_IR_PRINTER_H_
