grammar AuthLogic;

// Note: when changing this file, errors thrown by antlr will not
// be gracefully passed to cargo because errors in build scripts are 
// suppressed. Instead, the rust files in antlr_gen will remain unchanged.
// You can try to unsurpress them with "cargo build -vv", or run
// the antlr command that build.rs runs manually.

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------
// Principals are only superficially syntactically distinct from IDs at the
// moment, but they are separate syntactic objects because the principal
// syntax could plausibly change later
principal
    : ID
    ;

predicate
    : ID '(' ID (',' ID)* ')'
    ;

verbphrase
    // It might look like (principal predicate(<args>)) will
    // work the same as (predicate(principal, <args>)), but it does
    // not exactly. Expressions of the form (principal predicate(args))
    // can be passed to other principals using canActAs.
    : predicate #predphrase 
    | CANACTAS principal #actsAsPhrase
    ;

flatFact
    : principal verbphrase #prinFact
    | predicate #predFact
    ;

fact
    : flatFact #flatFactFact
    | principal CANSAY fact #canSayFact
    ;

assertion
    : fact '.' #factAssertion
    | fact ':-' flatFact (',' flatFact )* '.' #hornClauseAssertion
    ;

saysAssertion
    : principal SAYS assertion (EXPORT PATH)? #saysSingle
    | principal SAYS '{' assertion+ '}' (EXPORT PATH)?  #saysMulti
    ;

keyBind
    : BINDEX principal PATH #bindpriv
    | BINDIM principal PATH #bindpub
    ;

query
    : ID '=' QUERY principal SAYS fact '?'
    ;

program
    : ((saysAssertion | query | keyBind))+
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------

// keywords
QUERY: 'query' ;
SAYS: 'says';
CANACTAS: 'canActAs';
CANSAY: 'canSay';
EXPORT: 'exportTo';
BINDEX: 'BindPrivKey';
BINDIM: 'BindPubKey';


// identifiers wrapped in quotes are constants whereas
// identifiers without quotes are variables
ID : ('"')? [a-zA-Z] [_a-zA-Z0-9]* ('"')?;
PATH : [_a-zA-Z0-9/.]+;

WHITESPACE_IGNORE
    : [ \r\t\n]+ -> skip
    ;

COMMENT_SINGLE
    : '//' ~[\r\n]* '\r'? '\n' -> skip
    ;

COMMENT_MULTI
    : '/*' .*? '*/' -> skip
    ;
