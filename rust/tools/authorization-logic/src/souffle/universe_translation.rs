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
use std::collections::{HashMap, HashSet};

// Determine if a function argument or principal name is a constant or 
// return false if this name is a variable. Principals are constants if
// they are wrapped in quotes and arguments are constants if they are in quotes
// or are numbers. A better
// way to represent this in the future might be to add an AST node for
// arguments with varibales and constants as two possible forms.
fn is_name_constant(arg: &str) -> bool {
    // Anything in quotes is a constant
    if arg.starts_with("\"") {
        return true
    } else {
        // Numberic literals are constants

        // A better way to do this would be to just use a regex, but
        // regexes are not in the standard library and adding new crates
        // takes too long to integrate with the C++ side of Raksha.
        for c in arg.chars() {
            if !c.is_numeric() { return false }
        }
        return true
    }
}

fn push_option_to_vec<T>(vec: &mut Vec<T>, elt: Option<T>) {
    if let Some(x) = elt {
        vec.push(x);
    }
}

fn universe_condition_principal(principal: &AstPrincipal) -> Option<AstPredicate> {
    if is_name_constant(&principal.name) {
        None
    } else {
        Some(AstPredicate {
            sign: Sign::Positive,
            name: type_to_universe_name(&AstType::PrincipalType),
            args: vec![principal.name.clone()]
        })
    }
}

fn type_to_universe_name(ast_type: &AstType) -> String {
    match ast_type {
        AstType::NumberType => "isNumber".to_string(),
        AstType::PrincipalType => "isPrincipal".to_string(),
        AstType::CustomType { type_name } => format!("is{}", type_name.clone())
    }
}

fn universe_declarations(rel_decls: &Vec<AstRelationDeclaration>)
    -> Vec<AstRelationDeclaration> {
    // Set of all custom type names
    let mut custom_types = rel_decls.iter()
        .map(|rel_decl| rel_decl.arg_typings.clone())
        .flatten()
        .map(|(_name, typ)| typ)
        .filter(|type_| match type_ {
            AstType::CustomType { type_name: _ } => true,
            _ => false
        })
        .collect::<HashSet<_>>();
    custom_types.extend([AstType::NumberType, AstType::PrincipalType]);
    let universe_type_pairs = custom_types.iter()
        .map(|typ| (type_to_universe_name(typ), typ));

    universe_type_pairs.map(|(rel_name, typ)| AstRelationDeclaration {
        predicate_name: rel_name.to_string(),
        is_attribute: false,
        arg_typings: vec![("x".to_string(), typ.clone())]
    })
    .collect::<Vec<_>>()
}

//-----------------------------------------------------------------------------
// Populate Constant Type Environment & Type Checking
//-----------------------------------------------------------------------------
// These functions populate a mapping from constants (which may either be constant
// principals or other constant values applied as arguments to predicates) to types.
// This environment is needed to generate facts that put the constants into 
// universe relations; we need to know which universe relation to put each one in.

// TODO(#416): these functions should be refactored into a visitor. These are probably
// the first really good use-case for a visitor since this is the first
// functionality that traverses the AST in a way that is not highly order-dependent.
// TODO(#417) it might also be useful to move this into its own separate pass.

// This attempts to add a typing for principals and predicate arguments if they
// are constant. It does nothing if it is applied to an `arg_name` that is not
// constant. If there is no typing for this constant, it adds this typing to
// the environment. If the same typing already exists in the environment,
// it does nothing. If the constant already has a different typing in the
// environment, throw an error.
fn add_typing(constant_type_environment: &mut HashMap<String, AstType>,
                   arg_name: &str, typ: AstType) {
    if is_name_constant(arg_name) {
        let arg_name_string = arg_name.to_string();
        match constant_type_environment.get(&arg_name_string) {
            None => {
                constant_type_environment.insert(arg_name_string, typ);
            },
            Some(old_typ) => { 
                if old_typ != &typ {
                    // Type errors could be improved by getting line numbers.
                    panic!("type error: {} was used with distinct types {:?} and {:?}",
                           arg_name, old_typ, typ);
                }
            }
        }
    }
}


fn populate_constant_type_environment_prog(
    relation_type_environment: &RelationTypeEnv,
    prog: &AstProgram) -> HashMap<String, AstType> {
    let mut constant_type_environment = HashMap::new();

    // Add typings for constant speakers in says_assertions
    for says_assertion in &prog.assertions
    {
        add_typing(&mut constant_type_environment,
                   &says_assertion.prin.name,
                   AstType::PrincipalType);
    }

    // Add typings for constants appearing in assertions
    for assertion in prog.assertions.iter()
        .map(|says_assertion| &says_assertion.assertions)
        .flatten() {
        populate_constant_type_environment_assertion(
            &relation_type_environment, 
            &mut constant_type_environment, 
            &assertion);
    }

    for query in prog.queries.iter() {
        populate_constant_type_environment_query(
            &relation_type_environment, 
            &mut constant_type_environment, 
            &query);
    }

    constant_type_environment
}

