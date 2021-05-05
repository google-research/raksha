// try blocks are used in antlr4-rust
#![feature(try_blocks)]

mod parsing;
mod ast;
mod datalog_ir;
mod souffle_emitter;
mod lowering_ast_datalog;

use crate::parsing::astconstructionvisitor;
use std::env;
use std::fs;
use crate::souffle_emitter::*;
use crate::lowering_ast_datalog::*;

fn test_cons_ast(filename: &String) {
    let contents = fs::read_to_string(filename)
        .expect("Something went wrong reading the file");
    let prog = astconstructionvisitor::parse_program(&contents[..]);
    println!("ast from {}, \n {:#?}", &filename, prog);
}

fn test_emit_souffle(filename: &String) {
    let source = fs::read_to_string(filename)
        .expect("Something went wrong reading the file");
    let prog = astconstructionvisitor::parse_program(&source[..]);
    let dlir_prog = prog_to_dlir(&prog);
    let souffle_code = SouffleEmitter::emit_program(&dlir_prog);
    println!("souffle code from {}, \n{}", &filename, souffle_code);
}

fn main() {
    test_cons_ast(&String::from("test_inputs/input_file"));
    test_emit_souffle(&String::from("test_inputs/input_file"));
    test_cons_ast(&String::from("test_inputs/test2"));
    test_emit_souffle(&String::from("test_inputs/test2"));
}
