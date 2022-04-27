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
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/datalog/operation.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::backends::policy_engine::souffle {

using DatalogOperation = ir::datalog::Operation;
using DatalogSymbol = ir::datalog::Symbol;
using DatalogNumber = ir::datalog::Number;
using DatalogAttributeList = ir::datalog::AttributeList;
using DatalogOperandList = ir::datalog::OperandList;
using DatalogIsOperationFact = ir::datalog::IsOperationFact;
using DatalogAttribute = ir::datalog::Attribute;
using DatalogAttributePayload = ir::datalog::AttributePayload;
using DatalogStringAttributePayload = ir::datalog::StringAttributePayload;
using DatalogNumberAttributePayload = ir::datalog::NumberAttributePayload;
using IrOperation = ir::Operation;

static DatalogAttributePayload GetPayloadForAttribute(ir::Attribute attr,
                                                      ir::SsaNames &ssa_names) {
  if (auto int_attr = attr.GetIf<ir::Int64Attribute>()) {
    return DatalogNumberAttributePayload(DatalogNumber(int_attr->value()));
  } else if (auto string_attr = attr.GetIf<ir::StringAttribute>()) {
    return DatalogStringAttributePayload(DatalogSymbol(string_attr->value()));
  }
  LOG(FATAL) << "Unknown attribute kind.";
  // Unreachable, just to placate compiler.
  return DatalogStringAttributePayload(DatalogSymbol(""));
}

static DatalogAttribute TranslateIrNodeToDatalog(
    std::pair<std::string, ir::Attribute> name_attr_pair,
    ir::SsaNames &ssa_names) {
  return DatalogAttribute(
      DatalogSymbol(std::move(name_attr_pair.first)),
      GetPayloadForAttribute(std::move(name_attr_pair.second), ssa_names));
}

static DatalogSymbol TranslateIrNodeToDatalog(ir::Value value,
                                              ir::SsaNames &ssa_names) {
  return DatalogSymbol(value.ToString(ssa_names));
}

// Turn a range of IR nodes in a C++ container into a Datalog-style linked
// list.
template <class ListT, class IrNodeIter>
static ListT RangeToDatalogList(IrNodeIter ir_node_list_begin,
                                IrNodeIter ir_node_list_end,
                                ir::SsaNames &ssa_names) {
  if (ir_node_list_begin == ir_node_list_end) return ListT();
  auto head_in_dl = TranslateIrNodeToDatalog(*ir_node_list_begin, ssa_names);
  return ListT(std::move(head_in_dl),
               RangeToDatalogList<ListT>(++ir_node_list_begin, ir_node_list_end,
                                         ssa_names));
}

void DatalogLoweringVisitor::PreVisit(const ir::Operation &operation) {
  // We currently don't have any owner information when outputting IR. We
  // don't need it yet, really, but we do need to output something.
  const ir::Operator &op = operation.op();
  absl::string_view op_name = op.name();

  const ir::ValueList &ir_operand_list = operation.inputs();
  DatalogOperandList operand_list = RangeToDatalogList<DatalogOperandList>(
      ir_operand_list.begin(), ir_operand_list.end(), ssa_names_);
  const ir::NamedAttributeMap &ir_attr_map = operation.attributes();
  DatalogAttributeList attribute_list =
      RangeToDatalogList<DatalogAttributeList>(ir_attr_map.begin(),
                                               ir_attr_map.end(), ssa_names_);

  DatalogOperation datalog_operation(
      DatalogSymbol(kDefaultPrincipal), DatalogSymbol(op_name),
      DatalogSymbol(ir::value::OperationResult(operation, kDefaultOutputName)
                        .ToString(ssa_names_)),
      std::move(operand_list), std::move(attribute_list));
  datalog_facts_.AddIsOperationFact(
      DatalogIsOperationFact(std::move(datalog_operation)));
}

}  // namespace raksha::backends::policy_engine::souffle
