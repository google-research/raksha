#[cfg(test)]
mod test {
    use crate::{
        ast::*, compilation_top_level::*, souffle::souffle_interface::*
    };

    #[test]
    pub fn test_type_declarations() {
        let test_ast = source_file_to_ast_test_only("typeDeclarations",
                                                    "test_inputs");
        println!("test_type type declarations");
        println!("{:?}", test_ast.type_declarations);
        println!("about to do assertion:");
        assert!(test_ast.type_declarations == vec![
                AstTypeDeclaration {
                    predicate_name: "someFact".to_string(),
                    arg_typings: vec![("x".to_string(), AstType::SymbolType)]
                },
                AstTypeDeclaration {
                    predicate_name: "someOtherFact".to_string(),
                    arg_typings: vec![
                        ("x".to_string(), AstType::SymbolType),
                        ("y".to_string(), AstType::NumberType)
                    ]
                },
                AstTypeDeclaration {
                    predicate_name: "thirdFact".to_string(),
                    arg_typings: vec![("p".to_string(), AstType::PrincipalType)]
                }
        ]);
    }

}
