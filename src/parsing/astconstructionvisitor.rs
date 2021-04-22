// try blocks are used in antlr4-rust
#![feature(try_blocks)]
use crate::parsing::authlogiclexer::*;
use crate::parsing::authlogicparser::*;
use antlr_rust::tree::{ParseTree,ParseTreeVisitor};
use antlr_rust::InputStream;
use antlr_rust::common_token_stream::CommonTokenStream;
use crate::ast::*;
use crate::parsing::authlogicvisitor::AuthLogicVisitor;

pub fn parse_program(prog_text: &str) -> AstProgram {
    let mut lexer = AuthLogicLexer::new(
        InputStream::new(prog_text));
    let token_source = CommonTokenStream::new(lexer);
    let mut parser = AuthLogicParser::new(token_source);
    let parse_result = parser.program().expect("failed to parse!");
    construct_program(&parse_result)
}

fn construct_program(ctx: &ProgramContext) -> AstProgram {
    let mut assertions = Vec::new();
    for assertion_ctx in ctx.assertion_all() {
        assertions.push(construct_assertion(&assertion_ctx));
    }
    AstProgram { assertions }
}

fn construct_assertion(ctx: &AssertionContextAll)
    -> AstAssertion {
        match ctx {
            AssertionContextAll::FactContext(fctx) => {
                construct_fact(fctx)
            }
            AssertionContextAll::HornclauseContext(hctx) => {
                construct_hornclause(hctx)
            }
            _ => {
                panic!("construct_assertion tried to build error");
            }
        }
}

fn construct_fact(ctx: &FactContext) -> AstAssertion {
    let pred_context = ctx.predicate().unwrap();
    let pred = construct_predicate(&pred_context);
    AstAssertion::AstFact { p: pred }
}

fn construct_hornclause(ctx: &HornclauseContext)
    -> AstAssertion {
        let mut rhs = Vec::new();
        let mut some_lhs = None;
        let mut parsed_head = false;
        for pred_ctx in ctx.predicate_all() {
            if(!parsed_head) {
                parsed_head = true;
                some_lhs = Some(construct_predicate(&pred_ctx));
            } else {
                rhs.push(construct_predicate(&pred_ctx));
            }
        }
        let lhs = some_lhs.unwrap();
        AstAssertion::AstCondAssert{ lhs, rhs }
}

fn construct_predicate(ctx: &PredicateContext) -> AstPred {
    let name_ = ctx.ID(0).unwrap().get_text();
    let mut args_ = Vec::new();
    let mut idx = 1;
    while let Some(id) = ctx.ID(idx) {
        args_.push(id.get_text());
        idx += 1;
    }
    AstPred { name: name_, args: args_ }
}
