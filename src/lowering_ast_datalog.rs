use crate::ast::*;
use crate::datalog_ir::*;

// TODO give a comment that explains:
//  - how the overall translation works
//  - that its implementation favors safety/generality over performance

fn fresh_var() -> String {
    String::from("x")
}

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

fn verbphrase_to_dlir(v: &AstVerbPhrase) -> AstPredicate {
    match v {
        AstVerbPhrase::AstPredPhrase { p: pred } => { pred.clone() }
        AstVerbPhrase::AstCanActPhrase { p: prin } => {
            let mut args_ = Vec::new();
            args_.push(prin.name.clone());
            AstPredicate{ name: String::from("canActAs_"), args: args_ }
        }
    }
}

// TODO need to collect some assertions here as well
fn flat_fact_to_dlir(f: &AstFlatFact) -> AstPredicate {
    match f {
        AstFlatFact::AstPrinFact {p, v} => {
            push_prin(String::from("is_"), p, &verbphrase_to_dlir(v))
        }
        // TODO collect assertions on this branch:
        AstFlatFact::AstPredFact { p }  => { p.clone() }
    }
}

// When reading this, it may be useful to keep in mind that
// facts (by contrast to flat facts) only appear on the LHS of assertions.
fn fact_to_dlir(f: &AstFact, p: &AstPrincipal)
        -> (AstPredicate, Vec<DLIRAssertion>) {
    match f {
        AstFact::AstFlatFactFact { f: flat } => {
            (
                flat_fact_to_dlir(flat),
                Vec::new()
            )
        }
        AstFact::AstCanSayFact { p: q, f: f_plus } => {
            let (fact_plus_prime, mut collected) = fact_to_dlir(&*f_plus, p);

            let f_prime = push_prin(String::from("canSay_"), q,
                &fact_plus_prime);
            let x = AstPrincipal { name: fresh_var() };

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

fn says_assertion_to_dlir(x: &AstSaysAssertion) -> Vec<DLIRAssertion> {
    match &x.assertion {
        AstAssertion::AstFactAssertion { f } => {
            let (pred, mut gen_assert) = fact_to_dlir(&f, &x.prin);
            let pred_prime = push_prin(String::from("says_"), &x.prin, &pred);
            gen_assert.push(DLIRAssertion::DLIRFactAssertion { p: pred_prime });
            gen_assert
        }
        AstAssertion::AstCondAssertion { lhs, rhs } => {
            let mut dlir_rhs = Vec::new();
            for f in rhs {
                let flat = flat_fact_to_dlir(&f);
                dlir_rhs.push(push_prin(String::from("says_"), &x.prin, &flat));
            }
            let (lhs_prime, mut assertions) = fact_to_dlir(&lhs, &x.prin);
            let dlir_lhs = push_prin(String::from("says_"), &x.prin,
                &lhs_prime);
            let this_assertion = DLIRAssertion::DLIRCondAssertion {
                lhs: dlir_lhs, rhs: dlir_rhs };
            assertions.push(this_assertion);
            assertions
        }
    }
}

pub fn prog_to_dlir(prog: &AstProgram) -> DLIRProgram {
    let mut dlir_assertions = Vec::new();
    for assert in &prog.assertions {
        let mut dlir_assert = says_assertion_to_dlir(&assert);
        dlir_assertions.append(&mut dlir_assert);
    }
    DLIRProgram { assertions: dlir_assertions }
}

