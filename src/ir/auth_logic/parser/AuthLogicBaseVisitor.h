
// Generated from /usr/local/google/home/harshamandadi/.cache/bazel/_bazel_harshamandadi/f409c9ae133702a7ffaea49f1813009b/sandbox/linux-sandbox/1/execroot/__main__/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "AuthLogicVisitor.h"


/**
 * This class provides an empty implementation of AuthLogicVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  AuthLogicBaseVisitor : public AuthLogicVisitor {
public:

  virtual antlrcpp::Any visitPrincipal(AuthLogicParser::PrincipalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPredicate(AuthLogicParser::PredicateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPredphrase(AuthLogicParser::PredphraseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitActsAsPhrase(AuthLogicParser::ActsAsPhraseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPrinFact(AuthLogicParser::PrinFactContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitPredFact(AuthLogicParser::PredFactContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFlatFactFact(AuthLogicParser::FlatFactFactContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitCanSayFact(AuthLogicParser::CanSayFactContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFactAssertion(AuthLogicParser::FactAssertionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitHornClauseAssertion(AuthLogicParser::HornClauseAssertionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSaysSingle(AuthLogicParser::SaysSingleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitSaysMulti(AuthLogicParser::SaysMultiContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBindpriv(AuthLogicParser::BindprivContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitBindpub(AuthLogicParser::BindpubContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitQuery(AuthLogicParser::QueryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitImportAssertion(AuthLogicParser::ImportAssertionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitProgram(AuthLogicParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }


};

