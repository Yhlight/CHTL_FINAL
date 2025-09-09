
// Generated from grammar/CHTLJS.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"


namespace chtl.antlr {


class  CHTLJSLexer : public antlr4::Lexer {
public:
  enum {
    VAR = 1, LET = 2, CONST = 3, FUNCTION = 4, IF = 5, ELSE = 6, FOR = 7, 
    WHILE = 8, SWITCH = 9, CASE = 10, DEFAULT = 11, BREAK = 12, CONTINUE = 13, 
    RETURN = 14, THEN = 15, FILELOADER = 16, LISTEN = 17, DELEGATE = 18, 
    ANIMATE = 19, VIR = 20, ROUTER = 21, UTIL = 22, INEVERAWAY = 23, LEFT_BRACE = 24, 
    RIGHT_BRACE = 25, LEFT_BRACKET = 26, RIGHT_BRACKET = 27, LEFT_PAREN = 28, 
    RIGHT_PAREN = 29, SEMICOLON = 30, COMMA = 31, DOT = 32, COLON = 33, 
    QUESTION = 34, PLUS = 35, MINUS = 36, MULTIPLY = 37, DIVIDE = 38, MODULO = 39, 
    AND = 40, OR = 41, NOT = 42, GREATER = 43, LESS = 44, GREATER_EQUAL = 45, 
    LESS_EQUAL = 46, EQUAL_EQUAL = 47, NOT_EQUAL = 48, PLUS_EQUAL = 49, 
    MINUS_EQUAL = 50, MULTIPLY_EQUAL = 51, DIVIDE_EQUAL = 52, MODULO_EQUAL = 53, 
    HASH = 54, ASTERISK = 55, DOUBLE_BRACE = 56, STRING = 57, NUMBER = 58, 
    BOOLEAN = 59, NULL_ = 60, UNDEFINED = 61, IDENTIFIER = 62, COMMENT = 63, 
    MULTILINE_COMMENT = 64, WHITESPACE = 65
  };

  explicit CHTLJSLexer(antlr4::CharStream *input);

  ~CHTLJSLexer() override;


  std::string getGrammarFileName() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const std::vector<std::string>& getChannelNames() const override;

  const std::vector<std::string>& getModeNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;

  const antlr4::atn::ATN& getATN() const override;

  // By default the static state used to implement the lexer is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:

  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

};

}  // namespace chtl.antlr
