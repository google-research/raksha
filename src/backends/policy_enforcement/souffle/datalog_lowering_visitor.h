#ifndef SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_DATALOG_LOWERING_VISITOR_H_
#define SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_DATALOG_LOWERING_VISITOR_H_

#include "external/glog/src/glog/log_severity.h"
#include "src/backends/policy_enforcement/souffle/operation.h"
#include "src/common/logging/logging.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/ir_visitor.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"

namespace raksha::backends::policy_enforcement::souffle {

// A class containing the Datalog facts produced by the IR translator in
// structured (ie, C++ objects, not strings) form.
class RakshaDatalogFacts {
 public:
  void AddIsOperationFact(IsOperationFact fact) {
    is_operation_facts_.push_back(std::move(fact));
  }

  std::string ToDatalogString() const {
    return absl::StrJoin(is_operation_facts_, "\n");
  }

 private:
  std::vector<IsOperationFact> is_operation_facts_;
};

class DatalogLoweringVisitor : public ir::IRVisitor<DatalogLoweringVisitor> {
 public:
  // TODO: Dedup this from `DecoderContext` in the SQL frontend.
  static constexpr absl::string_view kDefaultOutputName = "out";
  DatalogLoweringVisitor(ir::SsaNames &ssa_names) : ssa_names_(ssa_names) {}
  virtual ~DatalogLoweringVisitor() {}

  void Visit(const ir::Block &) override {}

  void Visit(const ir::Module &) override {}

  void Visit(const ir::Operation &operation) override {
    // We currently don't have any owner information when outputting IR. We
    // don't need it yet, really, but we do need to output something.
    constexpr absl::string_view kUnknownPrincipal = "UNKNOWN";
    const ir::Operator &op = operation.op();
    absl::string_view op_name = op.name();

    const ir::ValueList &ir_operand_list = operation.inputs();
    OperandList operand_list = RangeToDatalogList<OperandList>(
        ir_operand_list.begin(), ir_operand_list.end());
    const ir::NamedAttributeMap &ir_attr_map = operation.attributes();
    AttributeList attribute_list = RangeToDatalogList<AttributeList>(
        ir_attr_map.begin(), ir_attr_map.end());

    Operation datalog_operation = Operation(
        Symbol(kUnknownPrincipal), Symbol(op_name),
        Symbol(ir::value::OperationResult(operation, kDefaultOutputName)
                   .ToString(ssa_names_)),
        std::move(operand_list), std::move(attribute_list));
    datalog_facts_.AddIsOperationFact(
        IsOperationFact(std::move(datalog_operation)));
  }

  RakshaDatalogFacts datalog_facts() { return datalog_facts_; }

 private:
  AttributePayload GetPayloadForAttribute(ir::Attribute attr) const {
    if (intrusive_ptr<const ir::Int64Attribute> int_attr =
            attr.GetIf<ir::Int64Attribute>()) {
      return NumberAttributePayload(Number(int_attr->value()));
    } else if (intrusive_ptr<const ir::StringAttribute> string_attr =
                   attr.GetIf<ir::StringAttribute>()) {
      return StringAttributePayload(Symbol(string_attr->value()));
    } else {
      LOG(FATAL) << "Unknown attribute kind.";
    }
  }

  Attribute TranslateIrNodeToDatalog(
      std::pair<std::string, ir::Attribute> name_attr_pair) {
    const auto &[attr_name, attr] = name_attr_pair;
  }

  Symbol TranslateIrNodeToDatalog(ir::Value value) {
    return Symbol(value.ToString(ssa_names_));
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

}  // namespace raksha::backends::policy_enforcement::souffle

#endif  // SRC_BACKENDS_POLICY_ENFORCEMENT_SOUFFLE_DATALOG_LOWERING_VISITOR_H_
