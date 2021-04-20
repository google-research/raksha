#![allow(nonstandard_style)]
// Generated from AuthLogic.g4 by ANTLR 4.8
use antlr_rust::tree::ParseTreeListener;
use super::authlogicparser::*;

pub trait AuthLogicListener<'input> : ParseTreeListener<'input,AuthLogicParserContextType>{

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

/**
 * Enter a parse tree produced by the {@code fact}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn enter_fact(&mut self, _ctx: &FactContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code fact}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn exit_fact(&mut self, _ctx: &FactContext<'input>) { }

/**
 * Enter a parse tree produced by the {@code hornclause}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn enter_hornclause(&mut self, _ctx: &HornclauseContext<'input>) { }
/**
 * Exit a parse tree produced by the {@code hornclause}
 * labeled alternative in {@link AuthLogicParser#assertion}.
 * @param ctx the parse tree
 */
fn exit_hornclause(&mut self, _ctx: &HornclauseContext<'input>) { }

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

}
