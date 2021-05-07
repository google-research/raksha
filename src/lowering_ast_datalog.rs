use crate::ast::*;
use crate::datalog_ir::*;

// TODO give a comment that explains:
//  - how the overall translation works
//  - that its implementation favors safety/generality over performance


// Note that this puts args_ on the front of the list of arguments because
// this is the conveninet way for it to work in the contexts in which it
// is used.
fn push_onto_pred(modifier: String, mut args_: Vec<String>,
                  pred: &AstPredicate) -> AstPredicate {
    let new_name = modifier + &pred.name.clone();
    for a in &pred.args {
        args_.push(a.to_string());
    }
    AstPredicate { name: new_name, args: args_.to_vec() }
}

fn push_prin(modifier: String, p: &AstPrincipal,
             pred: &AstPredicate) -> AstPredicate {
    let mut args = Vec::new();
    args.push(p.name.clone());
    push_onto_pred(modifier, args, pred)
}

// This struct only contains a counter for generating new
// fresh variables.
pub struct LoweringToDatalogPass {
    fresh_var_count: u32
}

impl LoweringToDatalogPass {

    // This is the only public function in the struct.
    pub fn lower(prog: &AstProgram) -> DLIRProgram {
        let mut lowering_pass = LoweringToDatalogPass::new();
        lowering_pass.prog_to_dlir(prog)
    }

    fn new() -> LoweringToDatalogPass {
        LoweringToDatalogPass { fresh_var_count: 0 }
    }

    fn fresh_var(&mut self) -> String {
        // Technically it is possible to have a collision with
        // a programmer-entered variable because the syntax of 
        // identifiers for this language and souffle is the same.
        // However, the hope is that programmers will not usually
        // write variable names with 3 underscores.
        self.fresh_var_count += 1;
        String::from("x___") + &self.fresh_var_count.to_string()
    }
    
    fn verbphrase_to_dlir(&mut self, v: &AstVerbPhrase) -> AstPredicate {
        match v {
            AstVerbPhrase::AstPredPhrase { p: pred } => { pred.clone() }
            AstVerbPhrase::AstCanActPhrase { p: prin } => {
                let mut args_ = Vec::new();
                args_.push(prin.name.clone());
                AstPredicate{ name: String::from("canActAs"), args: args_ }
            }
        }
    }
    
    fn flat_fact_to_dlir(&mut self, f: &AstFlatFact, in_lhs: bool,
             speaker: &AstPrincipal) -> (AstPredicate,Vec<DLIRAssertion>) {
        match f {
            AstFlatFact::AstPrinFact {p, v} => {
                let v_dlir = self.verbphrase_to_dlir(v);
                let pred = push_prin(String::from(""), p, &v_dlir);
    
                // Need to additionally generate:
                // speaker says x v :- speaker says x canActAs p,
                //                     speaker says p v
                // (where x is a fresh principal)
                let x = AstPrincipal { name: self.fresh_var() };
    
                // speaker says x v;
                let gen_lhs = push_prin(String::from("says_"), speaker, 
                        &push_prin(String::from(""), &x, &v_dlir));
                // speaker says x canActAs p
                let x_as_p = AstPredicate { name: String::from("canActAs"),
                    args: [x.name.clone(), p.name.clone()].to_vec() };
                let s_says_x_as_p = push_prin(String::from("says_"), speaker,
                    &x_as_p);
                // speaker says p v
                let s_says_p_v = push_prin(String::from("says_"), speaker,
                    &pred);
            
                let gen = DLIRAssertion::DLIRCondAssertion { lhs: gen_lhs,
                    rhs: [s_says_x_as_p, s_says_p_v].to_vec() };
    
                (pred, [gen].to_vec())
            }
            AstFlatFact::AstPredFact { p }  => { (p.clone(), Vec::new()) }
        }
    }
    
    // When reading this, it may be useful to keep in mind that
    // facts (by contrast to flat facts) only appear on the LHS of assertions.
    fn fact_to_dlir(&mut self, f: &AstFact, p: &AstPrincipal)
            -> (AstPredicate, Vec<DLIRAssertion>) {
        match f {
            AstFact::AstFlatFactFact { f: flat } => {
                self.flat_fact_to_dlir(flat, true, p)
            }
            AstFact::AstCanSayFact { p: q, f: f_plus } => {
                let (fact_plus_prime, mut collected) = self.fact_to_dlir(&*f_plus, p);
    
                let f_prime = push_prin(String::from("canSay_"), q,
                    &fact_plus_prime);
                let x = AstPrincipal { name: self.fresh_var() };
    
                // Note that f_prime does not begin with "p says"
                // because only the top-level fact should. This could
                // be a recursive call, so it might not be processing
                // the top-level fact. The top-level fact gets wrapped
                // in a "says" during the call to translate the assertion 
                // in which this appears.
                
                // The following code generates
                // p says fpf :- x says fpf, p says x canSay fpf
                // where fpf is fact_plus_prime
                
                // p says fact_plus_prime
                let lhs = push_prin(String::from("says_"), p,
                    &fact_plus_prime);
                // x says fact_plus_prime
                let x_says_term = push_prin(String::from("says_"), &x,
                    &fact_plus_prime);
                // p says x canSay fact_plus_prime
                let can_say_term = push_prin(String::from("says_"), p,
                    &push_prin(String::from("canSay_"), &x,
                    &fact_plus_prime));
                // p says fpf :- x says fpf, p says x canSay fpf
                let mut rhs = Vec::new();
                rhs.push(x_says_term);
                rhs.push(can_say_term);
                let gen = DLIRAssertion::DLIRCondAssertion { lhs, rhs };
                
                collected.push(gen);
                (f_prime, collected)
            }
        }
    }
    
    fn says_assertion_to_dlir(&mut self, x: &AstSaysAssertion) -> Vec<DLIRAssertion> {
        match &x.assertion {
            AstAssertion::AstFactAssertion { f } => {
                let (pred, mut gen_assert) = self.fact_to_dlir(&f, &x.prin);
                let pred_prime = push_prin(String::from("says_"), &x.prin, &pred);
                gen_assert.push(DLIRAssertion::DLIRFactAssertion { p: pred_prime });
                gen_assert
            }
            AstAssertion::AstCondAssertion { lhs, rhs } => {
                let mut dlir_rhs = Vec::new();
                for f in rhs {
                    let (flat, _) = self.flat_fact_to_dlir(&f, false, &x.prin);
                    dlir_rhs.push(push_prin(String::from("says_"),
                        &x.prin, &flat));
                }
                let (lhs_prime, mut assertions) =
                    self.fact_to_dlir(&lhs, &x.prin);
                let dlir_lhs = push_prin(String::from("says_"), &x.prin,
                    &lhs_prime);
                let this_assertion = DLIRAssertion::DLIRCondAssertion {
                    lhs: dlir_lhs, rhs: dlir_rhs };
                assertions.push(this_assertion);
                assertions
            }
        }
    }
    
    fn prog_to_dlir(&mut self, prog: &AstProgram) -> DLIRProgram {
        let mut dlir_assertions = Vec::new();
        for assert in &prog.assertions {
            let mut dlir_assert = self.says_assertion_to_dlir(&assert);
            dlir_assertions.append(&mut dlir_assert);
        }
        DLIRProgram { assertions: dlir_assertions }
    }
}

