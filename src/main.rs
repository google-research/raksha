// try blocks are used in antlr4-rust
#![feature(try_blocks)]

mod parsing;
mod ast;
mod datalog_ir;
mod souffle_emitter;
mod lowering_ast_datalog;
mod souffle_interface;
mod testing_queries;

fn main() {
    // The main function is now zenfully nothing.
}
