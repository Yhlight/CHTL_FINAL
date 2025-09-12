#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <map>

namespace CHTL {

/**
 * 词法单元类型
 */
enum class TokenType {
    // 关键字
    KEYWORD_HTML,           // html
    KEYWORD_HEAD,           // head
    KEYWORD_BODY,           // body
    KEYWORD_DIV,            // div
    KEYWORD_SPAN,           // span
    KEYWORD_P,              // p
    KEYWORD_H1,             // h1
    KEYWORD_H2,             // h2
    KEYWORD_H3,             // h3
    KEYWORD_H4,             // h4
    KEYWORD_H5,             // h5
    KEYWORD_H6,             // h6
    KEYWORD_TITLE,          // title
    KEYWORD_SCRIPT,         // script
    KEYWORD_STYLE,          // style
    KEYWORD_TEXT,           // text
    KEYWORD_TEMPLATE,       // [Template]
    KEYWORD_CUSTOM,         // [Custom]
    KEYWORD_ORIGIN,         // [Origin]
    KEYWORD_IMPORT,         // [Import]
    KEYWORD_NAMESPACE,      // [Namespace]
    KEYWORD_CONFIGURATION,  // [Configuration]
    
    // 操作符
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    SEMICOLON,              // ;
    COLON,                  // :
    EQUALS,                 // =
    COMMA,                  // ,
    DOT,                    // .
    ARROW,                  // ->
    QUESTION,               // ?
    EXCLAMATION,            // !
    AMPERSAND,              // &
    PIPE,                   // |
    TILDE,                  // ~
    CARET,                  // ^
    DOLLAR,                 // $
    AT,                     // @
    HASH,                   // #
    
    // 算术操作符
    PLUS,                   // +
    MINUS,                  // -
    MULTIPLY,               // *
    DIVIDE,                 // /
    MODULO,                 // %
    POWER,                  // **
    
    // 比较操作符
    LESS_THAN,              // <
    GREATER_THAN,           // >
    LESS_EQUAL,             // <=
    GREATER_EQUAL,          // >=
    EQUAL_EQUAL,            // ==
    NOT_EQUAL,              // !=
    
    // 逻辑操作符
    AND,                    // &&
    OR,                     // ||
    NOT,                    // !
    
    // 字面量
    IDENTIFIER,             // 标识符
    STRING_LITERAL,         // 字符串字面量
    NUMBER_LITERAL,         // 数字字面量
    UNQUOTED_LITERAL,       // 无修饰字面量
    
    // 注释
    SINGLE_LINE_COMMENT,    // //
    MULTI_LINE_COMMENT,     // /**/
    GENERATOR_COMMENT,      // --
    
    // 特殊
    NEWLINE,                // 换行
    WHITESPACE,             // 空白字符
    EOF_TOKEN,              // 文件结束
    UNKNOWN                 // 未知
};

/**
 * 词法单元
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
    
    bool isKeyword() const {
        return type >= TokenType::KEYWORD_HTML && 
               type <= TokenType::KEYWORD_CONFIGURATION;
    }
    
    bool isOperator() const {
        return type >= TokenType::LEFT_BRACE && 
               type <= TokenType::AT;
    }
    
    bool isLiteral() const {
        return type >= TokenType::IDENTIFIER && 
               type <= TokenType::UNQUOTED_LITERAL;
    }
    
    bool isComment() const {
        return type >= TokenType::SINGLE_LINE_COMMENT && 
               type <= TokenType::GENERATOR_COMMENT;
    }
    
    std::string toString() const;
};

/**
 * CHTL词法分析器
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer();
    
    // 设置输入
    void setInput(const std::string& input);
    void setInput(std::istream& input);
    
    // 词法分析
    std::vector<Token> tokenize();
    Token nextToken();
    Token peekToken();
    void reset();
    
    // 配置
    void setDebugMode(bool enabled);
    void setSkipWhitespace(bool skip);
    void setSkipComments(bool skip);
    
    // 状态查询
    bool hasMoreTokens() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    size_t getCurrentPosition() const;

private:
    // 输入管理
    std::string input_;
    size_t position_;
    size_t line_;
    size_t column_;
    
    // 配置
    bool debug_mode_;
    bool skip_whitespace_;
    bool skip_comments_;
    
    // 关键字映射
    std::unordered_map<std::string, TokenType> keywords_;
    
    // 初始化
    void initializeKeywords();
    
    // 字符处理
    char currentChar() const;
    char peekChar() const;
    char nextChar();
    void skipWhitespace();
    void skipLine();
    
    // 词法分析辅助函数
    Token scanIdentifier();
    Token scanString();
    Token scanNumber();
    Token scanOperator();
    Token scanComment();
    Token scanUnquotedLiteral();
    
    // 字符分类
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isOperatorChar(char c) const;
    
    // 错误处理
    void reportError(const std::string& message);
    Token createErrorToken(const std::string& message);
};

} // namespace CHTL