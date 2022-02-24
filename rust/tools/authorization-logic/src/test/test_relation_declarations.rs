#[cfg(test)]
mod test {
    use crate::{
        ast::*, compilation_top_level::*, souffle::souffle_interface::*
    };

    #[test]
    pub fn test_relation_declarations() {
        let test_ast = source_file_to_ast_test_only("typeDeclarations",
                                                    "test_inputs");
        assert!(test_ast.relation_declarations == vec![
                AstRelationDeclaration {
                    predicate_name: "someFact".to_string(),
                    is_attribute: false,
                    arg_typings: vec![("x".to_string(), AstType::SymbolType)]
                },
                AstRelationDeclaration {
                    predicate_name: "someOtherFact".to_string(),
                    is_attribute: false,
                    arg_typings: vec![
                        ("x".to_string(), AstType::SymbolType),
                        ("y".to_string(), AstType::NumberType)
                    ]
                },
                AstRelationDeclaration {
                    predicate_name: "thirdFact".to_string(),
                    is_attribute: false,
                    arg_typings: vec![("p".to_string(), AstType::PrincipalType)]
                }
        ]);
    }

}
