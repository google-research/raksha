/*
 * Copyright 2022 Google LLC.
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

grammar Ir;

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------

stringLiteral : NUMLITERAL | ID;
attribute
    : ID ':' NUMLITERAL #numAttribute
    | ID ':' '"'stringLiteral'"' #stringAttribute
    ;
attributeList
    : attribute (',' attribute)*
    ;
value
    : ANY #anyValue
    | VALUE_ID  #namedValue
    ;
argumentList
    : value (',' value)*
    ;
operation
    : VALUE_ID '=' ID '['(attributeList)?']''('(argumentList)?')'
    ;
block
    : BLOCK ID '{' (operation)+ '}'
    ;
module
    : MODULE ID '{' (block)+ '}'
    ;
irProgram
    : module
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------

// keywords
ANY: '<<ANY>>';
BLOCK: 'block';
MODULE: 'module';


NUMLITERAL : [0-9]+;
ID : [0-9_a-zA-Z/.]+;
VALUE_ID : ('%')? ID;


WHITESPACE_IGNORE
    : [ \r\t\n]+ -> skip
    ;

COMMENT_SINGLE
    : '//' ~[\r\n]* '\r'? '\n' -> skip
    ;

COMMENT_MULTI
    : '/*' .*? '*/' -> skip
    ;
