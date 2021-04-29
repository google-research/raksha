// use std::any::Any;

#[derive(Debug)]
pub struct AstPrincipal {
    pub name: String
}

// Hash and Eq are needed so that AstPred can be used in a HashSet in
// SouffleEmitter. The derive strategy for Eq does not work, so it is
// implemented manually.
#[derive(Debug, Hash)]
pub struct AstPredicate {
    pub name: String,
    pub args: Vec<String>
}

impl PartialEq for AstPredicate {
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
impl Eq for AstPredicate {}

#[derive(Debug)]
pub enum AstVerbPhrase {
    AstPredPhrase {p: AstPredicate },
    AstCanActPhrase {p: AstPrincipal }
}

#[derive(Debug)]
pub enum AstFlatFact {
    AstPrinFact {p: AstPrincipal, v: AstVerbPhrase },
    AstPredFact {p: AstPredicate }
}

#[derive(Debug)]
pub enum AstFact {
    AstFlatFactFact { f: AstFlatFact },
    AstCanSayFact { p: AstPrincipal, f: AstFlatFact }
}

#[derive(Debug)]
pub enum AstAssertion {
    AstFactAssertion { f: AstFact },
    AstCondAssertion { lhs: AstFact, rhs: Vec<AstFlatFact> }
}

#[derive(Debug)]
pub struct AstSaysAssertion {
    pub prin: AstPrincipal,
    pub assertion: AstAssertion
}

#[derive(Debug)]
pub struct AstProgram {
    pub assertions: Vec<AstSaysAssertion>
}

// pub trait AstObject {
//     fn get_string(&self) -> String;
//     fn as_any(&self) -> &dyn Any;
// }
// 
// impl AstObject for AstPred {
//     fn get_string(&self) -> String {
//         let mut ret = self.name.clone() + "(";
//         for arg in &self.args {
//             ret += &(arg.clone() + ", ");
//         }
//         ret += ")";
//         ret
//         // TODO remove last two chars
//     }
//     fn as_any(&self) -> &dyn Any { self }
// }
// 
// impl AstObject for AstAssertion {
//     fn get_string(&self) -> String {
//         match self {
//             AstAssertion::AstFact { p } =>
//                 p.get_string().to_owned() + ".",
//             AstAssertion::AstCondAssert { lhs, rhs } => {
//                 let mut ret = lhs.get_string().to_owned();
//                 for pred in rhs {
//                     ret += &pred.get_string().to_owned();
//                 }
//                 ret
//             }
//         }
//     }
//     fn as_any(&self) -> &dyn Any { self }
// }
// 
// impl AstObject for AstProgram {
//     fn get_string(&self) -> String { String::from("") }
//     /*
//     fn get_string(&self) -> String {
//         let mut ret = String::from("");
//         for assert in self.assertions {
//             ret += assert.get_string().to_owned();
//         }
//         ret
//     }
//     */
//     fn as_any(&self) -> &dyn Any { self }
// }
