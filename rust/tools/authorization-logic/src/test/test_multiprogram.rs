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
pub mod test{
    use crate::souffle::souffle_interface::*;
    use crate::compilation_top_level::*;

    #[test]
    pub fn multiprog_test() {
        compile(
            &vec!["test_inputs/multiprogram_part1.auth_logic".to_string(),
                "test_inputs/multiprogram_part2.auth_logic".to_string()],
            "test_outputs/merged_multiprogram.dl",
            &vec![]);

        run_souffle(
            &"test_outputs/merged_multiprogram.dl",
            &"test_outputs/");

        // This assertion checks that the query result is true, which
        // is the expected result.
        assert!(!is_file_empty(&"test_outputs/multiprog_test_query.csv"));

    }

}
