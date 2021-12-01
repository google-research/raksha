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

#[cfg(test)]
mod test {
    use crate::{
        ast::*, compilation_top_level::*, souffle::souffle_interface::*,
        test::test_queries::test::*,
    };
    use std::fs;

    // This dependency is used for generating keypairs.
    use crate::signing::tink_interface::*;

    #[test]
    fn test_multimic_overrides() {
        run_query_test(QueryTest {
            filename: "multimic-preferences.authlogic",
            input_dir: "raksha_examples/multimic-overrides",
            output_dir: "raksha_examples/multimic-overrides",
            query_expects: vec![
                ("may_notifierA", true),
                ("may_notifierB", true),
                ("may_notifierC", true),
                ("may_micComputeA", true),
                ("may_micComputeB", true),
                ("may_micComputeC", true),
                ("down_micComputeA", true),
                ("down_micComputeB", true),
                ("down_micComputeC", true),
                ("may_speechInA", true),
                ("may_speechInB", true),
                ("may_speechInC", true),
            ],
        });
    }


}
