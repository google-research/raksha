grammar AuthLogic;

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------
program
    : (assertion NEWLINE)+
    ;

assertion
    : predicate '.' #fact
    | predicate ' :- ' predicate (', ' predicate)* '.' #hornclause
    ;

predicate
    : ID '(' ID (', ' ID)* ')'
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
ID
    : IDSTARTCHAR (IDCHAR)*
    ;

NEWLINE : [\r\n]+ ;
