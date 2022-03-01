
// Generated from /usr/local/google/home/harshamandadi/.cache/bazel/_bazel_harshamandadi/f409c9ae133702a7ffaea49f1813009b/sandbox/linux-sandbox/1/execroot/__main__/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8


#include "AuthLogicListener.h"
#include "AuthLogicVisitor.h"

#include "AuthLogicParser.h"


using namespace antlrcpp;
using namespace antlr4;

AuthLogicParser::AuthLogicParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

AuthLogicParser::~AuthLogicParser() {
  delete _interpreter;
}

std::string AuthLogicParser::getGrammarFileName() const {
  return "AuthLogic.g4";
}

const std::vector<std::string>& AuthLogicParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& AuthLogicParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- PrincipalContext ------------------------------------------------------------------

AuthLogicParser::PrincipalContext::PrincipalContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* AuthLogicParser::PrincipalContext::ID() {
  return getToken(AuthLogicParser::ID, 0);
}


size_t AuthLogicParser::PrincipalContext::getRuleIndex() const {
  return AuthLogicParser::RulePrincipal;
}

void AuthLogicParser::PrincipalContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrincipal(this);
}

void AuthLogicParser::PrincipalContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrincipal(this);
}


antlrcpp::Any AuthLogicParser::PrincipalContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitPrincipal(this);
  else
    return visitor->visitChildren(this);
}

AuthLogicParser::PrincipalContext* AuthLogicParser::principal() {
  PrincipalContext *_localctx = _tracker.createInstance<PrincipalContext>(_ctx, getState());
  enterRule(_localctx, 0, AuthLogicParser::RulePrincipal);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(22);
    match(AuthLogicParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PredicateContext ------------------------------------------------------------------

AuthLogicParser::PredicateContext::PredicateContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<tree::TerminalNode *> AuthLogicParser::PredicateContext::ID() {
  return getTokens(AuthLogicParser::ID);
}

tree::TerminalNode* AuthLogicParser::PredicateContext::ID(size_t i) {
  return getToken(AuthLogicParser::ID, i);
}

tree::TerminalNode* AuthLogicParser::PredicateContext::NEG() {
  return getToken(AuthLogicParser::NEG, 0);
}


size_t AuthLogicParser::PredicateContext::getRuleIndex() const {
  return AuthLogicParser::RulePredicate;
}

void AuthLogicParser::PredicateContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPredicate(this);
}

void AuthLogicParser::PredicateContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPredicate(this);
}


antlrcpp::Any AuthLogicParser::PredicateContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitPredicate(this);
  else
    return visitor->visitChildren(this);
}

AuthLogicParser::PredicateContext* AuthLogicParser::predicate() {
  PredicateContext *_localctx = _tracker.createInstance<PredicateContext>(_ctx, getState());
  enterRule(_localctx, 2, AuthLogicParser::RulePredicate);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(25);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == AuthLogicParser::NEG) {
      setState(24);
      match(AuthLogicParser::NEG);
    }
    setState(27);
    match(AuthLogicParser::ID);
    setState(28);
    match(AuthLogicParser::T__0);
    setState(29);
    match(AuthLogicParser::ID);
    setState(34);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == AuthLogicParser::T__1) {
      setState(30);
      match(AuthLogicParser::T__1);
      setState(31);
      match(AuthLogicParser::ID);
      setState(36);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(37);
    match(AuthLogicParser::T__2);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VerbphraseContext ------------------------------------------------------------------

AuthLogicParser::VerbphraseContext::VerbphraseContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t AuthLogicParser::VerbphraseContext::getRuleIndex() const {
  return AuthLogicParser::RuleVerbphrase;
}

void AuthLogicParser::VerbphraseContext::copyFrom(VerbphraseContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PredphraseContext ------------------------------------------------------------------

AuthLogicParser::PredicateContext* AuthLogicParser::PredphraseContext::predicate() {
  return getRuleContext<AuthLogicParser::PredicateContext>(0);
}

AuthLogicParser::PredphraseContext::PredphraseContext(VerbphraseContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::PredphraseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPredphrase(this);
}
void AuthLogicParser::PredphraseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPredphrase(this);
}

antlrcpp::Any AuthLogicParser::PredphraseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitPredphrase(this);
  else
    return visitor->visitChildren(this);
}
//----------------- ActsAsPhraseContext ------------------------------------------------------------------

tree::TerminalNode* AuthLogicParser::ActsAsPhraseContext::CANACTAS() {
  return getToken(AuthLogicParser::CANACTAS, 0);
}

AuthLogicParser::PrincipalContext* AuthLogicParser::ActsAsPhraseContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

AuthLogicParser::ActsAsPhraseContext::ActsAsPhraseContext(VerbphraseContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::ActsAsPhraseContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterActsAsPhrase(this);
}
void AuthLogicParser::ActsAsPhraseContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitActsAsPhrase(this);
}

