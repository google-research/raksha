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

//! This file describes the translation from the AST (the
//! top-level language) to DLIR (the IR which is quite close to datalog).

//! Asside from queries, this translation is quite similar to the one given in
//! Section 7 of the SecPal paper "SecPAL: Design and Semantics of a Decentralized
//! Authorization Language"
//! https://www.pure.ed.ac.uk/ws/files/17662538/jcs_final.pdf. See that section
//! for a formal and completely general description of this translation. This
//! comment gives a more intuitive description using a few examples at the
//! expense of losing some generality.
//!
//! As a minor wrinkle, this language also supports grouping assertions together
//! as in:
//!      Prin says { assertion_1, ..., assertion_n }
//! and these are simply "unrolled" to
//!      Prin says assertion_1.
//!      ...
//!      Prin says assertion_n.
//!
//!
//! ## Translation of assertions
//!
//! First, predicates with modifiers like "says", "canSay", and "actAs" are
//! converted into normal predicates by appending these modifiers to the
//! predicate name and moving the principals referenced by these predicates
//! into the arguments of the predicates. So for example
//!         A says B canSay foo(x, y)
//! becomes
//!         says_canSay_foo(A, B, x, y)
//! The rest of this comment ignores this part of the translation for the sake
//! of readability.
//!
//! Next we need to add a principal for the conditions in assertions. To prove
//! an assertion like
//!         "A says fact(x) :- fact1(x), fact2(x).
//! we need to show that *A believes* both fact1(x) and fact2(x). So we prepend
//! predicates on the RHS in "A says", producing the new assertion:
//!         A says fact(x) :- A says fact1(x), A says fact2(x)
//! (where says is actually prepended to the names of predicates and A becomes
//! an extra argument as above).
//!
//!  Next we need to translate delegations which should pass beliefs from one
//!  principal to another. For example, if we have
//!      A says B canSay fact(x) :- fact1(x)
//!  we do the usual translation resulting in
//!      (1) A says B canSay fact(x) :- A says fact1(x)
//!  but we also add the additional rule
//!      (2) A says fact(x) :- x says fact(x), A says x canSay fact(x)
//!  where x is a fresh variable.
//!  The result is that rule (1) gives the condition for delegation
//!  and rule (2) passes beliefs from B to A when the condition from (1) is met.
//!
//! Finally we need to translate canActAs which should pass properties from
//! one principal to another:
//! Assertions of either the form
//!      A says B canActAs C :- ...
//! or
//!      A says B <anyPredicate(...)> :-
//! are claims by A that B has a property (where canActAs is just a special
//! case). Syntactically these are both treated as "A says B <verbphrase>" with
//! different instances of <verbphrase>. Whenever some other principal can act
//! as B, we want to pass these properties from B to that other principal.
//! (For the special case where this property is B canActAs C, this essentially
//! makes canActAs transitive). In either case, we add an additional rule
//!    A says x verbphrase <- A says x canActAs B, A says B verbphrase
//! which will add "verbphrase" as a property of D whenever we can prove
//! the assertion "A says D canActAsB".
//!
//! ## Translation of Queries
//! Neither Souffle nor SecPal support queries, but this language does.
//! Queries check of a single unconditional assertion is true. A query of
//! the form:
//! ```Datalog
//!      Q_NAME = query ASSERTION(<ARGS>) ?
//! ```
//!
//! is translated into an assertion with just one argument, and this
//! assertion is made an output, so we can simply check the CSV that
//! souffle emits as in:
//! ```Datalog
//!      Q_NAME("dummy_var") :- grounded("dummy_var"), ASSERTION(<ARGS>).
//!      grounded("dummy_var").
//!      .output Q_NAME
//! ```

use crate::{ast::*, souffle::datalog_ir::*};
use std::collections::HashMap;

fn pred_to_dlir_rvalue(pred: &AstPredicate) -> DLIRRvalue {
    DLIRRvalue::PredicateRvalue { predicate: pred.clone() }
}

// Note that this puts args_ on the front of the list of arguments because
// this is the conveninet way for it to work in the contexts in which it
// is used.
fn push_onto_pred(modifier: String, mut args_: Vec<String>, pred: &AstPredicate) -> AstPredicate {
    let new_name = modifier + &pred.name;
    for a in &pred.args {
        args_.push(a.clone());
    }
    AstPredicate {
        sign: pred.sign,
        name: new_name.clone(),
        args: args_.to_vec(),
    }
}

