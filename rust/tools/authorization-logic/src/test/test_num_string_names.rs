#[cfg(test)]
mod test {
    use crate::{
        compilation_top_level::*, utils::*,
    };

    #[test]
    pub fn test_num_string_names() {
        utils::setup_directories_for_bazeltest(vec!["test_inputs"], vec!["test_outputs"]);
        // The point of this test is to just see if it parses or if an
        // error is thrown.
        compile("test_inputs/numStringsInNames",
                "test_outputs/numStringsInNames.dl",
                &Vec::new());
    }

}
