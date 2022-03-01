//-------------------------------------------------------------------------------
// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-------------------------------------------------------------------------------

#include <string>
#include <vector>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <memory>
#include <variant>

#include "src/ir/auth_logic/ast.h"
#include "antlr4-runtime.h"
#include "src/utils/map_iter.h"
#include "src/utils/move_append.h"
#include "src/ir/auth_logic/AuthLogic.inc/AuthLogicLexer.h" //lexer, AuthLogicparser, listener
#include "src/ir/auth_logic/AuthLogic.inc/AuthLogicParser.h"
#include "src/ir/auth_logic/AuthLogic.inc/AuthLogicVisitor.h"
#include "src/ir/datalog/program.h"

namespace raksha::ir::auth_logic {
    

Principal construct_principal (AuthLogicParser::PrincipalContext* context) {
    std::cout << "ENtering principal\n" <<std::endl;
    std::string name_ = context->ID()->getText(); //.unwrap().get_text();
    std::cout << "leaving principal\n" <<std::endl;
    return Principal(name_);
}

datalog::Predicate construct_predicate (AuthLogicParser::PredicateContext* context ) {
    std::cout << "ENtering predicate\n" <<std::endl;
    std::string name_ = context->ID(0)->getText();
    std::vector<std::string> args_;
    datalog::Sign sign_;
    size_t index = 1;

    while (context->ID(index)) {
        args_.emplace_back(context->ID(index)->getText());
        index += 1;
    }

    if (context->NEG() == nullptr) {
        sign_ = datalog::Sign::kPositive;
    }
    else {
        sign_ = datalog::Sign::kNegated;
    }
    std::cout << "leaving predicate\n" <<std::endl;
    return datalog::Predicate(name_, args_, sign_);
}



template<typename T, typename = std::enable_if_t<std::is_base_of<AuthLogicParser::VerbphraseContext, T>::value>>
T* instanceofVerbPhrase(AuthLogicParser::VerbphraseContext* context) {
     return dynamic_cast<T*>(context);
}

BaseFact construct_verbphrase(Principal left_principal_, AuthLogicParser::VerbphraseContext* context) {
    if (auto* ctx = instanceofVerbPhrase<AuthLogicParser::PredphraseContext>(context)) {

        datalog::Predicate predicate_ = construct_predicate(ctx->predicate());   //PredicateContext
        return BaseFact(Attribute(left_principal_, datalog::Predicate(predicate_)));
    }
    else if (auto* ctx = instanceofVerbPhrase<AuthLogicParser::ActsAsPhraseContext>(context)) {
        Principal right_principal_ = construct_principal(ctx->principal());    //ActsAsPhraseContext
        return BaseFact(CanActAs(left_principal_, right_principal_));

    }

    throw std::invalid_argument("Received error");
}




template<typename T, typename = std::enable_if_t<std::is_base_of<AuthLogicParser::FlatFactContext, T>::value>>
T* instanceofFlatFactContext(AuthLogicParser::FlatFactContext* context) {
     return dynamic_cast<T*>(context);
}

BaseFact construct_flatFact(AuthLogicParser::FlatFactContext* context) {
    if (auto* ctx = instanceofFlatFactContext<AuthLogicParser::PrinFactContext>(context)) {                                               //PrinFactContexr
        Principal principal_ = construct_principal(ctx->principal());
        return construct_verbphrase(principal_, ctx->verbphrase());
    }
    else if (auto* ctx = instanceofFlatFactContext<AuthLogicParser::PredFactContext>(context)) {
        datalog::Predicate predicate_ = construct_predicate(ctx->predicate());   //predFactContext
        return BaseFact(predicate_);
    }

    throw std::invalid_argument("Received error");
    
}

template<typename T, typename = std::enable_if_t<std::is_base_of<AuthLogicParser::FactContext, T>::value>>
T* instanceofFactContext(AuthLogicParser::FactContext* context) {
     return dynamic_cast<T*>(context);
}

Fact construct_fact(AuthLogicParser::FactContext* context) {
    if (auto* ctx = instanceofFactContext<AuthLogicParser::FlatFactFactContext>(context)) {                                     //FlatFactFactContext
        return Fact(construct_flatFact(ctx->flatFact()));
     }
    else if (auto* ctx = instanceofFactContext<AuthLogicParser::CanSayFactContext>(context))  {                                                       //CanSayFactContext
        return construct_fact(ctx->fact());
    }

    throw std::invalid_argument("Received error");
}

template<typename T, typename = std::enable_if_t<std::is_base_of<AuthLogicParser::AssertionContext, T>::value>>
T* instanceofAssertionContext(AuthLogicParser::AssertionContext* context) {
     return dynamic_cast<T*>(context);
}

Assertion construct_assertion(AuthLogicParser::AssertionContext* context) {
        
    if (auto* ctx = instanceofAssertionContext<AuthLogicParser::HornClauseAssertionContext>(context)){
        //Fact lhs_ = construct_fact(ctx->fact());
        std::vector<BaseFact> rhs_;
        for (auto &statement : ctx->flatFact()) {
            rhs_.emplace_back(construct_flatFact(statement));
            //utils::MoveAppend(rhs_, std::move(construct_flatFact(statement)));
        }
        return Assertion(ConditionalAssertion(construct_fact(ctx->fact()), rhs_));
        
   }
     if (auto* ctx = instanceofAssertionContext<AuthLogicParser::FactAssertionContext>(context)){
        return Assertion(construct_fact(ctx->fact()));
        
    }

    throw std::invalid_argument("Received error");
}



template<typename T, typename = std::enable_if_t<std::is_base_of<AuthLogicParser::SaysAssertionContext, T>::value>>
T* instanceofSaysAssertionContext(AuthLogicParser::SaysAssertionContext* context) {
     return dynamic_cast<T*>(context);
}

SaysAssertion construct_says_assertion(AuthLogicParser::SaysAssertionContext* context) {
    
    if (auto* ctx = instanceofSaysAssertionContext<AuthLogicParser::SaysMultiContext>(context) ) {
        std::vector<Assertion> assertions_;
        Principal principal_ = construct_principal(ctx->principal());
        for ( auto const &statement : ctx->assertion()) {
            assertions_.emplace_back(construct_assertion(statement));
        }
        return SaysAssertion(principal_, std::move(assertions_));
    }

    if (auto* ctx = instanceofSaysAssertionContext<AuthLogicParser::SaysSingleContext>(context)) {
        std::vector<Assertion> assertions_;
        Principal principal_ = construct_principal(ctx->principal());
        assertions_.emplace_back(construct_assertion(ctx->assertion()));
        return SaysAssertion(principal_, std::move(assertions_));

    }

     throw std::invalid_argument("Received error");
 }



Query construct_query (AuthLogicParser::QueryContext* context) {
    
    std::string name_ = context->ID()->getText();
    Principal principal_ = construct_principal(context->principal());
    return Query(name_, principal_, construct_fact(context->fact()));
}

Program construct_program (AuthLogicParser::ProgramContext* context){ //ProgramContext from AuthlogicParser
    
    std::vector<SaysAssertion> says_assertions_;
    std::vector<Query> queries_;

    for ( auto const &statement : context->saysAssertion()) {
        says_assertions_.emplace_back(construct_says_assertion(statement));
    }
    
    
    for ( auto const &statement : context->query()) {
        queries_.emplace_back(construct_query(statement));
    }
    
    return Program(std::move(says_assertions_), std::move(queries_));
}

/// This function produces an abstract syntax tree (AST) rooted with a program node when given the
/// textual representation of a program.
Program parse_program (const std::string& prog_text){

    antlr4::ANTLRInputStream input (prog_text);
    AuthLogicLexer lexer (&input);
    antlr4::CommonTokenStream tokens (&lexer);
    AuthLogicParser parser (&tokens);
    AuthLogicParser::ProgramContext* program_context = parser.program(); //To-do: raise exception
    return construct_program(program_context);
    
}

    

} //namespace raksha::ir::auth_logic