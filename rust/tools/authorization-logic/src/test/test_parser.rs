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
        ast::*, parsing::astconstructionvisitor::*,
    };

    #[test]
    fn test_saysAssertion_simpleCanActAs() {
        let prog = r#""jack" says "jill" canActAs "jack"."#;
        
        let result = parse_program(prog);
        
        let assertion = AstSaysAssertion {
            prin: AstPrincipal { name: r#""jack""#.to_string() },
            assertions: vec![
                AstAssertion::AstFactAssertion {
                    f: AstFact::AstFlatFactFact {
                        f: AstFlatFact::AstPrinFact {
                            p: AstPrincipal { name: r#""jill""#.to_string() },
                            v: AstVerbPhrase::AstCanActPhrase { p: AstPrincipal { name: r#""jack""#.to_string() } },
                        }
                    }
                }
            ],
            export_file: None
        };
        let expected =
            AstProgram {
                assertions: vec![assertion],
                queries: Vec::new(),
                imports: Vec::new(),
                priv_binds: Vec::new(),
                pub_binds: Vec::new(),
        };
        
        assert!(result == expected);
    }
    
    #[test]
    fn test_saysAssertion_simpleConditional() {
        let prog = r#""jack" says foo("x") :- !bar("x")."#;
        
        let result = parse_program(prog);
        
        let assertion = AstSaysAssertion {
            prin: AstPrincipal { name: r#""jack""#.to_string() },
            assertions: vec![
                AstAssertion::AstCondAssertion {
                    lhs: AstFact::AstFlatFactFact {
                        f: AstFlatFact::AstPredFact {
                            p: AstPredicate {
                                sign: Sign::Positive,
                                name: "foo".to_string(),
                                args: vec![r#""x""#.to_string()],
                            }
                        }
                    },
                    rhs: vec![AstFlatFact::AstPredFact {
                        p: AstPredicate {
                            sign: Sign::Negated,
                            name: "bar".to_string(),
                            args: vec![r#""x""#.to_string()],
                        }
                    }]
                }
            ],
            export_file: None
        };
        let expected =
            AstProgram {
                assertions: vec![assertion],
                queries: Vec::new(),
                imports: Vec::new(),
                priv_binds: Vec::new(),
                pub_binds: Vec::new(),
        };
        
        assert!(result == expected);
    }
    
    #[test]
    fn test_query() {
        let prog = r#"q1 = query "jack" says foo("x")?"#;
        
        let result = parse_program(prog);
        
        let query = AstQuery {
            name: "q1".to_string(),
            principal: AstPrincipal { name: r#""jack""#.to_string() },
            fact: AstFact::AstFlatFactFact {
                f: AstFlatFact::AstPredFact {
                    p: AstPredicate {
                        sign: Sign::Positive,
                        name: "foo".to_string(),
                        args: vec![r#""x""#.to_string()],
                    }
                }
            },
        };
        let expected =
            AstProgram {
                assertions: Vec::new(),
                queries: vec![query],
                imports: Vec::new(),
                priv_binds: Vec::new(),
                pub_binds: Vec::new(),
        };
        
        assert!(result == expected);
    }
    
    #[test]
    fn test_keybinds() {
        let prog = r#"BindPubKey "jack" testdir/filename1.json
                      BindPrivKey "jill" testdir/filename2.json"#;
        
        let result = parse_program(prog);
        
        let pub_keybind = AstKeybind {
            filename: "testdir/filename1.json".to_string(),
            principal: AstPrincipal { name: r#""jack""#.to_string() },
            is_pub: true,
        };
        let priv_keybind = AstKeybind {
            filename: "testdir/filename2.json".to_string(),
            principal: AstPrincipal { name: r#""jill""#.to_string() },
            is_pub: false,
        };
        let expected =
            AstProgram {
                assertions: Vec::new(),
                queries: Vec::new(),
                imports: Vec::new(),
                priv_binds: vec![priv_keybind],
                pub_binds: vec![pub_keybind],
        };
        
        assert!(result == expected);
    }
    
    #[test]
    fn test_import() {
        let prog = r#"import "jack" testdir/filename"#;
        
        let result = parse_program(prog);
        
        let import = AstImport {
            filename: "testdir/filename".to_string(),
            principal: AstPrincipal { name: r#""jack""#.to_string() },
        };
        let expected =
            AstProgram {
                assertions: Vec::new(),
                queries: Vec::new(),
                imports: vec![import],
                priv_binds: Vec::new(),
                pub_binds: Vec::new(),
        };
        
        assert!(result == expected);
    }
}