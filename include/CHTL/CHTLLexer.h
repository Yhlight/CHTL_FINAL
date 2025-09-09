#pragma once

#include "CHTLContext.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief Token 类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 无修饰字面量
    
    // 符号
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
    
    // 注释
    COMMENT,        // 通用注释
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* */
    GENERATOR_COMMENT, // --
    
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
    EXCEPT,         // except
    INHERIT,        // inherit
    DELETE,         // delete
    INSERT,         // insert
    USE,            // use
    FROM,           // from
    AS,             // as
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM,      // at bottom
    
    // 特殊
    EOF_TOKEN,      // 文件结束
    UNKNOWN         // 未知
};

/**
 * @brief Token 结构体
 */
struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t = TokenType::UNKNOWN, const std::string& v = "", int l = 0, int c = 0)
        : type(t), value(v), line(l), column(c) {}
};

/**
 * @brief CHTL 词法分析器
 * 
 * 负责将 CHTL 源代码转换为 Token 序列
 */
class CHTLLexer {
public:
    explicit CHTLLexer(std::shared_ptr<CHTLContext> context);
    ~CHTLLexer() = default;

    // 词法分析
    std::vector<Token> tokenize(const std::string& source);
    
    // 重置状态
    void reset();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_source;
    size_t m_position;
    int m_line;
    int m_column;
    
    // 辅助方法
    char current() const;
    char peek(size_t offset = 1) const;
    void advance(size_t count = 1);
    bool isAtEnd() const;
    
    // Token 识别
    Token scanToken();
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanLiteral();
    Token scanComment();
    Token scanLineComment();
    Token scanBlockComment();
    Token scanGeneratorComment();
    
    // 字符分类
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    
    // 关键字识别
    TokenType getKeywordType(const std::string& identifier) const;
    bool isKeyword(const std::string& identifier) const;
    
    // 跳过空白字符
    void skipWhitespace();
    void skipNewline();
};

} // namespace CHTL