fn push_prin(modifier: String, p: &AstPrincipal, pred: &AstPredicate) -> AstPredicate {
    let mut args = Vec::new();
    args.push(p.name.clone());
    push_onto_pred(modifier, args, pred)
}

// This struct only contains a counter for generating new fresh variables.
pub struct LoweringToDatalogPass {
    fresh_var_count: u32,
    // This pass needs to generate relation declarations for predicates that appear in
    // `AstCanSayFact`s which look like `A1 says A2 canSay ... An canSay foo(args)`
    // and can in general be nested. The generated declarations look like:
    // `says_cansay_..._cansay_foo(A1, A2, ..., An, args)`, so to generate these
    // declarations, we need to know the number of "canSays" to include for each
    // base predicate (e.g., foo). This pass does this by tracking the maximum
    // depth of "canSay"s with which each predicate is used and storing this
    // in the `cansay_depth` HashMap. When a predicate appears with depth `n`,
    // it may also need to be used with depths (1..n), so we only maintain the maximum
    // depth in which it appears and we generate declarations for (1..n). `cansay_depth`
    // is only updated by `update_cansay_depth` which tracks this maximum. 
    cansay_depth: HashMap<String, u32>,
}

impl LoweringToDatalogPass {
    /// Convert an AST program into a DLIR one, which should ease translation into Souffle.
    pub fn lower(prog: &AstProgram) -> DLIRProgram {
        let mut lowering_pass = LoweringToDatalogPass::new();
        lowering_pass.prog_to_dlir(prog)
    }

    fn new() -> LoweringToDatalogPass {
        LoweringToDatalogPass { 
            fresh_var_count: 0,
            cansay_depth: HashMap::new()
        }
    }

    fn fresh_var(&mut self) -> String {
        // Technically it is possible to have a collision with
        // a programmer-entered variable because the syntax of
        // identifiers for this language and souffle is the same.
        // However, the hope is that programmers will not usually
        // write variable names with 3 underscores.
        self.fresh_var_count += 1;
        String::from("x___") + &self.fresh_var_count.to_string()
    }

    // This is used for updating the `cansay_depth` HashMap. See also the comment
    // above the `cansay_depth` field of LoweringToDatalogPass.
    fn update_cansay_depth(&mut self, predicate_name: &str, depth: u32) {
        match self.cansay_depth.get(predicate_name) {
            None => { 
                self.cansay_depth.insert(predicate_name.to_string(), depth);
            },
            Some(old_depth) => if depth > *old_depth { 
                self.cansay_depth.insert(predicate_name.to_string(), depth);
            }
        }
    }

    fn verbphrase_to_dlir(&mut self, v: &AstVerbPhrase) -> AstPredicate {
        match v {
            AstVerbPhrase::AstPredPhrase { p: pred } => pred.clone(),
            AstVerbPhrase::AstCanActPhrase { p: prin } => {
                let mut args_ = Vec::new();
                args_.push(prin.name.clone());
                AstPredicate {
                    sign: Sign::Positive,
                    name: String::from("canActAs"),
                    args: args_,
                }
            }
        }
    }

    fn flat_fact_to_dlir(
        &mut self,
        f: &AstFlatFact,
        speaker: &AstPrincipal,
    ) -> (AstPredicate, Vec<DLIRAssertion>) {
        match f {
            AstFlatFact::AstPrinFact { p, v } => {
                let v_dlir = self.verbphrase_to_dlir(v);
                let pred = push_prin(String::from(""), p, &v_dlir);

                // Need to additionally generate:
                // `speaker says x v :- speaker says x canActAs p,
                //                     speaker says p v`
                // (where x is a fresh principal)
                let x = AstPrincipal {
                    name: self.fresh_var(),
                };

                // This is `speaker says x v`.
                let gen_lhs = push_prin(
                    String::from("says_"),
                    speaker,
                    &push_prin(String::from(""), &x, &v_dlir),
                );

                // This is `speaker says x canActAs p`.
                let x_as_p = AstPredicate {
                    sign: Sign::Positive,
                    name: String::from("canActAs"),
                    args: [x.name.clone(), p.name.clone()].to_vec(),
                };
                let s_says_x_as_p = push_prin(String::from("says_"), speaker, &x_as_p);

                // This is `speaker says p v`.
                let s_says_p_v = push_prin(String::from("says_"), speaker, &pred);

                let gen = DLIRAssertion::DLIRCondAssertion {
                    lhs: gen_lhs,
                    rhs: [s_says_x_as_p, s_says_p_v]
                        .into_iter()
                        .map(|pred| pred_to_dlir_rvalue(pred))
                        .collect(),
                };

                (pred, [gen].to_vec())
            }
            AstFlatFact::AstPredFact { p } => (p.clone(), Vec::new()),
        }
    }


