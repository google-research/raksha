// try blocks are used in antlr4-rust
#![feature(try_blocks)]
use crate::parsing::antlr_gen::authlogiclexer::*;
use crate::parsing::antlr_gen::authlogicparser::*;
use antlr_rust::tree::{ParseTree,ParseTreeVisitor};
use antlr_rust::InputStream;
use antlr_rust::common_token_stream::CommonTokenStream;
use crate::ast::*;
// use crate::parsing::authlogicvisitor::AuthLogicVisitor;

// parse_program is the only public interface to the parser. It produces an AST
// tree rooted with a program node when given the textual representation of a
// program.
pub fn parse_program(prog_text: &str) -> AstProgram {
    let mut lexer = AuthLogicLexer::new(
        InputStream::new(prog_text));
    let token_source = CommonTokenStream::new(lexer);
    let mut parser = AuthLogicParser::new(token_source);
    let parse_result = parser.program().expect("failed to parse!");
    construct_program(&parse_result)
}

fn construct_principal(ctx: &PrincipalContext) -> AstPrincipal {
    let name_ = ctx.ID().unwrap().get_text();
    AstPrincipal { name: name_ }
}

fn construct_predicate(ctx: &PredicateContext) -> AstPredicate {
    let name_ = ctx.ID(0).unwrap().get_text();
    let mut args_ = Vec::new();
    let mut idx = 1;
    while let Some(id) = ctx.ID(idx) {
        args_.push(id.get_text());
        idx += 1;
    }
    AstPredicate { name: name_, args: args_ }
}

fn construct_verbphrase(ctx: &VerbphraseContextAll) -> AstVerbPhrase {
    match ctx {
        VerbphraseContextAll::PredphraseContext(pctx) => {
            construct_predphrase(pctx)
        }
        VerbphraseContextAll::ActsAsPhraseContext(actx) => {
            construct_actsasphrase(actx)
        }
        _ => {
            panic!("construct_verbphrase tried to build error");
        }
    }
}

fn construct_predphrase(ctx: &PredphraseContext) -> AstVerbPhrase {
    let pred_context = ctx.predicate().unwrap();
    let pred = construct_predicate(&pred_context);
    AstVerbPhrase::AstPredPhrase { p: pred }
}

fn construct_actsasphrase(ctx: &ActsAsPhraseContext) -> AstVerbPhrase {
    let prin_context = ctx.principal().unwrap();
    let prin = construct_principal(&prin_context);
    AstVerbPhrase::AstCanActPhrase { p: prin }
}

fn construct_flat_fact(ctx: &FlatFactContextAll) -> AstFlatFact {
    match ctx {
        FlatFactContextAll::PrinFactContext(fctx) => {
            construct_prin_fact(fctx)
        }
        FlatFactContextAll::PredFactContext(pctx) => {
            construct_pred_fact(pctx)
        }
        _ => {
            panic!("construct_flat_fact tried to build error");
        }
    }
}

fn construct_prin_fact(ctx: &PrinFactContext) -> AstFlatFact {
    let prin = construct_principal(&ctx.principal().unwrap());
    let verbphrase = construct_verbphrase(&ctx.verbphrase().unwrap());
    AstFlatFact::AstPrinFact { p: prin, v: verbphrase }
}

fn construct_pred_fact(ctx: &PredFactContext) -> AstFlatFact {
    let pred = construct_predicate(&ctx.predicate().unwrap());
    AstFlatFact::AstPredFact { p: pred }
}

fn construct_fact(ctx: &FactContextAll) -> AstFact {
    match ctx {
        FactContextAll::FlatFactFactContext(fctx) => {
            construct_flat_fact_fact(fctx)
        }
        FactContextAll::CanSayFactContext(sctx) => {
            construct_can_say_fact(sctx)
        }
        _ => {
            panic!("construct_fact tried to build error");
        }
    }
}

fn construct_flat_fact_fact(ctx: &FlatFactFactContext) -> AstFact {
    let flat = construct_flat_fact(&ctx.flatFact().unwrap());
    AstFact::AstFlatFactFact { f: flat }
}

fn construct_can_say_fact(ctx: &CanSayFactContext) -> AstFact {
    let prin = construct_principal(&ctx.principal().unwrap());
    let fact = construct_fact(&ctx.fact().unwrap());
    AstFact::AstCanSayFact { p: prin, f: Box::new(fact) }
}

fn construct_assertion(ctx: &AssertionContextAll)
    -> AstAssertion {
        match ctx {
            AssertionContextAll::FactAssertionContext(fctx) => {
                construct_fact_assertion(fctx)
            }
            AssertionContextAll::HornClauseAssertionContext(hctx) => {
                construct_hornclause(hctx)
            }
            _ => {
                panic!("construct_assertion tried to build error");
            }
        }
}

fn construct_fact_assertion(ctx: &FactAssertionContext) -> AstAssertion {
    let fact = construct_fact(&ctx.fact().unwrap());
    AstAssertion::AstFactAssertion { f: fact }
}

fn construct_hornclause(ctx: &HornClauseAssertionContext)
    -> AstAssertion {
        let lhs = construct_fact(&ctx.fact().unwrap());
        let mut rhs = Vec::new();
        for flat_fact_ctx in ctx.flatFact_all() {
                rhs.push(construct_flat_fact(&flat_fact_ctx));
        }
        AstAssertion::AstCondAssertion { lhs, rhs }
}

fn construct_says_assertion(ctx: &SaysAssertionContext)
    -> AstSaysAssertion {
        let prin_ = construct_principal(&ctx.principal().unwrap());
        let assertion_ = construct_assertion(&ctx.assertion().unwrap());
        AstSaysAssertion { prin: prin_, assertion: assertion_ }
}

fn construct_program(ctx: &ProgramContext) -> AstProgram {
    let mut assertions = Vec::new();
    for assertion_ctx in ctx.saysAssertion_all() {
        assertions.push(construct_says_assertion(&assertion_ctx));
    }
    AstProgram { assertions }
}
