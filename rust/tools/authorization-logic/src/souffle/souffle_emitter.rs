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

use crate::{ast::*, souffle::datalog_ir::*};
use std::collections::HashSet;

/// Produces Souffle code as a `String` when given a Datalog IR (DLIR) program.
pub fn emit_program(p: &DLIRProgram, decl_skip: &Option<Vec<String>>) -> String {
    vec![ 
        emit_type_declarations(p, 
                decl_skip.as_ref().unwrap_or(&Vec::new())),
        emit_relation_declarations(p,
                decl_skip.as_ref().unwrap_or(&Vec::new())),
        emit_program_body(p),
        emit_outputs(p)
    ].join("\n")
}

fn emit_type(auth_logic_type: &AstType) -> String {
    match auth_logic_type {
        AstType::NumberType => "number".to_string(),
        AstType::PrincipalType => "Principal".to_string(),
        AstType::CustomType{ type_name } => type_name.clone()
    }
}

fn emit_decl(relation_declaration: &AstRelationDeclaration) -> String {
    format!(".decl {}({})", relation_declaration.predicate_name,
        relation_declaration.arg_typings
            .iter()
            .map(|(param, al_type)|
                 format!("{}: {}", param, emit_type(al_type)))
            .collect::<Vec<_>>()
            .join(", "))
}

fn emit_pred(p: &AstPredicate) -> String {
    let neg = match p.sign {
        Sign::Positive => "",
        Sign::Negated => "!"
    };
    format!("{}{}({})", neg, &p.name, p.args.join(", "))
}

fn emit_op(operator: &AstComparisonOperator) -> String {
    match operator {
        AstComparisonOperator::LessThan => "<",
        AstComparisonOperator::GreaterThan => ">",
        AstComparisonOperator::Equals => "=",
        AstComparisonOperator::NotEquals =>  "!=",
        AstComparisonOperator::LessOrEquals => "<=",
        AstComparisonOperator::GreaterOrEquals => ">="
    }.to_string()
}

fn emit_rvalue(rvalue: &DLIRRValue) -> String {
    match rvalue {
        DLIRRValue::PredicateRValue { predicate } => emit_pred(predicate),
        DLIRRValue::ArithCompareRValue { arith_comp } => 
            format!("{} {} {}", &arith_comp.lnum,
                    emit_op(&arith_comp.op), &arith_comp.rnum)
    }
}

fn emit_assertion(a: &DLIRAssertion) -> String {
    match a {
        DLIRAssertion::DLIRFactAssertion { p } => emit_pred(p) + ".",
        DLIRAssertion::DLIRCondAssertion { lhs, rhs } => {
            format!(
                "{} :- {}.",
                emit_pred(lhs),
                rhs.iter()
                    .map(|dlir_rvalue| emit_rvalue(dlir_rvalue))
                    .collect::<Vec<_>>()
                    .join(", ")
            )
        }
    }
}

fn emit_program_body(p: &DLIRProgram) -> String {
    let mut body_vec = p.assertions
        .iter()
        .map(|x| emit_assertion(&x))
        .collect::<Vec<_>>();
    body_vec.sort();
    body_vec.join("\n")
}

fn emit_type_declarations(p: &DLIRProgram, decl_skip: &Vec<String>) -> String {
    let mut type_names : Vec<String> = p.relation_declarations.iter()
        .map(|rel_decl| rel_decl.arg_typings.iter()
             .map(|(_parameter, type_)| type_))
        .flatten()
        .filter_map(|type_| match type_ {
            AstType::CustomType { type_name } => Some(type_name.clone()),
            _ => None
        })
        .collect();
    type_names.push("Principal".to_string());

    let type_names_filtered = type_names.iter().filter(|type_name| {
        !decl_skip.contains(type_name) &&
        !(**type_name == "symbol".to_string())
    }).collect::<HashSet<_>>();

    let mut ret_vec = type_names_filtered.iter()
        .map(|type_| format!(".type {} <: symbol", type_))
        .collect::<Vec<_>>();
    ret_vec.sort();
    ret_vec.join("\n")
}

fn emit_relation_declarations(p: &DLIRProgram, decl_skip: &Vec<String>)
        -> String {
    let mut ret_vec = p.relation_declarations
        .iter()
        .filter(|x| !decl_skip.contains(&x.predicate_name))
        .map(|x| emit_decl(x))
        .collect::<Vec<_>>();
    ret_vec.sort();
    ret_vec.join("\n")
}

fn emit_outputs(p: &DLIRProgram) -> String {
    let mut ret_vec = p.outputs
        .iter()
        .map(|o| String::from(".output ") + &o)
        .collect::<Vec<String>>();
    ret_vec.sort();
    ret_vec.join("\n")
}
