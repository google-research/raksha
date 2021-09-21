#-------------------------------------------------------------------------------
# Copyright 2021 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#-------------------------------------------------------------------------------
package(default_visibility = ["//visibility:public"])

load("@rules_rust//rust:rust.bzl", "rust_binary")

licenses(["notice"])

rust_binary(
    name = "authorization_logic",
    srcs = [
        "src/ast.rs",
        "src/compilation_top_level.rs",
        "src/main.rs",
        "src/parsing/antlr_gen/authlogiclexer.rs",
        "src/parsing/antlr_gen/authlogiclistener.rs",
        "src/parsing/antlr_gen/authlogicparser.rs",
        "src/parsing/antlr_gen/mod.rs",
        "src/parsing/astconstructionvisitor.rs",
        "src/parsing/mod.rs",
        "src/signing/export_assertions.rs",
        "src/signing/import_assertions.rs",
        "src/signing/mod.rs",
        "src/signing/tink_interface.rs",
        "src/souffle/datalog_ir.rs",
        "src/souffle/lowering_ast_datalog.rs",
        "src/souffle/mod.rs",
        "src/souffle/souffle_emitter.rs",
        "src/souffle/souffle_interface.rs",
    ],
    deps = [
        "//rust/tools/authorization-logic/cargo:antlr_rust",
        "//rust/tools/authorization-logic/cargo:bincode",
        "//rust/tools/authorization-logic/cargo:serde",
        "//rust/tools/authorization-logic/cargo:tink_core",
        "//rust/tools/authorization-logic/cargo:tink_signature",
    ],
)
