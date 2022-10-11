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
    : ID ':' FLOATLITERAL #floatAttribute
    | ID ':' NUMLITERAL #numAttribute
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
result
    : VALUE_ID (',' VALUE_ID)*
    ;
operation
    : result '=' ID '['attributeList?']''('argumentList?')'
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

SIGN : '+' | '-';
NUMLITERAL : [0-9]+;
FRACTIONAL_PART : '.' [0-9]*;
EXPONENT: ('e'|'E') SIGN? [0-9]+;
FLOAT_TAIL
    : (FRACTIONAL_PART EXPONENT? 'l'?)
    | (FRACTIONAL_PART? EXPONENT 'l'?)
    | (FRACTIONAL_PART? EXPONENT? 'l')
    ;

// Either require a decimal point or an exponent or a trailing suffix (but allow both).
FLOATLITERAL : SIGN? NUMLITERAL FLOAT_TAIL;

ID : [0-9_a-zA-Z/.]+;
VALUE_ID : '%'? ID;


WHITESPACE_IGNORE
    : [ \r\t\n]+ -> skip
    ;

COMMENT_SINGLE
    : '//' ~[\r\n]* '\r'? '\n' -> skip
    ;

COMMENT_MULTI
    : '/*' .*? '*/' -> skip
    ;