fn populate_constant_type_environment_rvalue(
    relation_type_environment: &RelationTypeEnv,
    constant_type_environment: &mut HashMap<String, AstType>,
    rvalue: &AstRValue) {
    match rvalue {
        AstRValue::FlatFactRValue { flat_fact } => {
            populate_constant_type_environment_flat_fact(
                relation_type_environment,
                constant_type_environment,
                flat_fact);
        },
        AstRValue::ArithCompareRValue{ arith_comp } => {
            add_typing(constant_type_environment, &arith_comp.lnum,
                       AstType::NumberType);
            add_typing(constant_type_environment, &arith_comp.rnum,
                       AstType::NumberType);
        }
    }
}


fn populate_constant_type_environment_assertion(
    relation_type_environment: &RelationTypeEnv,
    constant_type_environment: &mut HashMap<String, AstType>,
    assertion: &AstAssertion) {
    match assertion {
        AstAssertion::AstFactAssertion { f: fact } => {
            populate_constant_type_environment_fact(
                &relation_type_environment,
                constant_type_environment,
                fact);
        }
        AstAssertion::AstCondAssertion { lhs, rhs } => {
            populate_constant_type_environment_fact(
                &relation_type_environment,
                constant_type_environment,
                lhs);
            for rvalue in rhs {
                populate_constant_type_environment_rvalue(
                    &relation_type_environment,
                    constant_type_environment,
                    rvalue);
            }
        }
    }
}

fn populate_constant_type_environment_query(
    relation_type_environment: &RelationTypeEnv,
    constant_type_environment: &mut HashMap<String, AstType>,
    query: &AstQuery) {

    add_typing(constant_type_environment, &query.principal.name,
               AstType::PrincipalType);
    populate_constant_type_environment_fact(relation_type_environment,
                                            constant_type_environment,
                                            &query.fact);
}

fn populate_constant_type_environment_fact(
    relation_type_environment: &RelationTypeEnv,
    constant_type_environment: &mut HashMap<String, AstType>,
    fact: &AstFact) {
    match fact {
        AstFact::AstFlatFactFact { f: flat_fact } => {
            populate_constant_type_environment_flat_fact(
                relation_type_environment,
                constant_type_environment,
                flat_fact);
        },
        AstFact::AstCanSayFact { p: speaker, f: fact } => {
            add_typing(constant_type_environment, &speaker.name,
                       AstType::PrincipalType);
            populate_constant_type_environment_fact(
                relation_type_environment,
                constant_type_environment,
                fact);
        }
    }
}

fn populate_constant_type_environment_flat_fact(
        relation_type_environment: &RelationTypeEnv,
        constant_type_environment: &mut HashMap<String, AstType>,
        flat_fact: &AstFlatFact) {
        match flat_fact {
            AstFlatFact::AstPrinFact { p: principal, v: verb_phrase } => {
                add_typing(constant_type_environment, &principal.name,
                           AstType::PrincipalType);
                match verb_phrase {
                    AstVerbPhrase::AstPredPhrase { p : predicate } => {
                        populate_constant_type_environment_predicate(
                            relation_type_environment,
                            constant_type_environment,
                            predicate);
                    }
                    AstVerbPhrase::AstCanActPhrase { p : as_principal } => {
                        add_typing(constant_type_environment, &as_principal.name,
                                   AstType::PrincipalType);
                    }
                }
            }
            AstFlatFact::AstPredFact { p: predicate } => {
                populate_constant_type_environment_predicate(
                    relation_type_environment,
                    constant_type_environment,
                    predicate);
            }
        }
}
    
fn populate_constant_type_environment_predicate(
    relation_type_environment: &RelationTypeEnv,
    constant_type_environment: &mut HashMap<String, AstType>,
    predicate: &AstPredicate) {

    let predicate_decl = relation_type_environment.get(&predicate.name).expect(
        &format!("The relation \"{}\" was used but not declared",
                &predicate.name));
    for (pos, arg_name) in predicate.args.iter().enumerate() {
        add_typing(constant_type_environment, arg_name,
                   predicate_decl.get(&pos).unwrap().clone());
    }

}

