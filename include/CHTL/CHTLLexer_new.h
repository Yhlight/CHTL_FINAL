#pragma once

#include "CHTLContext.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <regex>

namespace CHTL {

/**
 * @brief Token type enumeration
 */
enum class TokenType {
    // Basic types
    IDENTIFIER,     // identifier
    STRING,         // string literal
    NUMBER,         // number literal
    LITERAL,        // unquoted literal
    
    // Symbols
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    SEMICOLON,      // ;
    COLON,          // :
    EQUAL,          // =
    COMMA,          // ,
    DOT,            // .
    HASH,           // #
    AT,             // @
    AMPERSAND,      // &
    QUESTION,       // ?
    EXCLAMATION,    // !
    UNDERSCORE,     // _
    ARROW,          // ->
    PIPE,           // |
    TILDE,          // ~
    CARET,          // ^
    DOLLAR,         // $
    PERCENT,        // %
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    SLASH,          // /
    BACKSLASH,      // \
    LESS_THAN,      // <
    GREATER_THAN,   // >
    
    // Comments
    COMMENT,        // general comment
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* */
    GENERATOR_COMMENT, // --
    
    // Keywords
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    CONSTRAINTS,    // [Constraints]
    CONFIGURATION,  // [Configuration]
    NAMESPACE,      // [Namespace]
    USE,            // use
    INHERIT,        // inherit
    DELETE,         // delete
    INSERT,         // insert
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM,      // at bottom
    FROM,           // from
    AS,             // as
    EXCEPT,         // except
    HTML5,          // html5
    
    // CHTL JS keywords
    FILELOADER,     // fileloader
    LISTEN,         // listen
    DELEGATE,       // delegate
    ANIMATE,        // animate
    VIR,            // vir
    ROUTER,         // router
    
    // Operators
    AND,            // &&
    OR,             // ||
    NOT,            // !
    EQUALS,         // ==
    NOT_EQUALS,     // !=
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    PLUS_EQUAL,     // +=
    MINUS_EQUAL,    // -=
    MULTIPLY_EQUAL, // *=
    DIVIDE_EQUAL,   // /=
    MODULO_EQUAL,   // %=
    
    // Special tokens
    NEWLINE,        // \n
    WHITESPACE,     // whitespace
    EOF_TOKEN,      // end of file
    UNKNOWN         // unknown token
};

/**
 * @brief Token structure
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::UNKNOWN, const std::string& v = "", 
          size_t l = 0, size_t c = 0, size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
    
    bool operator!=(const Token& other) const {
        return !(*this == other);
    }
};

/**
 * @brief CHTL Lexer class
 * 
 * Responsible for tokenizing CHTL source code into a sequence of tokens
 */
class CHTLLexer {
public:
    explicit CHTLLexer(std::shared_ptr<CHTLContext> context);
    ~CHTLLexer() = default;
    
    // Main lexing functions
    std::vector<Token> tokenize(const std::string& source);
    Token getNextToken();
    bool hasMoreTokens() const;
    
    // State management
    void reset();
    void setSource(const std::string& source);
    
    // Error handling
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
    // Utility functions
    static std::string tokenTypeToString(TokenType type);
    static bool isKeyword(const std::string& word);
    static TokenType getKeywordType(const std::string& word);
    static bool isOperator(const std::string& word);
    static TokenType getOperatorType(const std::string& word);
    static bool isSymbol(const std::string& word);
    static TokenType getSymbolType(const std::string& word);
    
private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_source;
    size_t m_position;
    size_t m_line;
    size_t m_column;
    std::vector<std::string> m_errors;
    
    // Tokenization helpers
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    void skipGeneratorComment();
    
    Token scanLineComment();
    Token scanBlockComment();
    Token scanGeneratorComment();
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanLiteral();
    Token scanOperator();
    Token scanSymbol();
    
    // Character utilities
    char current() const;
    char peek(size_t offset = 1) const;
    void advance(size_t count = 1);
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // Error reporting
    void addError(const std::string& message);
    void addError(const std::string& message, size_t line, size_t column);
    
    // Keyword and operator maps
    static const std::unordered_map<std::string, TokenType> s_keywords;
    static const std::unordered_map<std::string, TokenType> s_operators;
    static const std::unordered_map<std::string, TokenType> s_symbols;
    
    // Regex patterns
    static const std::regex s_identifierPattern;
    static const std::regex s_stringPattern;
    static const std::regex s_numberPattern;
    static const std::regex s_literalPattern;
};

/**
 * @brief Token stream iterator
 */
class TokenStream {
public:
    explicit TokenStream(const std::vector<Token>& tokens);
    ~TokenStream() = default;
    
    // Iterator functions
    const Token& current() const;
    const Token& peek(size_t offset = 1) const;
    const Token& advance();
    bool hasMore() const;
    bool isAtEnd() const;
    
    // Position management
    size_t position() const;
    void setPosition(size_t pos);
    void reset();
    
    // Lookahead functions
    bool check(TokenType type) const;
    bool check(const std::vector<TokenType>& types) const;
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    
    // Error handling
    const Token& consume(TokenType type, const std::string& message);
    void synchronize();
    
private:
    const std::vector<Token>& m_tokens;
    size_t m_position;
};

} // namespace CHTL