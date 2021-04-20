// Generated from AuthLogic.g4 by ANTLR 4.8
#![allow(dead_code)]
#![allow(non_snake_case)]
#![allow(non_upper_case_globals)]
#![allow(nonstandard_style)]
#![allow(unused_imports)]
#![allow(unused_mut)]
use antlr_rust::PredictionContextCache;
use antlr_rust::parser::{Parser, BaseParser, ParserRecog, ParserNodeType};
use antlr_rust::token_stream::TokenStream;
use antlr_rust::TokenSource;
use antlr_rust::parser_atn_simulator::ParserATNSimulator;
use antlr_rust::errors::*;
use antlr_rust::rule_context::{BaseRuleContext, CustomRuleContext, RuleContext};
use antlr_rust::recognizer::{Recognizer,Actions};
use antlr_rust::atn_deserializer::ATNDeserializer;
use antlr_rust::dfa::DFA;
use antlr_rust::atn::{ATN, INVALID_ALT};
use antlr_rust::error_strategy::{ErrorStrategy, DefaultErrorStrategy};
use antlr_rust::parser_rule_context::{BaseParserRuleContext, ParserRuleContext,cast,cast_mut};
use antlr_rust::tree::*;
use antlr_rust::token::{TOKEN_EOF,OwningToken,Token};
use antlr_rust::int_stream::EOF;
use antlr_rust::vocabulary::{Vocabulary,VocabularyImpl};
use antlr_rust::token_factory::{CommonTokenFactory,TokenFactory, TokenAware};
use super::authlogiclistener::*;
use super::authlogicvisitor::*;

use antlr_rust::lazy_static;
use antlr_rust::{TidAble,TidExt};

use std::marker::PhantomData;
use std::sync::Arc;
use std::rc::Rc;
use std::convert::TryFrom;
use std::cell::RefCell;
use std::ops::{DerefMut, Deref};
use std::borrow::{Borrow,BorrowMut};
use std::any::{Any,TypeId};

		pub const T__0:isize=1; 
		pub const T__1:isize=2; 
		pub const T__2:isize=3; 
		pub const T__3:isize=4; 
		pub const T__4:isize=5; 
		pub const DIGIT:isize=6; 
		pub const IDSTARTCHAR:isize=7; 
		pub const IDCHAR:isize=8; 
		pub const ID:isize=9; 
		pub const NEWLINE:isize=10;
	pub const RULE_program:usize = 0; 
	pub const RULE_assertion:usize = 1; 
	pub const RULE_predicate:usize = 2;
	pub const ruleNames: [&'static str; 3] =  [
		"program", "assertion", "predicate"
	];


	pub const _LITERAL_NAMES: [Option<&'static str>;6] = [
		None, Some("'.'"), Some("' :- '"), Some("', '"), Some("'('"), Some("')'")
	];
	pub const _SYMBOLIC_NAMES: [Option<&'static str>;11]  = [
		None, None, None, None, None, None, Some("DIGIT"), Some("IDSTARTCHAR"), 
		Some("IDCHAR"), Some("ID"), Some("NEWLINE")
	];
	lazy_static!{
	    static ref _shared_context_cache: Arc<PredictionContextCache> = Arc::new(PredictionContextCache::new());
		static ref VOCABULARY: Box<dyn Vocabulary> = Box::new(VocabularyImpl::new(_LITERAL_NAMES.iter(), _SYMBOLIC_NAMES.iter(), None));
	}


type BaseParserType<'input, I> =
	BaseParser<'input,AuthLogicParserExt, I, AuthLogicParserContextType , dyn AuthLogicListener<'input> + 'input >;

type TokenType<'input> = <LocalTokenFactory<'input> as TokenFactory<'input>>::Tok;
pub type LocalTokenFactory<'input> = CommonTokenFactory;

pub type AuthLogicTreeWalker<'input,'a> =
	ParseTreeWalker<'input, 'a, AuthLogicParserContextType , dyn AuthLogicListener<'input> + 'a>;

/// Parser for AuthLogic grammar
pub struct AuthLogicParser<'input,I,H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
	base:BaseParserType<'input,I>,
	interpreter:Arc<ParserATNSimulator>,
	_shared_context_cache: Box<PredictionContextCache>,
    pub err_handler: H,
}

