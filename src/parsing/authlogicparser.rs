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
		pub const SAYS:isize=9; 
		pub const CANACTAS:isize=10; 
		pub const CANSAY:isize=11; 
		pub const ID:isize=12; 
		pub const NEWLINE:isize=13;
	pub const RULE_principal:usize = 0; 
	pub const RULE_predicate:usize = 1; 
	pub const RULE_verbphrase:usize = 2; 
	pub const RULE_flatFact:usize = 3; 
	pub const RULE_fact:usize = 4; 
	pub const RULE_assertion:usize = 5; 
	pub const RULE_saysAssertion:usize = 6; 
	pub const RULE_program:usize = 7;
	pub const ruleNames: [&'static str; 8] =  [
		"principal", "predicate", "verbphrase", "flatFact", "fact", "assertion", 
		"saysAssertion", "program"
	];


	pub const _LITERAL_NAMES: [Option<&'static str>;12] = [
		None, Some("'('"), Some("', '"), Some("')'"), Some("'.'"), Some("' :- '"), 
		None, None, None, Some("' says '"), Some("' canActAs '"), Some("' canSay '")
	];
	pub const _SYMBOLIC_NAMES: [Option<&'static str>;14]  = [
		None, None, None, None, None, None, Some("DIGIT"), Some("IDSTARTCHAR"), 
		Some("IDCHAR"), Some("SAYS"), Some("CANACTAS"), Some("CANSAY"), Some("ID"), 
		Some("NEWLINE")
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
//------------------- principal ----------------
pub type PrincipalContextAll<'input> = PrincipalContext<'input>;


pub type PrincipalContext<'input> = BaseParserRuleContext<'input,PrincipalContextExt<'input>>;

#[derive(Clone)]
pub struct PrincipalContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for PrincipalContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for PrincipalContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_principal(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_principal(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for PrincipalContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_principal(self);
	}
}

impl<'input> CustomRuleContext<'input> for PrincipalContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_principal }
	//fn type_rule_index() -> usize where Self: Sized { RULE_principal }
}
antlr_rust::type_id!{PrincipalContextExt<'a>}

impl<'input> PrincipalContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<PrincipalContextAll<'input>> {
		Rc::new(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,PrincipalContextExt{
				ph:PhantomData
			}),
		)
	}
}

pub trait PrincipalContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<PrincipalContextExt<'input>>{

/// Retrieves first TerminalNode corresponding to token ID
/// Returns `None` if there is no child corresponding to token ID
fn ID(&self) -> Option<Rc<TerminalNode<'input,AuthLogicParserContextType>>> where Self:Sized{
	self.get_token(ID, 0)
}

}

impl<'input> PrincipalContextAttrs<'input> for PrincipalContext<'input>{}

impl<'input, I, H> AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
	pub fn principal(&mut self,)
	-> Result<Rc<PrincipalContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = PrincipalContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 0, RULE_principal);
        let mut _localctx: Rc<PrincipalContextAll> = _localctx;
		let result: Result<(), ANTLRError> = try {

			//recog.base.enter_outer_alt(_localctx.clone(), 1);
			recog.base.enter_outer_alt(None, 1);
			{
			recog.base.set_state(16);
			recog.base.match_token(ID,&mut recog.err_handler)?;

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
        recog.base.enter_rule(_localctx.clone(), 2, RULE_predicate);
        let mut _localctx: Rc<PredicateContextAll> = _localctx;
		let mut _la: isize;
		let result: Result<(), ANTLRError> = try {

			//recog.base.enter_outer_alt(_localctx.clone(), 1);
			recog.base.enter_outer_alt(None, 1);
			{
			recog.base.set_state(18);
			recog.base.match_token(ID,&mut recog.err_handler)?;

			recog.base.set_state(19);
			recog.base.match_token(T__0,&mut recog.err_handler)?;

			recog.base.set_state(20);
			recog.base.match_token(ID,&mut recog.err_handler)?;

			recog.base.set_state(25);
			recog.err_handler.sync(&mut recog.base)?;
			_la = recog.base.input.la(1);
			while _la==T__1 {
				{
				{
				recog.base.set_state(21);
				recog.base.match_token(T__1,&mut recog.err_handler)?;

				recog.base.set_state(22);
				recog.base.match_token(ID,&mut recog.err_handler)?;

				}
				}
				recog.base.set_state(27);
				recog.err_handler.sync(&mut recog.base)?;
				_la = recog.base.input.la(1);
			}
			recog.base.set_state(28);
			recog.base.match_token(T__2,&mut recog.err_handler)?;

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
//------------------- verbphrase ----------------
#[derive(Debug)]
pub enum VerbphraseContextAll<'input>{
	PredphraseContext(PredphraseContext<'input>),
	ActsAsPhraseContext(ActsAsPhraseContext<'input>),
Error(VerbphraseContext<'input>)
}
antlr_rust::type_id!{VerbphraseContextAll<'a>}

impl<'input> antlr_rust::parser_rule_context::DerefSeal for VerbphraseContextAll<'input>{}

impl<'input> AuthLogicParserContext<'input> for VerbphraseContextAll<'input>{}

impl<'input> Deref for VerbphraseContextAll<'input>{
	type Target = dyn VerbphraseContextAttrs<'input> + 'input;
	fn deref(&self) -> &Self::Target{
		use VerbphraseContextAll::*;
		match self{
			PredphraseContext(inner) => inner,
			ActsAsPhraseContext(inner) => inner,
Error(inner) => inner
		}
	}
}
impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for VerbphraseContextAll<'input>{
	fn accept(&self, visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) { self.deref().accept(visitor) }
}
impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for VerbphraseContextAll<'input>{
    fn enter(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().enter(listener) }
    fn exit(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().exit(listener) }
}



pub type VerbphraseContext<'input> = BaseParserRuleContext<'input,VerbphraseContextExt<'input>>;

#[derive(Clone)]
pub struct VerbphraseContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for VerbphraseContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for VerbphraseContext<'input>{
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for VerbphraseContext<'input>{
}

impl<'input> CustomRuleContext<'input> for VerbphraseContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_verbphrase }
	//fn type_rule_index() -> usize where Self: Sized { RULE_verbphrase }
}
antlr_rust::type_id!{VerbphraseContextExt<'a>}

impl<'input> VerbphraseContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<VerbphraseContextAll<'input>> {
		Rc::new(
		VerbphraseContextAll::Error(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,VerbphraseContextExt{
				ph:PhantomData
			}),
		)
		)
	}
}

pub trait VerbphraseContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<VerbphraseContextExt<'input>>{


}

