use crate::parsing::authlogicvisitor::*;
use crate::parsing::authlogicparser::*;
use crate::ast::*;
use antlr_rust::tree::{ParseTree,ParseTreeVisitor};

fn is_some<T> (x: Option<T>) -> bool {
    match x {
        Some(_) => true,
        None => false
    }
}

pub struct AstConstructionVisitor {
    ret: Box<dyn AstObject>
}

impl AstConstructionVisitor {
    fn construct_program(&mut self, ctx: &ProgramContext) -> AstProgram {
        let mut assertions = Vec::new();
        for assertion_ctx in ctx.assertion_all() {
            assertions.push(self.construct_assertion(&assertion_ctx));
        }
        AstProgram { assertions }
    }

    fn construct_assertion(&mut self, ctx: &AssertionContextAll)
        -> AstAssertion {
            match ctx {
                AssertionContextAll::FactContext(fctx) => {
                    self.construct_fact(fctx)
                }
                AssertionContextAll::HornclauseContext(hctx) => {
                    self.construct_hornclause(hctx)
                }
                _ => {
                    panic!("construct_assertion tried to build error");
                }
            }
    }

    fn construct_fact(&mut self, ctx: &FactContext) -> AstAssertion {
        let pred_context = ctx.predicate().unwrap();
        let pred = self.construct_predicate(&pred_context);
        AstAssertion::AstFact { p: pred }
    }

    fn construct_hornclause(&mut self, ctx: &HornclauseContext)
        -> AstAssertion {
            let mut rhs = Vec::new();
            let mut some_lhs = None;
            let mut parsed_head = false;
            for pred_ctx in ctx.predicate_all() {
                if(!parsed_head) {
                    parsed_head = true;
                    some_lhs = Some(self.construct_predicate(&pred_ctx));
                } else {
                    rhs.push(self.construct_predicate(&pred_ctx));
                }
            }
            let lhs = some_lhs.unwrap();
            AstAssertion::AstCondAssert{ lhs, rhs }

    }
    
    fn construct_predicate(&mut self, ctx: &PredicateContext) -> AstPred {
        let name_ = ctx.ID(0).unwrap().get_text();
        let mut args_ = Vec::new();
        let mut idx = 1;
        while ( is_some(ctx.ID(idx)) ) {
            args_.push(ctx.ID(idx).unwrap().get_text());
            idx += 1;
        }
        AstPred { name: name_, args: args_ }
    }
}

impl<'a> ParseTreeVisitor<'a, AuthLogicParserContextType> for AstConstructionVisitor {}

impl<'a> AuthLogicVisitor<'a> for AstConstructionVisitor {
	fn visit_program(&mut self, ctx: &ProgramContext<'a>) {
        self.visit_children(ctx)
    }

	fn visit_fact(&mut self, ctx: &FactContext<'a>) {
        // let pc = ctx.predicate().unwrap();
        // self.visit_predicate(&pc);
        // let pred = self.ret.as_any()
        //     .downcast_ref::<AstPred>()
        //     .unwrap();
        // self.ret = Box::new(AstAssertion::AstFact { p: pred })
        self.visit_children(ctx)
    }

	fn visit_hornclause(&mut self, ctx: &HornclauseContext<'a>) {
        self.visit_children(ctx)
    }


	fn visit_predicate(&mut self, ctx: &PredicateContext<'a>) {
        let name_ = ctx.ID(0).unwrap().get_text();
        let mut args_ = Vec::new();
        // Clearly I would rather do (for id in <something>) { ... }
        // and ostensibly that something would be ID_all(), but actually it just emits all tokens 
        // including non-IDs (i.e., there is a bug). get_tokens(ID) does work, but it is private.
        // So instead, here is this hackery with while (is_some ...() )
        let mut idx = 1;
        while ( is_some(ctx.ID(idx)) ) {
            args_.push(ctx.ID(idx).unwrap().get_text());
            idx += 1;
        }
        self.ret = Box::new(AstPred { name: name_, args: args_ });
    }

}
