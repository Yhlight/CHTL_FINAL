#ifndef CHTLLEXER_H
#define CHTLLEXER_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Token types for CHTL
enum class TokenType {
    // Keywords
    Text,
    Style,
    Script,
    Template,
    Custom,
    Origin,
    Import,
    Namespace,
    Configuration,
    Use,
    As,
    Asterisk,
    
    // Operators
    Colon,      // :
    Equal,      // =
    Semicolon,  // ;
    Comma,      // ,
    Dot,        // .
    Slash,      // /
    At,         // @
    Hash,       // #
    Exclamation, // !
    Question,   // ?
    Ampersand,  // &
    
    // Brackets
    LeftBrace,  // {
    RightBrace, // }
    LeftBracket, // [
    RightBracket, // ]
    LeftParen,  // (
    RightParen, // )
    
    // Comparison operators
    GreaterThan,    // >
    LessThan,       // <
    GreaterEqual,   // >=
    LessEqual,      // <=
    EqualEqual,     // ==
    NotEqual,       // !=
    
    // Logical operators
    And,        // &&
    Or,         // ||
    
    // Special
    Identifier,
    StringLiteral,
    NumberLiteral,
    Comment,
    MultiLineComment,
    GeneratorComment,
    Whitespace,
    Newline,
    EndOfFile
};

// Token structure
struct Token {
    TokenType token_type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType type, const std::string& val, size_t l, size_t c, size_t p)
        : token_type(type), value(val), line(l), column(c), position(p) {}
};

// CHTL Lexer
class CHTLLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // Helper methods
    char currentChar() const;
    char peekChar() const;
    void advance();
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    std::string readString(char quote);
    std::string readGeneratorComment();
    std::string readIdentifier();
    std::string readNumber();
    TokenType getKeywordTokenType(const std::string& identifier);
    Token createToken(TokenType token_type, const std::string& value, size_t line, size_t column, size_t position);
    std::unique_ptr<Token> nextToken();

public:
    CHTLLexer();
    
    // Main tokenization method
    std::vector<Token> tokenize(const std::string& source);
    
    // Utility methods
    void reset();
    bool isAtEnd() const;
};

} // namespace CHTL

#endif // CHTLLEXER_H