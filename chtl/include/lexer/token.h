#pragma once

#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace chtl {
namespace lexer {

enum class TokenType {
    // Basic tokens
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    BOOLEAN_LITERAL,
    
    // Operators
    ASSIGN,           // =
    COLON,            // :
    SEMICOLON,        // ;
    COMMA,            // ,
    DOT,              // .
    ARROW,            // ->
    
    // Arithmetic operators
    PLUS,             // +
    MINUS,            // -
    MULTIPLY,         // *
    DIVIDE,           // /
    MODULO,           // %
    POWER,            // **
    
    // Comparison operators
    EQUAL,            // ==
    NOT_EQUAL,        // !=
    LESS,             // <
    LESS_EQUAL,       // <=
    GREATER,          // >
    GREATER_EQUAL,    // >=
    
    // Logical operators
    AND,              // &&
    OR,               // ||
    NOT,              // !
    
    // Ternary operator
    QUESTION,         // ?
    COLON_TERNARY,    // :
    
    // Brackets and braces
    LEFT_PAREN,       // (
    RIGHT_PAREN,      // )
    LEFT_BRACKET,     // [
    RIGHT_BRACKET,    // ]
    LEFT_BRACE,       // {
    RIGHT_BRACE,      // }
    
    // Comments
    SINGLE_LINE_COMMENT,    // //
    MULTI_LINE_COMMENT,     // /* */
    GENERATOR_COMMENT,      // --
    
    // Keywords
    TEXT,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    USE,
    EXCEPT,
    DELETE,
    INSERT,
    INHERIT,
    FROM,
    AS,
    AT_TOP,
    AT_BOTTOM,
    AFTER,
    BEFORE,
    REPLACE,
    
    // Template keywords
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    
    // Custom keywords
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    
    // Origin keywords
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    
    // Import keywords
    IMPORT_HTML,
    IMPORT_STYLE,
    IMPORT_JAVASCRIPT,
    IMPORT_CHTL,
    IMPORT_CJMOD,
    
    // Special tokens
    EOF_TOKEN,
    NEWLINE,
    WHITESPACE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::UNKNOWN, 
          std::string v = "", 
          size_t l = 0, 
          size_t c = 0, 
          size_t p = 0)
        : type(t), value(std::move(v)), line(l), column(c), position(p) {}
    
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
    
    bool operator!=(const Token& other) const {
        return !(*this == other);
    }
    
    std::string to_string() const;
    bool is_keyword() const;
    bool is_operator() const;
    bool is_literal() const;
    bool is_comment() const;
};

class TokenStream {
private:
    std::vector<Token> tokens_;
    size_t current_index_;
    
public:
    TokenStream() : current_index_(0) {}
    explicit TokenStream(std::vector<Token> tokens) 
        : tokens_(std::move(tokens)), current_index_(0) {}
    
    const Token& current() const;
    const Token& peek(size_t offset = 1) const;
    const Token& next();
    bool has_next() const;
    bool has_peek(size_t offset = 1) const;
    
    void reset();
    size_t size() const { return tokens_.size(); }
    size_t position() const { return current_index_; }
    
    void add_token(const Token& token);
    void add_tokens(const std::vector<Token>& tokens);
    
    std::vector<Token> get_tokens() const { return tokens_; }
};

} // namespace lexer
} // namespace chtl