antlrcpp::Any AuthLogicParser::ActsAsPhraseContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitActsAsPhrase(this);
  else
    return visitor->visitChildren(this);
}
AuthLogicParser::VerbphraseContext* AuthLogicParser::verbphrase() {
  VerbphraseContext *_localctx = _tracker.createInstance<VerbphraseContext>(_ctx, getState());
  enterRule(_localctx, 4, AuthLogicParser::RuleVerbphrase);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(42);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case AuthLogicParser::ID:
      case AuthLogicParser::NEG: {
        _localctx = dynamic_cast<VerbphraseContext *>(_tracker.createInstance<AuthLogicParser::PredphraseContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(39);
        predicate();
        break;
      }

      case AuthLogicParser::CANACTAS: {
        _localctx = dynamic_cast<VerbphraseContext *>(_tracker.createInstance<AuthLogicParser::ActsAsPhraseContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(40);
        match(AuthLogicParser::CANACTAS);
        setState(41);
        principal();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FlatFactContext ------------------------------------------------------------------

AuthLogicParser::FlatFactContext::FlatFactContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t AuthLogicParser::FlatFactContext::getRuleIndex() const {
  return AuthLogicParser::RuleFlatFact;
}

void AuthLogicParser::FlatFactContext::copyFrom(FlatFactContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- PrinFactContext ------------------------------------------------------------------

AuthLogicParser::PrincipalContext* AuthLogicParser::PrinFactContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

AuthLogicParser::VerbphraseContext* AuthLogicParser::PrinFactContext::verbphrase() {
  return getRuleContext<AuthLogicParser::VerbphraseContext>(0);
}

AuthLogicParser::PrinFactContext::PrinFactContext(FlatFactContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::PrinFactContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPrinFact(this);
}
void AuthLogicParser::PrinFactContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPrinFact(this);
}

antlrcpp::Any AuthLogicParser::PrinFactContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitPrinFact(this);
  else
    return visitor->visitChildren(this);
}
//----------------- PredFactContext ------------------------------------------------------------------

AuthLogicParser::PredicateContext* AuthLogicParser::PredFactContext::predicate() {
  return getRuleContext<AuthLogicParser::PredicateContext>(0);
}

AuthLogicParser::PredFactContext::PredFactContext(FlatFactContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::PredFactContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterPredFact(this);
}
void AuthLogicParser::PredFactContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitPredFact(this);
}

antlrcpp::Any AuthLogicParser::PredFactContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitPredFact(this);
  else
    return visitor->visitChildren(this);
}
AuthLogicParser::FlatFactContext* AuthLogicParser::flatFact() {
  FlatFactContext *_localctx = _tracker.createInstance<FlatFactContext>(_ctx, getState());
  enterRule(_localctx, 6, AuthLogicParser::RuleFlatFact);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(48);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 3, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<FlatFactContext *>(_tracker.createInstance<AuthLogicParser::PrinFactContext>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(44);
      principal();
      setState(45);
      verbphrase();
      break;
    }

    case 2: {
      _localctx = dynamic_cast<FlatFactContext *>(_tracker.createInstance<AuthLogicParser::PredFactContext>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(47);
      predicate();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FactContext ------------------------------------------------------------------

AuthLogicParser::FactContext::FactContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t AuthLogicParser::FactContext::getRuleIndex() const {
  return AuthLogicParser::RuleFact;
}

void AuthLogicParser::FactContext::copyFrom(FactContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- FlatFactFactContext ------------------------------------------------------------------

AuthLogicParser::FlatFactContext* AuthLogicParser::FlatFactFactContext::flatFact() {
  return getRuleContext<AuthLogicParser::FlatFactContext>(0);
}

AuthLogicParser::FlatFactFactContext::FlatFactFactContext(FactContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::FlatFactFactContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFlatFactFact(this);
}
void AuthLogicParser::FlatFactFactContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFlatFactFact(this);
}

antlrcpp::Any AuthLogicParser::FlatFactFactContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitFlatFactFact(this);
  else
    return visitor->visitChildren(this);
}
//----------------- CanSayFactContext ------------------------------------------------------------------

AuthLogicParser::PrincipalContext* AuthLogicParser::CanSayFactContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

tree::TerminalNode* AuthLogicParser::CanSayFactContext::CANSAY() {
  return getToken(AuthLogicParser::CANSAY, 0);
}

AuthLogicParser::FactContext* AuthLogicParser::CanSayFactContext::fact() {
  return getRuleContext<AuthLogicParser::FactContext>(0);
}

AuthLogicParser::CanSayFactContext::CanSayFactContext(FactContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::CanSayFactContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterCanSayFact(this);
}
void AuthLogicParser::CanSayFactContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitCanSayFact(this);
}

antlrcpp::Any AuthLogicParser::CanSayFactContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitCanSayFact(this);
  else
    return visitor->visitChildren(this);
}
AuthLogicParser::FactContext* AuthLogicParser::fact() {
  FactContext *_localctx = _tracker.createInstance<FactContext>(_ctx, getState());
  enterRule(_localctx, 8, AuthLogicParser::RuleFact);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(55);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 4, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<FactContext *>(_tracker.createInstance<AuthLogicParser::FlatFactFactContext>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(50);
      flatFact();
      break;
    }

    case 2: {
      _localctx = dynamic_cast<FactContext *>(_tracker.createInstance<AuthLogicParser::CanSayFactContext>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(51);
      principal();
      setState(52);
      match(AuthLogicParser::CANSAY);
      setState(53);
      fact();
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- AssertionContext ------------------------------------------------------------------

AuthLogicParser::AssertionContext::AssertionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t AuthLogicParser::AssertionContext::getRuleIndex() const {
  return AuthLogicParser::RuleAssertion;
}

void AuthLogicParser::AssertionContext::copyFrom(AssertionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- FactAssertionContext ------------------------------------------------------------------

AuthLogicParser::FactContext* AuthLogicParser::FactAssertionContext::fact() {
  return getRuleContext<AuthLogicParser::FactContext>(0);
}

AuthLogicParser::FactAssertionContext::FactAssertionContext(AssertionContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::FactAssertionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterFactAssertion(this);
}
void AuthLogicParser::FactAssertionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitFactAssertion(this);
}

antlrcpp::Any AuthLogicParser::FactAssertionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitFactAssertion(this);
  else
    return visitor->visitChildren(this);
}
//----------------- HornClauseAssertionContext ------------------------------------------------------------------

AuthLogicParser::FactContext* AuthLogicParser::HornClauseAssertionContext::fact() {
  return getRuleContext<AuthLogicParser::FactContext>(0);
}

std::vector<AuthLogicParser::FlatFactContext *> AuthLogicParser::HornClauseAssertionContext::flatFact() {
  return getRuleContexts<AuthLogicParser::FlatFactContext>();
}

AuthLogicParser::FlatFactContext* AuthLogicParser::HornClauseAssertionContext::flatFact(size_t i) {
  return getRuleContext<AuthLogicParser::FlatFactContext>(i);
}

AuthLogicParser::HornClauseAssertionContext::HornClauseAssertionContext(AssertionContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::HornClauseAssertionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterHornClauseAssertion(this);
}
void AuthLogicParser::HornClauseAssertionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitHornClauseAssertion(this);
}

antlrcpp::Any AuthLogicParser::HornClauseAssertionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitHornClauseAssertion(this);
  else
    return visitor->visitChildren(this);
}
AuthLogicParser::AssertionContext* AuthLogicParser::assertion() {
  AssertionContext *_localctx = _tracker.createInstance<AssertionContext>(_ctx, getState());
  enterRule(_localctx, 10, AuthLogicParser::RuleAssertion);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(72);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 6, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<AssertionContext *>(_tracker.createInstance<AuthLogicParser::FactAssertionContext>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(57);
      fact();
      setState(58);
      match(AuthLogicParser::T__3);
      break;
    }

    case 2: {
      _localctx = dynamic_cast<AssertionContext *>(_tracker.createInstance<AuthLogicParser::HornClauseAssertionContext>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(60);
      fact();
      setState(61);
      match(AuthLogicParser::T__4);
      setState(62);
      flatFact();
      setState(67);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == AuthLogicParser::T__1) {
        setState(63);
        match(AuthLogicParser::T__1);
        setState(64);
        flatFact();
        setState(69);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(70);
      match(AuthLogicParser::T__3);
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- SaysAssertionContext ------------------------------------------------------------------

AuthLogicParser::SaysAssertionContext::SaysAssertionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t AuthLogicParser::SaysAssertionContext::getRuleIndex() const {
  return AuthLogicParser::RuleSaysAssertion;
}

void AuthLogicParser::SaysAssertionContext::copyFrom(SaysAssertionContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- SaysMultiContext ------------------------------------------------------------------

AuthLogicParser::PrincipalContext* AuthLogicParser::SaysMultiContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

tree::TerminalNode* AuthLogicParser::SaysMultiContext::SAYS() {
  return getToken(AuthLogicParser::SAYS, 0);
}

std::vector<AuthLogicParser::AssertionContext *> AuthLogicParser::SaysMultiContext::assertion() {
  return getRuleContexts<AuthLogicParser::AssertionContext>();
}

AuthLogicParser::AssertionContext* AuthLogicParser::SaysMultiContext::assertion(size_t i) {
  return getRuleContext<AuthLogicParser::AssertionContext>(i);
}

tree::TerminalNode* AuthLogicParser::SaysMultiContext::EXPORT() {
  return getToken(AuthLogicParser::EXPORT, 0);
}

tree::TerminalNode* AuthLogicParser::SaysMultiContext::ID() {
  return getToken(AuthLogicParser::ID, 0);
}

AuthLogicParser::SaysMultiContext::SaysMultiContext(SaysAssertionContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::SaysMultiContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSaysMulti(this);
}
void AuthLogicParser::SaysMultiContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSaysMulti(this);
}

antlrcpp::Any AuthLogicParser::SaysMultiContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitSaysMulti(this);
  else
    return visitor->visitChildren(this);
}
//----------------- SaysSingleContext ------------------------------------------------------------------

AuthLogicParser::PrincipalContext* AuthLogicParser::SaysSingleContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

tree::TerminalNode* AuthLogicParser::SaysSingleContext::SAYS() {
  return getToken(AuthLogicParser::SAYS, 0);
}

AuthLogicParser::AssertionContext* AuthLogicParser::SaysSingleContext::assertion() {
  return getRuleContext<AuthLogicParser::AssertionContext>(0);
}

tree::TerminalNode* AuthLogicParser::SaysSingleContext::EXPORT() {
  return getToken(AuthLogicParser::EXPORT, 0);
}

tree::TerminalNode* AuthLogicParser::SaysSingleContext::ID() {
  return getToken(AuthLogicParser::ID, 0);
}

AuthLogicParser::SaysSingleContext::SaysSingleContext(SaysAssertionContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::SaysSingleContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterSaysSingle(this);
}
void AuthLogicParser::SaysSingleContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitSaysSingle(this);
}

antlrcpp::Any AuthLogicParser::SaysSingleContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitSaysSingle(this);
  else
    return visitor->visitChildren(this);
}
AuthLogicParser::SaysAssertionContext* AuthLogicParser::saysAssertion() {
  SaysAssertionContext *_localctx = _tracker.createInstance<SaysAssertionContext>(_ctx, getState());
  enterRule(_localctx, 12, AuthLogicParser::RuleSaysAssertion);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(94);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 10, _ctx)) {
    case 1: {
      _localctx = dynamic_cast<SaysAssertionContext *>(_tracker.createInstance<AuthLogicParser::SaysSingleContext>(_localctx));
      enterOuterAlt(_localctx, 1);
      setState(74);
      principal();
      setState(75);
      match(AuthLogicParser::SAYS);
      setState(76);
      assertion();
      setState(79);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == AuthLogicParser::EXPORT) {
        setState(77);
        match(AuthLogicParser::EXPORT);
        setState(78);
        match(AuthLogicParser::ID);
      }
      break;
    }

    case 2: {
      _localctx = dynamic_cast<SaysAssertionContext *>(_tracker.createInstance<AuthLogicParser::SaysMultiContext>(_localctx));
      enterOuterAlt(_localctx, 2);
      setState(81);
      principal();
      setState(82);
      match(AuthLogicParser::SAYS);
      setState(83);
      match(AuthLogicParser::T__5);
      setState(85); 
      _errHandler->sync(this);
      _la = _input->LA(1);
      do {
        setState(84);
        assertion();
        setState(87); 
        _errHandler->sync(this);
        _la = _input->LA(1);
      } while (_la == AuthLogicParser::ID

      || _la == AuthLogicParser::NEG);
      setState(89);
      match(AuthLogicParser::T__6);
      setState(92);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if (_la == AuthLogicParser::EXPORT) {
        setState(90);
        match(AuthLogicParser::EXPORT);
        setState(91);
        match(AuthLogicParser::ID);
      }
      break;
    }

    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- KeyBindContext ------------------------------------------------------------------

