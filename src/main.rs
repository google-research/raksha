// try blocks are used in antlr4-rust
#![feature(try_blocks)]
mod parsing;
use parsing::printvisitor::*;
use parsing::authlogiclexer::*;
use parsing::authlogicparser::*;
use antlr_rust::InputStream;
use antlr_rust::common_token_stream::CommonTokenStream;
use crate::parsing::authlogicvisitor::AuthLogicVisitor;
use crate::parsing::astconstructionvisitor;

mod ast;

fn test_print() {
    let mut lexer = AuthLogicLexer::new(
        InputStream::new("foo(bar, baz).\r\n"));
    let token_source = CommonTokenStream::new(lexer);
    let mut parser = AuthLogicParser::new(token_source);
    let parse_result = parser.program().expect("failed to parse!");
    let mut pvisitor = PrintVisitor::new();
    pvisitor.visit_program(&parse_result);
}

fn main() {
    test_print();
}
