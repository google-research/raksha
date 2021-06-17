use serde::*;

// Hash and Eq are implemented by AstPrincipal because they are used as keys
// in HashMaps in the implementation of signature exporting
#[derive(Debug, Hash, PartialEq, Eq, Clone, Serialize, Deserialize)]
pub struct AstPrincipal {
    pub name: String
}

// Hash and Eq are needed so that AstPred can be used in a HashSet in
// SouffleEmitter. The derive strategy for Eq does not work, so it is
// implemented manually.
#[derive(Debug, Hash, Clone, Serialize, Deserialize)]
pub struct AstPredicate {
    pub name: String,
    pub args: Vec<String>
}

impl PartialEq for AstPredicate {
    fn eq(&self, other: &Self) -> bool {
        if self.name != other.name ||
            self.args.len() != other.args.len() {
            return false;
        }
        for i in 0..self.args.len() - 1 {
            if self.args[i] != other.args[i] {
                return false;
            }
        }
        return true;
    }
}

// This is a hint that the compiler uses.
// See https://doc.rust-lang.org/std/cmp/trait.Eq.html
impl Eq for AstPredicate {}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstVerbPhrase {
    AstPredPhrase { p: AstPredicate },
    AstCanActPhrase { p: AstPrincipal }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstFlatFact {
    AstPrinFact { p: AstPrincipal, v: AstVerbPhrase },
    AstPredFact { p: AstPredicate }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstFact {
    AstFlatFactFact { f: AstFlatFact },
    AstCanSayFact { p: AstPrincipal, f: Box<AstFact> }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub enum AstAssertion {
    AstFactAssertion { f: AstFact },
    AstCondAssertion { lhs: AstFact, rhs: Vec<AstFlatFact> }
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstSaysAssertion {
    pub prin: AstPrincipal,
    pub assertions: Vec<AstAssertion>,
    pub export_file: Option<String>
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstQuery {
    pub name: String,
    pub principal: AstPrincipal,
    pub fact: AstFact
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstKeybind {
    pub filename: String,
    pub principal: AstPrincipal,
    pub is_pub: bool
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstImport {
    pub filename: String,
    pub principal: AstPrincipal
}

#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AstProgram {
    pub assertions: Vec<AstSaysAssertion>,
    pub queries: Vec<AstQuery>,
    pub imports: Vec<AstImport>,
    pub priv_binds: Vec<AstKeybind>,
    pub pub_binds: Vec<AstKeybind>,
}