AuthLogicParser::KeyBindContext::KeyBindContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}


size_t AuthLogicParser::KeyBindContext::getRuleIndex() const {
  return AuthLogicParser::RuleKeyBind;
}

void AuthLogicParser::KeyBindContext::copyFrom(KeyBindContext *ctx) {
  ParserRuleContext::copyFrom(ctx);
}

//----------------- BindprivContext ------------------------------------------------------------------

tree::TerminalNode* AuthLogicParser::BindprivContext::BINDEX() {
  return getToken(AuthLogicParser::BINDEX, 0);
}

AuthLogicParser::PrincipalContext* AuthLogicParser::BindprivContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

tree::TerminalNode* AuthLogicParser::BindprivContext::ID() {
  return getToken(AuthLogicParser::ID, 0);
}

AuthLogicParser::BindprivContext::BindprivContext(KeyBindContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::BindprivContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBindpriv(this);
}
void AuthLogicParser::BindprivContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBindpriv(this);
}

antlrcpp::Any AuthLogicParser::BindprivContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitBindpriv(this);
  else
    return visitor->visitChildren(this);
}
//----------------- BindpubContext ------------------------------------------------------------------

tree::TerminalNode* AuthLogicParser::BindpubContext::BINDIM() {
  return getToken(AuthLogicParser::BINDIM, 0);
}