    // This is meant to to encapsulate the fact that the 
    // recursion depth is tracked to do the translation of the relation
    // declarations. It can't be a nested function within fact_to_dlir
    // though because it needs mutable access to self and this is not allowed
    // within nested functions.
    fn fact_to_dlir_inner(&mut self, f: &AstFact, p: &AstPrincipal,
                      call_depth: u32) -> (AstPredicate, Vec<DLIRAssertion>) {
        match f {
            // When reading this, it may be useful to keep in mind that
            // facts (by contrast to flat facts) only appear on the LHS of assertions.
            AstFact::AstFlatFactFact { f: flat } => {
                // Because this is the base case, we can now add the
                // cansay depth to the environment that tracks this for
                // the generated predicate
                let (gen_pred, gen_rules) = self.flat_fact_to_dlir(flat, p);
                self.update_cansay_depth(&gen_pred.name, call_depth);
                (gen_pred, gen_rules)
            }
            AstFact::AstCanSayFact { p: q, f: f_plus } => {
                let (fact_plus_prime, mut collected) =
                    self.fact_to_dlir_inner(&*f_plus, p, call_depth + 1);
                let f_prime = push_prin(String::from("canSay_"), q,
                    &fact_plus_prime);
                let x = AstPrincipal {
                    name: self.fresh_var(),
                };

                // Note that f_prime does not begin with "p says"
                // because only the top-level fact should. This could
                // be a recursive call, so it might not be processing
                // the top-level fact. The top-level fact gets wrapped
                // in a "says" during the call to translate the assertion
                // in which this appears.

                // The following code generates
                // `p says fpf :- x says fpf, p says x canSay fpf`
                // where fpf is fact_plus_prime.

                // This is `p says fact_plus_prime`.
                let lhs = push_prin(String::from("says_"), p,
                    &fact_plus_prime);
                // This is `x says fact_plus_prime`.
                let x_says_term = push_prin(String::from("says_"), &x, 
                                            &fact_plus_prime);
                // This is `p says x canSay fact_plus_prime`.
                let can_say_term = push_prin(
                    String::from("says_"),
                    p,
                    &push_prin(String::from("canSay_"), &x, &fact_plus_prime),
                );
                // This is `p says fpf :- x says fpf, p says x canSay fpf`.
                let rhs = [x_says_term, can_say_term]
                    .into_iter()
                    .map(|pred| pred_to_dlir_rvalue(pred))
                    .collect();
                let gen = DLIRAssertion::DLIRCondAssertion { lhs, rhs };

                collected.push(gen);
                (f_prime, collected)
            }
        }
    }

    fn fact_to_dlir(&mut self, f: &AstFact,
                    p: &AstPrincipal) -> (AstPredicate, Vec<DLIRAssertion>) {
        self.fact_to_dlir_inner(f, p, 0)
    }

    fn says_assertion_to_dlir(&mut self, x: &AstSaysAssertion) -> Vec<DLIRAssertion> {
        x.assertions
            .iter()
            .map(|assertion| self.says_assertion_to_dlir_inner(&x.prin, assertion))
            .flatten()
            .collect()
    }
    
