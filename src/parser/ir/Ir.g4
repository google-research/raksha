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


attribute
    : ID ':' NUMLITERAL #numAttribute
    | ID ':' ('"')?ID('"')? #stringAttribute
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
irProgram
    : operation
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------

// keywords
ANY: '<<ANY>>';

ID : [_a-zA-Z0-9/.]*;
NUMLITERAL : [0-9]+;
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
