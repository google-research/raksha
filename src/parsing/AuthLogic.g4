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
    // NOTE the parser for this syntax does not fully work,
    // but it also does not increase expressiveness (other than 
    // superficially / for convenience).
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
    : principal SAYS assertion
    ;

query
    : ID '=' QUERY principal SAYS fact '?'
    ;

program
    : ((saysAssertion | query))+
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------

// keywords
QUERY: 'query' ;
SAYS: 'says';
CANACTAS: 'canActAs';
CANSAY: 'canSay';

ID : ('"')? [a-zA-Z] [_a-zA-Z0-9]* ('"')?;
// identifiers wrapped in quotes are constants whereas
// identifiers without quotes are variables

WHITESPACE_IGNORE
    : [ \r\t\n]+    -> skip
    ;

COMMENT_SINGLE
    : '//' ~[\r\n]* '\r'? '\n' -> skip
    ;

COMMENT_MULTI
    : '/*' .*? '*/' -> skip
    ;
