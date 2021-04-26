use std::collections::HashSet;
use crate::ast::*;

pub struct SouffleEmitter<'a> {
    decls: HashSet<&'a AstPred>,
}

impl<'a> SouffleEmitter<'a> {
    fn new() -> SouffleEmitter<'a> {
        SouffleEmitter { decls: HashSet::new() }
    }

    fn emit_pred(&mut self, p: &'a AstPred) -> String {
        self.decls.insert(p);
        let mut ret = String::new();
        ret += &p.name;
        ret += "(";
        let mut firstArgYet = false;
        for arg in &p.args {
            if !firstArgYet {
                firstArgYet = true;
            } else {
                ret += ", ";
            }
            ret += &arg;
        }
        ret += ")";
        ret
    }

    fn emit_assertion(&mut self, a: &'a AstAssertion) -> String {
        match a {
            AstAssertion::AstFact { p } => {
                self.emit_pred(p) + "."
            }
            AstAssertion::AstCondAssert { lhs, rhs } => {
                let mut ret = self.emit_pred(lhs) + " :- ";
                let mut firstArgYet = false;
                for p in rhs {
                    if !firstArgYet {
                        firstArgYet = true;
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

    fn emit_program_body(&mut self, p: &'a AstProgram) -> String {
        let mut ret = "".to_owned();
        for assertion in &p.assertions {
            ret += &self.emit_assertion(&assertion);
            ret += "\r\n";
        }
        ret
    }

    fn emit_decl(pred: &'a AstPred) -> String {
        let mut ret = ".decl ".to_owned();
        ret += &pred.name;
        ret += "(";
        let mut firstArgYet = false;
        for arg in &pred.args {
            if !firstArgYet {
                firstArgYet = true;
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

    pub fn emit_program(p: &AstProgram) -> String {
        let mut emitter = SouffleEmitter::new();
        let body = emitter.emit_program_body(p);
        let mut text = emitter.emit_declarations();
        text += "\r\n";
        text += &body;
        text
    }

}