impl<'input, I, H> AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
	pub fn get_serialized_atn() -> &'static str { _serializedATN }

    pub fn set_error_strategy(&mut self, strategy: H) {
        self.err_handler = strategy
    }

    pub fn with_strategy(input: I, strategy: H) -> Self {
		antlr_rust::recognizer::check_version("0","2");
		let interpreter = Arc::new(ParserATNSimulator::new(
			_ATN.clone(),
			_decision_to_DFA.clone(),
			_shared_context_cache.clone(),
		));
		Self {
			base: BaseParser::new_base_parser(
				input,
				Arc::clone(&interpreter),
				AuthLogicParserExt{
				}
			),
			interpreter,
            _shared_context_cache: Box::new(PredictionContextCache::new()),
            err_handler: strategy,
        }
    }

}

type DynStrategy<'input,I> = Box<dyn ErrorStrategy<'input,BaseParserType<'input,I>> + 'input>;

impl<'input, I> AuthLogicParser<'input, I, DynStrategy<'input,I>>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
{
    pub fn with_dyn_strategy(input: I) -> Self{
    	Self::with_strategy(input,Box::new(DefaultErrorStrategy::new()))
    }
}

impl<'input, I> AuthLogicParser<'input, I, DefaultErrorStrategy<'input,AuthLogicParserContextType>>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
{
    pub fn new(input: I) -> Self{
    	Self::with_strategy(input,DefaultErrorStrategy::new())
    }
}

/// Trait for monomorphized trait object that corresponds to the nodes of parse tree generated for AuthLogicParser
pub trait AuthLogicParserContext<'input>:
	for<'x> Listenable<dyn AuthLogicListener<'input> + 'x > + 
	for<'x> Visitable<dyn AuthLogicVisitor<'input> + 'x > + 
	ParserRuleContext<'input, TF=LocalTokenFactory<'input>, Ctx=AuthLogicParserContextType>
{}

impl<'input, 'x, T> VisitableDyn<T> for dyn AuthLogicParserContext<'input> + 'input
where
    T: AuthLogicVisitor<'input> + 'x,
{
    fn accept_dyn(&self, visitor: &mut T) {
        self.accept(visitor as &mut (dyn AuthLogicVisitor<'input> + 'x))
    }
}

impl<'input> AuthLogicParserContext<'input> for TerminalNode<'input,AuthLogicParserContextType> {}
impl<'input> AuthLogicParserContext<'input> for ErrorNode<'input,AuthLogicParserContextType> {}

#[antlr_rust::impl_tid]
impl<'input> antlr_rust::TidAble<'input> for dyn AuthLogicParserContext<'input> + 'input{}

#[antlr_rust::impl_tid]
impl<'input> antlr_rust::TidAble<'input> for dyn AuthLogicListener<'input> + 'input{}

pub struct AuthLogicParserContextType;
antlr_rust::type_id!{AuthLogicParserContextType}

impl<'input> ParserNodeType<'input> for AuthLogicParserContextType{
	type TF = LocalTokenFactory<'input>;
	type Type = dyn AuthLogicParserContext<'input> + 'input;
}

impl<'input, I, H> Deref for AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
    type Target = BaseParserType<'input,I>;

    fn deref(&self) -> &Self::Target {
        &self.base
    }
}

impl<'input, I, H> DerefMut for AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.base
    }
}

pub struct AuthLogicParserExt{
}

impl AuthLogicParserExt{
}


impl<'input> TokenAware<'input> for AuthLogicParserExt{
	type TF = LocalTokenFactory<'input>;
}

impl<'input,I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>> ParserRecog<'input, BaseParserType<'input,I>> for AuthLogicParserExt{}

impl<'input,I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>> Actions<'input, BaseParserType<'input,I>> for AuthLogicParserExt{
	fn get_grammar_file_name(&self) -> & str{ "AuthLogic.g4"}

   	fn get_rule_names(&self) -> &[& str] {&ruleNames}

   	fn get_vocabulary(&self) -> &dyn Vocabulary { &**VOCABULARY }
}
//------------------- program ----------------
pub type ProgramContextAll<'input> = ProgramContext<'input>;


pub type ProgramContext<'input> = BaseParserRuleContext<'input,ProgramContextExt<'input>>;

#[derive(Clone)]
pub struct ProgramContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for ProgramContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for ProgramContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_program(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_program(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for ProgramContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_program(self);
	}
}

