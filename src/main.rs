// try blocks are used in antlr4-rust
#![feature(try_blocks)]

// This may be refactored into two crates

// (1) These are about authorization logic
mod ast;
mod compilation_top_level;
mod parsing;
mod signing;
mod souffle;
mod test;


// (2) these are about graphs
mod graph_ir;
mod low_graph_ir;

// the top level parts might connect them

use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();
    let filename = &args[1];
    let in_dir = &args[2];
    let out_dir = &args[3];
    compilation_top_level::compile(filename, in_dir, out_dir);
    souffle::souffle_interface::run_souffle(
        &format!("{}/{}.dl", out_dir, filename), out_dir);
}
