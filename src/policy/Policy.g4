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

grammar Policy;

//-----------------------------------------------------------------------------
// Parser
//-----------------------------------------------------------------------------

accessPath
    : 'op.operator'
    | 'op.args' '[' NUMLITERAL ']'
    | 'op.attributes' '[' (STRING_LITERAL | NUMLITERAL) ']' //TODO:support floatLiteral as well
    | 'action.data'  '[' NUMLITERAL ']'
    | 'action.result' '[' NUMLITERAL ']'
    | 'data'
    | STRING_LITERAL // TODO: Access path to Ir value. Make this more structured later.
    ;
attributeConstant
    : STRING_LITERAL | NUMLITERAL | 'TRUE' | 'FALSE'
    ;

attributeName
    : VARIABLE
    ;

actionName
    : STRING_LITERAL
    ;

attributeAccessor
    :  accessPath ':' attributeName
    ;

attributeValue
 : attributeAccessor
 | attributeConstant
 ;

booleanFunctionName
    : VARIABLE
    ;

booleanFunction
    :booleanFunctionName '(' attributeValue (',' attributeValue)* ')'
    ;

binaryOp
    : '=' | '<' | '>' | '<=' | '>=' | '!='
    ;

attributeComparison
    : attributeValue binaryOp attributeValue
    ;

predicate
    : attributeComparison | booleanFunction
    ;

actionFieldType
    : 'Int' | 'String'
    ;

actionFieldName
    : VARIABLE ('[' NUMLITERAL ']')?
    ;

actionFieldDecl
    :  actionFieldType actionFieldName
    ;

actionDeclaration
    : 'decl' 'action' actionName '{' (actionFieldDecl ',')*  '}'
    ;

categoryName
    : STRING_LITERAL
    ;

categoryDeclaration
    : 'decl' 'category'  categoryName
    ;

principalName
    : STRING_LITERAL
    ;

principalDeclaration
    : 'decl' 'principal' principalName
    ;

declaration
    : actionDeclaration | principalDeclaration | categoryDeclaration
    ;

conditions
    : '{' predicate (',' predicate)* '}'
    ;

allowRule
    : 'allow' 'action' actionName 'on' categoryName ('by' principalName)? ('if' conditions)?
    ;

removeDataCategoryRule
    : 'remove' categoryName 'from' accessPath ('if' conditions)?
    ;

addDataCategoryRule
    : 'add' categoryName 'to' accessPath ('if' conditions)?
    ;

actionAttributeAssignment
    : actionFieldName ':=' accessPath
    | actionFieldName ':=' attributeValue
    | attributeAccessor ':=' attributeValue
    ;

actionAttributeAssignments
    : actionAttributeAssignment (',' actionAttributeAssignment)*
    ;

actionRule
    : 'op' 'is' 'action' actionName ('if'  conditions )? 'with' '{' actionAttributeAssignments? '}'
    ;

policyRule
    :  allowRule | removeDataCategoryRule | addDataCategoryRule | actionRule
    ;

policyRulesAfterAction
    : 'after' 'action' actionName '{' policyRule+ '}'
    ;

policy
    : ((declaration | policyRule | policyRulesAfterAction ))+ EOF
    ;

//-----------------------------------------------------------------------------
// Lexer
//-----------------------------------------------------------------------------

//keywords
ALLOW : 'allow';
ASSIGNOP : ':-';
COLON : ':';

VARIABLE : [_a-zA-Z][_a-zA-Z0-9/.#]*;
STRING_LITERAL : '"' ~["]*? '"';
MULTILINE_STRING_LITERAL : '"""' [\n _a-zA-Z+-][\n _a-zA-Z0-9/.#:+=-]* '"""';

//TODO Add support for floats
NUMLITERAL : [0-9]+;

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
