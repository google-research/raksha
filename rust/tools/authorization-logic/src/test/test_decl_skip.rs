#[cfg(test)]
mod test {
    use crate::{
        compilation_top_level::*, utils::*
    };
    use std::fs;

    fn file_line_list(filename: &str) -> Vec<String> {
        fs::read_to_string(filename)
            .expect("is_file_empty failed to read")
            .split("\n")
            .map(|s| s.to_string())
            .collect()
    }

    #[test]
    pub fn test_decl_skip() {
        utils::setup_directories_for_bazeltest(vec!["test_inputs"], vec!["test_outputs"]);
        // Compile with declarations
        compile("testDeclSkip", "test_inputs", "test_outputs", &Vec::new());
        let contents1 = file_line_list(&utils::get_resolved_path("test_outputs/testDeclSkip.dl"));
        assert!(contents1.contains(&".decl grounded_dummy(dummy_param: DummyType)".to_string()));
        assert!(contents1.contains(
            &".decl says_isRelevantFor(speaker: Principal, attributed_prin: Principal, subject: symbol)"
            .to_string()));
        
        // Compile with some declarations removed
        compile("testDeclSkip", "test_inputs","test_outputs",
             &vec!["grounded_dummy".to_string(),"says_isRelevantFor".to_string()]);
        let contents2 = file_line_list(&utils::get_resolved_path("test_outputs/testDeclSkip.dl"));
        assert!(!contents2.contains(&".decl grounded_dummy(dummy_param: DummyType)".to_string()));
        assert!(!contents2.contains(
            &".decl says_isRelevantFor(speaker: Principal, x1: symbol, x2: symbol)"
            .to_string()));


    }

}