impl<'input> CustomRuleContext<'input> for ProgramContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_program }
	//fn type_rule_index() -> usize where Self: Sized { RULE_program }
}
antlr_rust::type_id!{ProgramContextExt<'a>}

impl<'input> ProgramContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<ProgramContextAll<'input>> {
		Rc::new(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,ProgramContextExt{
				ph:PhantomData
			}),
		)
	}
}

pub trait ProgramContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<ProgramContextExt<'input>>{

fn assertion_all(&self) ->  Vec<Rc<AssertionContextAll<'input>>> where Self:Sized{
	self.children_of_type()
}
fn assertion(&self, i: usize) -> Option<Rc<AssertionContextAll<'input>>> where Self:Sized{
	self.child_of_type(i)
}
/// Retrieves all `TerminalNode`s corresponding to token NEWLINE in current rule
fn NEWLINE_all(&self) -> Vec<Rc<TerminalNode<'input,AuthLogicParserContextType>>>  where Self:Sized{
	self.children_of_type()
}
/// Retrieves 'i's TerminalNode corresponding to token NEWLINE, starting from 0.
/// Returns `None` if number of children corresponding to token NEWLINE is less or equal than `i`.
fn NEWLINE(&self, i: usize) -> Option<Rc<TerminalNode<'input,AuthLogicParserContextType>>> where Self:Sized{
	self.get_token(NEWLINE, i)
}

}

impl<'input> ProgramContextAttrs<'input> for ProgramContext<'input>{}

impl<'input, I, H> AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
	pub fn program(&mut self,)
	-> Result<Rc<ProgramContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = ProgramContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 0, RULE_program);
        let mut _localctx: Rc<ProgramContextAll> = _localctx;
		let mut _la: isize;
		let result: Result<(), ANTLRError> = try {

			//recog.base.enter_outer_alt(_localctx.clone(), 1);
			recog.base.enter_outer_alt(None, 1);
			{
			recog.base.set_state(9); 
			recog.err_handler.sync(&mut recog.base)?;
			_la = recog.base.input.la(1);
			loop {
				{
				{
				/*InvokeRule assertion*/
				recog.base.set_state(6);
				recog.assertion()?;

				recog.base.set_state(7);
				recog.base.match_token(NEWLINE,&mut recog.err_handler)?;

				}
				}
				recog.base.set_state(11); 
				recog.err_handler.sync(&mut recog.base)?;
				_la = recog.base.input.la(1);
				if !(_la==ID) {break}
			}
			}
		};
		match result {
		Ok(_)=>{},
        Err(e @ ANTLRError::FallThrough(_)) => return Err(e),
		Err(ref re) => {
				//_localctx.exception = re;
				recog.err_handler.report_error(&mut recog.base, re);
				recog.err_handler.recover(&mut recog.base, re)?;
			}
		}
		recog.base.exit_rule();

		Ok(_localctx)
	}
}
//------------------- assertion ----------------
#[derive(Debug)]
pub enum AssertionContextAll<'input>{
	FactContext(FactContext<'input>),
	HornclauseContext(HornclauseContext<'input>),
Error(AssertionContext<'input>)
}
antlr_rust::type_id!{AssertionContextAll<'a>}

impl<'input> antlr_rust::parser_rule_context::DerefSeal for AssertionContextAll<'input>{}

impl<'input> AuthLogicParserContext<'input> for AssertionContextAll<'input>{}

impl<'input> Deref for AssertionContextAll<'input>{
	type Target = dyn AssertionContextAttrs<'input> + 'input;
	fn deref(&self) -> &Self::Target{
		use AssertionContextAll::*;
		match self{
			FactContext(inner) => inner,
			HornclauseContext(inner) => inner,
Error(inner) => inner
		}
	}
}
impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for AssertionContextAll<'input>{
	fn accept(&self, visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) { self.deref().accept(visitor) }
}
impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for AssertionContextAll<'input>{
    fn enter(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().enter(listener) }
    fn exit(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().exit(listener) }
}



pub type AssertionContext<'input> = BaseParserRuleContext<'input,AssertionContextExt<'input>>;

#[derive(Clone)]
pub struct AssertionContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for AssertionContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for AssertionContext<'input>{
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for AssertionContext<'input>{
}

