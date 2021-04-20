// Generated from AuthLogic.g4 by ANTLR 4.8
#![allow(dead_code)]
#![allow(nonstandard_style)]
#![allow(unused_imports)]
#![allow(unused_variables)]
use antlr_rust::atn::ATN;
use antlr_rust::char_stream::CharStream;
use antlr_rust::int_stream::IntStream;
use antlr_rust::lexer::{BaseLexer, Lexer, LexerRecog};
use antlr_rust::atn_deserializer::ATNDeserializer;
use antlr_rust::dfa::DFA;
use antlr_rust::lexer_atn_simulator::{LexerATNSimulator, ILexerATNSimulator};
use antlr_rust::PredictionContextCache;
use antlr_rust::recognizer::{Recognizer,Actions};
use antlr_rust::error_listener::ErrorListener;
use antlr_rust::TokenSource;
use antlr_rust::token_factory::{TokenFactory,CommonTokenFactory,TokenAware};
use antlr_rust::token::*;
use antlr_rust::rule_context::{BaseRuleContext,EmptyCustomRuleContext,EmptyContext};
use antlr_rust::parser_rule_context::{ParserRuleContext,BaseParserRuleContext,cast};
use antlr_rust::vocabulary::{Vocabulary,VocabularyImpl};

use antlr_rust::{lazy_static,Tid,TidAble,TidExt};

use std::sync::Arc;
use std::cell::RefCell;
use std::rc::Rc;
use std::marker::PhantomData;
use std::ops::{Deref, DerefMut};


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
	pub const channelNames: [&'static str;0+2] = [
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	];

	pub const modeNames: [&'static str;1] = [
		"DEFAULT_MODE"
	];

	pub const ruleNames: [&'static str;10] = [
		"T__0", "T__1", "T__2", "T__3", "T__4", "DIGIT", "IDSTARTCHAR", "IDCHAR", 
		"ID", "NEWLINE"
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


pub type LexerContext<'input> = BaseRuleContext<'input,EmptyCustomRuleContext<'input,LocalTokenFactory<'input> >>;
pub type LocalTokenFactory<'input> = CommonTokenFactory;

type From<'a> = <LocalTokenFactory<'a> as TokenFactory<'a> >::From;

#[derive(Tid)]
pub struct AuthLogicLexer<'input, Input:CharStream<From<'input> >> {
	base: BaseLexer<'input,AuthLogicLexerActions,Input,LocalTokenFactory<'input>>,
}

impl<'input, Input:CharStream<From<'input> >> Deref for AuthLogicLexer<'input,Input>{
	type Target = BaseLexer<'input,AuthLogicLexerActions,Input,LocalTokenFactory<'input>>;

	fn deref(&self) -> &Self::Target {
		&self.base
	}
}

impl<'input, Input:CharStream<From<'input> >> DerefMut for AuthLogicLexer<'input,Input>{
	fn deref_mut(&mut self) -> &mut Self::Target {
		&mut self.base
	}
}


impl<'input, Input:CharStream<From<'input> >> AuthLogicLexer<'input,Input>{
    fn get_rule_names(&self) -> &'static [&'static str] {
        &ruleNames
    }
    fn get_literal_names(&self) -> &[Option<&str>] {
        &_LITERAL_NAMES
    }

    fn get_symbolic_names(&self) -> &[Option<&str>] {
        &_SYMBOLIC_NAMES
    }

    fn get_grammar_file_name(&self) -> &'static str {
        "AuthLogicLexer.g4"
    }

	pub fn new_with_token_factory(input: Input, tf: &'input LocalTokenFactory<'input>) -> Self {
		antlr_rust::recognizer::check_version("0","2");
    	Self {
			base: BaseLexer::new_base_lexer(
				input,
				LexerATNSimulator::new_lexer_atnsimulator(
					_ATN.clone(),
					_decision_to_DFA.clone(),
					_shared_context_cache.clone(),
				),
				AuthLogicLexerActions{},
				tf
			)
	    }
	}
}

impl<'input, Input:CharStream<From<'input> >> AuthLogicLexer<'input,Input> where &'input LocalTokenFactory<'input>:Default{
	pub fn new(input: Input) -> Self{
		AuthLogicLexer::new_with_token_factory(input, <&LocalTokenFactory<'input> as Default>::default())
	}
}

pub struct AuthLogicLexerActions {
}

impl AuthLogicLexerActions{
}

impl<'input, Input:CharStream<From<'input> >> Actions<'input,BaseLexer<'input,AuthLogicLexerActions,Input,LocalTokenFactory<'input>>> for AuthLogicLexerActions{
	}

	impl<'input, Input:CharStream<From<'input> >> AuthLogicLexer<'input,Input>{

}

impl<'input, Input:CharStream<From<'input> >> LexerRecog<'input,BaseLexer<'input,AuthLogicLexerActions,Input,LocalTokenFactory<'input>>> for AuthLogicLexerActions{
}
impl<'input> TokenAware<'input> for AuthLogicLexerActions{
	type TF = LocalTokenFactory<'input>;
}

impl<'input, Input:CharStream<From<'input> >> TokenSource<'input> for AuthLogicLexer<'input,Input>{
	type TF = LocalTokenFactory<'input>;

