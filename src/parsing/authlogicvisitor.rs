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
	 * Visit a parse tree produced by {@link AuthLogicParser#principal}.
	 * @param ctx the parse tree
	 */
	fn visit_principal(&mut self, ctx: &PrincipalContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by {@link AuthLogicParser#predicate}.
	 * @param ctx the parse tree
	 */
	fn visit_predicate(&mut self, ctx: &PredicateContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code predphrase}
	 * labeled alternative in {@link AuthLogicParser#verbphrase}.
	 * @param ctx the parse tree
	 */
	fn visit_predphrase(&mut self, ctx: &PredphraseContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code actsAsPhrase}
	 * labeled alternative in {@link AuthLogicParser#verbphrase}.
	 * @param ctx the parse tree
	 */
	fn visit_actsAsPhrase(&mut self, ctx: &ActsAsPhraseContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code prinFact}
	 * labeled alternative in {@link AuthLogicParser#flatFact}.
	 * @param ctx the parse tree
	 */
	fn visit_prinFact(&mut self, ctx: &PrinFactContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code predFact}
	 * labeled alternative in {@link AuthLogicParser#flatFact}.
	 * @param ctx the parse tree
	 */
	fn visit_predFact(&mut self, ctx: &PredFactContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code flatFactFact}
	 * labeled alternative in {@link AuthLogicParser#fact}.
	 * @param ctx the parse tree
	 */
	fn visit_flatFactFact(&mut self, ctx: &FlatFactFactContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code canSayFact}
	 * labeled alternative in {@link AuthLogicParser#fact}.
	 * @param ctx the parse tree
	 */
	fn visit_canSayFact(&mut self, ctx: &CanSayFactContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code factAssertion}
	 * labeled alternative in {@link AuthLogicParser#assertion}.
	 * @param ctx the parse tree
	 */
	fn visit_factAssertion(&mut self, ctx: &FactAssertionContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by the {@code hornClauseAssertion}
	 * labeled alternative in {@link AuthLogicParser#assertion}.
	 * @param ctx the parse tree
	 */
	fn visit_hornClauseAssertion(&mut self, ctx: &HornClauseAssertionContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by {@link AuthLogicParser#saysAssertion}.
	 * @param ctx the parse tree
	 */
	fn visit_saysAssertion(&mut self, ctx: &SaysAssertionContext<'input>) { self.visit_children(ctx) }

	/**
	 * Visit a parse tree produced by {@link AuthLogicParser#program}.
	 * @param ctx the parse tree
	 */
	fn visit_program(&mut self, ctx: &ProgramContext<'input>) { self.visit_children(ctx) }


}