impl<'input> CustomRuleContext<'input> for AssertionContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_assertion }
	//fn type_rule_index() -> usize where Self: Sized { RULE_assertion }
}
antlr_rust::type_id!{AssertionContextExt<'a>}

impl<'input> AssertionContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<AssertionContextAll<'input>> {
		Rc::new(
		AssertionContextAll::Error(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,AssertionContextExt{
				ph:PhantomData
			}),
		)
		)
	}
}

pub trait AssertionContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<AssertionContextExt<'input>>{


}

impl<'input> AssertionContextAttrs<'input> for AssertionContext<'input>{}

pub type FactContext<'input> = BaseParserRuleContext<'input,FactContextExt<'input>>;

pub trait FactContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn predicate(&self) -> Option<Rc<PredicateContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> FactContextAttrs<'input> for FactContext<'input>{}

pub struct FactContextExt<'input>{
	base:AssertionContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{FactContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for FactContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for FactContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_fact(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_fact(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for FactContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_fact(self);
	}
}

impl<'input> CustomRuleContext<'input> for FactContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_assertion }
	//fn type_rule_index() -> usize where Self: Sized { RULE_assertion }
}

impl<'input> Borrow<AssertionContextExt<'input>> for FactContext<'input>{
	fn borrow(&self) -> &AssertionContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<AssertionContextExt<'input>> for FactContext<'input>{
	fn borrow_mut(&mut self) -> &mut AssertionContextExt<'input> { &mut self.base }
}

impl<'input> AssertionContextAttrs<'input> for FactContext<'input> {}

impl<'input> FactContextExt<'input>{
	fn new(ctx: &dyn AssertionContextAttrs<'input>) -> Rc<AssertionContextAll<'input>>  {
		Rc::new(
			AssertionContextAll::FactContext(
				BaseParserRuleContext::copy_from(ctx,FactContextExt{
        			base: ctx.borrow().clone(),
        			ph:PhantomData
				})
			)
		)
	}
}

pub type HornclauseContext<'input> = BaseParserRuleContext<'input,HornclauseContextExt<'input>>;

pub trait HornclauseContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn predicate_all(&self) ->  Vec<Rc<PredicateContextAll<'input>>> where Self:Sized{
		self.children_of_type()
	}
	fn predicate(&self, i: usize) -> Option<Rc<PredicateContextAll<'input>>> where Self:Sized{
		self.child_of_type(i)
	}
}

impl<'input> HornclauseContextAttrs<'input> for HornclauseContext<'input>{}

pub struct HornclauseContextExt<'input>{
	base:AssertionContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{HornclauseContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for HornclauseContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for HornclauseContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_hornclause(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_hornclause(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for HornclauseContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_hornclause(self);
	}
}

impl<'input> CustomRuleContext<'input> for HornclauseContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_assertion }
	//fn type_rule_index() -> usize where Self: Sized { RULE_assertion }
}

impl<'input> Borrow<AssertionContextExt<'input>> for HornclauseContext<'input>{
	fn borrow(&self) -> &AssertionContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<AssertionContextExt<'input>> for HornclauseContext<'input>{
	fn borrow_mut(&mut self) -> &mut AssertionContextExt<'input> { &mut self.base }
}

impl<'input> AssertionContextAttrs<'input> for HornclauseContext<'input> {}

