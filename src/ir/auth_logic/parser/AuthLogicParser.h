
// Generated from /usr/local/google/home/harshamandadi/.cache/bazel/_bazel_harshamandadi/f409c9ae133702a7ffaea49f1813009b/sandbox/linux-sandbox/1/execroot/__main__/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"




class  AuthLogicParser : public antlr4::Parser {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, QUERY = 10, SAYS = 11, CANACTAS = 12, CANSAY = 13, 
    EXPORT = 14, IMPORT = 15, BINDEX = 16, BINDIM = 17, ID = 18, NEG = 19, 
    WHITESPACE_IGNORE = 20, COMMENT_SINGLE = 21, COMMENT_MULTI = 22
  };

  enum {
    RulePrincipal = 0, RulePredicate = 1, RuleVerbphrase = 2, RuleFlatFact = 3, 
    RuleFact = 4, RuleAssertion = 5, RuleSaysAssertion = 6, RuleKeyBind = 7, 
    RuleQuery = 8, RuleImportAssertion = 9, RuleProgram = 10
  };

  AuthLogicParser(antlr4::TokenStream *input);
  ~AuthLogicParser();

  virtual std::string getGrammarFileName() const override;
  virtual const antlr4::atn::ATN& getATN() const override { return _atn; };
  virtual const std::vector<std::string>& getTokenNames() const override { return _tokenNames; }; // deprecated: use vocabulary instead.
  virtual const std::vector<std::string>& getRuleNames() const override;
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;


  class PrincipalContext;
  class PredicateContext;
  class VerbphraseContext;
  class FlatFactContext;
  class FactContext;
  class AssertionContext;
  class SaysAssertionContext;
  class KeyBindContext;
  class QueryContext;
  class ImportAssertionContext;
  class ProgramContext; 

  class  PrincipalContext : public antlr4::ParserRuleContext {
  public:
    PrincipalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ID();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PrincipalContext* principal();

  class  PredicateContext : public antlr4::ParserRuleContext {
  public:
    PredicateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> ID();
    antlr4::tree::TerminalNode* ID(size_t i);
    antlr4::tree::TerminalNode *NEG();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PredicateContext* predicate();

  class  VerbphraseContext : public antlr4::ParserRuleContext {
  public:
    VerbphraseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    VerbphraseContext() = default;
    void copyFrom(VerbphraseContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PredphraseContext : public VerbphraseContext {
  public:
    PredphraseContext(VerbphraseContext *ctx);

    PredicateContext *predicate();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  ActsAsPhraseContext : public VerbphraseContext {
  public:
    ActsAsPhraseContext(VerbphraseContext *ctx);

    antlr4::tree::TerminalNode *CANACTAS();
    PrincipalContext *principal();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  VerbphraseContext* verbphrase();

  class  FlatFactContext : public antlr4::ParserRuleContext {
  public:
    FlatFactContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    FlatFactContext() = default;
    void copyFrom(FlatFactContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  PrinFactContext : public FlatFactContext {
  public:
    PrinFactContext(FlatFactContext *ctx);

    PrincipalContext *principal();
    VerbphraseContext *verbphrase();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  PredFactContext : public FlatFactContext {
  public:
    PredFactContext(FlatFactContext *ctx);

    PredicateContext *predicate();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  FlatFactContext* flatFact();

  class  FactContext : public antlr4::ParserRuleContext {
  public:
    FactContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    FactContext() = default;
    void copyFrom(FactContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  FlatFactFactContext : public FactContext {
  public:
    FlatFactFactContext(FactContext *ctx);

    FlatFactContext *flatFact();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  CanSayFactContext : public FactContext {
  public:
    CanSayFactContext(FactContext *ctx);

    PrincipalContext *principal();
    antlr4::tree::TerminalNode *CANSAY();
    FactContext *fact();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  FactContext* fact();

  class  AssertionContext : public antlr4::ParserRuleContext {
  public:
    AssertionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    AssertionContext() = default;
    void copyFrom(AssertionContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  FactAssertionContext : public AssertionContext {
  public:
    FactAssertionContext(AssertionContext *ctx);

    FactContext *fact();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  HornClauseAssertionContext : public AssertionContext {
  public:
    HornClauseAssertionContext(AssertionContext *ctx);

    FactContext *fact();
    std::vector<FlatFactContext *> flatFact();
    FlatFactContext* flatFact(size_t i);
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  AssertionContext* assertion();

  class  SaysAssertionContext : public antlr4::ParserRuleContext {
  public:
    SaysAssertionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    SaysAssertionContext() = default;
    void copyFrom(SaysAssertionContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  SaysMultiContext : public SaysAssertionContext {
  public:
    SaysMultiContext(SaysAssertionContext *ctx);

    PrincipalContext *principal();
    antlr4::tree::TerminalNode *SAYS();
    std::vector<AssertionContext *> assertion();
    AssertionContext* assertion(size_t i);
    antlr4::tree::TerminalNode *EXPORT();
    antlr4::tree::TerminalNode *ID();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  SaysSingleContext : public SaysAssertionContext {
  public:
    SaysSingleContext(SaysAssertionContext *ctx);

    PrincipalContext *principal();
    antlr4::tree::TerminalNode *SAYS();
    AssertionContext *assertion();
    antlr4::tree::TerminalNode *EXPORT();
    antlr4::tree::TerminalNode *ID();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  SaysAssertionContext* saysAssertion();

  class  KeyBindContext : public antlr4::ParserRuleContext {
  public:
    KeyBindContext(antlr4::ParserRuleContext *parent, size_t invokingState);
   
    KeyBindContext() = default;
    void copyFrom(KeyBindContext *context);
    using antlr4::ParserRuleContext::copyFrom;

    virtual size_t getRuleIndex() const override;

   
  };

  class  BindprivContext : public KeyBindContext {
  public:
    BindprivContext(KeyBindContext *ctx);

    antlr4::tree::TerminalNode *BINDEX();
    PrincipalContext *principal();
    antlr4::tree::TerminalNode *ID();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  class  BindpubContext : public KeyBindContext {
  public:
    BindpubContext(KeyBindContext *ctx);

    antlr4::tree::TerminalNode *BINDIM();
    PrincipalContext *principal();
    antlr4::tree::TerminalNode *ID();
    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
  };

  KeyBindContext* keyBind();

  class  QueryContext : public antlr4::ParserRuleContext {
  public:
    QueryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *ID();
    antlr4::tree::TerminalNode *QUERY();
    PrincipalContext *principal();
    antlr4::tree::TerminalNode *SAYS();
    FactContext *fact();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  QueryContext* query();

  class  ImportAssertionContext : public antlr4::ParserRuleContext {
  public:
    ImportAssertionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *IMPORT();
    PrincipalContext *principal();
    antlr4::tree::TerminalNode *SAYS();
    antlr4::tree::TerminalNode *ID();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ImportAssertionContext* importAssertion();

  class  ProgramContext : public antlr4::ParserRuleContext {
  public:
    ProgramContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<SaysAssertionContext *> saysAssertion();
    SaysAssertionContext* saysAssertion(size_t i);
    std::vector<QueryContext *> query();
    QueryContext* query(size_t i);
    std::vector<KeyBindContext *> keyBind();
    KeyBindContext* keyBind(size_t i);
    std::vector<ImportAssertionContext *> importAssertion();
    ImportAssertionContext* importAssertion(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual antlrcpp::Any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ProgramContext* program();


private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

