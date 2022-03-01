// Generated from /usr/local/google/home/harshamandadi/raksha/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class AuthLogicLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.8", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		QUERY=10, SAYS=11, CANACTAS=12, CANSAY=13, EXPORT=14, IMPORT=15, BINDEX=16, 
		BINDIM=17, ID=18, NEG=19, WHITESPACE_IGNORE=20, COMMENT_SINGLE=21, COMMENT_MULTI=22;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
			"QUERY", "SAYS", "CANACTAS", "CANSAY", "EXPORT", "IMPORT", "BINDEX", 
			"BINDIM", "ID", "NEG", "WHITESPACE_IGNORE", "COMMENT_SINGLE", "COMMENT_MULTI"
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


	public AuthLogicLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "AuthLogic.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2\30\u00b7\b\1\4\2"+
		"\t\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4"+
		"\13\t\13\4\f\t\f\4\r\t\r\4\16\t\16\4\17\t\17\4\20\t\20\4\21\t\21\4\22"+
		"\t\22\4\23\t\23\4\24\t\24\4\25\t\25\4\26\t\26\4\27\t\27\3\2\3\2\3\3\3"+
		"\3\3\4\3\4\3\5\3\5\3\6\3\6\3\6\3\7\3\7\3\b\3\b\3\t\3\t\3\n\3\n\3\13\3"+
		"\13\3\13\3\13\3\13\3\13\3\f\3\f\3\f\3\f\3\f\3\r\3\r\3\r\3\r\3\r\3\r\3"+
		"\r\3\r\3\r\3\16\3\16\3\16\3\16\3\16\3\16\3\16\3\17\3\17\3\17\3\17\3\17"+
		"\3\17\3\17\3\17\3\17\3\20\3\20\3\20\3\20\3\20\3\20\3\20\3\21\3\21\3\21"+
		"\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\21\3\22\3\22\3\22\3\22\3\22"+
		"\3\22\3\22\3\22\3\22\3\22\3\22\3\23\5\23\u0086\n\23\3\23\7\23\u0089\n"+
		"\23\f\23\16\23\u008c\13\23\3\23\5\23\u008f\n\23\3\24\3\24\3\25\6\25\u0094"+
		"\n\25\r\25\16\25\u0095\3\25\3\25\3\26\3\26\3\26\3\26\7\26\u009e\n\26\f"+
		"\26\16\26\u00a1\13\26\3\26\5\26\u00a4\n\26\3\26\3\26\3\26\3\26\3\27\3"+
		"\27\3\27\3\27\7\27\u00ae\n\27\f\27\16\27\u00b1\13\27\3\27\3\27\3\27\3"+
		"\27\3\27\3\u00af\2\30\3\3\5\4\7\5\t\6\13\7\r\b\17\t\21\n\23\13\25\f\27"+
		"\r\31\16\33\17\35\20\37\21!\22#\23%\24\'\25)\26+\27-\30\3\2\5\7\2%%\60"+
		"<C\\aac|\5\2\13\f\17\17\"\"\4\2\f\f\17\17\2\u00bd\2\3\3\2\2\2\2\5\3\2"+
		"\2\2\2\7\3\2\2\2\2\t\3\2\2\2\2\13\3\2\2\2\2\r\3\2\2\2\2\17\3\2\2\2\2\21"+
		"\3\2\2\2\2\23\3\2\2\2\2\25\3\2\2\2\2\27\3\2\2\2\2\31\3\2\2\2\2\33\3\2"+
		"\2\2\2\35\3\2\2\2\2\37\3\2\2\2\2!\3\2\2\2\2#\3\2\2\2\2%\3\2\2\2\2\'\3"+
		"\2\2\2\2)\3\2\2\2\2+\3\2\2\2\2-\3\2\2\2\3/\3\2\2\2\5\61\3\2\2\2\7\63\3"+
		"\2\2\2\t\65\3\2\2\2\13\67\3\2\2\2\r:\3\2\2\2\17<\3\2\2\2\21>\3\2\2\2\23"+
		"@\3\2\2\2\25B\3\2\2\2\27H\3\2\2\2\31M\3\2\2\2\33V\3\2\2\2\35]\3\2\2\2"+
		"\37f\3\2\2\2!m\3\2\2\2#y\3\2\2\2%\u0085\3\2\2\2\'\u0090\3\2\2\2)\u0093"+
		"\3\2\2\2+\u0099\3\2\2\2-\u00a9\3\2\2\2/\60\7*\2\2\60\4\3\2\2\2\61\62\7"+
		".\2\2\62\6\3\2\2\2\63\64\7+\2\2\64\b\3\2\2\2\65\66\7\60\2\2\66\n\3\2\2"+
		"\2\678\7<\2\289\7/\2\29\f\3\2\2\2:;\7}\2\2;\16\3\2\2\2<=\7\177\2\2=\20"+
		"\3\2\2\2>?\7?\2\2?\22\3\2\2\2@A\7A\2\2A\24\3\2\2\2BC\7s\2\2CD\7w\2\2D"+
		"E\7g\2\2EF\7t\2\2FG\7{\2\2G\26\3\2\2\2HI\7u\2\2IJ\7c\2\2JK\7{\2\2KL\7"+
		"u\2\2L\30\3\2\2\2MN\7e\2\2NO\7c\2\2OP\7p\2\2PQ\7C\2\2QR\7e\2\2RS\7v\2"+
		"\2ST\7C\2\2TU\7u\2\2U\32\3\2\2\2VW\7e\2\2WX\7c\2\2XY\7p\2\2YZ\7U\2\2Z"+
		"[\7c\2\2[\\\7{\2\2\\\34\3\2\2\2]^\7g\2\2^_\7z\2\2_`\7r\2\2`a\7q\2\2ab"+
		"\7t\2\2bc\7v\2\2cd\7V\2\2de\7q\2\2e\36\3\2\2\2fg\7k\2\2gh\7o\2\2hi\7r"+
		"\2\2ij\7q\2\2jk\7t\2\2kl\7v\2\2l \3\2\2\2mn\7D\2\2no\7k\2\2op\7p\2\2p"+
		"q\7f\2\2qr\7R\2\2rs\7t\2\2st\7k\2\2tu\7x\2\2uv\7M\2\2vw\7g\2\2wx\7{\2"+
		"\2x\"\3\2\2\2yz\7D\2\2z{\7k\2\2{|\7p\2\2|}\7f\2\2}~\7R\2\2~\177\7w\2\2"+
		"\177\u0080\7d\2\2\u0080\u0081\7M\2\2\u0081\u0082\7g\2\2\u0082\u0083\7"+
		"{\2\2\u0083$\3\2\2\2\u0084\u0086\7$\2\2\u0085\u0084\3\2\2\2\u0085\u0086"+
		"\3\2\2\2\u0086\u008a\3\2\2\2\u0087\u0089\t\2\2\2\u0088\u0087\3\2\2\2\u0089"+
		"\u008c\3\2\2\2\u008a\u0088\3\2\2\2\u008a\u008b\3\2\2\2\u008b\u008e\3\2"+
		"\2\2\u008c\u008a\3\2\2\2\u008d\u008f\7$\2\2\u008e\u008d\3\2\2\2\u008e"+
		"\u008f\3\2\2\2\u008f&\3\2\2\2\u0090\u0091\7#\2\2\u0091(\3\2\2\2\u0092"+
		"\u0094\t\3\2\2\u0093\u0092\3\2\2\2\u0094\u0095\3\2\2\2\u0095\u0093\3\2"+
		"\2\2\u0095\u0096\3\2\2\2\u0096\u0097\3\2\2\2\u0097\u0098\b\25\2\2\u0098"+
		"*\3\2\2\2\u0099\u009a\7\61\2\2\u009a\u009b\7\61\2\2\u009b\u009f\3\2\2"+
		"\2\u009c\u009e\n\4\2\2\u009d\u009c\3\2\2\2\u009e\u00a1\3\2\2\2\u009f\u009d"+
		"\3\2\2\2\u009f\u00a0\3\2\2\2\u00a0\u00a3\3\2\2\2\u00a1\u009f\3\2\2\2\u00a2"+
		"\u00a4\7\17\2\2\u00a3\u00a2\3\2\2\2\u00a3\u00a4\3\2\2\2\u00a4\u00a5\3"+
		"\2\2\2\u00a5\u00a6\7\f\2\2\u00a6\u00a7\3\2\2\2\u00a7\u00a8\b\26\2\2\u00a8"+
		",\3\2\2\2\u00a9\u00aa\7\61\2\2\u00aa\u00ab\7,\2\2\u00ab\u00af\3\2\2\2"+
		"\u00ac\u00ae\13\2\2\2\u00ad\u00ac\3\2\2\2\u00ae\u00b1\3\2\2\2\u00af\u00b0"+
		"\3\2\2\2\u00af\u00ad\3\2\2\2\u00b0\u00b2\3\2\2\2\u00b1\u00af\3\2\2\2\u00b2"+
		"\u00b3\7,\2\2\u00b3\u00b4\7\61\2\2\u00b4\u00b5\3\2\2\2\u00b5\u00b6\b\27"+
		"\2\2\u00b6.\3\2\2\2\n\2\u0085\u008a\u008e\u0095\u009f\u00a3\u00af\3\b"+
		"\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}