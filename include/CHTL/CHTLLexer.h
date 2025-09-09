#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 词法单元类型
 */
enum class TokenType {
    // 基础符号
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 字面量（无修饰字符串）
    
    // 括号和分隔符
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    SEMICOLON,      // ;
    COLON,          // :
    EQUALS,         // =
    COMMA,          // ,
    DOT,            // .
    HASH,           // #
    AMPERSAND,      // &
    
    // 操作符
    GREATER,        // >
    LESS,           // <
    GREATER_EQUAL,  // >=
    LESS_EQUAL,     // <=
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    AND,            // &&
    OR,             // ||
    QUESTION,       // ?
    ARROW,          // ->
    
    // 关键字
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 类型关键字
    AT_STYLE,       // @Style
    AT_ELEMENT,     // @Element
    AT_VAR,         // @Var
    AT_HTML,        // @Html
    AT_JAVASCRIPT,  // @JavaScript
    AT_CHTL,        // @Chtl
    AT_CONFIG,      // @Config
    
    // 操作关键字
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
    USE,            // use
    HTML5,          // html5
    
    // 注释
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* */
    GENERATOR_COMMENT, // --
    
    // 特殊
    END_OF_FILE,    // 文件结束
    UNKNOWN         // 未知
};

/**
 * @brief 词法单元
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    
    Token(TokenType t, const std::string& v, size_t l = 0, size_t c = 0)
        : type(t), value(v), line(l), column(c) {}
};

/**
 * @brief CHTL词法分析器
 */
class CHTLLexer {
public:
    CHTLLexer(const std::string& source);
    ~CHTLLexer();
    
    // 获取下一个词法单元
    Token nextToken();
    
    // 查看下一个词法单元但不消费
    Token peekToken();
    
    // 重置词法分析器
    void reset();
    
    // 获取当前位置
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 获取所有词法单元
    std::vector<Token> tokenize();

private:
    std::string m_source;
    size_t m_position;
    size_t m_line;
    size_t m_column;
    
    // 辅助方法
    char currentChar() const;
    char nextChar();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    void skipGeneratorComment();
    
    Token parseIdentifier();
    Token parseString();
    Token parseNumber();
    Token parseLiteral();
    
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    
    // 关键字识别
    TokenType identifyKeyword(const std::string& value) const;
    TokenType identifyOperator(const std::string& value) const;
};

} // namespace CHTL