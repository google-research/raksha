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

#include "src/backends/policy_engine/souffle/datalog_lowering_visitor.h"

#include "src/common/logging/logging.h"
#include "src/common/utils/fold.h"
#include "src/frontends/sql/decoder_context.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/float_attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/datalog/operation.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"
#include "src/ir/ssa_names.h"
#include "src/ir/ir_printer.h"

namespace raksha::backends::policy_engine::souffle {

using DatalogOperation = ir::datalog::Operation;
using DatalogSymbol = ir::datalog::Symbol;
using DatalogNumber = ir::datalog::Number;
using DatalogAttributeList = ir::datalog::AttributeList;
using DatalogOperandList = ir::datalog::OperandList;
using DatalogIsOperationFact = ir::datalog::IsOperationFact;
using DatalogAttribute = ir::datalog::Attribute;
using DatalogAttributePayload = ir::datalog::AttributePayload;

static DatalogAttributePayload GetPayloadForAttribute(ir::Attribute attr,
                                                      ir::SsaNames &ssa_names) {
  if (auto float_attr = attr.GetIf<ir::FloatAttribute>()) {
    return DatalogAttribute::Float(float_attr->value());
  } else if (auto int_attr = attr.GetIf<ir::Int64Attribute>()) {
    return DatalogAttribute::Number(int_attr->value());
  } else if (auto string_attr = attr.GetIf<ir::StringAttribute>()) {
    return DatalogAttribute::String(string_attr->value());
  }
  LOG(FATAL) << "Unknown attribute kind.";
  // Unreachable, just to placate compiler.
  return DatalogAttribute::String("");
}

Unit DatalogLoweringVisitor::PreVisit(const ir::Operation &operation) {
  const ir::Operator &op = operation.op();
  absl::string_view op_name = op.name();

  // Introduce a local to make capturing this field in lambdas easier.
  ir::SsaNames &ssa_names = ssa_names_;

  // Convert each operand into an `AccessPath` `Symbol` and put it in a
  // `DatalogOperandList` to send to Souffle.
  const ir::ValueList &ir_operand_list = operation.inputs();
  DatalogOperandList operand_list = common::utils::rfold(
      ir_operand_list, DatalogOperandList(),
      [&ssa_names](DatalogOperandList list_so_far, const ir::Value &value) {
        return DatalogOperandList(DatalogSymbol(value.ToString(ssa_names)),
                                  std::move(list_so_far));
      });

  // Convert each `Attribute` to the analogous record in datalog and put into an
  // `AttributeList`.
  const ir::NamedAttributeMap &ir_attr_map = operation.attributes();
  DatalogAttributeList attribute_list = common::utils::fold(
      ir_attr_map, DatalogAttributeList(),
      [&ssa_names](DatalogAttributeList list_so_far,
                   std::pair<std::string, ir::Attribute> name_attr_pair) {
        return DatalogAttributeList(
            DatalogAttribute(std::move(name_attr_pair.first),
                             GetPayloadForAttribute(
                                 std::move(name_attr_pair.second), ssa_names)),
            std::move(list_so_far));
      });
  DatalogOperation datalog_operation(
      DatalogSymbol(kDefaultPrincipal), DatalogSymbol(op_name),
      DatalogSymbol(ssa_names_.GetOrCreateID(operation)),
      std::move(operand_list), std::move(attribute_list));
  ssa_names_.GetOrCreateID(ir::Value(ir::value::OperationResult(
      operation, frontends::sql::DecoderContext::kDefaultOutputName)));
  datalog_facts_.AddIsOperationFact(
      DatalogIsOperationFact(std::move(datalog_operation)));
  return Unit();
}

}  // namespace raksha::backends::policy_engine::souffle
