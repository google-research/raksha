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

#[cfg(test)]
pub mod test {
    use crate::{
        ast::*, compilation_top_level::*, souffle::souffle_interface::*,
        test::test_queries::test::*,
    };

    #[test]
    fn test_basic_number_comparisons() {
        run_query_test(QueryTest {
            filename: "basic_num_comparisons",
            query_expects: vec![
                ("q1", true),
                ("q2", false),
                ("q3", true),
                ("q4", false),
                ("q5", true),
                ("q6", false),
                ("q7", true),
                ("q8", false),
                ("q9", true),
                ("q10", true),
                ("q11", false),
                ("q12", true),
                ("q13", true),
                ("q14", false)
            ],
            ..Default::default()
        });
    }

    #[test]
    fn test_comparisons_with_delegation() {
        run_query_test(QueryTest {
            filename: "comparisons_with_delegation",
            query_expects: vec![
                ("q1", true),
                ("q2", false),
            ],
            ..Default::default()
        });
    }

}
