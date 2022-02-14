/*
 * Copyright 2021 The Raksha Authors
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
// Principals are only superficially syntactically distinct from IDs at the
// moment, but they are separate syntactic objects because the principal
// syntax could plausibly change later.
principal
    : ID
    ;

predicate
    : (NEG)? ID '(' ID (',' ID)* ')'
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

// The IDs following "Export" are path names where JSON files containing
// private or public keys are stored
saysAssertion
    : principal SAYS assertion (EXPORT ID)? #saysSingle
    | principal SAYS '{' assertion+ '}' (EXPORT ID)?  #saysMulti
    ;

keyBind
    : BINDEX principal ID #bindpriv
    | BINDIM principal ID #bindpub
    ;

query
    : ID '=' QUERY principal SAYS fact '?'
    ;

// The ID here refers to a filename containing a signed policy statement.
importAssertion
    : IMPORT principal SAYS ID
    ;

program
    : ((saysAssertion | query | keyBind | importAssertion))+
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


// Identifiers wrapped in quotes are constants whereas
// identifiers without quotes are variables.
ID : ('"')? [_a-zA-Z0-9/.#:]* ('"')?;

NEG: '!';

WHITESPACE_IGNORE
    : [ \r\t\n]+ -> skip
    ;

COMMENT_SINGLE
    : '//' ~[\r\n]* '\r'? '\n' -> skip
    ;

COMMENT_MULTI
    : '/*' .*? '*/' -> skip
    ;
