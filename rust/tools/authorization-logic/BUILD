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

#load("@rules_rust//rust:defs.bzl", "rust_binary", "rust_library", "rust_static_library", "rust_test")
load("@rules_rust//rust:rust.bzl", "rust_binary", "rust_library", "rust_test")

licenses(["notice"])

filegroup(
    name = "antlr_gen_mod_rs",
    srcs = ["src/parsing/antlr_gen/mod.rs"],
)

java_binary(
    name = "antlr4rust",
    main_class = "org.antlr.v4.Tool",
    runtime_deps = ["third_party/antlr4-rust/antlr4rust.jar"],
)

# Generate the parsing-related files using antlr4rust
genrule(
    name = "antlr_gen_srcs",
    srcs = ["src/parsing/AuthLogic.g4"],
    outs = [
        "antlr_gen/authlogiclexer.rs",
        "antlr_gen/authlogiclistener.rs",
        "antlr_gen/authlogicparser.rs",
        "antlr_gen/lib.rs",
    ],
    cmd = "$(location :antlr4rust) " +
          "-Dlanguage=Rust -Xexact-output-dir -o $(RULEDIR)/antlr_gen $< ;" +
          "echo \"#![feature(try_blocks)]\" >  $(RULEDIR)/antlr_gen/lib.rs ;" +
          "cat $(location :antlr_gen_mod_rs) >> $(RULEDIR)/antlr_gen/lib.rs",
    tools = [
        ":antlr4rust",
        ":antlr_gen_mod_rs",
    ],
)

rust_library(
    name = "antlr_gen",
    srcs = [":antlr_gen_srcs"],
    deps = [
        "//rust/tools/authorization-logic/cargo:antlr_rust",
    ],
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
        "src/souffle/universe_translation.rs",
        "src/utils/mod.rs",
        "src/utils/utils.rs",
    ],
)

filegroup(
    name = "test_sources",
    srcs = [
        "src/test/mod.rs",
        "src/test/test_claim_importing.rs",
        "src/test/test_decl_skip.rs",
        "src/test/test_dots_and_quotes.rs",
        "src/test/test_export_signatures.rs",
        "src/test/test_multimic_no_overrides.rs",
        "src/test/test_multimic_overrides.rs",
        "src/test/test_multiverse_handling.rs",
        "src/test/test_negation.rs",
        "src/test/test_num_string_names.rs",
        "src/test/test_queries.rs",
        "src/test/test_relation_declarations.rs",
        "src/test/test_signing.rs",
        "src/test/test_type_error.rs",
    ],
)

rust_library(
    name = "authorization_logic_lib",
    srcs = [
        ":lib_sources",
        ":test_sources",
    ],
    crate_type = "staticlib",
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
)

filegroup(
    name = "test_inputs",
    srcs = glob(["test_inputs/**"]),
)

filegroup(
    name = "raksha_examples",
    srcs = glob(["raksha_examples/**"]),
)

rust_test(
    name = "authorization_logic_test",
    crate = ":authorization_logic_lib",
    data = [
        ":raksha_examples",
        ":test_inputs",
    ],
    rustc_flags = [
        "--cfg=feature=\"bazel_build\"",
    ],
)

rust_binary(
    name = "authorization_logic",
    srcs = [
        "src/main.rs",
        ":lib_sources",
        ":test_sources",
    ],
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
)
