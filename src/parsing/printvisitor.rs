use crate::parsing::authlogicvisitor::*;
use crate::parsing::authlogicparser::*;
use antlr_rust::tree::{ParseTree,ParseTreeVisitor};

pub struct PrintVisitor {
    ret: String
}

impl PrintVisitor {
    pub fn new() -> PrintVisitor {
        PrintVisitor{ret: String::from("")}
    }
}

fn is_some<T> (x: Option<T>) -> bool {
    match x {
        Some(_) => true,
        None => false
    }
}

impl<'a> ParseTreeVisitor<'a, AuthLogicParserContextType> for PrintVisitor {}

impl<'a> AuthLogicVisitor<'a> for PrintVisitor {

	fn visit_program(&mut self, ctx: &ProgramContext<'a>) {
        self.visit_children(ctx)
    }

	fn visit_fact(&mut self, ctx: &FactContext<'a>) {
        self.visit_children(ctx)
    }

	fn visit_hornclause(&mut self, ctx: &HornclauseContext<'a>) {
        self.visit_children(ctx)
    }


	fn visit_predicate(&mut self, ctx: &PredicateContext<'a>) {
        // Clearly I would rather do (for id in <something>) { ... }
        // and ostensibly that something would be ID_all(), but actually it just emits all tokens 
        // including non-IDs (i.e., there is a bug). get_tokens(ID) does work, but it is private.
        // So instead, here is this hackery
        let mut idx = 0;
        while ( is_some(ctx.ID(idx)) ) {
            println!("id {}: {}", idx, ctx.ID(idx).unwrap().get_text());
            idx += 1;
        }
        self.visit_children(ctx)
    }

}
