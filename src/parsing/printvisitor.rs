use crate::parsing::authlogicvisitor::*;
use crate::parsing::authlogicparser::*;
use antlr_rust::tree::{ParseTree,ParseTreeVisitor};

// this code is now vestigial, but shows roughly how to make a visitor that side-effects.

// fn test_print() {
//     let mut lexer = AuthLogicLexer::new(
//         InputStream::new("foo(bar, baz).\r\n"));
//     let token_source = CommonTokenStream::new(lexer);
//     let mut parser = AuthLogicParser::new(token_source);
//     let parse_result = parser.program().expect("failed to parse!");
//     let mut pvisitor = PrintVisitor::new();
//     pvisitor.visit_program(&parse_result);
// }

pub struct PrintVisitor {
    ret: String
}

impl PrintVisitor {
    pub fn new() -> PrintVisitor {
        PrintVisitor { ret: String::from("") }
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
        let mut idx = 0;
        while let Some(id) = ctx.ID(idx) {
            println!("id {}: {}", idx, id.get_text());
            idx += 1;
        }
        self.visit_children(ctx)
    }

}
