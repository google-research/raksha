
// Generated from /usr/local/google/home/harshamandadi/.cache/bazel/_bazel_harshamandadi/f409c9ae133702a7ffaea49f1813009b/sandbox/linux-sandbox/1/execroot/__main__/src/ir/auth_logic/auth_FE/AuthLogic.g4 by ANTLR 4.8

#pragma once


#include "antlr4-runtime.h"




class  AuthLogicLexer : public antlr4::Lexer {
public:
  enum {
    T__0 = 1, T__1 = 2, T__2 = 3, T__3 = 4, T__4 = 5, T__5 = 6, T__6 = 7, 
    T__7 = 8, T__8 = 9, QUERY = 10, SAYS = 11, CANACTAS = 12, CANSAY = 13, 
    EXPORT = 14, IMPORT = 15, BINDEX = 16, BINDIM = 17, ID = 18, NEG = 19, 
    WHITESPACE_IGNORE = 20, COMMENT_SINGLE = 21, COMMENT_MULTI = 22
  };

  AuthLogicLexer(antlr4::CharStream *input);
  ~AuthLogicLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

