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


// (2) these are about graphs
mod graph_ir;
mod low_graph_ir;

// the top level parts might connect them


// testing (including these here even though they are not
// used in main or below does appear necessary)
mod testing_queries;
mod test_signing;

fn main() {
    // At the moment, the main function does nothing. See 
    // testing_queries to understand how this works
}
