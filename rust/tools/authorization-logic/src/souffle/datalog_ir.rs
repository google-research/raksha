/// This file contains the datalog IR (DLIR) which makes the translation from
/// this authorization logic into datalog simpler.
use crate::ast::*;

#[derive(Clone)]
pub enum DLIRAssertion {
    DLIRFactAssertion {
        p: AstPredicate,
    },
    DLIRCondAssertion {
        lhs: AstPredicate,
        rhs: Vec<AstPredicate>,
    },
}

#[derive(Clone)]
pub struct DLIRProgram {
    pub assertions: Vec<DLIRAssertion>,
    pub outputs: Vec<String>,
}
