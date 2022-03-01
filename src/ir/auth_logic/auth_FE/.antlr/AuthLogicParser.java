// Generated from /usr/local/google/home/harshamandadi/raksha/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class AuthLogicParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.8", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		QUERY=10, SAYS=11, CANACTAS=12, CANSAY=13, EXPORT=14, IMPORT=15, BINDEX=16, 
		BINDIM=17, ID=18, NEG=19, WHITESPACE_IGNORE=20, COMMENT_SINGLE=21, COMMENT_MULTI=22;
	public static final int
		RULE_principal = 0, RULE_predicate = 1, RULE_verbphrase = 2, RULE_flatFact = 3, 
		RULE_fact = 4, RULE_assertion = 5, RULE_saysAssertion = 6, RULE_keyBind = 7, 
		RULE_query = 8, RULE_importAssertion = 9, RULE_program = 10;
	private static String[] makeRuleNames() {
		return new String[] {
			"principal", "predicate", "verbphrase", "flatFact", "fact", "assertion", 
			"saysAssertion", "keyBind", "query", "importAssertion", "program"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'('", "','", "')'", "'.'", "':-'", "'{'", "'}'", "'='", "'?'", 
			"'query'", "'says'", "'canActAs'", "'canSay'", "'exportTo'", "'import'", 
			"'BindPrivKey'", "'BindPubKey'", null, "'!'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, null, null, null, null, null, null, null, null, null, "QUERY", 
			"SAYS", "CANACTAS", "CANSAY", "EXPORT", "IMPORT", "BINDEX", "BINDIM", 
			"ID", "NEG", "WHITESPACE_IGNORE", "COMMENT_SINGLE", "COMMENT_MULTI"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "AuthLogic.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public AuthLogicParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	public static class PrincipalContext extends ParserRuleContext {
		public TerminalNode ID() { return getToken(AuthLogicParser.ID, 0); }
		public PrincipalContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_principal; }
	}

	public final PrincipalContext principal() throws RecognitionException {
		PrincipalContext _localctx = new PrincipalContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_principal);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(22);
			match(ID);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class PredicateContext extends ParserRuleContext {
		public List<TerminalNode> ID() { return getTokens(AuthLogicParser.ID); }
		public TerminalNode ID(int i) {
			return getToken(AuthLogicParser.ID, i);
		}
		public TerminalNode NEG() { return getToken(AuthLogicParser.NEG, 0); }
		public PredicateContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_predicate; }
	}

	public final PredicateContext predicate() throws RecognitionException {
		PredicateContext _localctx = new PredicateContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_predicate);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(25);
			_errHandler.sync(this);
			_la = _input.LA(1);
			if (_la==NEG) {
				{
				setState(24);
				match(NEG);
				}
			}

			setState(27);
			match(ID);
			setState(28);
			match(T__0);
			setState(29);
			match(ID);
			setState(34);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==T__1) {
				{
				{
				setState(30);
				match(T__1);
				setState(31);
				match(ID);
				}
				}
				setState(36);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(37);
			match(T__2);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class VerbphraseContext extends ParserRuleContext {
		public VerbphraseContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_verbphrase; }
	 
		public VerbphraseContext() { }
		public void copyFrom(VerbphraseContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class PredphraseContext extends VerbphraseContext {
		public PredicateContext predicate() {
			return getRuleContext(PredicateContext.class,0);
		}
		public PredphraseContext(VerbphraseContext ctx) { copyFrom(ctx); }
	}
	public static class ActsAsPhraseContext extends VerbphraseContext {
		public TerminalNode CANACTAS() { return getToken(AuthLogicParser.CANACTAS, 0); }
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public ActsAsPhraseContext(VerbphraseContext ctx) { copyFrom(ctx); }
	}

	public final VerbphraseContext verbphrase() throws RecognitionException {
		VerbphraseContext _localctx = new VerbphraseContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_verbphrase);
		try {
			setState(42);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case ID:
			case NEG:
				_localctx = new PredphraseContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(39);
				predicate();
				}
				break;
			case CANACTAS:
				_localctx = new ActsAsPhraseContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(40);
				match(CANACTAS);
				setState(41);
				principal();
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FlatFactContext extends ParserRuleContext {
		public FlatFactContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_flatFact; }
	 
		public FlatFactContext() { }
		public void copyFrom(FlatFactContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class PrinFactContext extends FlatFactContext {
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public VerbphraseContext verbphrase() {
			return getRuleContext(VerbphraseContext.class,0);
		}
		public PrinFactContext(FlatFactContext ctx) { copyFrom(ctx); }
	}
	public static class PredFactContext extends FlatFactContext {
		public PredicateContext predicate() {
			return getRuleContext(PredicateContext.class,0);
		}
		public PredFactContext(FlatFactContext ctx) { copyFrom(ctx); }
	}

	public final FlatFactContext flatFact() throws RecognitionException {
		FlatFactContext _localctx = new FlatFactContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_flatFact);
		try {
			setState(48);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,3,_ctx) ) {
			case 1:
				_localctx = new PrinFactContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(44);
				principal();
				setState(45);
				verbphrase();
				}
				break;
			case 2:
				_localctx = new PredFactContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(47);
				predicate();
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FactContext extends ParserRuleContext {
		public FactContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_fact; }
	 
		public FactContext() { }
		public void copyFrom(FactContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class FlatFactFactContext extends FactContext {
		public FlatFactContext flatFact() {
			return getRuleContext(FlatFactContext.class,0);
		}
		public FlatFactFactContext(FactContext ctx) { copyFrom(ctx); }
	}
	public static class CanSayFactContext extends FactContext {
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public TerminalNode CANSAY() { return getToken(AuthLogicParser.CANSAY, 0); }
		public FactContext fact() {
			return getRuleContext(FactContext.class,0);
		}
		public CanSayFactContext(FactContext ctx) { copyFrom(ctx); }
	}

	public final FactContext fact() throws RecognitionException {
		FactContext _localctx = new FactContext(_ctx, getState());
		enterRule(_localctx, 8, RULE_fact);
		try {
			setState(55);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,4,_ctx) ) {
			case 1:
				_localctx = new FlatFactFactContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(50);
				flatFact();
				}
				break;
			case 2:
				_localctx = new CanSayFactContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(51);
				principal();
				setState(52);
				match(CANSAY);
				setState(53);
				fact();
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class AssertionContext extends ParserRuleContext {
		public AssertionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_assertion; }
	 
		public AssertionContext() { }
		public void copyFrom(AssertionContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class FactAssertionContext extends AssertionContext {
		public FactContext fact() {
			return getRuleContext(FactContext.class,0);
		}
		public FactAssertionContext(AssertionContext ctx) { copyFrom(ctx); }
	}
	public static class HornClauseAssertionContext extends AssertionContext {
		public FactContext fact() {
			return getRuleContext(FactContext.class,0);
		}
		public List<FlatFactContext> flatFact() {
			return getRuleContexts(FlatFactContext.class);
		}
		public FlatFactContext flatFact(int i) {
			return getRuleContext(FlatFactContext.class,i);
		}
		public HornClauseAssertionContext(AssertionContext ctx) { copyFrom(ctx); }
	}

	public final AssertionContext assertion() throws RecognitionException {
		AssertionContext _localctx = new AssertionContext(_ctx, getState());
		enterRule(_localctx, 10, RULE_assertion);
		int _la;
		try {
			setState(72);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,6,_ctx) ) {
			case 1:
				_localctx = new FactAssertionContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(57);
				fact();
				setState(58);
				match(T__3);
				}
				break;
			case 2:
				_localctx = new HornClauseAssertionContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(60);
				fact();
				setState(61);
				match(T__4);
				setState(62);
				flatFact();
				setState(67);
				_errHandler.sync(this);
				_la = _input.LA(1);
				while (_la==T__1) {
					{
					{
					setState(63);
					match(T__1);
					setState(64);
					flatFact();
					}
					}
					setState(69);
					_errHandler.sync(this);
					_la = _input.LA(1);
				}
				setState(70);
				match(T__3);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class SaysAssertionContext extends ParserRuleContext {
		public SaysAssertionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_saysAssertion; }
	 
		public SaysAssertionContext() { }
		public void copyFrom(SaysAssertionContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class SaysMultiContext extends SaysAssertionContext {
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public TerminalNode SAYS() { return getToken(AuthLogicParser.SAYS, 0); }
		public List<AssertionContext> assertion() {
			return getRuleContexts(AssertionContext.class);
		}
		public AssertionContext assertion(int i) {
			return getRuleContext(AssertionContext.class,i);
		}
		public TerminalNode EXPORT() { return getToken(AuthLogicParser.EXPORT, 0); }
		public TerminalNode ID() { return getToken(AuthLogicParser.ID, 0); }
		public SaysMultiContext(SaysAssertionContext ctx) { copyFrom(ctx); }
	}
	public static class SaysSingleContext extends SaysAssertionContext {
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public TerminalNode SAYS() { return getToken(AuthLogicParser.SAYS, 0); }
		public AssertionContext assertion() {
			return getRuleContext(AssertionContext.class,0);
		}
		public TerminalNode EXPORT() { return getToken(AuthLogicParser.EXPORT, 0); }
		public TerminalNode ID() { return getToken(AuthLogicParser.ID, 0); }
		public SaysSingleContext(SaysAssertionContext ctx) { copyFrom(ctx); }
	}

	public final SaysAssertionContext saysAssertion() throws RecognitionException {
		SaysAssertionContext _localctx = new SaysAssertionContext(_ctx, getState());
		enterRule(_localctx, 12, RULE_saysAssertion);
		int _la;
		try {
			setState(94);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,10,_ctx) ) {
			case 1:
				_localctx = new SaysSingleContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(74);
				principal();
				setState(75);
				match(SAYS);
				setState(76);
				assertion();
				setState(79);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==EXPORT) {
					{
					setState(77);
					match(EXPORT);
					setState(78);
					match(ID);
					}
				}

				}
				break;
			case 2:
				_localctx = new SaysMultiContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(81);
				principal();
				setState(82);
				match(SAYS);
				setState(83);
				match(T__5);
				setState(85); 
				_errHandler.sync(this);
				_la = _input.LA(1);
				do {
					{
					{
					setState(84);
					assertion();
					}
					}
					setState(87); 
					_errHandler.sync(this);
					_la = _input.LA(1);
				} while ( _la==ID || _la==NEG );
				setState(89);
				match(T__6);
				setState(92);
				_errHandler.sync(this);
				_la = _input.LA(1);
				if (_la==EXPORT) {
					{
					setState(90);
					match(EXPORT);
					setState(91);
					match(ID);
					}
				}

				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class KeyBindContext extends ParserRuleContext {
		public KeyBindContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_keyBind; }
	 
		public KeyBindContext() { }
		public void copyFrom(KeyBindContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class BindprivContext extends KeyBindContext {
		public TerminalNode BINDEX() { return getToken(AuthLogicParser.BINDEX, 0); }
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public TerminalNode ID() { return getToken(AuthLogicParser.ID, 0); }
		public BindprivContext(KeyBindContext ctx) { copyFrom(ctx); }
	}
	public static class BindpubContext extends KeyBindContext {
		public TerminalNode BINDIM() { return getToken(AuthLogicParser.BINDIM, 0); }
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public TerminalNode ID() { return getToken(AuthLogicParser.ID, 0); }
		public BindpubContext(KeyBindContext ctx) { copyFrom(ctx); }
	}

	public final KeyBindContext keyBind() throws RecognitionException {
		KeyBindContext _localctx = new KeyBindContext(_ctx, getState());
		enterRule(_localctx, 14, RULE_keyBind);
		try {
			setState(104);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case BINDEX:
				_localctx = new BindprivContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(96);
				match(BINDEX);
				setState(97);
				principal();
				setState(98);
				match(ID);
				}
				break;
			case BINDIM:
				_localctx = new BindpubContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(100);
				match(BINDIM);
				setState(101);
				principal();
				setState(102);
				match(ID);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class QueryContext extends ParserRuleContext {
		public TerminalNode ID() { return getToken(AuthLogicParser.ID, 0); }
		public TerminalNode QUERY() { return getToken(AuthLogicParser.QUERY, 0); }
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public TerminalNode SAYS() { return getToken(AuthLogicParser.SAYS, 0); }
		public FactContext fact() {
			return getRuleContext(FactContext.class,0);
		}
		public QueryContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_query; }
	}

	public final QueryContext query() throws RecognitionException {
		QueryContext _localctx = new QueryContext(_ctx, getState());
		enterRule(_localctx, 16, RULE_query);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(106);
			match(ID);
			setState(107);
			match(T__7);
			setState(108);
			match(QUERY);
			setState(109);
			principal();
			setState(110);
			match(SAYS);
			setState(111);
			fact();
			setState(112);
			match(T__8);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ImportAssertionContext extends ParserRuleContext {
		public TerminalNode IMPORT() { return getToken(AuthLogicParser.IMPORT, 0); }
		public PrincipalContext principal() {
			return getRuleContext(PrincipalContext.class,0);
		}
		public TerminalNode SAYS() { return getToken(AuthLogicParser.SAYS, 0); }
		public TerminalNode ID() { return getToken(AuthLogicParser.ID, 0); }
		public ImportAssertionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_importAssertion; }
	}

	public final ImportAssertionContext importAssertion() throws RecognitionException {
		ImportAssertionContext _localctx = new ImportAssertionContext(_ctx, getState());
		enterRule(_localctx, 18, RULE_importAssertion);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(114);
			match(IMPORT);
			setState(115);
			principal();
			setState(116);
			match(SAYS);
			setState(117);
			match(ID);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class ProgramContext extends ParserRuleContext {
		public List<SaysAssertionContext> saysAssertion() {
			return getRuleContexts(SaysAssertionContext.class);
		}
		public SaysAssertionContext saysAssertion(int i) {
			return getRuleContext(SaysAssertionContext.class,i);
		}
		public List<QueryContext> query() {
			return getRuleContexts(QueryContext.class);
		}
		public QueryContext query(int i) {
			return getRuleContext(QueryContext.class,i);
		}
		public List<KeyBindContext> keyBind() {
			return getRuleContexts(KeyBindContext.class);
		}
		public KeyBindContext keyBind(int i) {
			return getRuleContext(KeyBindContext.class,i);
		}
		public List<ImportAssertionContext> importAssertion() {
			return getRuleContexts(ImportAssertionContext.class);
		}
		public ImportAssertionContext importAssertion(int i) {
			return getRuleContext(ImportAssertionContext.class,i);
		}
		public ProgramContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_program; }
	}

	public final ProgramContext program() throws RecognitionException {
		ProgramContext _localctx = new ProgramContext(_ctx, getState());
		enterRule(_localctx, 20, RULE_program);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(125); 
			_errHandler.sync(this);
			_la = _input.LA(1);
			do {
				{
				{
				setState(123);
				_errHandler.sync(this);
				switch ( getInterpreter().adaptivePredict(_input,12,_ctx) ) {
				case 1:
					{
					setState(119);
					saysAssertion();
					}
					break;
				case 2:
					{
					setState(120);
					query();
					}
					break;
				case 3:
					{
					setState(121);
					keyBind();
					}
					break;
				case 4:
					{
					setState(122);
					importAssertion();
					}
					break;
				}
				}
				}
				setState(127); 
				_errHandler.sync(this);
				_la = _input.LA(1);
			} while ( (((_la) & ~0x3f) == 0 && ((1L << _la) & ((1L << IMPORT) | (1L << BINDEX) | (1L << BINDIM) | (1L << ID))) != 0) );
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\3\30\u0084\4\2\t\2"+
		"\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13"+
		"\t\13\4\f\t\f\3\2\3\2\3\3\5\3\34\n\3\3\3\3\3\3\3\3\3\3\3\7\3#\n\3\f\3"+
		"\16\3&\13\3\3\3\3\3\3\4\3\4\3\4\5\4-\n\4\3\5\3\5\3\5\3\5\5\5\63\n\5\3"+
		"\6\3\6\3\6\3\6\3\6\5\6:\n\6\3\7\3\7\3\7\3\7\3\7\3\7\3\7\3\7\7\7D\n\7\f"+
		"\7\16\7G\13\7\3\7\3\7\5\7K\n\7\3\b\3\b\3\b\3\b\3\b\5\bR\n\b\3\b\3\b\3"+
		"\b\3\b\6\bX\n\b\r\b\16\bY\3\b\3\b\3\b\5\b_\n\b\5\ba\n\b\3\t\3\t\3\t\3"+
		"\t\3\t\3\t\3\t\3\t\5\tk\n\t\3\n\3\n\3\n\3\n\3\n\3\n\3\n\3\n\3\13\3\13"+
		"\3\13\3\13\3\13\3\f\3\f\3\f\3\f\5\f~\n\f\6\f\u0080\n\f\r\f\16\f\u0081"+
		"\3\f\2\2\r\2\4\6\b\n\f\16\20\22\24\26\2\2\2\u0088\2\30\3\2\2\2\4\33\3"+
		"\2\2\2\6,\3\2\2\2\b\62\3\2\2\2\n9\3\2\2\2\fJ\3\2\2\2\16`\3\2\2\2\20j\3"+
		"\2\2\2\22l\3\2\2\2\24t\3\2\2\2\26\177\3\2\2\2\30\31\7\24\2\2\31\3\3\2"+
		"\2\2\32\34\7\25\2\2\33\32\3\2\2\2\33\34\3\2\2\2\34\35\3\2\2\2\35\36\7"+
		"\24\2\2\36\37\7\3\2\2\37$\7\24\2\2 !\7\4\2\2!#\7\24\2\2\" \3\2\2\2#&\3"+
		"\2\2\2$\"\3\2\2\2$%\3\2\2\2%\'\3\2\2\2&$\3\2\2\2\'(\7\5\2\2(\5\3\2\2\2"+
		")-\5\4\3\2*+\7\16\2\2+-\5\2\2\2,)\3\2\2\2,*\3\2\2\2-\7\3\2\2\2./\5\2\2"+
		"\2/\60\5\6\4\2\60\63\3\2\2\2\61\63\5\4\3\2\62.\3\2\2\2\62\61\3\2\2\2\63"+
		"\t\3\2\2\2\64:\5\b\5\2\65\66\5\2\2\2\66\67\7\17\2\2\678\5\n\6\28:\3\2"+
		"\2\29\64\3\2\2\29\65\3\2\2\2:\13\3\2\2\2;<\5\n\6\2<=\7\6\2\2=K\3\2\2\2"+
		">?\5\n\6\2?@\7\7\2\2@E\5\b\5\2AB\7\4\2\2BD\5\b\5\2CA\3\2\2\2DG\3\2\2\2"+
		"EC\3\2\2\2EF\3\2\2\2FH\3\2\2\2GE\3\2\2\2HI\7\6\2\2IK\3\2\2\2J;\3\2\2\2"+
		"J>\3\2\2\2K\r\3\2\2\2LM\5\2\2\2MN\7\r\2\2NQ\5\f\7\2OP\7\20\2\2PR\7\24"+
		"\2\2QO\3\2\2\2QR\3\2\2\2Ra\3\2\2\2ST\5\2\2\2TU\7\r\2\2UW\7\b\2\2VX\5\f"+
		"\7\2WV\3\2\2\2XY\3\2\2\2YW\3\2\2\2YZ\3\2\2\2Z[\3\2\2\2[^\7\t\2\2\\]\7"+
		"\20\2\2]_\7\24\2\2^\\\3\2\2\2^_\3\2\2\2_a\3\2\2\2`L\3\2\2\2`S\3\2\2\2"+
		"a\17\3\2\2\2bc\7\22\2\2cd\5\2\2\2de\7\24\2\2ek\3\2\2\2fg\7\23\2\2gh\5"+
		"\2\2\2hi\7\24\2\2ik\3\2\2\2jb\3\2\2\2jf\3\2\2\2k\21\3\2\2\2lm\7\24\2\2"+
		"mn\7\n\2\2no\7\f\2\2op\5\2\2\2pq\7\r\2\2qr\5\n\6\2rs\7\13\2\2s\23\3\2"+
		"\2\2tu\7\21\2\2uv\5\2\2\2vw\7\r\2\2wx\7\24\2\2x\25\3\2\2\2y~\5\16\b\2"+
		"z~\5\22\n\2{~\5\20\t\2|~\5\24\13\2}y\3\2\2\2}z\3\2\2\2}{\3\2\2\2}|\3\2"+
		"\2\2~\u0080\3\2\2\2\177}\3\2\2\2\u0080\u0081\3\2\2\2\u0081\177\3\2\2\2"+
		"\u0081\u0082\3\2\2\2\u0082\27\3\2\2\2\20\33$,\629EJQY^`j}\u0081";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}