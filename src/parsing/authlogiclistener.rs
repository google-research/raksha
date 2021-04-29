#![allow(nonstandard_style)]
// Generated from AuthLogic.g4 by ANTLR 4.8
use antlr_rust::tree::ParseTreeListener;
use super::authlogicparser::*;

pub trait AuthLogicListener<'input> : ParseTreeListener<'input,AuthLogicParserContextType>{

/**
 * Enter a parse tree produced by {@link AuthLogicParser#principal}.
 * @param ctx the parse tree
 */
fn enter_principal(&mut self, _ctx: &PrincipalContext<'input>) { }
/**
 * Exit a parse tree produced by {@link AuthLogicParser#principal}.
 * @param ctx the parse tree
 */
fn exit_principal(&mut self, _ctx: &PrincipalContext<'input>) { }

/**
 * Enter a parse tree produced by {@link AuthLogicParser#predicate}.
 * @param ctx the parse tree
 */
fn enter_predicate(&mut self, _ctx: &PredicateContext<'input>) { }
/**
 * Exit a parse tree produced by {@link AuthLogicParser#predicate}.
 * @param ctx the parse tree
 */
fn exit_predicate(&mut self, _ctx: &PredicateContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code predphrase}
 * labeled alternative in {@link AuthLogicParser#verbphrase}.
 * @param ctx the parse tree
 */
fn enter_predphrase(&mut self, _ctx: &PredphraseContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code predphrase}
 * labeled alternative in {@link AuthLogicParser#verbphrase}.
 * @param ctx the parse tree
 */
fn exit_predphrase(&mut self, _ctx: &PredphraseContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code actsAsPhrase}
 * labeled alternative in {@link AuthLogicParser#verbphrase}.
 * @param ctx the parse tree
 */
fn enter_actsAsPhrase(&mut self, _ctx: &ActsAsPhraseContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code actsAsPhrase}
 * labeled alternative in {@link AuthLogicParser#verbphrase}.
 * @param ctx the parse tree
 */
fn exit_actsAsPhrase(&mut self, _ctx: &ActsAsPhraseContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code prinFact}
 * labeled alternative in {@link AuthLogicParser#flatFact}.
 * @param ctx the parse tree
 */
fn enter_prinFact(&mut self, _ctx: &PrinFactContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code prinFact}
 * labeled alternative in {@link AuthLogicParser#flatFact}.
 * @param ctx the parse tree
 */
fn exit_prinFact(&mut self, _ctx: &PrinFactContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code predFact}
 * labeled alternative in {@link AuthLogicParser#flatFact}.
 * @param ctx the parse tree
 */
fn enter_predFact(&mut self, _ctx: &PredFactContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code predFact}
 * labeled alternative in {@link AuthLogicParser#flatFact}.
 * @param ctx the parse tree
 */
fn exit_predFact(&mut self, _ctx: &PredFactContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code flatFactFact}
 * labeled alternative in {@link AuthLogicParser#fact}.
 * @param ctx the parse tree
 */
fn enter_flatFactFact(&mut self, _ctx: &FlatFactFactContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code flatFactFact}
 * labeled alternative in {@link AuthLogicParser#fact}.
 * @param ctx the parse tree
 */
fn exit_flatFactFact(&mut self, _ctx: &FlatFactFactContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code canSayFact}
 * labeled alternative in {@link AuthLogicParser#fact}.
 * @param ctx the parse tree
 */
fn enter_canSayFact(&mut self, _ctx: &CanSayFactContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code canSayFact}
 * labeled alternative in {@link AuthLogicParser#fact}.
 * @param ctx the parse tree
 */
fn exit_canSayFact(&mut self, _ctx: &CanSayFactContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code factAssertion}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn enter_factAssertion(&mut self, _ctx: &FactAssertionContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code factAssertion}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn exit_factAssertion(&mut self, _ctx: &FactAssertionContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code hornClauseAssertion}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn enter_hornClauseAssertion(&mut self, _ctx: &HornClauseAssertionContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code hornClauseAssertion}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn exit_hornClauseAssertion(&mut self, _ctx: &HornClauseAssertionContext<'input>) { }

/**
 * Enter a parse tree produced by {@link AuthLogicParser#saysAssertion}.
 * @param ctx the parse tree
 */
fn enter_saysAssertion(&mut self, _ctx: &SaysAssertionContext<'input>) { }
/**
 * Exit a parse tree produced by {@link AuthLogicParser#saysAssertion}.
 * @param ctx the parse tree
 */
fn exit_saysAssertion(&mut self, _ctx: &SaysAssertionContext<'input>) { }

/**
 * Enter a parse tree produced by {@link AuthLogicParser#program}.
 * @param ctx the parse tree
 */
fn enter_program(&mut self, _ctx: &ProgramContext<'input>) { }
/**
 * Exit a parse tree produced by {@link AuthLogicParser#program}.
 * @param ctx the parse tree
 */
fn exit_program(&mut self, _ctx: &ProgramContext<'input>) { }

}
