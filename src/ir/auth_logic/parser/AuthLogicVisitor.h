
// Generated from /usr/local/google/home/harshamandadi/.cache/bazel/_bazel_harshamandadi/f409c9ae133702a7ffaea49f1813009b/sandbox/linux-sandbox/1/execroot/__main__/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "AuthLogicParser.h"



/**
 * This class defines an abstract visitor for a parse tree
 * produced by AuthLogicParser.
 */
class  AuthLogicVisitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by AuthLogicParser.
   */
    virtual antlrcpp::Any visitPrincipal(AuthLogicParser::PrincipalContext *context) = 0;

    virtual antlrcpp::Any visitPredicate(AuthLogicParser::PredicateContext *context) = 0;

    virtual antlrcpp::Any visitPredphrase(AuthLogicParser::PredphraseContext *context) = 0;

    virtual antlrcpp::Any visitActsAsPhrase(AuthLogicParser::ActsAsPhraseContext *context) = 0;

    virtual antlrcpp::Any visitPrinFact(AuthLogicParser::PrinFactContext *context) = 0;

    virtual antlrcpp::Any visitPredFact(AuthLogicParser::PredFactContext *context) = 0;

    virtual antlrcpp::Any visitFlatFactFact(AuthLogicParser::FlatFactFactContext *context) = 0;

    virtual antlrcpp::Any visitCanSayFact(AuthLogicParser::CanSayFactContext *context) = 0;

    virtual antlrcpp::Any visitFactAssertion(AuthLogicParser::FactAssertionContext *context) = 0;

    virtual antlrcpp::Any visitHornClauseAssertion(AuthLogicParser::HornClauseAssertionContext *context) = 0;

    virtual antlrcpp::Any visitSaysSingle(AuthLogicParser::SaysSingleContext *context) = 0;

    virtual antlrcpp::Any visitSaysMulti(AuthLogicParser::SaysMultiContext *context) = 0;

    virtual antlrcpp::Any visitBindpriv(AuthLogicParser::BindprivContext *context) = 0;

    virtual antlrcpp::Any visitBindpub(AuthLogicParser::BindpubContext *context) = 0;

    virtual antlrcpp::Any visitQuery(AuthLogicParser::QueryContext *context) = 0;

    virtual antlrcpp::Any visitImportAssertion(AuthLogicParser::ImportAssertionContext *context) = 0;

    virtual antlrcpp::Any visitProgram(AuthLogicParser::ProgramContext *context) = 0;


};