impl<'input> VerbphraseContextAttrs<'input> for VerbphraseContext<'input>{}

pub type PredphraseContext<'input> = BaseParserRuleContext<'input,PredphraseContextExt<'input>>;

pub trait PredphraseContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn predicate(&self) -> Option<Rc<PredicateContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> PredphraseContextAttrs<'input> for PredphraseContext<'input>{}

pub struct PredphraseContextExt<'input>{
	base:VerbphraseContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{PredphraseContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for PredphraseContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for PredphraseContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_predphrase(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_predphrase(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for PredphraseContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_predphrase(self);
	}
}

impl<'input> CustomRuleContext<'input> for PredphraseContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_verbphrase }
	//fn type_rule_index() -> usize where Self: Sized { RULE_verbphrase }
}

impl<'input> Borrow<VerbphraseContextExt<'input>> for PredphraseContext<'input>{
	fn borrow(&self) -> &VerbphraseContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<VerbphraseContextExt<'input>> for PredphraseContext<'input>{
	fn borrow_mut(&mut self) -> &mut VerbphraseContextExt<'input> { &mut self.base }
}

impl<'input> VerbphraseContextAttrs<'input> for PredphraseContext<'input> {}

impl<'input> PredphraseContextExt<'input>{
	fn new(ctx: &dyn VerbphraseContextAttrs<'input>) -> Rc<VerbphraseContextAll<'input>>  {
		Rc::new(
			VerbphraseContextAll::PredphraseContext(
				BaseParserRuleContext::copy_from(ctx,PredphraseContextExt{
        			base: ctx.borrow().clone(),
        			ph:PhantomData
				})
			)
		)
	}
}

pub type ActsAsPhraseContext<'input> = BaseParserRuleContext<'input,ActsAsPhraseContextExt<'input>>;

pub trait ActsAsPhraseContextAttrs<'input>: AuthLogicParserContext<'input>{
	/// Retrieves first TerminalNode corresponding to token CANACTAS
	/// Returns `None` if there is no child corresponding to token CANACTAS
	fn CANACTAS(&self) -> Option<Rc<TerminalNode<'input,AuthLogicParserContextType>>> where Self:Sized{
		self.get_token(CANACTAS, 0)
	}
	fn principal(&self) -> Option<Rc<PrincipalContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> ActsAsPhraseContextAttrs<'input> for ActsAsPhraseContext<'input>{}

pub struct ActsAsPhraseContextExt<'input>{
	base:VerbphraseContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{ActsAsPhraseContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for ActsAsPhraseContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for ActsAsPhraseContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_actsAsPhrase(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_actsAsPhrase(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for ActsAsPhraseContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_actsAsPhrase(self);
	}
}

impl<'input> CustomRuleContext<'input> for ActsAsPhraseContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_verbphrase }
	//fn type_rule_index() -> usize where Self: Sized { RULE_verbphrase }
}

impl<'input> Borrow<VerbphraseContextExt<'input>> for ActsAsPhraseContext<'input>{
	fn borrow(&self) -> &VerbphraseContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<VerbphraseContextExt<'input>> for ActsAsPhraseContext<'input>{
	fn borrow_mut(&mut self) -> &mut VerbphraseContextExt<'input> { &mut self.base }
}

impl<'input> VerbphraseContextAttrs<'input> for ActsAsPhraseContext<'input> {}

