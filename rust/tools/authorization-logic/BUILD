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

load("@rules_rust//rust:rust.bzl", "rust_binary", "rust_library")

licenses(["notice"])

filegroup(
    name = "antlr4rust_jar",
    srcs = ["third_party/antlr4-rust/antlr4rust.jar"]
)

filegroup(
    name = "antlr_gen_mod_rs",
    srcs = ["src/parsing/antlr_gen/mod.rs"]
)

# Generate the parsing-related files using antlr4rust
genrule(
    name = "antlr_gen_srcs",
    srcs = ["src/parsing/AuthLogic.g4"],
    tools = [
        ":antlr4rust_jar",
        ":antlr_gen_mod_rs",
        "@bazel_tools//tools/jdk:current_java_runtime",
        "@bazel_tools//tools/jdk:java",
    ],
    outs = [
       "antlr_gen/authlogiclexer.rs",
       "antlr_gen/authlogiclistener.rs",
       "antlr_gen/authlogicparser.rs",
       "antlr_gen/lib.rs",
    ],
    cmd = "$(execpath @bazel_tools//tools/jdk:java) " +
          "-jar $(location :antlr4rust_jar) " +
          "-Dlanguage=Rust -Xexact-output-dir -o $(RULEDIR)/antlr_gen $< ;" +
          "echo \"#![feature(try_blocks)]\" >  $(RULEDIR)/antlr_gen/lib.rs ;" +
          "cat $(location :antlr_gen_mod_rs) >> $(RULEDIR)/antlr_gen/lib.rs"
)

rust_library(
    name = "antlr_gen",
    srcs = [":antlr_gen_srcs"],
    deps = [
        "//rust/tools/authorization-logic/cargo:antlr_rust",
    ]
)

filegroup(
    name = "lib_sources",
    srcs = [
        "src/ast.rs",
        "src/compilation_top_level.rs",
        "src/lib.rs",
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
)

filegroup(
    name = "test_sources",
    srcs = [
        "src/test/mod.rs",
        "src/test/test_claim_importing.rs",
        "src/test/test_export_signatures.rs",
        "src/test/test_queries.rs",
        "src/test/test_signing.rs",
        "src/test/test_decl_skip.rs",
    ],
)

rust_library(
    name = "authorization_logic_lib",
    srcs = [":lib_sources"],
    rustc_flags = [
        "--cfg=feature=\"bazel_build\"",
    ],
    deps = [
        ":antlr_gen",
        "//rust/tools/authorization-logic/cargo:antlr_rust",
        "//rust/tools/authorization-logic/cargo:bincode",
        "//rust/tools/authorization-logic/cargo:serde",
        "//rust/tools/authorization-logic/cargo:tink_core",
        "//rust/tools/authorization-logic/cargo:tink_signature",
    ],
    crate_type = "staticlib",
)

rust_binary(
    name = "authorization_logic",
    srcs = [
        "src/main.rs",
        ":lib_sources",
        ":test_sources",
    ],
    rustc_flags = [
        "--cfg=feature=\"bazel_build\""
    ],
    deps = [
        ":antlr_gen",
        "//rust/tools/authorization-logic/cargo:antlr_rust",
        "//rust/tools/authorization-logic/cargo:bincode",
        "//rust/tools/authorization-logic/cargo:serde",
        "//rust/tools/authorization-logic/cargo:tink_core",
        "//rust/tools/authorization-logic/cargo:tink_signature",
    ],
)