type RelationTypeEnv = HashMap<String, HashMap<usize, AstType>>;
pub struct UniverseHandlingPass {
    // mapping from relation names to relation typings. The relation
    // typings map from positions in the relation declaration to types.
    // A map from positions in the relation declarations to the
    // type of the parameter at those positions. For example,
    // `.decl foo(x: SomeThing, y: SomeOtherThing)`
    // would yield the map
    // ```
    // (foo ->
    //  (1 -> CustomType{ SomeThing },
    //  2 -> CustomType { SomeOtherThing }))
    //  ```
    relation_type_environment: RelationTypeEnv,
    // maping from constant names to constant typings
    constant_type_environment: HashMap<String, AstType>
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
        new_prog.relation_declarations.append(&mut universe_declarations(
                &prog.relation_declarations));
        universe_handling_pass.add_universe_defining_says_assertions(&new_prog)
    }

    fn new(prog: &AstProgram) -> UniverseHandlingPass {
        let rel_typ_env = (&prog.relation_declarations)
                .iter()
                .map(|decl| {
                    let position_map = decl.arg_typings.clone()
                        .into_iter()
                        .map(|(_param_name, typ)| typ)
                        .into_iter()
                        .enumerate()
                        .collect::<HashMap<usize, AstType>>();
                    (decl.predicate_name.clone(), position_map)
                })
                .collect();

        UniverseHandlingPass {
            constant_type_environment: 
                populate_constant_type_environment_prog(
                    &rel_typ_env, prog),
            relation_type_environment: rel_typ_env
        }
    }

    //-----------------------------------------------------------------------------
    // Add Universe Defining Facts
    //-----------------------------------------------------------------------------
    // These functions find all the constant principals, and all the constant predicate
    // arguments, and emits facts that populate the universes.
    fn add_universe_defining_says_assertions(&self, prog: &AstProgram) -> AstProgram {
        let constant_speakers: Vec<AstPrincipal> = prog.assertions.clone()
            .into_iter()
            .map(|says_assertion| says_assertion.prin)
            .filter(|prin| is_name_constant(&prin.name))
            .collect();
        let constant_subexpressions: Vec<&String> = 
            self.constant_type_environment.keys()
            .collect();

        let mut universe_defining_says_assertions = Vec::new();
        for speaker in &constant_speakers {
            for subexpr in &constant_subexpressions {
                universe_defining_says_assertions.push(
                    self.universe_defining_says_assertion(&speaker, subexpr));
            }
        }

        let mut ret = prog.clone();
        ret.assertions.extend(universe_defining_says_assertions);
        ret
    }

    fn universe_defining_says_assertion(&self, speaker: &AstPrincipal,
                                        expr: &str) -> AstSaysAssertion {
        AstSaysAssertion {
            prin: speaker.clone(),
            assertions: vec![ AstAssertion::AstFactAssertion {
                f: AstFact::AstFlatFactFact {
                    f: AstFlatFact::AstPredFact {
                        p: AstPredicate {
                            sign: Sign::Positive,
                            name: type_to_universe_name(
                                self.constant_type_environment.get(expr).unwrap()),
                            args: vec![expr.to_string()]
                        }
                    }
                }
            }],
            export_file: None
        }
    }

    //-----------------------------------------------------------------------------
    // Add Grounding Conditions
    //-----------------------------------------------------------------------------
    // For rules and facts that would contain otherwise ungrounded variables,
    // add universe relations to the RHS to make these assertions grounded.
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
    // with prediates that put them in the universe relations.
    // TODO(#418) at the moment, this conservatively adds a universe condition
    // for all variables on the LHS rather than just the ungrounded ones. Extend
    // this to capture just the ungrounded variables.
    fn add_universe_conditions_assertion(&self, assertion: &AstAssertion)
        -> AstAssertion {
            match assertion {
                AstAssertion::AstFactAssertion { f: fact } => {
                    let new_rhs_predicates = self.universe_conditions_fact(&fact);
                    if new_rhs_predicates.len() > 0 {
                        AstAssertion::AstCondAssertion {
                            lhs: fact.clone(),
                            rhs: new_rhs_predicates
                                .iter()
                                .map(|pred| AstRValue::FlatFactRValue {
                                     flat_fact: AstFlatFact::AstPredFact{p: pred.clone()}
                                })
                                .collect()
                        }
                    } else {
                        assertion.clone()
                    }
                }
                AstAssertion::AstCondAssertion { lhs, rhs } => {
                    let mut new_rhs = rhs.clone();
                    for x in self.universe_conditions_fact(lhs).iter() {
                        new_rhs.push(
                            AstRValue::FlatFactRValue {
                                flat_fact: AstFlatFact::AstPredFact {p: x.clone()}
                            }
                        );
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
            predicate.args.iter().enumerate()
                .map(|(pos, arg)| self.universe_condition_arg(
                        &predicate.name, arg, pos))
                .flatten()
                .collect()
    }

    fn universe_condition_arg(&self, predicate_name: &str, argument: &str, position: usize)
        -> Option<AstPredicate> {
            if is_name_constant(argument) {
                None
            } else {
                let predicate_decl = self.relation_type_environment
                    .get(predicate_name).expect(
                        &format!("The relation \"{}\" was used but not declared",
                            &predicate_name));
                let arg_type = predicate_decl
                    .get(&position)
                    .unwrap();
                Some(AstPredicate {
                    sign: Sign::Positive,
                    name: type_to_universe_name(arg_type),
                    args: vec![argument.to_string()]
                })
            }
    }
}
