#[cfg(test)]
mod test {
    use crate::{
        ast::*, compilation_top_level::*, souffle::souffle_interface::*, utils::*
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
        // Compile with declarations
        compile("testDeclSkip", "test_inputs", "test_outputs", "");
        let contents1 = file_line_list(&utils::get_resolved_output_path("test_outputs/testDeclSkip.dl"));
        assert!(contents1.contains(&".decl grounded_dummy(dummy_param: DummyType)".to_string()));
        assert!(contents1.contains(
            &".decl says_isRelevantFor(speaker: Principal, attributed_prin: Principal, subject: symbol)"
            .to_string()));
        
        // Compile with some declarations removed
        compile("testDeclSkip", "test_inputs","test_outputs",
            "grounded_dummy,says_isRelevantFor");
        let contents2 = file_line_list(&utils::get_resolved_output_path("test_outputs/testDeclSkip.dl"));
        assert!(!contents2.contains(&".decl grounded_dummy(dummy_param: DummyType)".to_string()));
        assert!(!contents2.contains(
            &".decl says_isRelevantFor(speaker: Principal, x1: symbol, x2: symbol)"
            .to_string()));


    }

}
