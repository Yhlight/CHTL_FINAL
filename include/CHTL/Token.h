#pragma once

#include "Common.h"
#include <string_view>

namespace CHTL {

// Token类型枚举
enum class TokenType {
    // 基础Token
    EOF_TOKEN,
    UNKNOWN,
    WHITESPACE,
    NEWLINE,
    
    // 标识符和字面量
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    BOOLEAN_LITERAL,
    
    // 分隔符
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }
    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    LEFT_BRACKET,    // [
    RIGHT_BRACKET,   // ]
    SEMICOLON,       // ;
    COMMA,           // ,
    DOT,             // .
    COLON,           // :
    EQUALS,          // =
    
    // 操作符
    PLUS,            // +
    MINUS,           // -
    MULTIPLY,        // *
    DIVIDE,          // /
    MODULO,          // %
    POWER,           // **
    LESS_THAN,       // <
    GREATER_THAN,    // >
    LESS_EQUAL,      // <=
    GREATER_EQUAL,   // >=
    EQUAL,           // ==
    NOT_EQUAL,       // !=
    LOGICAL_AND,     // &&
    LOGICAL_OR,      // ||
    NOT,             // !
    
    // 关键字
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_CONFIGURATION,
    KEYWORD_ORIGIN,
    KEYWORD_USE,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_AT_TOP,
    KEYWORD_AT_BOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_EXCEPT,
    KEYWORD_HTML5,
    
    // 注释
    LINE_COMMENT,    // //
    BLOCK_COMMENT,   // /* */
    GENERATOR_COMMENT, // #
    
    // 特殊Token
    AT_SYMBOL,       // @
    DOLLAR_SYMBOL,   // $
    AMPERSAND,       // &
    HASH_SYMBOL,     // #
    QUESTION_MARK,   // ?
    EXCLAMATION,     // !
    UNDERSCORE,      // _
    PIPE,            // |
    TILDE,           // ~
    CARET,           // ^
    
    // CHTL JS特殊Token
    DOUBLE_LEFT_BRACE,  // {{
    DOUBLE_RIGHT_BRACE, // }}
    ARROW,              // ->
    DOUBLE_ARROW,       // =>
    
    // 模板和自定义相关
    TEMPLATE_STYLE,     // @Style
    TEMPLATE_ELEMENT,   // @Element
    TEMPLATE_VAR,       // @Var
    CUSTOM_STYLE,       // [Custom] @Style
    CUSTOM_ELEMENT,     // [Custom] @Element
    CUSTOM_VAR,         // [Custom] @Var
    ORIGIN_HTML,        // [Origin] @Html
    ORIGIN_STYLE,       // [Origin] @Style
    ORIGIN_JAVASCRIPT,  // [Origin] @JavaScript
    IMPORT_HTML,        // [Import] @Html
    IMPORT_STYLE,       // [Import] @Style
    IMPORT_JAVASCRIPT,  // [Import] @JavaScript
    IMPORT_CHTL,        // [Import] @Chtl
    IMPORT_CJMOD,       // [Import] @CJmod
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    CONFIG,             // @Config
    INFO,               // [Info]
    EXPORT,             // [Export]
    
    // 选择器相关
    CLASS_SELECTOR,     // .
    ID_SELECTOR,        // #
    DESCENDANT_SELECTOR,// 空格
    CHILD_SELECTOR,     // >
    ADJACENT_SELECTOR,  // +
    GENERAL_SIBLING_SELECTOR, // ~
    ATTRIBUTE_SELECTOR, // [attr]
    PSEUDO_CLASS,       // :hover
    PSEUDO_ELEMENT,     // ::before
    INDEX_SELECTOR,     // [0]
    
    // 单位
    PX, EM, REM, PERCENT, VW, VH, CM, MM, IN, PT, PC
};

// Token结构
struct Token {
    TokenType type;
    StringView value;
    SourceLocation location;
    
    Token() : type(TokenType::UNKNOWN) {}
    Token(TokenType t, StringView v, const SourceLocation& loc = {})
        : type(t), value(v), location(loc) {}
    
    bool isKeyword() const {
        return type >= TokenType::KEYWORD_TEXT && 
               type <= TokenType::KEYWORD_HTML5;
    }
    
    bool isOperator() const {
        return type >= TokenType::PLUS && type <= TokenType::NOT;
    }
    
    bool isDelimiter() const {
        return type >= TokenType::LEFT_BRACE && type <= TokenType::COLON;
    }
    
    bool isLiteral() const {
        return type == TokenType::STRING_LITERAL ||
               type == TokenType::NUMBER_LITERAL ||
               type == TokenType::BOOLEAN_LITERAL;
    }
    
    bool isComment() const {
        return type == TokenType::LINE_COMMENT ||
               type == TokenType::BLOCK_COMMENT ||
               type == TokenType::GENERATOR_COMMENT;
    }
    
    String toString() const {
        return String(value);
    }
};

// Token列表
using TokenList = std::vector<Token>;

// Token迭代器
class TokenIterator {
private:
    const TokenList* tokens_;
    size_t index_;
    
public:
    TokenIterator(const TokenList& tokens, size_t index = 0)
        : tokens_(&tokens), index_(index) {}
    
    const Token& operator*() const { return (*tokens_)[index_]; }
    const Token* operator->() const { return &(*tokens_)[index_]; }
    
    TokenIterator& operator++() { ++index_; return *this; }
    TokenIterator operator++(int) { 
        TokenIterator temp = *this; 
        ++index_; 
        return temp; 
    }
    
    bool operator==(const TokenIterator& other) const {
        return tokens_ == other.tokens_ && index_ == other.index_;
    }
    
    bool operator!=(const TokenIterator& other) const {
        return !(*this == other);
    }
    
    size_t getIndex() const { return index_; }
    bool hasNext() const { return index_ < tokens_->size() - 1; }
    bool hasPrevious() const { return index_ > 0; }
    
    const Token& peek(int offset = 1) const {
        size_t peekIndex = index_ + offset;
        if (peekIndex >= tokens_->size()) {
            static Token eofToken(TokenType::EOF_TOKEN, "");
            return eofToken;
        }
        return (*tokens_)[peekIndex];
    }
    
    const Token& current() const { return (*tokens_)[index_]; }
    void advance() { ++index_; }
    void retreat() { if (index_ > 0) --index_; }
};

} // namespace CHTL