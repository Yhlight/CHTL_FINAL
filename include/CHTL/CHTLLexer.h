#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 词法单元类型
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    COMMENT,        // 注释
    
    // 关键字
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    USE,            // use
    
    // 样式相关
    STYLE,          // style
    SCRIPT,         // script
    TEXT,           // text
    
    // 模板类型
    STYLE_TEMPLATE, // @Style
    ELEMENT_TEMPLATE, // @Element
    VAR_TEMPLATE,   // @Var
    
    // 原始嵌入类型
    HTML_ORIGIN,    // @Html
    STYLE_ORIGIN,   // @Style
    JS_ORIGIN,      // @JavaScript
    
    // 导入类型
    HTML_IMPORT,    // @Html
    STYLE_IMPORT,   // @Style
    JS_IMPORT,      // @JavaScript
    CHTL_IMPORT,    // @Chtl
    CJMOD_IMPORT,   // @CJmod
    
    // 操作符
    COLON,          // :
    EQUAL,          // =
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    SLASH,          // /
    BACKSLASH,      // \
    
    // 括号
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    LPAREN,         // (
    RPAREN,         // )
    
    // 比较操作符
    GT,             // >
    LT,             // <
    GE,             // >=
    LE,             // <=
    EQ,             // ==
    NE,             // !=
    
    // 逻辑操作符
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // 算术操作符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    
    // 特殊符号
    QUESTION,       // ?
    AMPERSAND,      // &
    PIPE,           // |
    TILDE,          // ~
    CARET,          // ^
    DOLLAR,         // $
    AT,             // @
    HASH,           // #
    
    // 关键字
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
    
    // 特殊标记
    NEWLINE,        // 换行符
    WHITESPACE,     // 空白字符
    EOF_TOKEN,      // 文件结束
    
    // 错误
    ERROR           // 错误标记
};

/**
 * @brief 词法单元结构
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::ERROR, 
          const std::string& v = "", 
          size_t l = 0, 
          size_t c = 0, 
          size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
};

/**
 * @brief CHTL词法分析器
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;

    /**
     * @brief 词法分析
     * @param source_code 源代码
     * @return 词法单元列表
     */
    std::vector<Token> tokenize(const std::string& source_code);

    /**
     * @brief 重置词法分析器状态
     */
    void reset();

    /**
     * @brief 设置调试模式
     * @param enabled 是否启用调试模式
     */
    void setDebugMode(bool enabled);

    /**
     * @brief 获取当前行号
     * @return 当前行号
     */
    size_t getCurrentLine() const;

    /**
     * @brief 获取当前列号
     * @return 当前列号
     */
    size_t getCurrentColumn() const;

    /**
     * @brief 获取当前位置
     * @return 当前位置
     */
    size_t getCurrentPosition() const;

private:
    /**
     * @brief 跳过空白字符
     */
    void skipWhitespace();

    /**
     * @brief 跳过注释
     * @return 是否跳过了注释
     */
    bool skipComment();

    /**
     * @brief 读取标识符
     * @return 标识符词法单元
     */
    Token readIdentifier();

    /**
     * @brief 读取字符串
     * @return 字符串词法单元
     */
    Token readString();

    /**
     * @brief 读取数字
     * @return 数字词法单元
     */
    Token readNumber();

    /**
     * @brief 读取操作符
     * @return 操作符词法单元
     */
    Token readOperator();

    /**
     * @brief 读取关键字
     * @param word 单词
     * @return 关键字词法单元
     */
    Token readKeyword(const std::string& word);

    /**
     * @brief 检查是否为关键字
     * @param word 单词
     * @return 是否为关键字
     */
    bool isKeyword(const std::string& word) const;

    /**
     * @brief 检查是否为操作符
     * @param ch 字符
     * @return 是否为操作符
     */
    bool isOperator(char ch) const;

    /**
     * @brief 检查是否为字母
     * @param ch 字符
     * @return 是否为字母
     */
    bool isAlpha(char ch) const;

    /**
     * @brief 检查是否为数字
     * @param ch 字符
     * @return 是否为数字
     */
    bool isDigit(char ch) const;

    /**
     * @brief 检查是否为字母或数字
     * @param ch 字符
     * @return 是否为字母或数字
     */
    bool isAlphaNumeric(char ch) const;

    /**
     * @brief 获取下一个字符
     * @return 下一个字符
     */
    char peek() const;

    /**
     * @brief 获取下n个字符
     * @param n 字符数量
     * @return 下n个字符
     */
    std::string peek(int n) const;

    /**
     * @brief 消费当前字符
     * @return 当前字符
     */
    char consume();

    /**
     * @brief 消费下n个字符
     * @param n 字符数量
     * @return 下n个字符
     */
    std::string consume(int n);

    /**
     * @brief 检查是否到达文件末尾
     * @return 是否到达文件末尾
     */
    bool isAtEnd() const;

private:
    std::string source_code_;
    size_t current_pos_;
    size_t current_line_;
    size_t current_column_;
    bool debug_mode_;
    
    // 关键字映射
    std::unordered_map<std::string, TokenType> keywords_;
    
    // 操作符映射
    std::unordered_map<std::string, TokenType> operators_;
};

} // namespace CHTL