    fn rvalue_to_dlir(
        &mut self,
        speaker: &AstPrincipal,
        rvalue: &AstRValue) -> DLIRRvalue {
        match rvalue {
            AstRValue::FlatFactRValue { flat_fact } => {
                let (flat_pred, _) = self.flat_fact_to_dlir(&flat_fact, &speaker);
                pred_to_dlir_rvalue(&push_prin(String::from("says_"),
                    &speaker, &flat_pred))
            }
            AstRValue::BinopRValue { lnum, binop, rnum } => {
                DLIRRvalue::BinopRValue { 
                    lnum: *lnum, 
                    binop: *binop, 
                    rnum: *rnum 
                }
            }
        }
    }

    fn says_assertion_to_dlir_inner(
        &mut self,
        speaker: &AstPrincipal,
        assertion: &AstAssertion,
    ) -> Vec<DLIRAssertion> {
        match &assertion {
            AstAssertion::AstFactAssertion { f } => {
                let (pred, mut gen_assert) = self.fact_to_dlir(&f, &speaker);
                let pred_prime = push_prin(String::from("says_"), &speaker, &pred);
                gen_assert.push(DLIRAssertion::DLIRFactAssertion { p: pred_prime });
                gen_assert
            }
            AstAssertion::AstCondAssertion { lhs, rhs } => {
                let (lhs_prime, mut assertions) = self.fact_to_dlir(&lhs, &speaker);
                let dlir_lhs = 
                    push_prin(String::from("says_"), &speaker, &lhs_prime);
                let this_assertion = DLIRAssertion::DLIRCondAssertion {
                    lhs: dlir_lhs,
                    rhs: rhs.clone()
                        .into_iter()
                        .map(|ast_rvalue| self.rvalue_to_dlir(
                                &speaker, &ast_rvalue))
                        .collect()
                };
                assertions.push(this_assertion);
                assertions
            }
        }
    }

    // This can't be a const because Strings (by contrast to &str's) can't be
    // constructed in consts.
    fn dummy_fact() -> AstPredicate {
        AstPredicate {
            sign: Sign::Positive,
            name: "grounded_dummy".to_string(),
            args: vec!["\"dummy_var\"".to_string()],
        }
    }

    fn query_to_dlir(&mut self, query: &AstQuery) -> DLIRAssertion {
        // The assertions that are normally generated during the translation
        // from facts to dlir facts are not used for queries.
        let (main_fact, _) = self.fact_to_dlir(&query.fact, &query.principal);
        let main_fact = push_prin(String::from("says_"), &query.principal, &main_fact);
        let lhs = AstPredicate {
            sign: Sign::Positive,
            name: query.name.clone(),
            args: vec![String::from("\"dummy_var\"")],
        };
        DLIRAssertion::DLIRCondAssertion {
            lhs: lhs,
            rhs: [main_fact, LoweringToDatalogPass::dummy_fact()]
                .into_iter()
                .map(|pred| pred_to_dlir_rvalue(pred))
                .collect()
        }
    }