impl<'input> ActsAsPhraseContextExt<'input>{
	fn new(ctx: &dyn VerbphraseContextAttrs<'input>) -> Rc<VerbphraseContextAll<'input>>  {
		Rc::new(
			VerbphraseContextAll::ActsAsPhraseContext(
				BaseParserRuleContext::copy_from(ctx,ActsAsPhraseContextExt{
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
	pub fn verbphrase(&mut self,)
	-> Result<Rc<VerbphraseContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = VerbphraseContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 4, RULE_verbphrase);
        let mut _localctx: Rc<VerbphraseContextAll> = _localctx;
		let result: Result<(), ANTLRError> = try {

			recog.base.set_state(33);
			recog.err_handler.sync(&mut recog.base)?;
			match recog.base.input.la(1) {
			 ID 
				=> {
					let tmp = PredphraseContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 1);
					_localctx = tmp;
					{
					/*InvokeRule predicate*/
					recog.base.set_state(30);
					recog.predicate()?;

					}
				}

			 CANACTAS 
				=> {
					let tmp = ActsAsPhraseContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 2);
					_localctx = tmp;
					{
					recog.base.set_state(31);
					recog.base.match_token(CANACTAS,&mut recog.err_handler)?;

					/*InvokeRule principal*/
					recog.base.set_state(32);
					recog.principal()?;

					}
				}

				_ => Err(ANTLRError::NoAltError(NoViableAltError::new(&mut recog.base)))?
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
//------------------- flatFact ----------------
#[derive(Debug)]
pub enum FlatFactContextAll<'input>{
	PrinFactContext(PrinFactContext<'input>),
	PredFactContext(PredFactContext<'input>),
Error(FlatFactContext<'input>)
}
antlr_rust::type_id!{FlatFactContextAll<'a>}

impl<'input> antlr_rust::parser_rule_context::DerefSeal for FlatFactContextAll<'input>{}

impl<'input> AuthLogicParserContext<'input> for FlatFactContextAll<'input>{}

impl<'input> Deref for FlatFactContextAll<'input>{
	type Target = dyn FlatFactContextAttrs<'input> + 'input;
	fn deref(&self) -> &Self::Target{
		use FlatFactContextAll::*;
		match self{
			PrinFactContext(inner) => inner,
			PredFactContext(inner) => inner,
Error(inner) => inner
		}
	}
}
impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for FlatFactContextAll<'input>{
	fn accept(&self, visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) { self.deref().accept(visitor) }
}
impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for FlatFactContextAll<'input>{
    fn enter(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().enter(listener) }
    fn exit(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().exit(listener) }
}



pub type FlatFactContext<'input> = BaseParserRuleContext<'input,FlatFactContextExt<'input>>;

#[derive(Clone)]
pub struct FlatFactContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for FlatFactContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for FlatFactContext<'input>{
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for FlatFactContext<'input>{
}

impl<'input> CustomRuleContext<'input> for FlatFactContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_flatFact }
	//fn type_rule_index() -> usize where Self: Sized { RULE_flatFact }
}
antlr_rust::type_id!{FlatFactContextExt<'a>}

impl<'input> FlatFactContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<FlatFactContextAll<'input>> {
		Rc::new(
		FlatFactContextAll::Error(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,FlatFactContextExt{
				ph:PhantomData
			}),
		)
		)
	}
}

pub trait FlatFactContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<FlatFactContextExt<'input>>{


}

impl<'input> FlatFactContextAttrs<'input> for FlatFactContext<'input>{}

pub type PrinFactContext<'input> = BaseParserRuleContext<'input,PrinFactContextExt<'input>>;

pub trait PrinFactContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn principal(&self) -> Option<Rc<PrincipalContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
	fn verbphrase(&self) -> Option<Rc<VerbphraseContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> PrinFactContextAttrs<'input> for PrinFactContext<'input>{}

pub struct PrinFactContextExt<'input>{
	base:FlatFactContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{PrinFactContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for PrinFactContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for PrinFactContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_prinFact(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_prinFact(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for PrinFactContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_prinFact(self);
	}
}

impl<'input> CustomRuleContext<'input> for PrinFactContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_flatFact }
	//fn type_rule_index() -> usize where Self: Sized { RULE_flatFact }
}

impl<'input> Borrow<FlatFactContextExt<'input>> for PrinFactContext<'input>{
	fn borrow(&self) -> &FlatFactContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<FlatFactContextExt<'input>> for PrinFactContext<'input>{
	fn borrow_mut(&mut self) -> &mut FlatFactContextExt<'input> { &mut self.base }
}

impl<'input> FlatFactContextAttrs<'input> for PrinFactContext<'input> {}

impl<'input> PrinFactContextExt<'input>{
	fn new(ctx: &dyn FlatFactContextAttrs<'input>) -> Rc<FlatFactContextAll<'input>>  {
		Rc::new(
			FlatFactContextAll::PrinFactContext(
				BaseParserRuleContext::copy_from(ctx,PrinFactContextExt{
        			base: ctx.borrow().clone(),
        			ph:PhantomData
				})
			)
		)
	}
}

pub type PredFactContext<'input> = BaseParserRuleContext<'input,PredFactContextExt<'input>>;

pub trait PredFactContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn predicate(&self) -> Option<Rc<PredicateContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> PredFactContextAttrs<'input> for PredFactContext<'input>{}

pub struct PredFactContextExt<'input>{
	base:FlatFactContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{PredFactContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for PredFactContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for PredFactContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_predFact(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_predFact(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for PredFactContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_predFact(self);
	}
}

impl<'input> CustomRuleContext<'input> for PredFactContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_flatFact }
	//fn type_rule_index() -> usize where Self: Sized { RULE_flatFact }
}

impl<'input> Borrow<FlatFactContextExt<'input>> for PredFactContext<'input>{
	fn borrow(&self) -> &FlatFactContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<FlatFactContextExt<'input>> for PredFactContext<'input>{
	fn borrow_mut(&mut self) -> &mut FlatFactContextExt<'input> { &mut self.base }
}

impl<'input> FlatFactContextAttrs<'input> for PredFactContext<'input> {}

impl<'input> PredFactContextExt<'input>{
	fn new(ctx: &dyn FlatFactContextAttrs<'input>) -> Rc<FlatFactContextAll<'input>>  {
		Rc::new(
			FlatFactContextAll::PredFactContext(
				BaseParserRuleContext::copy_from(ctx,PredFactContextExt{
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
	pub fn flatFact(&mut self,)
	-> Result<Rc<FlatFactContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = FlatFactContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 6, RULE_flatFact);
        let mut _localctx: Rc<FlatFactContextAll> = _localctx;
		let result: Result<(), ANTLRError> = try {

			recog.base.set_state(39);
			recog.err_handler.sync(&mut recog.base)?;
			match  recog.interpreter.adaptive_predict(2,&mut recog.base)? {
				1 =>{
					let tmp = PrinFactContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 1);
					_localctx = tmp;
					{
					/*InvokeRule principal*/
					recog.base.set_state(35);
					recog.principal()?;

					/*InvokeRule verbphrase*/
					recog.base.set_state(36);
					recog.verbphrase()?;

					}
				}
			,
				2 =>{
					let tmp = PredFactContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 2);
					_localctx = tmp;
					{
					/*InvokeRule predicate*/
					recog.base.set_state(38);
					recog.predicate()?;

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
//------------------- fact ----------------
#[derive(Debug)]
pub enum FactContextAll<'input>{
	FlatFactFactContext(FlatFactFactContext<'input>),
	CanSayFactContext(CanSayFactContext<'input>),
Error(FactContext<'input>)
}
antlr_rust::type_id!{FactContextAll<'a>}

impl<'input> antlr_rust::parser_rule_context::DerefSeal for FactContextAll<'input>{}

impl<'input> AuthLogicParserContext<'input> for FactContextAll<'input>{}

impl<'input> Deref for FactContextAll<'input>{
	type Target = dyn FactContextAttrs<'input> + 'input;
	fn deref(&self) -> &Self::Target{
		use FactContextAll::*;
		match self{
			FlatFactFactContext(inner) => inner,
			CanSayFactContext(inner) => inner,
Error(inner) => inner
		}
	}
}
impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for FactContextAll<'input>{
	fn accept(&self, visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) { self.deref().accept(visitor) }
}
impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for FactContextAll<'input>{
    fn enter(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().enter(listener) }
    fn exit(&self, listener: &mut (dyn AuthLogicListener<'input> + 'a)) { self.deref().exit(listener) }
}



pub type FactContext<'input> = BaseParserRuleContext<'input,FactContextExt<'input>>;

#[derive(Clone)]
pub struct FactContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for FactContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for FactContext<'input>{
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for FactContext<'input>{
}

impl<'input> CustomRuleContext<'input> for FactContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_fact }
	//fn type_rule_index() -> usize where Self: Sized { RULE_fact }
}
antlr_rust::type_id!{FactContextExt<'a>}

impl<'input> FactContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<FactContextAll<'input>> {
		Rc::new(
		FactContextAll::Error(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,FactContextExt{
				ph:PhantomData
			}),
		)
		)
	}
}

pub trait FactContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<FactContextExt<'input>>{


}

impl<'input> FactContextAttrs<'input> for FactContext<'input>{}

pub type FlatFactFactContext<'input> = BaseParserRuleContext<'input,FlatFactFactContextExt<'input>>;

pub trait FlatFactFactContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn flatFact(&self) -> Option<Rc<FlatFactContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> FlatFactFactContextAttrs<'input> for FlatFactFactContext<'input>{}

pub struct FlatFactFactContextExt<'input>{
	base:FactContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{FlatFactFactContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for FlatFactFactContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for FlatFactFactContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_flatFactFact(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_flatFactFact(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for FlatFactFactContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_flatFactFact(self);
	}
}

impl<'input> CustomRuleContext<'input> for FlatFactFactContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_fact }
	//fn type_rule_index() -> usize where Self: Sized { RULE_fact }
}

impl<'input> Borrow<FactContextExt<'input>> for FlatFactFactContext<'input>{
	fn borrow(&self) -> &FactContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<FactContextExt<'input>> for FlatFactFactContext<'input>{
	fn borrow_mut(&mut self) -> &mut FactContextExt<'input> { &mut self.base }
}

impl<'input> FactContextAttrs<'input> for FlatFactFactContext<'input> {}

impl<'input> FlatFactFactContextExt<'input>{
	fn new(ctx: &dyn FactContextAttrs<'input>) -> Rc<FactContextAll<'input>>  {
		Rc::new(
			FactContextAll::FlatFactFactContext(
				BaseParserRuleContext::copy_from(ctx,FlatFactFactContextExt{
        			base: ctx.borrow().clone(),
        			ph:PhantomData
				})
			)
		)
	}
}

pub type CanSayFactContext<'input> = BaseParserRuleContext<'input,CanSayFactContextExt<'input>>;

pub trait CanSayFactContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn principal(&self) -> Option<Rc<PrincipalContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
	/// Retrieves first TerminalNode corresponding to token CANSAY
	/// Returns `None` if there is no child corresponding to token CANSAY
	fn CANSAY(&self) -> Option<Rc<TerminalNode<'input,AuthLogicParserContextType>>> where Self:Sized{
		self.get_token(CANSAY, 0)
	}
	fn fact(&self) -> Option<Rc<FactContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> CanSayFactContextAttrs<'input> for CanSayFactContext<'input>{}

pub struct CanSayFactContextExt<'input>{
	base:FactContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{CanSayFactContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for CanSayFactContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for CanSayFactContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_canSayFact(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_canSayFact(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for CanSayFactContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_canSayFact(self);
	}
}

impl<'input> CustomRuleContext<'input> for CanSayFactContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_fact }
	//fn type_rule_index() -> usize where Self: Sized { RULE_fact }
}

impl<'input> Borrow<FactContextExt<'input>> for CanSayFactContext<'input>{
	fn borrow(&self) -> &FactContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<FactContextExt<'input>> for CanSayFactContext<'input>{
	fn borrow_mut(&mut self) -> &mut FactContextExt<'input> { &mut self.base }
}

impl<'input> FactContextAttrs<'input> for CanSayFactContext<'input> {}

impl<'input> CanSayFactContextExt<'input>{
	fn new(ctx: &dyn FactContextAttrs<'input>) -> Rc<FactContextAll<'input>>  {
		Rc::new(
			FactContextAll::CanSayFactContext(
				BaseParserRuleContext::copy_from(ctx,CanSayFactContextExt{
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
	pub fn fact(&mut self,)
	-> Result<Rc<FactContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = FactContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 8, RULE_fact);
        let mut _localctx: Rc<FactContextAll> = _localctx;
		let result: Result<(), ANTLRError> = try {

			recog.base.set_state(46);
			recog.err_handler.sync(&mut recog.base)?;
			match  recog.interpreter.adaptive_predict(3,&mut recog.base)? {
				1 =>{
					let tmp = FlatFactFactContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 1);
					_localctx = tmp;
					{
					/*InvokeRule flatFact*/
					recog.base.set_state(41);
					recog.flatFact()?;

					}
				}
			,
				2 =>{
					let tmp = CanSayFactContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 2);
					_localctx = tmp;
					{
					/*InvokeRule principal*/
					recog.base.set_state(42);
					recog.principal()?;

					recog.base.set_state(43);
					recog.base.match_token(CANSAY,&mut recog.err_handler)?;

					/*InvokeRule fact*/
					recog.base.set_state(44);
					recog.fact()?;

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
//------------------- assertion ----------------
#[derive(Debug)]
pub enum AssertionContextAll<'input>{
	FactAssertionContext(FactAssertionContext<'input>),
	HornClauseAssertionContext(HornClauseAssertionContext<'input>),
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
			FactAssertionContext(inner) => inner,
			HornClauseAssertionContext(inner) => inner,
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

pub type FactAssertionContext<'input> = BaseParserRuleContext<'input,FactAssertionContextExt<'input>>;

pub trait FactAssertionContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn fact(&self) -> Option<Rc<FactContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
}

impl<'input> FactAssertionContextAttrs<'input> for FactAssertionContext<'input>{}

pub struct FactAssertionContextExt<'input>{
	base:AssertionContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{FactAssertionContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for FactAssertionContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for FactAssertionContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_factAssertion(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_factAssertion(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for FactAssertionContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_factAssertion(self);
	}
}

impl<'input> CustomRuleContext<'input> for FactAssertionContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_assertion }
	//fn type_rule_index() -> usize where Self: Sized { RULE_assertion }
}

impl<'input> Borrow<AssertionContextExt<'input>> for FactAssertionContext<'input>{
	fn borrow(&self) -> &AssertionContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<AssertionContextExt<'input>> for FactAssertionContext<'input>{
	fn borrow_mut(&mut self) -> &mut AssertionContextExt<'input> { &mut self.base }
}

impl<'input> AssertionContextAttrs<'input> for FactAssertionContext<'input> {}

impl<'input> FactAssertionContextExt<'input>{
	fn new(ctx: &dyn AssertionContextAttrs<'input>) -> Rc<AssertionContextAll<'input>>  {
		Rc::new(
			AssertionContextAll::FactAssertionContext(
				BaseParserRuleContext::copy_from(ctx,FactAssertionContextExt{
        			base: ctx.borrow().clone(),
        			ph:PhantomData
				})
			)
		)
	}
}

pub type HornClauseAssertionContext<'input> = BaseParserRuleContext<'input,HornClauseAssertionContextExt<'input>>;

pub trait HornClauseAssertionContextAttrs<'input>: AuthLogicParserContext<'input>{
	fn fact(&self) -> Option<Rc<FactContextAll<'input>>> where Self:Sized{
		self.child_of_type(0)
	}
	fn flatFact_all(&self) ->  Vec<Rc<FlatFactContextAll<'input>>> where Self:Sized{
		self.children_of_type()
	}
	fn flatFact(&self, i: usize) -> Option<Rc<FlatFactContextAll<'input>>> where Self:Sized{
		self.child_of_type(i)
	}
}

impl<'input> HornClauseAssertionContextAttrs<'input> for HornClauseAssertionContext<'input>{}

pub struct HornClauseAssertionContextExt<'input>{
	base:AssertionContextExt<'input>,
	ph:PhantomData<&'input str>
}

antlr_rust::type_id!{HornClauseAssertionContextExt<'a>}

impl<'input> AuthLogicParserContext<'input> for HornClauseAssertionContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for HornClauseAssertionContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_hornClauseAssertion(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_hornClauseAssertion(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for HornClauseAssertionContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_hornClauseAssertion(self);
	}
}

impl<'input> CustomRuleContext<'input> for HornClauseAssertionContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_assertion }
	//fn type_rule_index() -> usize where Self: Sized { RULE_assertion }
}

impl<'input> Borrow<AssertionContextExt<'input>> for HornClauseAssertionContext<'input>{
	fn borrow(&self) -> &AssertionContextExt<'input> { &self.base }
}
impl<'input> BorrowMut<AssertionContextExt<'input>> for HornClauseAssertionContext<'input>{
	fn borrow_mut(&mut self) -> &mut AssertionContextExt<'input> { &mut self.base }
}

impl<'input> AssertionContextAttrs<'input> for HornClauseAssertionContext<'input> {}

impl<'input> HornClauseAssertionContextExt<'input>{
	fn new(ctx: &dyn AssertionContextAttrs<'input>) -> Rc<AssertionContextAll<'input>>  {
		Rc::new(
			AssertionContextAll::HornClauseAssertionContext(
				BaseParserRuleContext::copy_from(ctx,HornClauseAssertionContextExt{
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
        recog.base.enter_rule(_localctx.clone(), 10, RULE_assertion);
        let mut _localctx: Rc<AssertionContextAll> = _localctx;
		let mut _la: isize;
		let result: Result<(), ANTLRError> = try {

			recog.base.set_state(63);
			recog.err_handler.sync(&mut recog.base)?;
			match  recog.interpreter.adaptive_predict(5,&mut recog.base)? {
				1 =>{
					let tmp = FactAssertionContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 1);
					_localctx = tmp;
					{
					/*InvokeRule fact*/
					recog.base.set_state(48);
					recog.fact()?;

					recog.base.set_state(49);
					recog.base.match_token(T__3,&mut recog.err_handler)?;

					}
				}
			,
				2 =>{
					let tmp = HornClauseAssertionContextExt::new(&**_localctx);
					recog.base.enter_outer_alt(Some(tmp.clone()), 2);
					_localctx = tmp;
					{
					/*InvokeRule fact*/
					recog.base.set_state(51);
					recog.fact()?;

					recog.base.set_state(52);
					recog.base.match_token(T__4,&mut recog.err_handler)?;

					/*InvokeRule flatFact*/
					recog.base.set_state(53);
					recog.flatFact()?;

					recog.base.set_state(58);
					recog.err_handler.sync(&mut recog.base)?;
					_la = recog.base.input.la(1);
					while _la==T__1 {
						{
						{
						recog.base.set_state(54);
						recog.base.match_token(T__1,&mut recog.err_handler)?;

						/*InvokeRule flatFact*/
						recog.base.set_state(55);
						recog.flatFact()?;

						}
						}
						recog.base.set_state(60);
						recog.err_handler.sync(&mut recog.base)?;
						_la = recog.base.input.la(1);
					}
					recog.base.set_state(61);
					recog.base.match_token(T__3,&mut recog.err_handler)?;

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
//------------------- saysAssertion ----------------
pub type SaysAssertionContextAll<'input> = SaysAssertionContext<'input>;


pub type SaysAssertionContext<'input> = BaseParserRuleContext<'input,SaysAssertionContextExt<'input>>;

#[derive(Clone)]
pub struct SaysAssertionContextExt<'input>{
ph:PhantomData<&'input str>
}

impl<'input> AuthLogicParserContext<'input> for SaysAssertionContext<'input>{}

impl<'input,'a> Listenable<dyn AuthLogicListener<'input> + 'a> for SaysAssertionContext<'input>{
	fn enter(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.enter_every_rule(self);
		listener.enter_saysAssertion(self);
	}
	fn exit(&self,listener: &mut (dyn AuthLogicListener<'input> + 'a)) {
		listener.exit_saysAssertion(self);
		listener.exit_every_rule(self);
	}
}

impl<'input,'a> Visitable<dyn AuthLogicVisitor<'input> + 'a> for SaysAssertionContext<'input>{
	fn accept(&self,visitor: &mut (dyn AuthLogicVisitor<'input> + 'a)) {
		visitor.visit_saysAssertion(self);
	}
}

impl<'input> CustomRuleContext<'input> for SaysAssertionContextExt<'input>{
	type TF = LocalTokenFactory<'input>;
	type Ctx = AuthLogicParserContextType;
	fn get_rule_index(&self) -> usize { RULE_saysAssertion }
	//fn type_rule_index() -> usize where Self: Sized { RULE_saysAssertion }
}
antlr_rust::type_id!{SaysAssertionContextExt<'a>}

impl<'input> SaysAssertionContextExt<'input>{
	fn new(parent: Option<Rc<dyn AuthLogicParserContext<'input> + 'input > >, invoking_state: isize) -> Rc<SaysAssertionContextAll<'input>> {
		Rc::new(
			BaseParserRuleContext::new_parser_ctx(parent, invoking_state,SaysAssertionContextExt{
				ph:PhantomData
			}),
		)
	}
}

pub trait SaysAssertionContextAttrs<'input>: AuthLogicParserContext<'input> + BorrowMut<SaysAssertionContextExt<'input>>{

fn principal(&self) -> Option<Rc<PrincipalContextAll<'input>>> where Self:Sized{
	self.child_of_type(0)
}
/// Retrieves first TerminalNode corresponding to token SAYS
/// Returns `None` if there is no child corresponding to token SAYS
fn SAYS(&self) -> Option<Rc<TerminalNode<'input,AuthLogicParserContextType>>> where Self:Sized{
	self.get_token(SAYS, 0)
}
fn assertion(&self) -> Option<Rc<AssertionContextAll<'input>>> where Self:Sized{
	self.child_of_type(0)
}

}

impl<'input> SaysAssertionContextAttrs<'input> for SaysAssertionContext<'input>{}

impl<'input, I, H> AuthLogicParser<'input, I, H>
where
    I: TokenStream<'input, TF = LocalTokenFactory<'input> > + TidAble<'input>,
    H: ErrorStrategy<'input,BaseParserType<'input,I>>
{
	pub fn saysAssertion(&mut self,)
	-> Result<Rc<SaysAssertionContextAll<'input>>,ANTLRError> {
		let mut recog = self;
		let _parentctx = recog.ctx.take();
		let mut _localctx = SaysAssertionContextExt::new(_parentctx.clone(), recog.base.get_state());
        recog.base.enter_rule(_localctx.clone(), 12, RULE_saysAssertion);
        let mut _localctx: Rc<SaysAssertionContextAll> = _localctx;
		let result: Result<(), ANTLRError> = try {

			//recog.base.enter_outer_alt(_localctx.clone(), 1);
			recog.base.enter_outer_alt(None, 1);
			{
			/*InvokeRule principal*/
			recog.base.set_state(65);
			recog.principal()?;

			recog.base.set_state(66);
			recog.base.match_token(SAYS,&mut recog.err_handler)?;

			/*InvokeRule assertion*/
			recog.base.set_state(67);
			recog.assertion()?;

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

fn saysAssertion_all(&self) ->  Vec<Rc<SaysAssertionContextAll<'input>>> where Self:Sized{
	self.children_of_type()
}
fn saysAssertion(&self, i: usize) -> Option<Rc<SaysAssertionContextAll<'input>>> where Self:Sized{
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
        recog.base.enter_rule(_localctx.clone(), 14, RULE_program);
        let mut _localctx: Rc<ProgramContextAll> = _localctx;
		let mut _la: isize;
		let result: Result<(), ANTLRError> = try {

			//recog.base.enter_outer_alt(_localctx.clone(), 1);
			recog.base.enter_outer_alt(None, 1);
			{
			recog.base.set_state(72); 
			recog.err_handler.sync(&mut recog.base)?;
			_la = recog.base.input.la(1);
			loop {
				{
				{
				/*InvokeRule saysAssertion*/
				recog.base.set_state(69);
				recog.saysAssertion()?;

				recog.base.set_state(70);
				recog.base.match_token(NEWLINE,&mut recog.err_handler)?;

				}
				}
				recog.base.set_state(74); 
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
	\x0f\x4f\x04\x02\x09\x02\x04\x03\x09\x03\x04\x04\x09\x04\x04\x05\x09\x05\
	\x04\x06\x09\x06\x04\x07\x09\x07\x04\x08\x09\x08\x04\x09\x09\x09\x03\x02\
	\x03\x02\x03\x03\x03\x03\x03\x03\x03\x03\x03\x03\x07\x03\x1a\x0a\x03\x0c\
	\x03\x0e\x03\x1d\x0b\x03\x03\x03\x03\x03\x03\x04\x03\x04\x03\x04\x05\x04\
	\x24\x0a\x04\x03\x05\x03\x05\x03\x05\x03\x05\x05\x05\x2a\x0a\x05\x03\x06\
	\x03\x06\x03\x06\x03\x06\x03\x06\x05\x06\x31\x0a\x06\x03\x07\x03\x07\x03\
	\x07\x03\x07\x03\x07\x03\x07\x03\x07\x03\x07\x07\x07\x3b\x0a\x07\x0c\x07\
	\x0e\x07\x3e\x0b\x07\x03\x07\x03\x07\x05\x07\x42\x0a\x07\x03\x08\x03\x08\
	\x03\x08\x03\x08\x03\x09\x03\x09\x03\x09\x06\x09\x4b\x0a\x09\x0d\x09\x0e\
	\x09\x4c\x03\x09\x02\x02\x0a\x02\x04\x06\x08\x0a\x0c\x0e\x10\x02\x02\x02\
	\x4d\x02\x12\x03\x02\x02\x02\x04\x14\x03\x02\x02\x02\x06\x23\x03\x02\x02\
	\x02\x08\x29\x03\x02\x02\x02\x0a\x30\x03\x02\x02\x02\x0c\x41\x03\x02\x02\
	\x02\x0e\x43\x03\x02\x02\x02\x10\x4a\x03\x02\x02\x02\x12\x13\x07\x0e\x02\
	\x02\x13\x03\x03\x02\x02\x02\x14\x15\x07\x0e\x02\x02\x15\x16\x07\x03\x02\
	\x02\x16\x1b\x07\x0e\x02\x02\x17\x18\x07\x04\x02\x02\x18\x1a\x07\x0e\x02\
	\x02\x19\x17\x03\x02\x02\x02\x1a\x1d\x03\x02\x02\x02\x1b\x19\x03\x02\x02\
	\x02\x1b\x1c\x03\x02\x02\x02\x1c\x1e\x03\x02\x02\x02\x1d\x1b\x03\x02\x02\
	\x02\x1e\x1f\x07\x05\x02\x02\x1f\x05\x03\x02\x02\x02\x20\x24\x05\x04\x03\
	\x02\x21\x22\x07\x0c\x02\x02\x22\x24\x05\x02\x02\x02\x23\x20\x03\x02\x02\
	\x02\x23\x21\x03\x02\x02\x02\x24\x07\x03\x02\x02\x02\x25\x26\x05\x02\x02\
	\x02\x26\x27\x05\x06\x04\x02\x27\x2a\x03\x02\x02\x02\x28\x2a\x05\x04\x03\
	\x02\x29\x25\x03\x02\x02\x02\x29\x28\x03\x02\x02\x02\x2a\x09\x03\x02\x02\
	\x02\x2b\x31\x05\x08\x05\x02\x2c\x2d\x05\x02\x02\x02\x2d\x2e\x07\x0d\x02\
	\x02\x2e\x2f\x05\x0a\x06\x02\x2f\x31\x03\x02\x02\x02\x30\x2b\x03\x02\x02\
	\x02\x30\x2c\x03\x02\x02\x02\x31\x0b\x03\x02\x02\x02\x32\x33\x05\x0a\x06\
	\x02\x33\x34\x07\x06\x02\x02\x34\x42\x03\x02\x02\x02\x35\x36\x05\x0a\x06\
	\x02\x36\x37\x07\x07\x02\x02\x37\x3c\x05\x08\x05\x02\x38\x39\x07\x04\x02\
	\x02\x39\x3b\x05\x08\x05\x02\x3a\x38\x03\x02\x02\x02\x3b\x3e\x03\x02\x02\
	\x02\x3c\x3a\x03\x02\x02\x02\x3c\x3d\x03\x02\x02\x02\x3d\x3f\x03\x02\x02\
	\x02\x3e\x3c\x03\x02\x02\x02\x3f\x40\x07\x06\x02\x02\x40\x42\x03\x02\x02\
	\x02\x41\x32\x03\x02\x02\x02\x41\x35\x03\x02\x02\x02\x42\x0d\x03\x02\x02\
	\x02\x43\x44\x05\x02\x02\x02\x44\x45\x07\x0b\x02\x02\x45\x46\x05\x0c\x07\
	\x02\x46\x0f\x03\x02\x02\x02\x47\x48\x05\x0e\x08\x02\x48\x49\x07\x0f\x02\
	\x02\x49\x4b\x03\x02\x02\x02\x4a\x47\x03\x02\x02\x02\x4b\x4c\x03\x02\x02\
	\x02\x4c\x4a\x03\x02\x02\x02\x4c\x4d\x03\x02\x02\x02\x4d\x11\x03\x02\x02\
	\x02\x09\x1b\x23\x29\x30\x3c\x41\x4c";

