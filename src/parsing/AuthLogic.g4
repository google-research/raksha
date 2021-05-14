grammar AuthLogic;

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
    : ID '(' ID (', ' ID)* ')'
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
    | fact ' :- ' flatFact (', ' flatFact )* '.' #hornClauseAssertion
    ;

saysAssertion
    : principal SAYS assertion
    ;

query
    : ID ' =' QUERY principal SAYS fact '?'
    ;

program
    : ((saysAssertion | query) NEWLINE)+
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------
DIGIT: [0-9];
IDSTARTCHAR: [a-zA-Z"];
IDCHAR
    : IDSTARTCHAR
    | DIGIT
    | '_'
    | '"' // for writing constants
    ;

QUERY
    : ' query '
    ;

SAYS
    : ' says '
    ;

CANACTAS
    : ' canActAs '
    ;

CANSAY
    : ' canSay '
    ;

ID
    : IDSTARTCHAR (IDCHAR)*
    ;

//TODO a real version would catch mis-uses of keywords.

NEWLINE : [\r\n]+ ;
