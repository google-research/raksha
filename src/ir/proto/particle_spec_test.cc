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
//-----------------------------------------------------------------------------

#include "src/ir/proto/particle_spec.h"

#include <memory>
#include <optional>
#include <ostream>
#include <sstream>

#include "src/common/testing/gtest.h"
#include "src/common/utils/test/utils.h"
#include "src/ir/ir_printer.h"
#include "src/ir/ir_visitor.h"
#include "src/ir/proto/operators.h"
#include "src/ir/proto/type.h"
#include "src/ir/value.h"
#include "third_party/arcs/proto/manifest.pb.h"

namespace raksha::ir::proto {
namespace {

template <typename C>
const typename C::mapped_type *FindOrNull(const C &c,
                                          const typename C::key_type &key) {
  auto find_result = c.find(key);
  return find_result == c.end() ? nullptr : std::addressof(find_result->second);
}

class ParticleDatalogPrinter {
 public:
  static void ToDatalog(std::ostream &out, IRPrinter &ir_printer,
                        const Block &particle_spec, const Operation &particle) {
    ParticleDatalogPrinter printer(out, ir_printer, particle_spec, particle);
    for (const auto &operation : particle_spec.operations()) {
      printer.ProcessOperation(*operation);
    }
    for (const auto &[name, result] : particle_spec.results()) {
      printer.ProcessResult(name, result);
    }
    for (const auto &[name, input] : particle.inputs()) {
      printer.ProcessInput(name, input);
    }
  }

  void ProcessOperation(const Operation &operation) {
    if (operation.op().name() == arcs::operators::kMerge) {
      PrintMergeOperation(operation);
    }
    // else if (operation.op().name() == arcs::operators::kWriteStorage) {
    //   PrintWriteStorage(operation);
    // }
  }

  void ProcessResult(absl::string_view name, Value value) {
    if (auto any = value.If<value::Any>()) return;
    out_ << "edge(";
    PrintValue(value);
    out_ << ", " << particle_->attributes().at("name")->ToString() << "_"
         << ssa_names_.GetOrCreateID(*particle_) << "." << name << ").\n";
  }

  void ProcessInput(absl::string_view name, Value value) {
    if (particle_spec_->inputs().FindDecl(name) != 0) {
      out_ << "edge(";
      if (const auto *store = value.If<value::StoredValue>()) {
        out_ << store->storage().name();
      } else {
        out_ << value.ToString(ir_printer_.ssa_names());
      }
      out_ << ", " << particle_prefix_ << name << ").\n";
    }
    if (particle_spec_->outputs().FindDecl(name) != 0) {
      out_ << "edge(" << particle_prefix_ << name << ", ";
      if (const auto *store = value.If<value::StoredValue>()) {
        out_ << store->storage().name();
      } else {
        out_ << value.ToString(ir_printer_.ssa_names());
      }
      out_ << ").\n";
    }
  }

 private:
  void PrintMergeOperation(const Operation &merge_op) {
    LOG(WARNING) << "Merge operation " << IRPrinter::ToString(merge_op);
    std::string merge_result_name = absl::StrFormat(
        "%s%%%d.result", particle_prefix_, ssa_names_.GetOrCreateID(merge_op));
    for (const auto &[_, value] : merge_op.inputs()) {
      if (auto any = value.If<value::Any>()) continue;
      out_ << "edge(";
      PrintValue(value);
      out_ << ", " << merge_result_name << ").\n";
    }
  }

  void PrintValue(const Value &value) {
    if (value.If<value::Any>() != nullptr) return;
    out_ << particle_prefix_ << value.ToString(ir_printer_.ssa_names());
    CHECK(particle_->inputs().size() >= 0);
    // if (const auto *arg = value.If<value::BlockArgument>()) {
    //   // CHECK(&arg->block() == &block)
    //   //     << "particle referring to blocks outside of its module";
    //   auto find_result = particle_->inputs().find(arg->name());

    //   CHECK(find_result != particle_->inputs().end())
    //       << "Unable to find input " << arg->name();
    //   PrintValue(find_result->second);
    // } else if (const auto *res = value.If<value::OperationResult>()) {
    //   out_ << absl::StreamFormat(
    //       "%%%d.%s", ssa_names_.GetOrCreateID(res->operation()),
    //       res->name());

    // } else if (const auto *store = value.If<value::StoredValue>()) {
    //   out_ << store->storage().name();
    // } else if (const auto *any = value.If<value::Any>()) {
    //   // Ignore this.
    // }
  }

  ParticleDatalogPrinter(std::ostream &out, IRPrinter &ir_printer,
                         const Block &particle_spec, const Operation &particle)
      : out_(out),
        ir_printer_(ir_printer),
        ssa_names_(ir_printer.ssa_names()),
        particle_spec_(&particle_spec),
        particle_(&particle) {
    CHECK(particle_spec_ != nullptr);
    particle_prefix_ = absl::StrFormat(
        "%s_%d.", particle_->attributes().at("name")->ToString(),
        ssa_names_.GetOrCreateID(*particle_));
  }

  std::ostream &out_;

  IRPrinter &ir_printer_;
  SsaNames &ssa_names_;
  const Block *particle_spec_;
  const Operation *particle_;
  std::string particle_prefix_;
};

class DatalogPrinter : public IRVisitor<DatalogPrinter> {
 public:
  template <typename T>
  static void ToDatalog(std::ostream &out, const T &entity) {
    DatalogPrinter printer(out);
    entity.Accept(printer);
  }

