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

use serde::*;

/// Hash and Eq are implemented by AstPrincipal because they are used as keys
/// in HashMaps in the implementation of signature exporting
#[derive(Debug, Hash, PartialEq, Eq, Clone, Serialize, Deserialize)]
pub struct AstPrincipal {
    pub name: String,
}

#[derive(Copy, Debug, Hash, Clone, Serialize, Deserialize)]
pub enum Sign { Negated, Positive }

/// Hash and Eq are needed so that AstPred can be used in a HashSet in
/// SouffleEmitter. The derive strategy for Eq does not work, so it is
/// implemented manually.
#[derive(Debug, Hash, Clone, Serialize, Deserialize)]
pub struct AstPredicate {
    pub sign: Sign,
    pub name: String,
    pub args: Vec<String>,
}

impl PartialEq for AstPredicate {
    fn eq(&self, other: &Self) -> bool {
        if self.name != other.name || self.args.len() != other.args.len() {
            return false;
        }
        for i in 0..self.args.len() {
            if self.args[i] != other.args[i] {
                return false;
            }
        }
        return true;
    }
}

// This is a hint that the compiler uses.
// See https://doc.rust-lang.org/std/cmp/trait.Eq.html
impl Eq for AstPredicate {}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstVerbPhrase {
    AstPredPhrase { p: AstPredicate },
    AstCanActPhrase { p: AstPrincipal },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstFlatFact {
    AstPrinFact { p: AstPrincipal, v: AstVerbPhrase },
    AstPredFact { p: AstPredicate },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstFact {
    AstFlatFactFact { f: AstFlatFact },
    AstCanSayFact { p: AstPrincipal, f: Box<AstFact> },
}

#[derive(Copy, Debug, Clone, Serialize, Deserialize)]
pub enum AstBinop {
    LessThan,
    GreaterThan,
    Equals,
    NotEquals,
    LessOrEquals,
    GreaterOrEquals
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstRValue {
    FlatFactRValue { flat_fact: AstFlatFact},
    BinopRValue { lnum: String , binop: AstBinop, rnum: String }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstAssertion {
    AstFactAssertion { f: AstFact },
    AstCondAssertion { lhs: AstFact, rhs: Vec<AstRValue> },
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstSaysAssertion {
    pub prin: AstPrincipal,
    pub assertions: Vec<AstAssertion>,
    pub export_file: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstQuery {
    pub name: String,
    pub principal: AstPrincipal,
    pub fact: AstFact,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstKeybind {
    pub filename: String,
    pub principal: AstPrincipal,
    pub is_pub: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstImport {
    pub filename: String,
    pub principal: AstPrincipal,
}

#[derive(Debug, Hash, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub enum AstType { 
    NumberType, 
    PrincipalType, 
    CustomType { type_name: String }
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Eq)]
pub struct AstRelationDeclaration {
    pub predicate_name: String,
    pub is_attribute: bool,
    pub arg_typings: Vec<(String, AstType)>
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstProgram {
    pub relation_declarations: Vec<AstRelationDeclaration>,
    pub assertions: Vec<AstSaysAssertion>,
    pub queries: Vec<AstQuery>,
    pub imports: Vec<AstImport>,
    pub priv_binds: Vec<AstKeybind>,
    pub pub_binds: Vec<AstKeybind>,
}
