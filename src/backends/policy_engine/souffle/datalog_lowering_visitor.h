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

#ifndef SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_LOWERING_VISITOR_H_
#define SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_LOWERING_VISITOR_H_

#include <variant>
#include "src/backends/policy_engine/souffle/raksha_datalog_facts.h"
#include "src/common/logging/logging.h"
#include "src/ir/attributes/attribute.h"
#include "src/ir/attributes/int_attribute.h"
#include "src/ir/attributes/string_attribute.h"
#include "src/ir/datalog/operation.h"
#include "src/ir/ir_traversing_visitor.h"
#include "src/ir/module.h"
#include "src/ir/operator.h"
#include "src/ir/ssa_names.h"


namespace raksha::backends::policy_engine::souffle {

class DatalogLoweringVisitor
    : public ir::IRTraversingVisitor<DatalogLoweringVisitor, std::monostate> {
 public:
  // We currently don't have any owner information when outputting IR. We don't
  // need it yet, really, but we do need to output something.
  static constexpr absl::string_view kDefaultPrincipal = "sql";

  std::monostate PreVisit(const ir::Operation &operation) override;

  const RakshaDatalogFacts &datalog_facts() { return datalog_facts_; }

 private:
  ir::SsaNames ssa_names_;
  RakshaDatalogFacts datalog_facts_;
};

}  // namespace raksha::backends::policy_engine::souffle

#endif  // SRC_BACKENDS_POLICY_ENGINE_SOUFFLE_DATALOG_LOWERING_VISITOR_H_