impl<'input> HornclauseContextExt<'input>{
	fn new(ctx: &dyn AssertionContextAttrs<'input>) -> Rc<AssertionContextAll<'input>>  {
		Rc::new(
			AssertionContextAll::HornclauseContext(
				BaseParserRuleContext::copy_from(ctx,HornclauseContextExt{
        			base: ctx.borrow().clone(),
        			ph:PhantomData
				})
			)
		)
	}
}

impl<'input, I, H> AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
	pub fn assertion(&mut self,)
	-> Result<Rc<AssertionContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = AssertionContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 2, RULE_assertion);
        let mut _localctx: Rc<AssertionContextAll> = _localctx;
		let mut _la: isize;
		let result: Result<(), ANTLRError> = try {

			recog.base.set_state(28);
			recog.err_handler.sync(&mut recog.base)?;
			match  recog.interpreter.adaptive_predict(2,&mut recog.base)? {
				1 =>{
					let tmp = FactContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 1);
					_localctx = tmp;
					{
					/*InvokeRule predicate*/
					recog.base.set_state(13);
					recog.predicate()?;

					recog.base.set_state(14);
					recog.base.match_token(T__0,&mut recog.err_handler)?;

					}
				}
			,
				2 =>{
					let tmp = HornclauseContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 2);
					_localctx = tmp;
					{
					/*InvokeRule predicate*/
					recog.base.set_state(16);
					recog.predicate()?;

					recog.base.set_state(17);
					recog.base.match_token(T__1,&mut recog.err_handler)?;

					/*InvokeRule predicate*/
					recog.base.set_state(18);
					recog.predicate()?;

					recog.base.set_state(23);
					recog.err_handler.sync(&mut recog.base)?;
					_la = recog.base.input.la(1);
					while _la==T__2 {
						{
						{
						recog.base.set_state(19);
						recog.base.match_token(T__2,&mut recog.err_handler)?;

						/*InvokeRule predicate*/
						recog.base.set_state(20);
						recog.predicate()?;

						}
						}
						recog.base.set_state(25);
						recog.err_handler.sync(&mut recog.base)?;
						_la = recog.base.input.la(1);
					}
					recog.base.set_state(26);
					recog.base.match_token(T__0,&mut recog.err_handler)?;

					}
				}

				_ => {}
			}
		};
		match result {
		Ok(_)=>{},
        Err(e @ ANTLRError::FallThrough(_)) => return Err(e),
		Err(ref re) => {
				//_localctx.exception = re;
				recog.err_handler.report_error(&mut recog.base, re);
				recog.err_handler.recover(&mut recog.base, re)?;
			}
		}
		recog.base.exit_rule();

		Ok(_localctx)
	}
}
//------------------- predicate ----------------
pub type PredicateContextAll<'input> = PredicateContext<'input>;


pub type PredicateContext<'input> = BaseParserRuleContext<'input,PredicateContextExt<'input>>;

#[derive(Clone)]
pub struct PredicateContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for PredicateContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for PredicateContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_predicate(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_predicate(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for PredicateContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_predicate(self);
	}
}

impl<'input> CustomRuleContext<'input> for PredicateContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_predicate }
	//fn type_rule_index() -> usize where Self: Sized { RULE_predicate }
}
antlr_rust::type_id!{PredicateContextExt<'a>}

impl<'input> PredicateContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<PredicateContextAll<'input>> {
		Rc::new(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,PredicateContextExt{
				ph:PhantomData
			}),
		)
	}
}

pub trait PredicateContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<PredicateContextExt<'input>>{

/// Retrieves all `TerminalNode`s corresponding to token ID in current rule
fn ID_all(&self) -> Vec<Rc<TerminalNode<'input,AuthLogicParserContextType>>>  where Self:Sized{
	self.children_of_type()
}
/// Retrieves 'i's TerminalNode corresponding to token ID, starting from 0.
/// Returns `None` if number of children corresponding to token ID is less or equal than `i`.
fn ID(&self, i: usize) -> Option<Rc<TerminalNode<'input,AuthLogicParserContextType>>> where Self:Sized{
	self.get_token(ID, i)
}

}

impl<'input> PredicateContextAttrs<'input> for PredicateContext<'input>{}

impl<'input, I, H> AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
	pub fn predicate(&mut self,)
	-> Result<Rc<PredicateContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = PredicateContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 4, RULE_predicate);
        let mut _localctx: Rc<PredicateContextAll> = _localctx;
		let mut _la: isize;
		let result: Result<(), ANTLRError> = try {

			//recog.base.enter_outer_alt(_localctx.clone(), 1);
			recog.base.enter_outer_alt(None, 1);
			{
			recog.base.set_state(30);
			recog.base.match_token(ID,&mut recog.err_handler)?;

			recog.base.set_state(31);
			recog.base.match_token(T__3,&mut recog.err_handler)?;

			recog.base.set_state(32);
			recog.base.match_token(ID,&mut recog.err_handler)?;

			recog.base.set_state(37);
			recog.err_handler.sync(&mut recog.base)?;
			_la = recog.base.input.la(1);
			while _la==T__2 {
				{
				{
				recog.base.set_state(33);
				recog.base.match_token(T__2,&mut recog.err_handler)?;

				recog.base.set_state(34);
				recog.base.match_token(ID,&mut recog.err_handler)?;

				}
				}
				recog.base.set_state(39);
				recog.err_handler.sync(&mut recog.base)?;
				_la = recog.base.input.la(1);
			}
			recog.base.set_state(40);
			recog.base.match_token(T__4,&mut recog.err_handler)?;

			}
		};
		match result {
		Ok(_)=>{},
        Err(e @ ANTLRError::FallThrough(_)) => return Err(e),
		Err(ref re) => {
				//_localctx.exception = re;
				recog.err_handler.report_error(&mut recog.base, re);
				recog.err_handler.recover(&mut recog.base, re)?;
			}
		}
		recog.base.exit_rule();

		Ok(_localctx)
	}
}

