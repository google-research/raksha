/*
 * Copyright 2021 Google LLC.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

grammar AuthLogic;

// Note: when changing this file, errors thrown by antlr will not
// be gracefully passed to cargo because errors in build scripts are 
// suppressed. Instead, the rust files in antlr_gen will remain unchanged.
// You can try to unsurpress them with "cargo build -vv", or run
// the antlr command that build.rs runs manually.

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------

principal
    : STRING_LITERAL
    ;

pred_arg
    : STRING_LITERAL        #constant
    | MULTILINE_STRING_LITERAL #multilineConstant
    | VARIABLE      #variable
    | NUMLITERAL    #number
    ;

predicate
    : (NEG)? VARIABLE '(' pred_arg (',' pred_arg )* ')'
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

binop
  : LESSTHAN #ltbinop
  | GRTHAN #grbinop
  | EQUALS #eqbinop
  | NEQUALS #nebinop
  | LEQ #leqbinop
  | GEQ #geqbinop
  ;

rvalue
  : flatFact #flatFactRvalue
  | pred_arg binop pred_arg #binopRvalue
  ;

fact
    : flatFact #flatFactFact
    | principal CANSAY fact #canSayFact
    ;

assertion
    : fact '.' #factAssertion
    | fact ':-' rvalue (',' rvalue )* '.' #hornClauseAssertion
    ;

// The VARIABLEs following "Export" are path names where JSON files containing
// private or public keys are stored
saysAssertion
    : principal SAYS assertion (EXPORT VARIABLE)? #saysSingle
    | principal SAYS '{' assertion+ '}' (EXPORT VARIABLE)?  #saysMulti
    ;

keyBind
    : BINDEX principal VARIABLE #bindpriv
    | BINDIM principal VARIABLE #bindpub
    ;

query
    : VARIABLE '=' QUERY principal SAYS fact '?'
    ;

// The VARIABLE here refers to a filename containing a signed policy statement.
importAssertion
    : IMPORT principal SAYS VARIABLE
    ;

// The name "type" would cause name collisions in the code that antlr generates
authLogicType
    : NUMBERTYPE #numberType
    | PRINCIPALTYPE #principalType
    | VARIABLE #customType
    ;

relationDeclaration
    : '.decl' ATTRIBUTE? VARIABLE '(' VARIABLE ':' authLogicType (',' VARIABLE ':' authLogicType)* ')'
    ;

program
    : ((saysAssertion | query | keyBind | importAssertion | relationDeclaration))+
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------

// keywords
QUERY: 'query' ;
SAYS: 'says';
CANACTAS: 'canActAs';
CANSAY: 'canSay';
// These are related to importing/exporting saysAssertions from other 
// principals.
EXPORT: 'exportTo';
IMPORT: 'import';
BINDEX: 'BindPrivKey';
BINDIM: 'BindPubKey';

// These are keywords are related to relation declarations
// and types.
NUMBERTYPE: 'Number';
PRINCIPALTYPE: 'Principal';
ATTRIBUTE: 'attribute';

// TODO (#661) Separate out quotes and % from VARIABLE and tokenize them.
// Identifiers wrapped in quotes are constants whereas
// identifiers without quotes are variables.
VARIABLE : [_a-zA-Z][_a-zA-Z0-9/.#:]*;
STRING_LITERAL : '"' [_a-zA-Z%][@_a-zA-Z0-9/.#:-]* '"';
MULTILINE_STRING_LITERAL : '"""' [\n _a-zA-Z+-][\n _a-zA-Z0-9/.#:+=-]* '"""';

NUMLITERAL : [0-9]+;

NEG: '!';

// BINOPS
LESSTHAN: '<';
GRTHAN: '>';
EQUALS: '=';
NEQUALS: '!=';
LEQ: '<=';
GEQ: '>=';

WHITESPACE_IGNORE
    : [ \r\t\n]+ -> skip
    ;

COMMENT_SINGLE
    : '//' ~[\r\n]* '\r'? '\n' -> skip
    ;

COMMENT_MULTI
    : '/*' .*? '*/' -> skip
    ;
