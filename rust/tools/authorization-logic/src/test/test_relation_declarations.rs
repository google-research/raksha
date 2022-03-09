#[cfg(test)]
mod test {
    use crate::{
        ast::*, compilation_top_level::*,
    };

    #[test]
    pub fn test_relation_declarations() {
        let test_ast = source_file_to_ast_test_only("typeDeclarations",
                                                    "test_inputs");
        let custom_type = AstType::CustomType{ type_name: "CustomType".to_string() };
        assert!(test_ast.relation_declarations == vec![
                AstRelationDeclaration {
                    predicate_name: "someFact".to_string(),
                    is_attribute: false,
                    arg_typings: vec![("x".to_string(), custom_type.clone())]
                },
                AstRelationDeclaration {
                    predicate_name: "someOtherFact".to_string(),
                    is_attribute: false,
                    arg_typings: vec![
                        ("x".to_string(), custom_type),
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
