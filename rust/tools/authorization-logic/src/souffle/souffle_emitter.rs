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

/// Produces Souffle code as a `String` when given a Datalog IR (DLIR) program.
pub fn emit_program(p: &DLIRProgram, decl_skip: &Option<Vec<String>>) -> String {
    vec![ 
        emit_declarations(p, decl_skip.as_ref().unwrap_or(&Vec::new())),
        emit_program_body(p),
        emit_outputs(p)
    ].join("\n")
}

fn emit_type(auth_logic_type: &AstType) -> String {
    match auth_logic_type {
        AstType::NumberType => "number".to_string(),
        AstType::SymbolType => "symbol".to_string(),
        AstType::PrincipalType => "principal".to_string()
    }
}

fn emit_decl(type_declaration: &AstTypeDeclaration) -> String {
    format!(".decl {}({})", type_declaration.predicate_name,
        type_declaration.arg_typings
            .iter()
            .map(|(param, al_type)|
                 format!("{}: {}", param, emit_type(al_type)))
            .collect::<Vec<_>>()
            .join(", "))
}

fn emit_pred(p: &AstPredicate) -> String {
    let neg = match p.sign {
        Sign::Positive => "",
        Negative => "!"
    };
    format!("{}{}({})", neg, &p.name, p.args.join(", "))
}

fn emit_assertion(a: &DLIRAssertion) -> String {
    match a {
        DLIRAssertion::DLIRFactAssertion { p } => emit_pred(p) + ".",
        DLIRAssertion::DLIRCondAssertion { lhs, rhs } => {
            format!(
                "{} :- {}.",
                emit_pred(lhs),
                rhs.iter()
                    .map(|ast_pred| emit_pred(ast_pred))
                    .collect::<Vec<_>>()
                    .join(", ")
            )
        }
    }
}

fn emit_program_body(p: &DLIRProgram) -> String {
    p.assertions
        .iter()
        .map(|x| emit_assertion(&x))
        .collect::<Vec<_>>()
        .join("\n")
}

fn emit_declarations(p: &DLIRProgram, decl_skip: &Vec<String>) -> String {
    let principal_type = String::from(".type principal <: symbol");
    let generated_declarations = p.type_declarations
        .iter()
        .filter(|x| !decl_skip.contains(&x.predicate_name))
        .map(|x| emit_decl(x))
        .collect::<Vec<_>>()
        .join("\n");
    vec![principal_type, generated_declarations].join("\n")
}

fn emit_outputs(p: &DLIRProgram) -> String {
    p.outputs
        .iter()
        .map(|o| String::from(".output ") + &o)
        .collect::<Vec<String>>()
        .join("\n")
}
