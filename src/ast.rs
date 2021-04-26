use std::any::Any;

#[derive(Debug, Hash)]
// Hash and Eq are needed so that AstPred can be used in a HashSet in
// SouffleEmitter. The derive strategy for Eq does not work, so it is
// implemented manually.
pub struct AstPred {
    pub name: String,
    pub args: Vec<String>
}

impl PartialEq for AstPred {
    fn eq(&self, other: &Self) -> bool {
        if (self.name != other.name ||
            self.args.len() != other.args.len()) {
            return false;
        }
        for i in (0..self.args.len()-1) {
            if ( self.args[i] != other.args[i] ) {
                return false;
            }
        }
        return true;
    }
}

// This is a hint that the compiler uses.
// See https://doc.rust-lang.org/std/cmp/trait.Eq.html
impl Eq for AstPred {}

#[derive(Debug)]
pub enum AstAssertion {
    AstFact {p: AstPred},
    AstCondAssert {lhs: AstPred, rhs: Vec<AstPred>}
}

#[derive(Debug)]
pub struct AstProgram {
    pub assertions: Vec<AstAssertion>
}

pub trait AstObject {
    fn get_string(&self) -> String;
    fn as_any(&self) -> &dyn Any;
}

impl AstObject for AstPred {
    fn get_string(&self) -> String {
        let mut ret = self.name.clone() + "(";
        for arg in &self.args {
            ret += &(arg.clone() + ", ");
        }
        ret += ")";
        ret
        // TODO remove last two chars
    }
    fn as_any(&self) -> &dyn Any { self }
}

impl AstObject for AstAssertion {
    fn get_string(&self) -> String {
        match self {
            AstAssertion::AstFact { p } =>
                p.get_string().to_owned() + ".",
            AstAssertion::AstCondAssert { lhs, rhs } => {
                let mut ret = lhs.get_string().to_owned();
                for pred in rhs {
                    ret += &pred.get_string().to_owned();
                }
                ret
            }
        }
    }
    fn as_any(&self) -> &dyn Any { self }
}

impl AstObject for AstProgram {
    fn get_string(&self) -> String { String::from("") }
    /*
    fn get_string(&self) -> String {
        let mut ret = String::from("");
        for assert in self.assertions {
            ret += assert.get_string().to_owned();
        }
        ret
    }
    */
    fn as_any(&self) -> &dyn Any { self }
}
