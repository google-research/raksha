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

//! This file contains the datalog IR (DLIR) which makes the translation from
//! this authorization logic into datalog simpler.
use crate::ast::*;

#[derive(Clone)]
pub enum DLIRAssertion {
    DLIRFactAssertion {
        p: AstPredicate,
    },
    DLIRCondAssertion {
        lhs: AstPredicate,
        rhs: Vec<AstPredicate>,
    },
}

#[derive(Clone)]
pub struct DLIRProgram {
    pub relation_declarations: Vec<AstRelationDeclaration>,
    pub assertions: Vec<DLIRAssertion>,
    pub outputs: Vec<String>,
}
