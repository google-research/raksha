#ifndef SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_LOWERING_VISITOR_H_
#define SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_LOWERING_VISITOR_H_

#include "src/common/logging/logging.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/datalog/operation.h"
#include "src/ir/ir_visitor.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"
#include "src/ir/ir_traversing_visitor.h"

namespace raksha::backends::policy_engine::souffle {

// A class containing the Datalog facts produced by the IR translator in
// structured (ie, C++ objects, not strings) form.
class RakshaDatalogFacts {
 public:
  void AddIsOperationFact(ir::datalog::IsOperationFact fact) {
    is_operation_facts_.push_back(std::move(fact));
  }

  std::string ToDatalogString() const {
    return absl::StrJoin(
        is_operation_facts_, "\n",
        [](std::string *out, const ir::datalog::IsOperationFact &arg) {
          absl::StrAppend(out, arg.ToDatalogString());
        });
  }

 private:
  std::vector<ir::datalog::IsOperationFact> is_operation_facts_;
};

 class DatalogLoweringVisitor : public ir::IRTraversingVisitor<DatalogLoweringVisitor> {
 public:
  // TODO: Dedup this from `DecoderContext` in the SQL frontend.
  static constexpr absl::string_view kDefaultOutputName = "out";
  DatalogLoweringVisitor(ir::SsaNames &ssa_names) : ssa_names_(ssa_names) {}
  virtual ~DatalogLoweringVisitor() {}

  void PreVisit(const ir::Operation &operation) override {
    // We currently don't have any owner information when outputting IR. We
    // don't need it yet, really, but we do need to output something.
    constexpr absl::string_view kUnknownPrincipal = "UNKNOWN";
    const ir::Operator &op = operation.op();
    absl::string_view op_name = op.name();

    const ir::ValueList &ir_operand_list = operation.inputs();
    ir::datalog::OperandList operand_list =
        RangeToDatalogList<ir::datalog::OperandList>(ir_operand_list.begin(),
                                                     ir_operand_list.end());
    const ir::NamedAttributeMap &ir_attr_map = operation.attributes();
    ir::datalog::AttributeList attribute_list =
        RangeToDatalogList<ir::datalog::AttributeList>(ir_attr_map.begin(),
                                                       ir_attr_map.end());

    ir::datalog::Operation datalog_operation(
        ir::datalog::Symbol(kUnknownPrincipal), ir::datalog::Symbol(op_name),
        ir::datalog::Symbol(
            ir::value::OperationResult(operation, kDefaultOutputName)
                .ToString(ssa_names_)),
        std::move(operand_list), std::move(attribute_list));
    datalog_facts_.AddIsOperationFact(
        ir::datalog::IsOperationFact(std::move(datalog_operation)));
  }

  const RakshaDatalogFacts &datalog_facts() { return datalog_facts_; }

 private:
  ir::datalog::AttributePayload GetPayloadForAttribute(
      ir::Attribute attr) const {
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

  ir::datalog::Attribute TranslateIrNodeToDatalog(
      std::pair<std::string, ir::Attribute> name_attr_pair) {
    return ir::datalog::Attribute(
        ir::datalog::Symbol(std::move(name_attr_pair.first)),
        GetPayloadForAttribute(std::move(name_attr_pair.second)));
  }

  ir::datalog::Symbol TranslateIrNodeToDatalog(ir::Value value) {
    return ir::datalog::Symbol(value.ToString(ssa_names_));
  }

  // Turn a range of IR nodes in a C++ container into a Datalog-style linked
  // list.
  template <class ListT, class IrNodeIter>
  ListT RangeToDatalogList(IrNodeIter ir_node_list_begin,
                           IrNodeIter ir_node_list_end) {
    if (ir_node_list_begin == ir_node_list_end) return ListT();
    auto head_in_dl = TranslateIrNodeToDatalog(*ir_node_list_begin);
    return ListT(
        std::move(head_in_dl),
        RangeToDatalogList<ListT>(++ir_node_list_begin, ir_node_list_end));
  }

  ir::SsaNames &ssa_names_;
  RakshaDatalogFacts datalog_facts_;
};

}  // namespace raksha::backends::policy_engine::souffle

#endif  // SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_LOWERING_VISITOR_H_
