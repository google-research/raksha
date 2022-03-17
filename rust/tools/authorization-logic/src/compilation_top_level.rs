/*
 * Copyright 2021 The Raksha Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//! This file has the top level function, `compile`
//! to compile to souffle. This is a separate file and module from
//! souffle-interface because part of the compilation step involces
//! importing and exporting claims. The importing and exporting of claims
//! happens at the AST level rather than an IR and works independently of the
//! fact that the particular choice of solver is datalog. This separation might
//! make it easier to switch to a different solver later. This is also separate
//! from main because "compile" is also used for tests.

use crate::{
    ast::*,
    parsing::astconstructionvisitor,
    signing::{export_assertions, import_assertions},
    souffle::souffle_interface,
    utils::*,
};

use std::fs;

fn source_file_to_ast(input_file_path: &str) -> AstProgram {
    let source = fs::read_to_string(input_file_path)
        .expect(&format!("failed to read {}",input_file_path));
    astconstructionvisitor::parse_program(&source[..])
}

// Make source_file_to_ast publicly visible only in tests
#[cfg(test)]
pub fn source_file_to_ast_test_only(input_file_path: &str) -> AstProgram {
    let resolved_in_file_path = utils::get_resolved_path(&input_file_path);
    source_file_to_ast(&resolved_in_file_path)
}

pub fn compile(input_file_path: &str, output_file_path: &str, decl_skip: &Vec<String>){
    let resolved_in_file = utils::get_resolved_path(&input_file_path);
    let resolved_out_file = utils::get_resolved_path(&output_file_path);
    let prog = source_file_to_ast(&resolved_in_file);
    let prog_with_imports = import_assertions::handle_imports(&prog);
    souffle_interface::ast_to_souffle_file(
        &prog_with_imports,
        &resolved_out_file,
        &Some(decl_skip.to_vec()));
    export_assertions::export_assertions(&prog_with_imports);
}