  void Visit(const Module &module) override {
    for (const auto &block : module.blocks()) {
      block->Accept(*this);
    }
  }
  void Visit(const Block &block) override {
    for (const auto &operation : block.operations()) {
      operation->Accept(*this);
    }
  }
  void Visit(const Operation &operation) override {
    if (operation.op().name() == arcs::operators::kParticle) {
      Attribute attr = operation.attributes().at(arcs::kParticleNameAttribute);
      const Block *particle_spec = particle_specs_.at(attr->ToString());
      ParticleDatalogPrinter::ToDatalog(out_, ir_printer_, *particle_spec,
                                        operation);
    } else if (operation.op().name() == arcs::operators::kParticleSpec) {
      const Module &module = *CHECK_NOTNULL(operation.impl_module());
      CHECK(module.blocks().size() == 1);
      const Block &block = *module.blocks().front();
      Attribute attr = operation.attributes().at(arcs::kParticleNameAttribute);
      particle_specs_[attr->ToString()] = &block;
    }
  }

 private:
  DatalogPrinter(std::ostream &out)
      : ir_printer_(out), out_(out), ssa_names_(ir_printer_.ssa_names()) {}
  IRPrinter ir_printer_;
  std::ostream &out_;
  SsaNames &ssa_names_;
  absl::flat_hash_map<std::string, const Block *> particle_specs_;
};

TEST(ParticleSpecTest, TestSomething) {
  auto test_data =
      utils::test::GetTestDataDir("src/ir/proto/test_arcs_proto.binarypb");
  // Parse manifest stream and dump as datalog program.
  std::ifstream manifest_proto_stream(test_data);
  if (!manifest_proto_stream) {
    LOG(ERROR) << "Error reading manifest proto file " << test_data << ":"
               << strerror(errno);
    return;
  }

  arcs::ManifestProto manifest_proto;
  if (!manifest_proto.ParseFromIstream(&manifest_proto_stream)) {
    LOG(ERROR) << "Error parsing the manifest proto " << test_data;
  }
  IRContext context;
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kParticleSpec));
  context.RegisterOperator(std::make_unique<Operator>(arcs::operators::kClaim));
  context.RegisterOperator(std::make_unique<Operator>(arcs::operators::kCheck));
  context.RegisterOperator(std::make_unique<Operator>(arcs::operators::kMerge));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kReadAccessPath));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kUpdateAccessPath));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kParticle));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kWriteStorage));
  context.RegisterOperator(
      std::make_unique<Operator>(arcs::operators::kReadStorage));

  Module global_module;
  BlockBuilder builder;
  absl::flat_hash_map<std::string, const Operation *> particle_specs_;
  for (const arcs::ParticleSpecProto &particle_spec_proto :
       manifest_proto.particle_specs()) {
    const auto &operation =
        ir::proto::Decode(context, builder, particle_spec_proto);
    particle_specs_.insert(
        {particle_spec_proto.name(), std::addressof(operation)});
    LOG(WARNING) << operation;
  }
  for (const arcs::RecipeProto &recipe_proto : manifest_proto.recipes()) {
    // const std::string &recipe_name =
    //     recipe_proto.name().empty()
    //         ? absl::StrCat(generated_recipe_name_prefix, recipe_num++)
    //         : recipe_proto.name();

    // For each Particle, generate the relevant facts for that particle.
    // These fall into two categories: facts that lie within the ParticleSpec
    // that just need to be instantiated for this particular Particle, and
    // edges that connect this Particle to input/output Handles.
    // uint64_t particle_num = 0;
    for (const arcs::HandleProto &handle_proto : recipe_proto.handles()) {
      auto type =
          types::proto::Decode(context.type_factory(), handle_proto.type());
      context.RegisterStorage(
          std::make_unique<Storage>(handle_proto.name(), type));
    }

    for (const arcs::ParticleProto &particle_proto : recipe_proto.particles()) {
      // const Operation *particle_spec = *CHECK_NOTNULL(
      //     FindOrNull(particle_specs_, particle_proto.spec_name()));

      // const Module *module = CHECK_NOTNULL(particle_spec->impl_module());
      // CHECK(module->blocks().size() == 1);
      // const Block &impl = *module->blocks().front();
      NamedValueMap inputs;
      for (const auto &handle_connection_proto : particle_proto.connections()) {
        // if (impl.inputs().FindDecl(handle_connection_proto.name()) ==
        // nullptr) {
        //   LOG(WARNING) << "Skipping " << handle_connection_proto.name();
        //   continue;
        // }
        inputs.insert({handle_connection_proto.name(),
                       Value(value::StoredValue(context.GetStorage(
                           handle_connection_proto.handle())))});
      }
      // const Operation &invocation =
      builder.AddOperation(
          context.GetOperator(arcs::operators::kParticle),
          {{"name", StringAttribute::Create(particle_proto.spec_name())}},
          inputs);
      // for (const auto &handle_connection_proto :
      // particle_proto.connections()) {
      //   if (impl.outputs().FindDecl(handle_connection_proto.name()) ==
      //       nullptr) {
      //     LOG(WARNING) << "Skipping output" <<
      //     handle_connection_proto.name(); continue;
      //   }
      //   builder.AddOperation(
      //       context.GetOperator(arcs::operators::kWriteStorage),
      //       {{"name", StringAttribute::Create(
      //                     context.GetStorage(handle_connection_proto.handle())
      //                         .name())}},
      //       {{"input", Value(value::OperationResult(
      //                      invocation, handle_connection_proto.name()))}});
      // }
    }
    global_module.AddBlock(builder.build());
    LOG(WARNING) << global_module;
    std::ostringstream str_stream;
    DatalogPrinter::ToDatalog(str_stream, global_module);
    LOG(WARNING) << str_stream.str();
  }
}

}  // namespace
}  // namespace raksha::ir::proto
