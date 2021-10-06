/*
 * Copyright 2021 The Raksha Authors
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

use crate::{ast::*, souffle::datalog_ir::*};
use std::collections::HashSet;

pub struct SouffleEmitter {
    // This is a set of predicates that need to be declared. This set is
    // populated by side effect while traversing the program body.
    decls: HashSet<AstPredicate>,
}

impl SouffleEmitter {
    /// Produces Souffle code as a `String` when given a Datalog IR (DLIR) program.
    pub fn emit_program(p: &DLIRProgram, decl_skip: &Option<Vec<String>>) -> String {
        let mut emitter = SouffleEmitter::new();
        // It is important to generate the body first in this case
        // because the declarations are populated by side-effect while
        // generating the body.
        let body = emitter.emit_program_body(p);
        let outputs = emitter.emit_outputs(p);
        let decls = emitter.emit_declarations(
            decl_skip.as_ref().unwrap_or(&Vec::new()));
        vec![decls, body, outputs].join("\n")
    }

    fn new() -> SouffleEmitter {
        SouffleEmitter {
            decls: HashSet::new(),
        }
    }

    // This function produces a normalized version of predicates to
    // be used when generating declarations of the predicates. It replaces
    // argument names with generic numbered ones. It is applied
    // to predicates before they are added to the set of declarations so that
    // there are no duplicate delcarations (which would otherwise happen
    // whenever a predicate is referenced more than once with different
    // arguments).
    fn pred_to_declaration(p: &AstPredicate) -> AstPredicate {
        AstPredicate {
            name: p.name.clone(),
            args: (0..p.args.len())
                .map(|i| String::from("x") + &i.to_string())
                .collect(),
        }
    }

    fn emit_pred(&mut self, p: &AstPredicate) -> String {
        let decl = SouffleEmitter::pred_to_declaration(p);
        self.decls.insert(decl);
        format!("{}({})", &p.name, p.args.join(", "))
    }

    fn emit_assertion(&mut self, a: &DLIRAssertion) -> String {
        match a {
            DLIRAssertion::DLIRFactAssertion { p } => self.emit_pred(p) + ".",
            DLIRAssertion::DLIRCondAssertion { lhs, rhs } => {
                format!(
                    "{} :- {}.",
                    self.emit_pred(lhs),
                    rhs.iter()
                        .map(|ast_pred| self.emit_pred(ast_pred))
                        .collect::<Vec<_>>()
                        .join(", ")
                )
            }
        }
    }

    fn emit_program_body(&mut self, p: &DLIRProgram) -> String {
        p.assertions
            .iter()
            .map(|x| self.emit_assertion(&x))
            .collect::<Vec<_>>()
            .join("\n")
    }

    fn emit_decl(pred: &AstPredicate) -> String {
        format!(
            ".decl {}({})",
            &pred.name,
            &pred
                .args
                .iter()
                .map(|x| format!("{}: symbol", x))
                .collect::<Vec<_>>()
                .join(", ")
        )
    }

    fn emit_declarations(&self, decl_skip: &Vec<String>) -> String {
        let mut decls = self.decls
            .iter()
            .map(|x| 
                 if decl_skip.contains(&x.name)
                { "".to_string() } else { SouffleEmitter::emit_decl(x) })
            .collect::<Vec<_>>();
        decls.sort();
        decls.join("\n")
    }

    fn emit_outputs(&self, p: &DLIRProgram) -> String {
        p.outputs
            .iter()
            .map(|o| String::from(".output ") + &o)
            .collect::<Vec<String>>()
            .join("\n")
    }
}
