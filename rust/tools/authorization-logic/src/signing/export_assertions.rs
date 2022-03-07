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

//! This file contains a compiler pass that handles executing of policy statements.
//! For each `SaysAssertion` with an export clause, it serializes the object
//! that corresponds to the (set of) assertions to a file and it produces an
//! ECDSA signature of that object in a separate file. It operates on the
//! highest-level IR in the policy language (in ast.rs).

/// AstPrincipal implements Hash and Eq, so different
/// occurances of the same principal within a program will not
/// create multiple entries in this HashMap.

use crate::{ast::*, signing::tink_interface::*, utils::*};
use std::collections::HashMap;

type BindingEnv = HashMap<AstPrincipal, String>;

fn collect_priv_bindings(prog: &AstProgram) -> BindingEnv {
    prog.priv_binds
        .iter()
        .map(|kb| (kb.principal.clone(), kb.filename.clone()))
        .collect()
}

fn export_says_assertion(says_assertion: &AstSaysAssertion, priv_env: &BindingEnv) {
    match &says_assertion.export_file {
        None => {}
        Some(filename) => {
            let obj_file = utils::get_resolved_output_path(&(filename.clone() + ".obj"));
            let sig_file = utils::get_resolved_output_path(&(filename.clone() + ".sig"));
            let priv_key_file = utils::get_resolved_output_path(
                &priv_env.get(&says_assertion.prin).unwrap());
            serialize_to_file(&says_assertion.assertions, &obj_file).unwrap();
            sign_claim(&priv_key_file, &sig_file, &says_assertion.assertions);
        }
    }
}

/// When given an AstProgram this function exports signatures.
pub fn export_assertions(prog: &AstProgram) {
    let priv_env = collect_priv_bindings(&prog);
    for assertion in &prog.assertions {
        export_says_assertion(&assertion, &priv_env);
    }
}