AuthLogicParser::PrincipalContext* AuthLogicParser::BindpubContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

tree::TerminalNode* AuthLogicParser::BindpubContext::ID() {
  return getToken(AuthLogicParser::ID, 0);
}

AuthLogicParser::BindpubContext::BindpubContext(KeyBindContext *ctx) { copyFrom(ctx); }

void AuthLogicParser::BindpubContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterBindpub(this);
}
void AuthLogicParser::BindpubContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitBindpub(this);
}

antlrcpp::Any AuthLogicParser::BindpubContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitBindpub(this);
  else
    return visitor->visitChildren(this);
}
AuthLogicParser::KeyBindContext* AuthLogicParser::keyBind() {
  KeyBindContext *_localctx = _tracker.createInstance<KeyBindContext>(_ctx, getState());
  enterRule(_localctx, 14, AuthLogicParser::RuleKeyBind);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    setState(104);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case AuthLogicParser::BINDEX: {
        _localctx = dynamic_cast<KeyBindContext *>(_tracker.createInstance<AuthLogicParser::BindprivContext>(_localctx));
        enterOuterAlt(_localctx, 1);
        setState(96);
        match(AuthLogicParser::BINDEX);
        setState(97);
        principal();
        setState(98);
        match(AuthLogicParser::ID);
        break;
      }

      case AuthLogicParser::BINDIM: {
        _localctx = dynamic_cast<KeyBindContext *>(_tracker.createInstance<AuthLogicParser::BindpubContext>(_localctx));
        enterOuterAlt(_localctx, 2);
        setState(100);
        match(AuthLogicParser::BINDIM);
        setState(101);
        principal();
        setState(102);
        match(AuthLogicParser::ID);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- QueryContext ------------------------------------------------------------------

AuthLogicParser::QueryContext::QueryContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* AuthLogicParser::QueryContext::ID() {
  return getToken(AuthLogicParser::ID, 0);
}

tree::TerminalNode* AuthLogicParser::QueryContext::QUERY() {
  return getToken(AuthLogicParser::QUERY, 0);
}

AuthLogicParser::PrincipalContext* AuthLogicParser::QueryContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

tree::TerminalNode* AuthLogicParser::QueryContext::SAYS() {
  return getToken(AuthLogicParser::SAYS, 0);
}

AuthLogicParser::FactContext* AuthLogicParser::QueryContext::fact() {
  return getRuleContext<AuthLogicParser::FactContext>(0);
}


size_t AuthLogicParser::QueryContext::getRuleIndex() const {
  return AuthLogicParser::RuleQuery;
}

void AuthLogicParser::QueryContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterQuery(this);
}

void AuthLogicParser::QueryContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitQuery(this);
}


