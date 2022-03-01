
// Generated from /usr/local/google/home/harshamandadi/.cache/bazel/_bazel_harshamandadi/f409c9ae133702a7ffaea49f1813009b/sandbox/linux-sandbox/1/execroot/__main__/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "AuthLogicParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by AuthLogicParser.
 */
class  AuthLogicListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterPrincipal(AuthLogicParser::PrincipalContext *ctx) = 0;
  virtual void exitPrincipal(AuthLogicParser::PrincipalContext *ctx) = 0;

  virtual void enterPredicate(AuthLogicParser::PredicateContext *ctx) = 0;
  virtual void exitPredicate(AuthLogicParser::PredicateContext *ctx) = 0;

  virtual void enterPredphrase(AuthLogicParser::PredphraseContext *ctx) = 0;
  virtual void exitPredphrase(AuthLogicParser::PredphraseContext *ctx) = 0;

  virtual void enterActsAsPhrase(AuthLogicParser::ActsAsPhraseContext *ctx) = 0;
  virtual void exitActsAsPhrase(AuthLogicParser::ActsAsPhraseContext *ctx) = 0;

  virtual void enterPrinFact(AuthLogicParser::PrinFactContext *ctx) = 0;
  virtual void exitPrinFact(AuthLogicParser::PrinFactContext *ctx) = 0;

  virtual void enterPredFact(AuthLogicParser::PredFactContext *ctx) = 0;
  virtual void exitPredFact(AuthLogicParser::PredFactContext *ctx) = 0;

  virtual void enterFlatFactFact(AuthLogicParser::FlatFactFactContext *ctx) = 0;
  virtual void exitFlatFactFact(AuthLogicParser::FlatFactFactContext *ctx) = 0;

  virtual void enterCanSayFact(AuthLogicParser::CanSayFactContext *ctx) = 0;
  virtual void exitCanSayFact(AuthLogicParser::CanSayFactContext *ctx) = 0;

  virtual void enterFactAssertion(AuthLogicParser::FactAssertionContext *ctx) = 0;
  virtual void exitFactAssertion(AuthLogicParser::FactAssertionContext *ctx) = 0;

  virtual void enterHornClauseAssertion(AuthLogicParser::HornClauseAssertionContext *ctx) = 0;
  virtual void exitHornClauseAssertion(AuthLogicParser::HornClauseAssertionContext *ctx) = 0;

  virtual void enterSaysSingle(AuthLogicParser::SaysSingleContext *ctx) = 0;
  virtual void exitSaysSingle(AuthLogicParser::SaysSingleContext *ctx) = 0;

  virtual void enterSaysMulti(AuthLogicParser::SaysMultiContext *ctx) = 0;
  virtual void exitSaysMulti(AuthLogicParser::SaysMultiContext *ctx) = 0;

  virtual void enterBindpriv(AuthLogicParser::BindprivContext *ctx) = 0;
  virtual void exitBindpriv(AuthLogicParser::BindprivContext *ctx) = 0;

  virtual void enterBindpub(AuthLogicParser::BindpubContext *ctx) = 0;
  virtual void exitBindpub(AuthLogicParser::BindpubContext *ctx) = 0;

  virtual void enterQuery(AuthLogicParser::QueryContext *ctx) = 0;
  virtual void exitQuery(AuthLogicParser::QueryContext *ctx) = 0;

  virtual void enterImportAssertion(AuthLogicParser::ImportAssertionContext *ctx) = 0;
  virtual void exitImportAssertion(AuthLogicParser::ImportAssertionContext *ctx) = 0;

  virtual void enterProgram(AuthLogicParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(AuthLogicParser::ProgramContext *ctx) = 0;


};