lazy_static! {
    static ref _ATN: Arc<ATN> =
        Arc::new(ATNDeserializer::new(None).deserialize(_serializedATN.chars()));
    static ref _decision_to_DFA: Arc<Vec<antlr_rust::RwLock<DFA>>> = {
        let mut dfa = Vec::new();
        let size = _ATN.decision_to_state.len();
        for i in 0..size {
            dfa.push(DFA::new(
                _ATN.clone(),
                _ATN.get_decision_state(i),
                i as isize,
            ).into())
        }
        Arc::new(dfa)
    };
}



const _serializedATN:&'static str =
	"\x03\u{608b}\u{a72a}\u{8133}\u{b9ed}\u{417c}\u{3be7}\u{7786}\u{5964}\x03\
	\x0c\x2d\x04\x02\x09\x02\x04\x03\x09\x03\x04\x04\x09\x04\x03\x02\x03\x02\
	\x03\x02\x06\x02\x0c\x0a\x02\x0d\x02\x0e\x02\x0d\x03\x03\x03\x03\x03\x03\
	\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03\x07\x03\x18\x0a\x03\x0c\x03\x0e\
	\x03\x1b\x0b\x03\x03\x03\x03\x03\x05\x03\x1f\x0a\x03\x03\x04\x03\x04\x03\
	\x04\x03\x04\x03\x04\x07\x04\x26\x0a\x04\x0c\x04\x0e\x04\x29\x0b\x04\x03\
	\x04\x03\x04\x03\x04\x02\x02\x05\x02\x04\x06\x02\x02\x02\x2d\x02\x0b\x03\
	\x02\x02\x02\x04\x1e\x03\x02\x02\x02\x06\x20\x03\x02\x02\x02\x08\x09\x05\
	\x04\x03\x02\x09\x0a\x07\x0c\x02\x02\x0a\x0c\x03\x02\x02\x02\x0b\x08\x03\
	\x02\x02\x02\x0c\x0d\x03\x02\x02\x02\x0d\x0b\x03\x02\x02\x02\x0d\x0e\x03\
	\x02\x02\x02\x0e\x03\x03\x02\x02\x02\x0f\x10\x05\x06\x04\x02\x10\x11\x07\
	\x03\x02\x02\x11\x1f\x03\x02\x02\x02\x12\x13\x05\x06\x04\x02\x13\x14\x07\
	\x04\x02\x02\x14\x19\x05\x06\x04\x02\x15\x16\x07\x05\x02\x02\x16\x18\x05\
	\x06\x04\x02\x17\x15\x03\x02\x02\x02\x18\x1b\x03\x02\x02\x02\x19\x17\x03\
	\x02\x02\x02\x19\x1a\x03\x02\x02\x02\x1a\x1c\x03\x02\x02\x02\x1b\x19\x03\
	\x02\x02\x02\x1c\x1d\x07\x03\x02\x02\x1d\x1f\x03\x02\x02\x02\x1e\x0f\x03\
	\x02\x02\x02\x1e\x12\x03\x02\x02\x02\x1f\x05\x03\x02\x02\x02\x20\x21\x07\
	\x0b\x02\x02\x21\x22\x07\x06\x02\x02\x22\x27\x07\x0b\x02\x02\x23\x24\x07\
	\x05\x02\x02\x24\x26\x07\x0b\x02\x02\x25\x23\x03\x02\x02\x02\x26\x29\x03\
	\x02\x02\x02\x27\x25\x03\x02\x02\x02\x27\x28\x03\x02\x02\x02\x28\x2a\x03\
	\x02\x02\x02\x29\x27\x03\x02\x02\x02\x2a\x2b\x07\x07\x02\x02\x2b\x07\x03\
	\x02\x02\x02\x06\x0d\x19\x1e\x27";

