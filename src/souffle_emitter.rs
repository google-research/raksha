use std::collections::HashSet;
use crate::ast::*;
use crate::datalog_ir::*;

pub struct SouffleEmitter {
    decls: HashSet<AstPredicate>,
}

impl<'a> SouffleEmitter {
    // This is the only public method of the souffle emitter. When given
    // A Datalog IR (DLIR) program, it produces text for Souffle.
    pub fn emit_program(p: &DLIRProgram) -> String {
        let mut emitter = SouffleEmitter::new();
        // It is important to generate the body first in this case
        // because the declarations are populated by side-effect while
        // generating the body.
        let body = emitter.emit_program_body(p);
        let outputs = emitter.emit_outputs(p);
        let decls = emitter.emit_declarations();
        vec![decls, body, outputs].join("\r\n")
    }

    fn new() -> SouffleEmitter {
        SouffleEmitter { decls: HashSet::new() }
    }

    // This function produces a normalized version of predicates to
    // be used when generating declarations of the predicates. It replaces
    // argument names with generic numbered ones. It is applied
    // to predicates before they are added to the set of declarations so that
    // there are no duplicate delcarations (which would otherwise happen
    // whenever a predicate is referenced more than once with different
    // arguments)
    fn pred_to_declaration(p: &'a AstPredicate) -> AstPredicate {
        AstPredicate {
            name: p.name.clone(), 
            args: (0..p.args.len())
                .map(|i| String::from("x") + &i.to_string())
                .collect()
        }
    }

    fn emit_pred(&mut self, p: &'a AstPredicate) -> String {
        let decl = SouffleEmitter::pred_to_declaration(p);
        self.decls.insert(decl);
        format!("{}({})", &p.name,
            p.args.join(", "))
    }

    fn emit_assertion(&mut self, a: &'a DLIRAssertion) -> String {
        match a {
            DLIRAssertion::DLIRFactAssertion { p } => {
                self.emit_pred(p) + "."
            }
            DLIRAssertion::DLIRCondAssertion { lhs, rhs } => {
                format!("{} :- {}.",
                        self.emit_pred(lhs),
                        rhs.iter()
                            .map(|ast_pred| self.emit_pred(ast_pred))
                            .collect::<Vec<_>>()
                            .join(", ")
                    )
            }
        }
    }

    fn emit_program_body(&mut self, p: &'a DLIRProgram) -> String {
        p.assertions.iter()
            .map(|x| self.emit_assertion(&x))
            .collect::<Vec<_>>()
            .join("\r\n")
    }

    fn emit_decl(pred: &'a AstPredicate) -> String {
        format!(".decl {}({})", 
                &pred.name,
                &pred.args.iter()
                    .map(|x| format!("{}: symbol", x))
                    .collect::<Vec<_>>()
                    .join(", ")
        )
    }

    fn emit_declarations(&self) -> String {
        self.decls.iter()
            .map(|x| SouffleEmitter::emit_decl(x))
            .collect::<Vec<_>>()
            .join("\r\n")
    }

    fn emit_outputs(&self, p: &'a DLIRProgram) -> String {
        p.outputs.iter()
            .map(|o| String::from(".output ") + &o)
            .collect::<Vec<String>>()
            .join("\r\n")
    }

}

