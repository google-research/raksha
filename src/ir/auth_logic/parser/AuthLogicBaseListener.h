
// Generated from /usr/local/google/home/harshamandadi/.cache/bazel/_bazel_harshamandadi/f409c9ae133702a7ffaea49f1813009b/sandbox/linux-sandbox/1/execroot/__main__/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"
#include "AuthLogicListener.h"


/**
 * This class provides an empty implementation of AuthLogicListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  AuthLogicBaseListener : public AuthLogicListener {
public:

  virtual void enterPrincipal(AuthLogicParser::PrincipalContext * /*ctx*/) override { }
  virtual void exitPrincipal(AuthLogicParser::PrincipalContext * /*ctx*/) override { }

  virtual void enterPredicate(AuthLogicParser::PredicateContext * /*ctx*/) override { }
  virtual void exitPredicate(AuthLogicParser::PredicateContext * /*ctx*/) override { }

  virtual void enterPredphrase(AuthLogicParser::PredphraseContext * /*ctx*/) override { }
  virtual void exitPredphrase(AuthLogicParser::PredphraseContext * /*ctx*/) override { }

  virtual void enterActsAsPhrase(AuthLogicParser::ActsAsPhraseContext * /*ctx*/) override { }
  virtual void exitActsAsPhrase(AuthLogicParser::ActsAsPhraseContext * /*ctx*/) override { }

  virtual void enterPrinFact(AuthLogicParser::PrinFactContext * /*ctx*/) override { }
  virtual void exitPrinFact(AuthLogicParser::PrinFactContext * /*ctx*/) override { }

  virtual void enterPredFact(AuthLogicParser::PredFactContext * /*ctx*/) override { }
  virtual void exitPredFact(AuthLogicParser::PredFactContext * /*ctx*/) override { }

  virtual void enterFlatFactFact(AuthLogicParser::FlatFactFactContext * /*ctx*/) override { }
  virtual void exitFlatFactFact(AuthLogicParser::FlatFactFactContext * /*ctx*/) override { }

  virtual void enterCanSayFact(AuthLogicParser::CanSayFactContext * /*ctx*/) override { }
  virtual void exitCanSayFact(AuthLogicParser::CanSayFactContext * /*ctx*/) override { }

  virtual void enterFactAssertion(AuthLogicParser::FactAssertionContext * /*ctx*/) override { }
  virtual void exitFactAssertion(AuthLogicParser::FactAssertionContext * /*ctx*/) override { }

  virtual void enterHornClauseAssertion(AuthLogicParser::HornClauseAssertionContext * /*ctx*/) override { }
  virtual void exitHornClauseAssertion(AuthLogicParser::HornClauseAssertionContext * /*ctx*/) override { }

  virtual void enterSaysSingle(AuthLogicParser::SaysSingleContext * /*ctx*/) override { }
  virtual void exitSaysSingle(AuthLogicParser::SaysSingleContext * /*ctx*/) override { }

  virtual void enterSaysMulti(AuthLogicParser::SaysMultiContext * /*ctx*/) override { }
  virtual void exitSaysMulti(AuthLogicParser::SaysMultiContext * /*ctx*/) override { }

  virtual void enterBindpriv(AuthLogicParser::BindprivContext * /*ctx*/) override { }
  virtual void exitBindpriv(AuthLogicParser::BindprivContext * /*ctx*/) override { }

  virtual void enterBindpub(AuthLogicParser::BindpubContext * /*ctx*/) override { }
  virtual void exitBindpub(AuthLogicParser::BindpubContext * /*ctx*/) override { }

  virtual void enterQuery(AuthLogicParser::QueryContext * /*ctx*/) override { }
  virtual void exitQuery(AuthLogicParser::QueryContext * /*ctx*/) override { }

  virtual void enterImportAssertion(AuthLogicParser::ImportAssertionContext * /*ctx*/) override { }
  virtual void exitImportAssertion(AuthLogicParser::ImportAssertionContext * /*ctx*/) override { }

  virtual void enterProgram(AuthLogicParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(AuthLogicParser::ProgramContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