antlrcpp::Any AuthLogicParser::QueryContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitQuery(this);
  else
    return visitor->visitChildren(this);
}

AuthLogicParser::QueryContext* AuthLogicParser::query() {
  QueryContext *_localctx = _tracker.createInstance<QueryContext>(_ctx, getState());
  enterRule(_localctx, 16, AuthLogicParser::RuleQuery);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(106);
    match(AuthLogicParser::ID);
    setState(107);
    match(AuthLogicParser::T__7);
    setState(108);
    match(AuthLogicParser::QUERY);
    setState(109);
    principal();
    setState(110);
    match(AuthLogicParser::SAYS);
    setState(111);
    fact();
    setState(112);
    match(AuthLogicParser::T__8);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ImportAssertionContext ------------------------------------------------------------------

AuthLogicParser::ImportAssertionContext::ImportAssertionContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* AuthLogicParser::ImportAssertionContext::IMPORT() {
  return getToken(AuthLogicParser::IMPORT, 0);
}

AuthLogicParser::PrincipalContext* AuthLogicParser::ImportAssertionContext::principal() {
  return getRuleContext<AuthLogicParser::PrincipalContext>(0);
}

tree::TerminalNode* AuthLogicParser::ImportAssertionContext::SAYS() {
  return getToken(AuthLogicParser::SAYS, 0);
}

tree::TerminalNode* AuthLogicParser::ImportAssertionContext::ID() {
  return getToken(AuthLogicParser::ID, 0);
}


size_t AuthLogicParser::ImportAssertionContext::getRuleIndex() const {
  return AuthLogicParser::RuleImportAssertion;
}

void AuthLogicParser::ImportAssertionContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterImportAssertion(this);
}

void AuthLogicParser::ImportAssertionContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitImportAssertion(this);
}


antlrcpp::Any AuthLogicParser::ImportAssertionContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitImportAssertion(this);
  else
    return visitor->visitChildren(this);
}

AuthLogicParser::ImportAssertionContext* AuthLogicParser::importAssertion() {
  ImportAssertionContext *_localctx = _tracker.createInstance<ImportAssertionContext>(_ctx, getState());
  enterRule(_localctx, 18, AuthLogicParser::RuleImportAssertion);

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(114);
    match(AuthLogicParser::IMPORT);
    setState(115);
    principal();
    setState(116);
    match(AuthLogicParser::SAYS);
    setState(117);
    match(AuthLogicParser::ID);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ProgramContext ------------------------------------------------------------------

AuthLogicParser::ProgramContext::ProgramContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<AuthLogicParser::SaysAssertionContext *> AuthLogicParser::ProgramContext::saysAssertion() {
  return getRuleContexts<AuthLogicParser::SaysAssertionContext>();
}

AuthLogicParser::SaysAssertionContext* AuthLogicParser::ProgramContext::saysAssertion(size_t i) {
  return getRuleContext<AuthLogicParser::SaysAssertionContext>(i);
}

std::vector<AuthLogicParser::QueryContext *> AuthLogicParser::ProgramContext::query() {
  return getRuleContexts<AuthLogicParser::QueryContext>();
}

AuthLogicParser::QueryContext* AuthLogicParser::ProgramContext::query(size_t i) {
  return getRuleContext<AuthLogicParser::QueryContext>(i);
}

std::vector<AuthLogicParser::KeyBindContext *> AuthLogicParser::ProgramContext::keyBind() {
  return getRuleContexts<AuthLogicParser::KeyBindContext>();
}

AuthLogicParser::KeyBindContext* AuthLogicParser::ProgramContext::keyBind(size_t i) {
  return getRuleContext<AuthLogicParser::KeyBindContext>(i);
}

std::vector<AuthLogicParser::ImportAssertionContext *> AuthLogicParser::ProgramContext::importAssertion() {
  return getRuleContexts<AuthLogicParser::ImportAssertionContext>();
}

AuthLogicParser::ImportAssertionContext* AuthLogicParser::ProgramContext::importAssertion(size_t i) {
  return getRuleContext<AuthLogicParser::ImportAssertionContext>(i);
}


size_t AuthLogicParser::ProgramContext::getRuleIndex() const {
  return AuthLogicParser::RuleProgram;
}

void AuthLogicParser::ProgramContext::enterRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->enterProgram(this);
}

