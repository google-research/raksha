// try blocks are used in antlr4-rust
#![feature(try_blocks)]
mod parsing;
mod ast;
use crate::parsing::astconstructionvisitor;
use std::env;
use std::fs;
mod souffle_emitter;
use crate::souffle_emitter::*;

fn test_cons_ast(filename: String) {
    let contents = fs::read_to_string(filename)
        .expect("Something went wrong reading the file");
    let prog = astconstructionvisitor::parse_program(&contents[..]);
    println!("parsed\n {:#?}", prog);
}

fn test_emit_souffle(filename: String) {
    let source = fs::read_to_string(filename)
        .expect("Something went wrong reading the file");
    let prog = astconstructionvisitor::parse_program(&source[..]);
    let souffle_code = SouffleEmitter::emit_program(&prog);
    println!("souffle code \n{}", souffle_code);
}

fn main() {
    test_cons_ast(String::from("test_inputs/input_file"));
    test_emit_souffle(String::from("test_inputs/input_file"));
}
