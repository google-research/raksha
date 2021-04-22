// try blocks are used in antlr4-rust
#![feature(try_blocks)]
mod parsing;
use crate::parsing::astconstructionvisitor;

mod ast;


fn test_cons_ast2() {
    let prog = astconstructionvisitor::parse_program(
        "foo(bar, baz) :- other(thing).\r\n");
    println!("parsed\n {:#?}", prog);
}

fn main() {
    test_cons_ast2();
}
