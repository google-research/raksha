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

use crate::ast::*;

pub fn merge_multiprogram(multiprog: &AstMultiProgram) -> AstProgram {
    AstProgram {
        relation_declarations: multiprog.programs
            .iter()
            .map(|prog| prog.relation_declarations.clone())
            .flatten()
            .collect(),
        assertions: multiprog.programs
            .iter()
            .map(|prog| prog.assertions.clone())
            .flatten()
            .collect(),
        queries: multiprog.programs
            .iter()
            .map(|prog| prog.queries.clone())
            .flatten()
            .collect(),
        imports: multiprog.programs
            .iter()
            .map(|prog| prog.imports.clone())
            .flatten()
            .collect(),
        priv_binds: multiprog.programs
            .iter()
            .map(|prog| prog.priv_binds.clone())
            .flatten()
            .collect(),
        pub_binds: multiprog.programs
            .iter()
            .map(|prog| prog.pub_binds.clone())
            .flatten()
            .collect(),
    }
}
