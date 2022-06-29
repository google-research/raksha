/*
 * Copyright 2021 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

use crate::{
    ast::*,
};

#[cfg(feature = "bazel_build")]
use antlr_gen::{authlogiclexer::*, authlogicparser::*};

#[cfg(not(feature = "bazel_build"))]
use crate::{
    parsing::antlr_gen::{authlogiclexer::*, authlogicparser::*},
};

use antlr_rust::{common_token_stream::CommonTokenStream, tree::ParseTree, InputStream};

/// This function produces an abstract syntax tree (AST) rooted with a program node when given the
/// textual representation of a program.
pub fn parse_program(prog_text: &str) -> AstProgram {
    let lexer = AuthLogicLexer::new(InputStream::new(prog_text));
    let token_source = CommonTokenStream::new(lexer);
    let mut parser = AuthLogicParser::new(token_source);
    let parse_result = parser.program().expect("failed to parse!");
    construct_program(&parse_result)
}


fn construct_principal(ctx: &PrincipalContext) -> AstPrincipal {
    let name_ = ctx.id().unwrap().get_text();
    AstPrincipal { name: name_ }
}

fn construct_predicate(ctx: &PredicateContext) -> AstPredicate {
    let sign_ = match ctx.NEG() {
        Some(_) => Sign::Negated,
        None => Sign::Positive 
    };
    let name_ = ctx.id().unwrap().get_text();
    let args_ = (&ctx).pred_arg_all()
        .iter()
        .map(|arg_ctx| arg_ctx.get_text())
        .collect();
    AstPredicate {
        sign: sign_,
        name: name_,
        args: args_,
    }
}

fn construct_verbphrase(ctx: &VerbphraseContextAll) -> AstVerbPhrase {
    match ctx {
        VerbphraseContextAll::PredphraseContext(pctx) => construct_predphrase(pctx),
        VerbphraseContextAll::ActsAsPhraseContext(actx) => construct_actsasphrase(actx),
        _ => { panic!("construct_verbphrase tried to build error"); }
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
        FlatFactContextAll::PrinFactContext(fctx) => construct_prin_fact(fctx),
        FlatFactContextAll::PredFactContext(pctx) => construct_pred_fact(pctx),
        _ => { panic!("construct_flat_fact tried to build error"); }
    }
}

fn construct_prin_fact(ctx: &PrinFactContext) -> AstFlatFact {
    let prin = construct_principal(&ctx.principal().unwrap());
    let verbphrase = construct_verbphrase(&ctx.verbphrase().unwrap());
    AstFlatFact::AstPrinFact {
        p: prin,
        v: verbphrase,
    }
}

fn construct_pred_fact(ctx: &PredFactContext) -> AstFlatFact {
    let pred = construct_predicate(&ctx.predicate().unwrap());
    AstFlatFact::AstPredFact { p: pred }
}

fn construct_fact(ctx: &FactContextAll) -> AstFact {
    match ctx {
        FactContextAll::FlatFactFactContext(fctx) => construct_flat_fact_fact(fctx),
        FactContextAll::CanSayFactContext(sctx) => construct_can_say_fact(sctx),
        _ => { panic!("construct_fact tried to build error"); }
    }
}

fn construct_flat_fact_fact(ctx: &FlatFactFactContext) -> AstFact {
    let flat = construct_flat_fact(&ctx.flatFact().unwrap());
    AstFact::AstFlatFactFact { f: flat }
}

fn construct_can_say_fact(ctx: &CanSayFactContext) -> AstFact {
    let prin = construct_principal(&ctx.principal().unwrap());
    let fact = construct_fact(&ctx.fact().unwrap());
    AstFact::AstCanSayFact {
        p: prin,
        f: Box::new(fact),
    }
}

fn construct_binop(ctx: &BinopContextAll) -> AstComparisonOperator {
    match ctx {
        BinopContextAll::LtbinopContext(_) => AstComparisonOperator::LessThan,
        BinopContextAll::GrbinopContext(_) => AstComparisonOperator::GreaterThan,
        BinopContextAll::EqbinopContext(_) => AstComparisonOperator::Equals,
        BinopContextAll::NebinopContext(_) => AstComparisonOperator::NotEquals,
        BinopContextAll::LeqbinopContext(_) => AstComparisonOperator::LessOrEquals,
        BinopContextAll::GeqbinopContext(_) => AstComparisonOperator::GreaterOrEquals,
        _ => { panic!("construct_binop tried to build error"); }
    }
}

fn construct_rvalue(ctx: &RvalueContextAll) -> AstRValue {
    match ctx {
        RvalueContextAll::FlatFactRvalueContext(ffctx) => {
            AstRValue::FlatFactRValue { 
                flat_fact: construct_flat_fact(&ffctx.flatFact().unwrap())
            }
        },
        RvalueContextAll::BinopRvalueContext(bctx) => {
            AstRValue::ArithCompareRValue {
                arith_comp: AstArithmeticComparison {
                    lnum: bctx.pred_arg(0).unwrap().get_text(),
                    op: construct_binop(&bctx.binop().unwrap()),
                    rnum: bctx.pred_arg(1).unwrap().get_text()
                }
            }
        }
        _ => { panic!("construct_rvalue tried to build error"); }
    }
}

fn construct_assertion(ctx: &AssertionContextAll) -> AstAssertion {
    match ctx {
        AssertionContextAll::FactAssertionContext(fctx) => construct_fact_assertion(fctx),
        AssertionContextAll::HornClauseAssertionContext(hctx) => construct_hornclause(hctx),
        _ => { panic!("construct_assertion tried to build error"); }
    }
}

fn construct_fact_assertion(ctx: &FactAssertionContext) -> AstAssertion {
    let fact = construct_fact(&ctx.fact().unwrap());
    AstAssertion::AstFactAssertion { f: fact }
}

fn construct_hornclause(ctx: &HornClauseAssertionContext) -> AstAssertion {
    let lhs = construct_fact(&ctx.fact().unwrap());
    let rhs = ctx.rvalue_all()
        .iter()
        .map(|rvalue_ctx| construct_rvalue(&rvalue_ctx))
        .collect();
    AstAssertion::AstCondAssertion { lhs, rhs }
}

fn construct_says_assertion(ctx: &SaysAssertionContextAll) -> AstSaysAssertion {
    match ctx {
        SaysAssertionContextAll::SaysSingleContext(ctx_prime) => {
            let prin = construct_principal(&ctx_prime.principal().unwrap());
            let assertions = vec![construct_assertion(&ctx_prime.assertion().unwrap())];
            let export_file = ctx_prime.id().map(|p| p.get_text().replace("\"", ""));
            AstSaysAssertion {
                prin,
                assertions,
                export_file,
            }
        }
        SaysAssertionContextAll::SaysMultiContext(ctx_prime) => {
            let prin = construct_principal(&ctx_prime.principal().unwrap());
            let assertions: Vec<_> = (&ctx_prime)
                .assertion_all()
                .iter()
                .map(|x| construct_assertion(x))
                .collect();
            let export_file = ctx_prime.id().map(|p| p.get_text().replace("\"", ""));
            AstSaysAssertion {
                prin,
                assertions,
                export_file,
            }
        }
        _ => { panic!("construct_says_assertion tried to build Error()"); }
    }
}

fn construct_query(ctx: &QueryContext) -> AstQuery {
    let name = ctx.id().unwrap().get_text();
    let principal = construct_principal(&ctx.principal().unwrap());
    let fact = construct_fact(&ctx.fact().unwrap());
    AstQuery {
        name,
        principal,
        fact,
    }
}

fn construct_keybinding(ctx: &KeyBindContextAll) -> AstKeybind {
    match ctx {
        KeyBindContextAll::BindprivContext(ctx_prime) => AstKeybind {
            filename: ctx_prime.id().unwrap().get_text().replace("\"", ""),
            principal: construct_principal(&ctx_prime.principal().unwrap()),
            is_pub: false,
        },
        KeyBindContextAll::BindpubContext(ctx_prime) => AstKeybind {
            filename: ctx_prime.id().unwrap().get_text().replace("\"", ""),
            principal: construct_principal(&ctx_prime.principal().unwrap()),
            is_pub: true,
        },
        _ => { panic!("construct_keybinding tried to build Error()"); }
    }
}

fn construct_import(ctx: &ImportAssertionContext) -> AstImport {
    AstImport {
        principal: construct_principal(&ctx.principal().unwrap()),
        filename: ctx.id().unwrap().get_text().replace("\"", ""),
    }
}

fn construct_type(ctx: &AuthLogicTypeContextAll) -> AstType {
    match ctx {
        AuthLogicTypeContextAll::NumberTypeContext(_ctx_prime) =>
            AstType::NumberType,
        AuthLogicTypeContextAll::PrincipalTypeContext(_ctx_prime) =>
            AstType::PrincipalType,
        AuthLogicTypeContextAll::CustomTypeContext(ctx_prime) => {
            AstType::CustomType { type_name: ctx_prime.id().unwrap().get_text() }
        }
        _ => { panic!("construct_type tried to build error"); }
    }
}

fn construct_relation_declaration(ctx: &RelationDeclarationContext) -> 
        AstRelationDeclaration {
    let predicate_name_ = ctx.id(0).unwrap().get_text();
    // Note that ID_all() in the generated antlr-rust code is buggy,
    // (because all {LEX_RULE}_all() generations are buggy)
    // so rather than using a more idomatic iterator, "while Some(...)" is
    // used to populate this vector.
    let mut arg_names = Vec::new();
    let mut idx = 1;
    while let Some(param_name) = ctx.id(idx) {
        arg_names.push(param_name.get_text());
        idx += 1;
    }
    let types : Vec<AstType> = ctx.authLogicType_all()
        .iter()
        .map(|s| construct_type(s))
        .collect();
    let is_attribute_ = match ctx.ATTRIBUTE() {
        Some(_) => true,
        None => false
    };
    AstRelationDeclaration {
        predicate_name: predicate_name_,
        is_attribute: is_attribute_,
        arg_typings: arg_names
            .into_iter()
            .zip(types.into_iter())
            .collect()
    }
}

fn construct_program(ctx: &ProgramContext) -> AstProgram {
    AstProgram {
        relation_declarations: ctx
            .relationDeclaration_all()
            .iter()
            .map(|t| construct_relation_declaration(t))
            .collect(),
        assertions: ctx
            .saysAssertion_all()
            .iter()
            .map(|s| construct_says_assertion(s))
            .collect(),
        queries: ctx.query_all().iter().map(|q| construct_query(q)).collect(),
        imports: ctx
            .importAssertion_all()
            .iter()
            .map(|s| construct_import(s))
            .collect(),
        priv_binds: ctx
            .keyBind_all()
            .iter()
            .map(|kb| construct_keybinding(kb))
            .filter(|k| !k.is_pub)
            .collect(),
        pub_binds: ctx
            .keyBind_all()
            .iter()
            .map(|kb| construct_keybinding(kb))
            .filter(|k| k.is_pub)
            .collect(),
    }
}