    fn next_token(&mut self) -> <Self::TF as TokenFactory<'input>>::Tok {
        self.base.next_token()
    }

    fn get_line(&self) -> isize {
        self.base.get_line()
    }

    fn get_char_position_in_line(&self) -> isize {
        self.base.get_char_position_in_line()
    }

    fn get_input_stream(&mut self) -> Option<&mut dyn IntStream> {
        self.base.get_input_stream()
    }

	fn get_source_name(&self) -> String {
		self.base.get_source_name()
	}

    fn get_token_factory(&self) -> &'input Self::TF {
        self.base.get_token_factory()
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
		"\x03\u{608b}\u{a72a}\u{8133}\u{b9ed}\u{417c}\u{3be7}\u{7786}\u{5964}\x02\
		\x0c\x3a\x08\x01\x04\x02\x09\x02\x04\x03\x09\x03\x04\x04\x09\x04\x04\x05\
		\x09\x05\x04\x06\x09\x06\x04\x07\x09\x07\x04\x08\x09\x08\x04\x09\x09\x09\
		\x04\x0a\x09\x0a\x04\x0b\x09\x0b\x03\x02\x03\x02\x03\x03\x03\x03\x03\x03\
		\x03\x03\x03\x03\x03\x04\x03\x04\x03\x04\x03\x05\x03\x05\x03\x06\x03\x06\
		\x03\x07\x03\x07\x03\x08\x03\x08\x03\x09\x03\x09\x03\x09\x05\x09\x2d\x0a\
		\x09\x03\x0a\x03\x0a\x07\x0a\x31\x0a\x0a\x0c\x0a\x0e\x0a\x34\x0b\x0a\x03\
		\x0b\x06\x0b\x37\x0a\x0b\x0d\x0b\x0e\x0b\x38\x02\x02\x0c\x03\x03\x05\x04\
		\x07\x05\x09\x06\x0b\x07\x0d\x08\x0f\x09\x11\x0a\x13\x0b\x15\x0c\x03\x02\
		\x05\x03\x02\x32\x3b\x04\x02\x43\x5c\x63\x7c\x04\x02\x0c\x0c\x0f\x0f\x02\
		\x3d\x02\x03\x03\x02\x02\x02\x02\x05\x03\x02\x02\x02\x02\x07\x03\x02\x02\
		\x02\x02\x09\x03\x02\x02\x02\x02\x0b\x03\x02\x02\x02\x02\x0d\x03\x02\x02\
		\x02\x02\x0f\x03\x02\x02\x02\x02\x11\x03\x02\x02\x02\x02\x13\x03\x02\x02\
		\x02\x02\x15\x03\x02\x02\x02\x03\x17\x03\x02\x02\x02\x05\x19\x03\x02\x02\
		\x02\x07\x1e\x03\x02\x02\x02\x09\x21\x03\x02\x02\x02\x0b\x23\x03\x02\x02\
		\x02\x0d\x25\x03\x02\x02\x02\x0f\x27\x03\x02\x02\x02\x11\x2c\x03\x02\x02\
		\x02\x13\x2e\x03\x02\x02\x02\x15\x36\x03\x02\x02\x02\x17\x18\x07\x30\x02\
		\x02\x18\x04\x03\x02\x02\x02\x19\x1a\x07\x22\x02\x02\x1a\x1b\x07\x3c\x02\
		\x02\x1b\x1c\x07\x2f\x02\x02\x1c\x1d\x07\x22\x02\x02\x1d\x06\x03\x02\x02\
		\x02\x1e\x1f\x07\x2e\x02\x02\x1f\x20\x07\x22\x02\x02\x20\x08\x03\x02\x02\
		\x02\x21\x22\x07\x2a\x02\x02\x22\x0a\x03\x02\x02\x02\x23\x24\x07\x2b\x02\
		\x02\x24\x0c\x03\x02\x02\x02\x25\x26\x09\x02\x02\x02\x26\x0e\x03\x02\x02\
		\x02\x27\x28\x09\x03\x02\x02\x28\x10\x03\x02\x02\x02\x29\x2d\x05\x0f\x08\
		\x02\x2a\x2d\x05\x0d\x07\x02\x2b\x2d\x07\x61\x02\x02\x2c\x29\x03\x02\x02\
		\x02\x2c\x2a\x03\x02\x02\x02\x2c\x2b\x03\x02\x02\x02\x2d\x12\x03\x02\x02\
		\x02\x2e\x32\x05\x0f\x08\x02\x2f\x31\x05\x11\x09\x02\x30\x2f\x03\x02\x02\
		\x02\x31\x34\x03\x02\x02\x02\x32\x30\x03\x02\x02\x02\x32\x33\x03\x02\x02\
		\x02\x33\x14\x03\x02\x02\x02\x34\x32\x03\x02\x02\x02\x35\x37\x09\x04\x02\
		\x02\x36\x35\x03\x02\x02\x02\x37\x38\x03\x02\x02\x02\x38\x36\x03\x02\x02\
		\x02\x38\x39\x03\x02\x02\x02\x39\x16\x03\x02\x02\x02\x06\x02\x2c\x32\x38\
		\x02";
