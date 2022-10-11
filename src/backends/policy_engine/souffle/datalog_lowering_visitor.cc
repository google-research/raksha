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
#include "src/ir/ir_printer.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"
#include "src/ir/ssa_names.h"
#include "src/ir/value.h"

namespace raksha::backends::policy_engine::souffle {

using DatalogOperation = ir::datalog::Operation;
using DatalogSymbol = ir::datalog::Symbol;
using DatalogNumber = ir::datalog::Number;
using DatalogResultList = ir::datalog::ResultList;
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
  uint64_t number_of_op_return_values = op.number_of_return_values();

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

  // Convert to std::vector for sorting (this avoids exposing flat_hash_map
  // ordering to datalog).
  std::vector<std::pair<std::string, ir::Attribute>> attribute_vec(
      ir_attr_map.begin(), ir_attr_map.end());
  std::sort(attribute_vec.begin(), attribute_vec.end(),
            [](auto &left, auto &right) { return left.first > right.first; });
  DatalogAttributeList attribute_list = common::utils::fold(
      attribute_vec, DatalogAttributeList(),
      [&ssa_names](DatalogAttributeList list_so_far,
                   std::pair<std::string, ir::Attribute> name_attr_pair) {
        return DatalogAttributeList(
            DatalogAttribute(std::move(name_attr_pair.first),
                             GetPayloadForAttribute(
                                 std::move(name_attr_pair.second), ssa_names)),
            std::move(list_so_far));
      });


  std::vector<std::string> op_return_values;
  for (uint64_t i = 0; i < number_of_op_return_values; ++i) {
    op_return_values.push_back(ssa_names_.GetOrCreateID(
        ir::Value::MakeOperationResultValue(operation, absl::StrCat(".", i))));
  }
  DatalogResultList result_list = common::utils::rfold(
      op_return_values, DatalogResultList(),
      [](DatalogResultList list_so_far, const std::string &return_value) {
        return DatalogResultList(DatalogSymbol(return_value),
                                 std::move(list_so_far));
      });

  DatalogOperation datalog_operation(
      DatalogSymbol(kDefaultPrincipal), DatalogSymbol(op_name),
      std::move(result_list), std::move(operand_list),
      std::move(attribute_list));

  datalog_facts_.AddIsOperationFact(
      DatalogIsOperationFact(std::move(datalog_operation)));
  return Unit();
}

}  // namespace raksha::backends::policy_engine::souffle
