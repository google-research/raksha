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

static ir::datalog::AttributePayload GetPayloadForAttribute(
    ir::Attribute attr, ir::SsaNames &ssa_names) {
  if (intrusive_ptr<const ir::Int64Attribute> int_attr =
          attr.GetIf<ir::Int64Attribute>()) {
    return ir::datalog::NumberAttributePayload(
        ir::datalog::Number(int_attr->value()));
  } else if (intrusive_ptr<const ir::StringAttribute> string_attr =
                 attr.GetIf<ir::StringAttribute>()) {
    return ir::datalog::StringAttributePayload(
        ir::datalog::Symbol(string_attr->value()));
  } else {
    LOG(FATAL) << "Unknown attribute kind.";
  }
  // Unreachable, just to placate compiler.
  return ir::datalog::StringAttributePayload(ir::datalog::Symbol(""));
}

static ir::datalog::Attribute TranslateIrNodeToDatalog(
    std::pair<std::string, ir::Attribute> name_attr_pair,
    ir::SsaNames &ssa_names) {
  return ir::datalog::Attribute(
      ir::datalog::Symbol(std::move(name_attr_pair.first)),
      GetPayloadForAttribute(std::move(name_attr_pair.second), ssa_names));
}

static ir::datalog::Symbol TranslateIrNodeToDatalog(ir::Value value,
                                                    ir::SsaNames &ssa_names) {
  return ir::datalog::Symbol(value.ToString(ssa_names));
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
  constexpr absl::string_view kUnknownPrincipal = "UNKNOWN";
  const ir::Operator &op = operation.op();
  absl::string_view op_name = op.name();

  const ir::ValueList &ir_operand_list = operation.inputs();
  ir::datalog::OperandList operand_list =
      RangeToDatalogList<ir::datalog::OperandList>(
          ir_operand_list.begin(), ir_operand_list.end(), ssa_names_);
  const ir::NamedAttributeMap &ir_attr_map = operation.attributes();
  ir::datalog::AttributeList attribute_list =
      RangeToDatalogList<ir::datalog::AttributeList>(
          ir_attr_map.begin(), ir_attr_map.end(), ssa_names_);

  ir::datalog::Operation datalog_operation(
      ir::datalog::Symbol(kUnknownPrincipal), ir::datalog::Symbol(op_name),
      ir::datalog::Symbol(
          ir::value::OperationResult(operation, kDefaultOutputName)
              .ToString(ssa_names_)),
      std::move(operand_list), std::move(attribute_list));
  datalog_facts_.AddIsOperationFact(
      ir::datalog::IsOperationFact(std::move(datalog_operation)));
}

}  // namespace raksha::backends::policy_engine::souffle
