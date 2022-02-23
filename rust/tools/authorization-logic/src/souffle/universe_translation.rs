/*
 * Copyright 2022 Google LLC.
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

//! This file contains a translation that populates the universe relations.
//! Universe relations are facts of the form `isSymbol(x)` that are used to
//! keep datalog rules grounded and finite. This pattern works by putting facts
//! of the form `isSymbol(x)` on the RHS of a conditional rule where the variable `x`
//! would otherwise be ungrounded; then the set of symbols is populated by adding unconditional 
//! facts of the form `isSymbol("specific_symbol").` to populate these universes.
//!
//! A problem with using this pattern in the context of authorization logic is that these universes 
//! need to be shared across principals. Manually using this pattern is also tedius, and makes the 
//! code less clear. This translation handles both of these problems by:
//! 1) extending the program to implement the universe relation pattern by: adding universe 
//!    predicates to the RHS of rules, and adding facts that add elements to universes.
//! 2) extending the program with facts that use delegation to pass membership of universes across 
//!    principals.
//!
//! This translation is a compiler pass that operates on the data structure described in `ast.rs` 
//! and produces a new instance of the data structure in `ast.rs`.
//! It should come before the translation in `lowering_ast_datalog.rs`.

use crate::ast::*;
use std::collections::HashMap;

// Determine if a function argument or principal name is a constant or 
// return false if this name is a variable. In the ast, all arguments are
// represented as strings, and
// they are constants if and only if they are wrapped in quotes. A better
// way to represent this in the future might be to add an AST node for
// arguments with varibales and constants as two possible forms.
fn is_name_constant(arg: &String) -> bool {
    return arg.contains("\"")
}

fn push_option_to_vec<T>(vec: &mut Vec<T>, elt: Option<T>) {
    if let Some(x) = elt {
        vec.push(x);
    }
}

fn universe_condition_principal(principal: &AstPrincipal) -> Option<AstPredicate> {
    if !is_name_constant(&principal.name) {
        Some(AstPredicate {
            sign: Sign::Positive,
            name: "isPrincipal".to_string(),
            args: vec![principal.name.clone()]
        })
    } else {
        None
    }
}

fn type_to_universe_name(ast_type: &AstType) -> String {
    match ast_type {
        AstType::NumberType => "isNumber".to_string(),
        AstType::SymbolType => "isSymbol".to_string(),
        AstType::PrincipalType => "isPrincipal".to_string()
    }
}

fn universe_declarations() -> Vec<AstTypeDeclaration> {
    [
        ("isNumber", AstType::NumberType),
        ("isSymbol", AstType::SymbolType),
        ("isPrincipal", AstType::PrincipalType)
    ].iter().map(|(rel, typ)| AstTypeDeclaration {
        predicate_name: rel.to_string(),
        is_attribute: false,
        arg_typings: vec![("x".to_string(), typ.clone())]
    })
    .collect::<Vec<_>>()
}

pub struct UniverseHandlingPass {
    // mapping from predicate names to predicate typings
    type_environment: HashMap<String, AstTypeDeclaration>
}

impl UniverseHandlingPass {

    pub fn handle_universes(prog: &AstProgram) -> AstProgram {
        let universe_handling_pass = UniverseHandlingPass::new(prog);
        let mut new_prog = prog.clone();
        new_prog.assertions = prog.assertions.iter()
            .map(|x| {
                universe_handling_pass.add_universe_conditions_says_assertion(x)
            })
            .collect();
        new_prog.type_declarations.append(&mut universe_declarations());
        new_prog
    }

    fn new(prog: &AstProgram) -> UniverseHandlingPass {
        UniverseHandlingPass {
            type_environment: (&prog.type_declarations)
                .iter()
                .map(|decl| (decl.predicate_name.clone(), decl.clone()))
                .collect()
        }
    }

    fn add_universe_conditions_says_assertion(&self,
        says_assertion: &AstSaysAssertion)  -> AstSaysAssertion {
        AstSaysAssertion {
            prin: says_assertion.prin.clone(),
            assertions: says_assertion.assertions.iter()
                .map(|x| self.add_universe_conditions_assertion(x))
                .collect(),
            export_file: says_assertion.export_file.clone()
        }
    }

    // For ungrounded variables appearing in the LHS, extend the RHS
    // with prediates that put them in the universe relations
    fn add_universe_conditions_assertion(&self, assertion: &AstAssertion)
        -> AstAssertion {
            match assertion {
                AstAssertion::AstFactAssertion { f: fact } => {
                    let new_rhs = self.universe_conditions_fact(&fact);
                    AstAssertion::AstCondAssertion {
                        lhs: fact.clone(),
                        rhs: self.universe_conditions_fact(&fact)
                            .iter()
                            .map(|pred| AstFlatFact::AstPredFact{p: pred.clone()})
                            .collect()
                    }
                }
                AstAssertion::AstCondAssertion { lhs, rhs } => {
                    let mut new_rhs = rhs.clone();
                    for x in self.universe_conditions_fact(lhs).iter() {
                        new_rhs.push(
                            AstFlatFact::AstPredFact {p: x.clone()});
                    }
                    AstAssertion::AstCondAssertion {
                        lhs: lhs.clone(),
                        rhs: new_rhs
                    }
                }
            }
    }

    // For an AstFact appearing on the LHS of a conditional assertion,
    // generate the set of universe predicates that should be added to the RHS
    // from the variables that are subexprs of the AstFact. 
    // TODO: it would be more precise to instead add universe conditions just
    // for the variables that are genuinely ungrounded rather than all the
    // variables on the LHS.
    fn universe_conditions_fact(&self, ast_fact: &AstFact) -> Vec<AstPredicate> {
        match ast_fact {
            AstFact::AstFlatFactFact { f: flat_fact } => {
                self.universe_conditions_flat_fact(flat_fact)
            },
            AstFact::AstCanSayFact { p: delegatee, f: fact } => {
                let mut ret = self.universe_conditions_fact(fact);
                push_option_to_vec(&mut ret,
                                   universe_condition_principal(&delegatee));
                ret
            }
        }
    }

    fn universe_conditions_flat_fact(&self, ast_flat_fact: &AstFlatFact)
        -> Vec<AstPredicate> {
            match ast_flat_fact {
                AstFlatFact::AstPrinFact {p: principal, v: verb_phrase} => {
                    let mut ret = self
                        .universe_conditions_verb_phrase(verb_phrase);
                    push_option_to_vec(&mut ret,
                               universe_condition_principal(&principal));
                    ret
                }
                AstFlatFact::AstPredFact { p: predicate } => {
                    self.universe_conditions_predicate(predicate)
                }
            }
    }

    fn universe_conditions_verb_phrase(&self, verb_phrase: &AstVerbPhrase) 
        -> Vec<AstPredicate> {
            match verb_phrase {
                AstVerbPhrase::AstPredPhrase { p: predicate } => {
                    self.universe_conditions_predicate(&predicate)
                }
                AstVerbPhrase::AstCanActPhrase { p: principal } => {
                    universe_condition_principal(&principal)
                        .iter()
                        // iter returns references so copying is needed
                        .map(|x| x.clone())
                        .collect::<Vec<_>>()
                }
            }
    }

    fn universe_conditions_predicate(&self, predicate: &AstPredicate)
        -> Vec<AstPredicate> {
            predicate.args.iter()
                .map(|arg| self.universe_condition_arg(&predicate.name, arg))
                .flatten()
                .collect()
    }

    fn universe_condition_arg(&self, predicate_name: &String, argument: &String)
        -> Option<AstPredicate> {
            let predicate_decl = self.type_environment.get(predicate_name).expect(
                &format!("UniverseHandlingPass couldn't find a declaration for {}",
                        &predicate_name));
            let arg_to_type_map: HashMap<_, _> = predicate_decl.arg_typings
                .clone()
                .into_iter()
                .collect();
            let arg_type = arg_to_type_map.get(argument).unwrap();
            if !is_name_constant(argument) {
                Some(AstPredicate {
                    sign: Sign::Positive,
                    name: type_to_universe_name(arg_type),
                    args: vec![argument.to_string()]
                })
            } else {
                None
            }
    }
}
