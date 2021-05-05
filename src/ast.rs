#[derive(Debug, Clone)]
pub struct AstPrincipal {
    pub name: String
}

// Hash and Eq are needed so that AstPred can be used in a HashSet in
// SouffleEmitter. The derive strategy for Eq does not work, so it is
// implemented manually.
#[derive(Debug, Hash, Clone)]
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
        for i in (0..self.args.len()-1) {
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

#[derive(Debug, Clone)]
pub enum AstVerbPhrase {
    AstPredPhrase { p: AstPredicate },
    AstCanActPhrase { p: AstPrincipal }
}

#[derive(Debug, Clone)]
pub enum AstFlatFact {
    AstPrinFact {p: AstPrincipal, v: AstVerbPhrase },
    AstPredFact {p: AstPredicate }
}

#[derive(Debug, Clone)]
pub enum AstFact {
    AstFlatFactFact { f: AstFlatFact },
    AstCanSayFact { p: AstPrincipal, f: Box<AstFact> }
}

#[derive(Debug, Clone)]
pub enum AstAssertion {
    AstFactAssertion { f: AstFact },
    AstCondAssertion { lhs: AstFact, rhs: Vec<AstFlatFact> }
}

#[derive(Debug, Clone)]
pub struct AstSaysAssertion {
    pub prin: AstPrincipal,
    pub assertion: AstAssertion
}

#[derive(Debug, Clone)]
pub struct AstProgram {
    pub assertions: Vec<AstSaysAssertion>
}
