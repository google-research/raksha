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

#[cfg(test)]
mod test {
    use crate::{
        compilation_top_level::*, utils::*,
    };

    #[test]
    pub fn test_hyphen_ids() {
        utils::setup_directories_for_bazeltest(vec!["test_inputs"], vec!["test_outputs"]);
        // The point of this test is to just see if it parses or if an
        // error is thrown.
        compile("test_inputs/hyphenIDs",
                "test_outputs/hyphenIDs.dl",
                &Vec::new());
    }

}