void AuthLogicParser::ProgramContext::exitRule(tree::ParseTreeListener *listener) {
  auto parserListener = dynamic_cast<AuthLogicListener *>(listener);
  if (parserListener != nullptr)
    parserListener->exitProgram(this);
}


antlrcpp::Any AuthLogicParser::ProgramContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<AuthLogicVisitor*>(visitor))
    return parserVisitor->visitProgram(this);
  else
    return visitor->visitChildren(this);
}

AuthLogicParser::ProgramContext* AuthLogicParser::program() {
  ProgramContext *_localctx = _tracker.createInstance<ProgramContext>(_ctx, getState());
  enterRule(_localctx, 20, AuthLogicParser::RuleProgram);
  size_t _la = 0;

  auto onExit = finally([=] {
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(125); 
    _errHandler->sync(this);
    _la = _input->LA(1);
    do {
      setState(123);
      _errHandler->sync(this);
      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
      case 1: {
        setState(119);
        saysAssertion();
        break;
      }

      case 2: {
        setState(120);
        query();
        break;
      }

      case 3: {
        setState(121);
        keyBind();
        break;
      }

      case 4: {
        setState(122);
        importAssertion();
        break;
      }

      }
      setState(127); 
      _errHandler->sync(this);
      _la = _input->LA(1);
    } while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << AuthLogicParser::IMPORT)
      | (1ULL << AuthLogicParser::BINDEX)
      | (1ULL << AuthLogicParser::BINDIM)
      | (1ULL << AuthLogicParser::ID))) != 0));
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

// Static vars and initialization.
std::vector<dfa::DFA> AuthLogicParser::_decisionToDFA;
atn::PredictionContextCache AuthLogicParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN AuthLogicParser::_atn;
std::vector<uint16_t> AuthLogicParser::_serializedATN;

std::vector<std::string> AuthLogicParser::_ruleNames = {
  "principal", "predicate", "verbphrase", "flatFact", "fact", "assertion", 
  "saysAssertion", "keyBind", "query", "importAssertion", "program"
};

std::vector<std::string> AuthLogicParser::_literalNames = {
  "", "'('", "','", "')'", "'.'", "':-'", "'{'", "'}'", "'='", "'?'", "'query'", 
  "'says'", "'canActAs'", "'canSay'", "'exportTo'", "'import'", "'BindPrivKey'", 
  "'BindPubKey'", "", "'!'"
};

std::vector<std::string> AuthLogicParser::_symbolicNames = {
  "", "", "", "", "", "", "", "", "", "", "QUERY", "SAYS", "CANACTAS", "CANSAY", 
  "EXPORT", "IMPORT", "BINDEX", "BINDIM", "ID", "NEG", "WHITESPACE_IGNORE", 
  "COMMENT_SINGLE", "COMMENT_MULTI"
};

dfa::Vocabulary AuthLogicParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> AuthLogicParser::_tokenNames;

AuthLogicParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  _serializedATN = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
    0x3, 0x18, 0x84, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 0x9, 
    0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 0x7, 0x4, 
    0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 0x4, 0xb, 0x9, 
    0xb, 0x4, 0xc, 0x9, 0xc, 0x3, 0x2, 0x3, 0x2, 0x3, 0x3, 0x5, 0x3, 0x1c, 
    0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x7, 0x3, 
    0x23, 0xa, 0x3, 0xc, 0x3, 0xe, 0x3, 0x26, 0xb, 0x3, 0x3, 0x3, 0x3, 0x3, 
    0x3, 0x4, 0x3, 0x4, 0x3, 0x4, 0x5, 0x4, 0x2d, 0xa, 0x4, 0x3, 0x5, 0x3, 
    0x5, 0x3, 0x5, 0x3, 0x5, 0x5, 0x5, 0x33, 0xa, 0x5, 0x3, 0x6, 0x3, 0x6, 
    0x3, 0x6, 0x3, 0x6, 0x3, 0x6, 0x5, 0x6, 0x3a, 0xa, 0x6, 0x3, 0x7, 0x3, 
    0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x7, 
    0x7, 0x44, 0xa, 0x7, 0xc, 0x7, 0xe, 0x7, 0x47, 0xb, 0x7, 0x3, 0x7, 0x3, 
    0x7, 0x5, 0x7, 0x4b, 0xa, 0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 
    0x3, 0x8, 0x5, 0x8, 0x52, 0xa, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 
    0x8, 0x6, 0x8, 0x58, 0xa, 0x8, 0xd, 0x8, 0xe, 0x8, 0x59, 0x3, 0x8, 0x3, 
    0x8, 0x3, 0x8, 0x5, 0x8, 0x5f, 0xa, 0x8, 0x5, 0x8, 0x61, 0xa, 0x8, 0x3, 
    0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 
    0x9, 0x5, 0x9, 0x6b, 0xa, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 
    0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 
    0x3, 0xb, 0x3, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x5, 0xc, 
    0x7e, 0xa, 0xc, 0x6, 0xc, 0x80, 0xa, 0xc, 0xd, 0xc, 0xe, 0xc, 0x81, 
    0x3, 0xc, 0x2, 0x2, 0xd, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 
    0x14, 0x16, 0x2, 0x2, 0x2, 0x88, 0x2, 0x18, 0x3, 0x2, 0x2, 0x2, 0x4, 
    0x1b, 0x3, 0x2, 0x2, 0x2, 0x6, 0x2c, 0x3, 0x2, 0x2, 0x2, 0x8, 0x32, 
    0x3, 0x2, 0x2, 0x2, 0xa, 0x39, 0x3, 0x2, 0x2, 0x2, 0xc, 0x4a, 0x3, 0x2, 
    0x2, 0x2, 0xe, 0x60, 0x3, 0x2, 0x2, 0x2, 0x10, 0x6a, 0x3, 0x2, 0x2, 
    0x2, 0x12, 0x6c, 0x3, 0x2, 0x2, 0x2, 0x14, 0x74, 0x3, 0x2, 0x2, 0x2, 
    0x16, 0x7f, 0x3, 0x2, 0x2, 0x2, 0x18, 0x19, 0x7, 0x14, 0x2, 0x2, 0x19, 
    0x3, 0x3, 0x2, 0x2, 0x2, 0x1a, 0x1c, 0x7, 0x15, 0x2, 0x2, 0x1b, 0x1a, 
    0x3, 0x2, 0x2, 0x2, 0x1b, 0x1c, 0x3, 0x2, 0x2, 0x2, 0x1c, 0x1d, 0x3, 
    0x2, 0x2, 0x2, 0x1d, 0x1e, 0x7, 0x14, 0x2, 0x2, 0x1e, 0x1f, 0x7, 0x3, 
    0x2, 0x2, 0x1f, 0x24, 0x7, 0x14, 0x2, 0x2, 0x20, 0x21, 0x7, 0x4, 0x2, 
    0x2, 0x21, 0x23, 0x7, 0x14, 0x2, 0x2, 0x22, 0x20, 0x3, 0x2, 0x2, 0x2, 
    0x23, 0x26, 0x3, 0x2, 0x2, 0x2, 0x24, 0x22, 0x3, 0x2, 0x2, 0x2, 0x24, 
    0x25, 0x3, 0x2, 0x2, 0x2, 0x25, 0x27, 0x3, 0x2, 0x2, 0x2, 0x26, 0x24, 
    0x3, 0x2, 0x2, 0x2, 0x27, 0x28, 0x7, 0x5, 0x2, 0x2, 0x28, 0x5, 0x3, 
    0x2, 0x2, 0x2, 0x29, 0x2d, 0x5, 0x4, 0x3, 0x2, 0x2a, 0x2b, 0x7, 0xe, 
    0x2, 0x2, 0x2b, 0x2d, 0x5, 0x2, 0x2, 0x2, 0x2c, 0x29, 0x3, 0x2, 0x2, 
    0x2, 0x2c, 0x2a, 0x3, 0x2, 0x2, 0x2, 0x2d, 0x7, 0x3, 0x2, 0x2, 0x2, 
    0x2e, 0x2f, 0x5, 0x2, 0x2, 0x2, 0x2f, 0x30, 0x5, 0x6, 0x4, 0x2, 0x30, 
    0x33, 0x3, 0x2, 0x2, 0x2, 0x31, 0x33, 0x5, 0x4, 0x3, 0x2, 0x32, 0x2e, 
    0x3, 0x2, 0x2, 0x2, 0x32, 0x31, 0x3, 0x2, 0x2, 0x2, 0x33, 0x9, 0x3, 
    0x2, 0x2, 0x2, 0x34, 0x3a, 0x5, 0x8, 0x5, 0x2, 0x35, 0x36, 0x5, 0x2, 
    0x2, 0x2, 0x36, 0x37, 0x7, 0xf, 0x2, 0x2, 0x37, 0x38, 0x5, 0xa, 0x6, 
    0x2, 0x38, 0x3a, 0x3, 0x2, 0x2, 0x2, 0x39, 0x34, 0x3, 0x2, 0x2, 0x2, 
    0x39, 0x35, 0x3, 0x2, 0x2, 0x2, 0x3a, 0xb, 0x3, 0x2, 0x2, 0x2, 0x3b, 
    0x3c, 0x5, 0xa, 0x6, 0x2, 0x3c, 0x3d, 0x7, 0x6, 0x2, 0x2, 0x3d, 0x4b, 
    0x3, 0x2, 0x2, 0x2, 0x3e, 0x3f, 0x5, 0xa, 0x6, 0x2, 0x3f, 0x40, 0x7, 
    0x7, 0x2, 0x2, 0x40, 0x45, 0x5, 0x8, 0x5, 0x2, 0x41, 0x42, 0x7, 0x4, 
    0x2, 0x2, 0x42, 0x44, 0x5, 0x8, 0x5, 0x2, 0x43, 0x41, 0x3, 0x2, 0x2, 
    0x2, 0x44, 0x47, 0x3, 0x2, 0x2, 0x2, 0x45, 0x43, 0x3, 0x2, 0x2, 0x2, 
    0x45, 0x46, 0x3, 0x2, 0x2, 0x2, 0x46, 0x48, 0x3, 0x2, 0x2, 0x2, 0x47, 
    0x45, 0x3, 0x2, 0x2, 0x2, 0x48, 0x49, 0x7, 0x6, 0x2, 0x2, 0x49, 0x4b, 
    0x3, 0x2, 0x2, 0x2, 0x4a, 0x3b, 0x3, 0x2, 0x2, 0x2, 0x4a, 0x3e, 0x3, 
    0x2, 0x2, 0x2, 0x4b, 0xd, 0x3, 0x2, 0x2, 0x2, 0x4c, 0x4d, 0x5, 0x2, 
    0x2, 0x2, 0x4d, 0x4e, 0x7, 0xd, 0x2, 0x2, 0x4e, 0x51, 0x5, 0xc, 0x7, 
    0x2, 0x4f, 0x50, 0x7, 0x10, 0x2, 0x2, 0x50, 0x52, 0x7, 0x14, 0x2, 0x2, 
    0x51, 0x4f, 0x3, 0x2, 0x2, 0x2, 0x51, 0x52, 0x3, 0x2, 0x2, 0x2, 0x52, 
    0x61, 0x3, 0x2, 0x2, 0x2, 0x53, 0x54, 0x5, 0x2, 0x2, 0x2, 0x54, 0x55, 
    0x7, 0xd, 0x2, 0x2, 0x55, 0x57, 0x7, 0x8, 0x2, 0x2, 0x56, 0x58, 0x5, 
    0xc, 0x7, 0x2, 0x57, 0x56, 0x3, 0x2, 0x2, 0x2, 0x58, 0x59, 0x3, 0x2, 
    0x2, 0x2, 0x59, 0x57, 0x3, 0x2, 0x2, 0x2, 0x59, 0x5a, 0x3, 0x2, 0x2, 
    0x2, 0x5a, 0x5b, 0x3, 0x2, 0x2, 0x2, 0x5b, 0x5e, 0x7, 0x9, 0x2, 0x2, 
    0x5c, 0x5d, 0x7, 0x10, 0x2, 0x2, 0x5d, 0x5f, 0x7, 0x14, 0x2, 0x2, 0x5e, 
    0x5c, 0x3, 0x2, 0x2, 0x2, 0x5e, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x5f, 0x61, 
    0x3, 0x2, 0x2, 0x2, 0x60, 0x4c, 0x3, 0x2, 0x2, 0x2, 0x60, 0x53, 0x3, 
    0x2, 0x2, 0x2, 0x61, 0xf, 0x3, 0x2, 0x2, 0x2, 0x62, 0x63, 0x7, 0x12, 
    0x2, 0x2, 0x63, 0x64, 0x5, 0x2, 0x2, 0x2, 0x64, 0x65, 0x7, 0x14, 0x2, 
    0x2, 0x65, 0x6b, 0x3, 0x2, 0x2, 0x2, 0x66, 0x67, 0x7, 0x13, 0x2, 0x2, 
    0x67, 0x68, 0x5, 0x2, 0x2, 0x2, 0x68, 0x69, 0x7, 0x14, 0x2, 0x2, 0x69, 
    0x6b, 0x3, 0x2, 0x2, 0x2, 0x6a, 0x62, 0x3, 0x2, 0x2, 0x2, 0x6a, 0x66, 
    0x3, 0x2, 0x2, 0x2, 0x6b, 0x11, 0x3, 0x2, 0x2, 0x2, 0x6c, 0x6d, 0x7, 
    0x14, 0x2, 0x2, 0x6d, 0x6e, 0x7, 0xa, 0x2, 0x2, 0x6e, 0x6f, 0x7, 0xc, 
    0x2, 0x2, 0x6f, 0x70, 0x5, 0x2, 0x2, 0x2, 0x70, 0x71, 0x7, 0xd, 0x2, 
    0x2, 0x71, 0x72, 0x5, 0xa, 0x6, 0x2, 0x72, 0x73, 0x7, 0xb, 0x2, 0x2, 
    0x73, 0x13, 0x3, 0x2, 0x2, 0x2, 0x74, 0x75, 0x7, 0x11, 0x2, 0x2, 0x75, 
    0x76, 0x5, 0x2, 0x2, 0x2, 0x76, 0x77, 0x7, 0xd, 0x2, 0x2, 0x77, 0x78, 
    0x7, 0x14, 0x2, 0x2, 0x78, 0x15, 0x3, 0x2, 0x2, 0x2, 0x79, 0x7e, 0x5, 
    0xe, 0x8, 0x2, 0x7a, 0x7e, 0x5, 0x12, 0xa, 0x2, 0x7b, 0x7e, 0x5, 0x10, 
    0x9, 0x2, 0x7c, 0x7e, 0x5, 0x14, 0xb, 0x2, 0x7d, 0x79, 0x3, 0x2, 0x2, 
    0x2, 0x7d, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x7d, 0x7b, 0x3, 0x2, 0x2, 0x2, 
    0x7d, 0x7c, 0x3, 0x2, 0x2, 0x2, 0x7e, 0x80, 0x3, 0x2, 0x2, 0x2, 0x7f, 
    0x7d, 0x3, 0x2, 0x2, 0x2, 0x80, 0x81, 0x3, 0x2, 0x2, 0x2, 0x81, 0x7f, 
    0x3, 0x2, 0x2, 0x2, 0x81, 0x82, 0x3, 0x2, 0x2, 0x2, 0x82, 0x17, 0x3, 
    0x2, 0x2, 0x2, 0x10, 0x1b, 0x24, 0x2c, 0x32, 0x39, 0x45, 0x4a, 0x51, 
    0x59, 0x5e, 0x60, 0x6a, 0x7d, 0x81, 
  };

  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

AuthLogicParser::Initializer AuthLogicParser::_init;
