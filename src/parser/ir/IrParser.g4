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

grammar IrParser;

// Note: when changing this file, errors thrown by antlr will not
// be gracefully passed to cargo because errors in build scripts are 
// suppressed. Instead, the rust files in antlr_gen will remain unchanged.
// You can try to unsurpress them with "cargo build -vv", or run
// the antlr command that build.rs runs manually.

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------

operator_name
    : ID
    ;
booleanType
    : 'True'
    | 'False'
    ;
//Califiration needed. Current ID is expected to start with a alpha and can have numbers, #, :.
attribute
    : ID ':' NUMLITERAL
    | ID ':' ID
    | ID ':' booleanType
    ;
attributeList
    : attribute (',' attribute)*
    ;
//Clarification needed. Is ID is expected to start with a alpha?
argument
    : ANY
    | '%'NUMLITERAL'.'ID
    ;
argumentList
    : argument (',' argument)*
    ;
operation
    : RESULT '=' ID '['(attributeList)?']''('(argumentList)?')'
    ;
irProgram
    : (operation)+
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------

// keywords
ANY: '<<ANY>>';

// Identifiers wrapped in quotes are constants whereas
// identifiers without quotes are variables.
ID : ('"')? [_a-zA-Z][_a-zA-Z0-9/.#:]* ('"')?;
NUMLITERAL : [0-9]+;
RESULT : '%'[0-9]+;

WHITESPACE_IGNORE
    : [ \r\t\n]+ -> skip
    ;

COMMENT_SINGLE
    : '//' ~[\r\n]* '\r'? '\n' -> skip
    ;

COMMENT_MULTI
    : '/*' .*? '*/' -> skip
    ;
