
// Generated from grammar/CHTL.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"


namespace chtl.antlr {


class  CHTLLexer : public antlr4::Lexer {
public:
  enum {
    TEMPLATE = 1, CUSTOM = 2, ORIGIN = 3, IMPORT = 4, NAMESPACE = 5, CONFIGURATION = 6, 
    INFO = 7, EXPORT = 8, TEXT = 9, STYLE = 10, SCRIPT = 11, USE = 12, EXCEPT = 13, 
    DELETE = 14, INSERT = 15, INHERIT = 16, FROM = 17, AS = 18, FILELOADER = 19, 
    LISTEN = 20, DELEGATE = 21, ANIMATE = 22, VIR = 23, ROUTER = 24, UTIL = 25, 
    INEVERAWAY = 26, AFTER = 27, BEFORE = 28, REPLACE = 29, AT_TOP = 30, 
    AT_BOTTOM = 31, HTML = 32, CSS = 33, JAVASCRIPT = 34, VUE = 35, HTML5 = 36, 
    LEFT_BRACE = 37, RIGHT_BRACE = 38, LEFT_BRACKET = 39, RIGHT_BRACKET = 40, 
    LEFT_PAREN = 41, RIGHT_PAREN = 42, SEMICOLON = 43, COMMA = 44, DOT = 45, 
    COLON = 46, EQUAL = 47, PLUS = 48, MINUS = 49, MULTIPLY = 50, DIVIDE = 51, 
    MODULO = 52, AND = 53, OR = 54, NOT = 55, GREATER = 56, LESS = 57, GREATER_EQUAL = 58, 
    LESS_EQUAL = 59, EQUAL_EQUAL = 60, NOT_EQUAL = 61, AT = 62, HASH = 63, 
    DOLLAR = 64, QUESTION = 65, EXCLAMATION = 66, AMPERSAND = 67, PIPE = 68, 
    TILDE = 69, CARET = 70, ARROW = 71, STRING = 72, UNQUOTED_LITERAL = 73, 
    NUMBER = 74, IDENTIFIER = 75, COMMENT = 76, GENERATOR_COMMENT = 77, 
    MULTILINE_COMMENT = 78, WHITESPACE = 79, TEXT_CONTENT = 80
  };

  explicit CHTLLexer(antlr4::CharStream *input);

  ~CHTLLexer() override;


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
