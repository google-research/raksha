#![allow(nonstandard_style)]
// Generated from AuthLogic.g4 by ANTLR 4.8
use antlr_rust::tree::{ParseTreeVisitor};
use super::authlogicparser::*;

/**
 * This interface defines a complete generic visitor for a parse tree produced
 * by {@link AuthLogicParser}.
 */
pub trait AuthLogicVisitor<'input>: ParseTreeVisitor<'input,AuthLogicParserContextType>{
	/**
	 * Visit a parse tree produced by {@link AuthLogicParser#program}.
	 * @param ctx the parse tree
	 */
	fn visit_program(&mut self, ctx: &ProgramContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code fact}
	 * labeled alternative in {@link AuthLogicParser#assertion}.
	 * @param ctx the parse tree
	 */
	fn visit_fact(&mut self, ctx: &FactContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code hornclause}
	 * labeled alternative in {@link AuthLogicParser#assertion}.
	 * @param ctx the parse tree
	 */
	fn visit_hornclause(&mut self, ctx: &HornclauseContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by {@link AuthLogicParser#predicate}.
	 * @param ctx the parse tree
	 */
	fn visit_predicate(&mut self, ctx: &PredicateContext<'input>) { self.visit_children(ctx) }


}