    // Note that the additional relation declarations that are generated for
    // (possibly nested) cansay expressions are based on an environment
    // that tracks the maximum depth with which they appear. This environment is
    // populated by side-effect while translating the program body. As a result, it
    // is assumed that this function is called after the body is translated.
    fn relation_declarations_to_dlir(&mut self,
         decls: &Vec<AstRelationDeclaration>) -> Vec<AstRelationDeclaration>{

        // The base relation declarations are the same as the ones from the 
        // surface program plus another one for "canActAs"
        let mut base_decls = decls.clone();
        base_decls.push(AstRelationDeclaration {
            predicate_name: "canActAs".to_string(),
            is_attribute: false,
            arg_typings: vec![
                ("p1".to_string(), AstType::PrincipalType),
                ("p2".to_string(), AstType::PrincipalType)
            ]
        });

        // Attributes have a principal that the attribute is applied to
        // as an additional parameter at the beginning of the param list.
        fn handle_attribute_declaration(decl: &AstRelationDeclaration) -> 
                AstRelationDeclaration {
            match decl.is_attribute {
                true => {
                    let mut arg_typings_ = vec![("attributed_prin".to_string(),
                                                 AstType::PrincipalType)];
                    arg_typings_.append(&mut decl.arg_typings.clone());
                    AstRelationDeclaration {
                        predicate_name: decl.predicate_name.clone(),
                        is_attribute: false,
                        arg_typings: arg_typings_
                    }
                }
                false => decl.clone()
            }
        }

        // Transform the attributes
        base_decls = base_decls.iter()
            .map(|decl| handle_attribute_declaration(decl))
            .collect();

        // Produce a mapping from predicate names to predicate typings
        // (where a predicate typing is the same as a predicate relation declaration)
        let type_environment :HashMap<_,_> = (&base_decls)
            .iter()
            .map(|decl| (decl.predicate_name.clone(), decl.clone()))
            .collect();

        // Prefix a relation declaration with "P cansay ..." `delegation_depth`
        // times.
        fn prefix_with_cansay(t: &AstRelationDeclaration,
                             delegation_depth: u32) -> AstRelationDeclaration {
            match delegation_depth {
                0 => t.clone(),
                x => {
                    let mut decl_less = prefix_with_cansay(t, x-1);
                    let mut arg_typings_ = vec![(format!("delegatee{}", x),
                                AstType::PrincipalType)];
                    arg_typings_.append(&mut decl_less.arg_typings);
                    AstRelationDeclaration {
                        predicate_name: format!("canSay_{}", decl_less.predicate_name),
                        is_attribute: false,
                        arg_typings: arg_typings_
                    }
                }
            }
        }

        // Generate relation declarations for delegated predicates with
        // each encountered delegation depth
        let mut cansay_decls = self.cansay_depth.iter()
            .filter(|(name, depth)| **depth > 0)
            .map(|(name, depth)| { 
                prefix_with_cansay(type_environment.get(name).expect(
                        &format!("couldnt find type named: {}", name)), *depth)
            });
        base_decls.extend(&mut cansay_decls);

        // The translated declarations are all extended with "says_" and a speaker
        // argument
        base_decls.iter()
            .map(|decl| {
                let mut arg_typings_ = vec![("speaker".to_string(),
                    AstType::PrincipalType)];
                arg_typings_.extend(decl.arg_typings.clone());
                AstRelationDeclaration {
                    predicate_name: format!("says_{}", decl.predicate_name),
                    is_attribute: false,
                    arg_typings: arg_typings_
                }
            })
            .collect()
    }

    fn relation_declarations_for_queries(&mut self, queries: &Vec<AstQuery>) ->
            Vec<AstRelationDeclaration> {
        let mut query_decls: Vec<_> = queries.iter()
            .map(|q| AstRelationDeclaration {
                predicate_name: q.name.clone(),
                is_attribute: false,
                arg_typings: vec![("dummy_param".to_string(),
                    AstType::CustomType{type_name: "DummyType".to_string()})]
            })
            .collect();
        query_decls.push(AstRelationDeclaration {
            predicate_name: "grounded_dummy".to_string(),
            is_attribute: false,
            arg_typings: vec![("dummy_param".to_string(),
                AstType::CustomType{type_name: "DummyType".to_string()})]
        });
        query_decls
    }

    fn prog_to_dlir(&mut self, prog: &AstProgram) -> DLIRProgram {
        let mut dlir_assertions: Vec<DLIRAssertion> = prog
            .assertions
            .iter()
            .map(|assert| self.says_assertion_to_dlir(&assert))
            .flatten()
            .collect();
        let mut dlir_queries: Vec<DLIRAssertion> = prog
            .queries
            .iter()
            .map(|query| self.query_to_dlir(&query))
            .collect();
        let outs = prog.queries.iter().map(|q| q.name.clone()).collect();
        let dummy_assertion = DLIRAssertion::DLIRFactAssertion {
            p: LoweringToDatalogPass::dummy_fact(),
        };
        dlir_assertions.append(&mut dlir_queries);
        dlir_assertions.push(dummy_assertion);

        // Note that the additional relation declarations that are generated for
        // (possibly nested) cansay expressions are based on an environment
        // that tracks the maximum depth with which they appear. This environment is
        // populated by side-effect while translating the program body. As a result, it
        // is assumed that this function is called after the body is translated.
        let mut dlir_relation_declarations = self.relation_declarations_to_dlir(
            &prog.relation_declarations);
        dlir_relation_declarations.extend(
            self.relation_declarations_for_queries(&prog.queries));

        DLIRProgram {
            relation_declarations: dlir_relation_declarations,
            assertions: dlir_assertions,
            outputs: outs,
        }
    }
}
