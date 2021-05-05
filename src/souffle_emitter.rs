use std::collections::HashSet;
use crate::ast::*;
use crate::datalog_ir::*;

pub struct SouffleEmitter<'a> {
    decls: HashSet<&'a AstPredicate>,
}

impl<'a> SouffleEmitter<'a> {
    // This is the only public method of the souffle emitter. When given
    // A Datalog IR (DLIR) program, it produces text for Souffle.
    pub fn emit_program(p: &DLIRProgram) -> String {
        let mut emitter = SouffleEmitter::new();
        let body = emitter.emit_program_body(p);
        let mut text = emitter.emit_declarations();
        text += "\r\n";
        text += &body;
        text
    }

    fn new() -> SouffleEmitter<'a> {
        SouffleEmitter { decls: HashSet::new() }
    }

    fn emit_pred(&mut self, p: &'a AstPredicate) -> String {
        self.decls.insert(p);
        let mut ret = String::new();
        ret += &p.name;
        ret += "(";
        let mut first_arg_yet = false;
        for arg in &p.args {
            if !first_arg_yet {
                first_arg_yet = true;
            } else {
                ret += ", ";
            }
            ret += &arg;
        }
        ret += ")";
        ret
    }

    fn emit_assertion(&mut self, a: &'a DLIRAssertion) -> String {
        match a {
            DLIRAssertion::DLIRFactAssertion { p } => {
                self.emit_pred(p) + "."
            }
            DLIRAssertion::DLIRCondAssertion { lhs, rhs } => {
                let mut ret = self.emit_pred(lhs) + " :- ";
                let mut first_arg_yet = false;
                for p in rhs {
                    if !first_arg_yet {
                        first_arg_yet = true;
                    } else {
                        ret += ", ";
                    }
                    ret += &(self.emit_pred(p));
                }
                ret += ".";
                ret
            }
        }
    }

    fn emit_program_body(&mut self, p: &'a DLIRProgram) -> String {
        let mut ret = "".to_owned();
        for assertion in &p.assertions {
            ret += &self.emit_assertion(&assertion);
            ret += "\r\n";
        }
        ret
    }

    fn emit_decl(pred: &'a AstPredicate) -> String {
        let mut ret = ".decl ".to_owned();
        ret += &pred.name;
        ret += "(";
        let mut first_arg_yet = false;
        for arg in &pred.args {
            if !first_arg_yet {
                first_arg_yet = true;
            } else {
                ret += ", ";
            }
            ret += &arg;
            ret += ": symbol";
        }
        ret += ")";
        ret
    }

    fn emit_declarations(&self) -> String {
        let mut ret = String::new();
        for decl in &self.decls {
            ret += &SouffleEmitter::emit_decl(decl);
            ret += "\r\n";
        }
        ret
    }

}

