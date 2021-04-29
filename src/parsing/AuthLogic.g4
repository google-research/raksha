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
    : predicate #predphrase
    | CANACTAS principal #actsAsPhrase
    ;

flatFact
    : principal verbphrase #prinFact
    | predicate #predFact
    ;

fact
    : flatFact #flatFactFact
    | principal CANSAY flatFact #canSayFact
    ;

assertion
    : fact '.' #factAssertion
    | fact ' :- ' flatFact (', ' flatFact )* '.' #hornClauseAssertion
    ;

saysAssertion
    : principal SAYS assertion
    ;

program
    : (saysAssertion NEWLINE)+
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------
DIGIT: [0-9];
IDSTARTCHAR: [a-zA-Z];
IDCHAR
    : IDSTARTCHAR
    | DIGIT
    | '_'
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
