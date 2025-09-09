#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    COMMENT,        // 注释
    
    // 分隔符
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    COLON,          // :
    EQUAL,          // =
    
    // 运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    AND,            // &&
    OR,             // ||
    NOT,            // !
    GREATER,        // >
    LESS,           // <
    GREATER_EQUAL,  // >=
    LESS_EQUAL,     // <=
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    
    // 特殊符号
    AT,             // @
    HASH,           // #
    DOLLAR,         // $
    QUESTION,       // ?
    EXCLAMATION,    // !
    AMPERSAND,      // &
    PIPE,           // |
    TILDE,          // ~
    CARET,          // ^
    
    // 关键字
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // CHTL关键字
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    USE,            // use
    EXCEPT,         // except
    DELETE,         // delete
    INSERT,         // insert
    INHERIT,        // inherit
    FROM,           // from
    AS,             // as
    
    // 位置标记
    NEWLINE,        // 换行
    WHITESPACE,     // 空白
    EOF_TOKEN,      // 文件结束
    
    // 错误
    ERROR           // 错误标记
};

class Token {
public:
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::ERROR, const std::string& v = "", 
          size_t l = 0, size_t c = 0, size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
    
    bool operator!=(const Token& other) const {
        return !(*this == other);
    }
    
    std::string toString() const;
    bool isKeyword() const;
    bool isOperator() const;
    bool isDelimiter() const;
    bool isLiteral() const;
};

class TokenStream {
private:
    std::vector<Token> tokens;
    size_t current;
    
public:
    TokenStream() : current(0) {}
    TokenStream(const std::vector<Token>& t) : tokens(t), current(0) {}
    
    void addToken(const Token& token);
    Token peek(size_t offset = 0) const;
    Token consume();
    bool hasMore() const;
    size_t size() const;
    void reset();
    size_t getCurrentPosition() const;
    
    // 便利方法
    bool match(TokenType type) const;
    bool match(const std::string& value) const;
    bool match(TokenType type, const std::string& value) const;
    
    // 错误处理
    void throwError(const std::string& message) const;
};

// 关键字映射
extern const std::map<std::string, TokenType> KEYWORDS;
extern const std::map<std::string, TokenType> OPERATORS;
extern const std::map<std::string, TokenType> DELIMITERS;

// 工具函数
TokenType getTokenType(const std::string& value);
std::string tokenTypeToString(TokenType type);
bool isKeyword(const std::string& value);
bool isOperator(const std::string& value);
bool isDelimiter(const std::string& value);

} // namespace CHTL