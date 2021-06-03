// try blocks are used in antlr4-rust
#![feature(try_blocks)]


// This may be refactored into two crates

// (1) These are about authorization logic
mod parsing;
mod ast;
mod souffle_emitter;
mod datalog_ir;
mod lowering_ast_datalog;
mod souffle_interface;
mod signing;
mod export_signatures;

// (2) these are about graphs
mod graph_ir;
mod low_graph_ir;

// the top level parts might connect them

// TODO: surely this is not the place to list my test modules --
// figure out the right place by using the internet.
// testing (including these here even though they are not
// used in main or below does appear necessary)
mod testing_queries;
mod test_signing;
mod test_export_signatures;

use std::fs;
use crate::parsing::astconstructionvisitor;
use crate::ast::*;
use crate::export_signatures::*;

fn source_file_to_ast(filename: &String, in_dir: &String) -> AstProgram {
    let source = fs::read_to_string(&format!("{}/{}", in_dir, filename))
        .expect("failed to read input in input_to_souffle_file");
    astconstructionvisitor::parse_program(&source[..])
}

fn compile(filename: &String, in_dir: &String, out_dir: &String) {
    let prog = source_file_to_ast(filename, in_dir);
    souffle_interface::ast_to_souffle_file(&prog, filename, out_dir);
    export_signatures::export_assertions(&prog);
}


fn main() {
    // At the moment, the main function does nothing. See
    // testing_queries to understand how this works
}
