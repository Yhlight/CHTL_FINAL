#pragma once

#include <string>
#include <vector>
#include <ostream>

namespace CHTL {

/**
 * @brief Token 类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,         ///< 标识符
    STRING,             ///< 字符串
    NUMBER,             ///< 数字
    SYMBOL,             ///< 符号
    
    // 注释类型
    LINE_COMMENT,       ///< 行注释 //
    BLOCK_COMMENT,      ///< 块注释 /* */
    GENERATOR_COMMENT,  ///< 生成器注释 --
    
    // 关键字
    TEMPLATE,           ///< [Template]
    CUSTOM,             ///< [Custom]
    ORIGIN,             ///< [Origin]
    IMPORT,             ///< [Import]
    CONFIGURATION,      ///< [Configuration]
    NAMESPACE,          ///< [Namespace]
    
    // 类型关键字
    STYLE,              ///< @Style
    ELEMENT,            ///< @Element
    VAR,                ///< @Var
    HTML,               ///< @Html
    JAVASCRIPT,         ///< @JavaScript
    CHTL,               ///< @Chtl
    CJMOD,              ///< @CJmod
    
    // 操作符
    COLON,              ///< :
    EQUALS,             ///< =
    SEMICOLON,          ///< ;
    COMMA,              ///< ,
    LBRACE,             ///< {
    RBRACE,             ///< }
    LBRACKET,           ///< [
    RBRACKET,           ///< ]
    LPAREN,             ///< (
    RPAREN,             ///< )
    
    // 特殊符号
    AT_SYMBOL,          ///< @
    HASH,               ///< #
    DOT,                ///< .
    AMPERSAND,          ///< &
    QUESTION,           ///< ?
    EXCLAMATION,        ///< !
    UNDERSCORE,         ///< _
    
    // 逻辑操作符
    AND,                ///< &&
    OR,                 ///< ||
    NOT,                ///< !
    
    // 比较操作符
    EQUAL,              ///< ==
    NOT_EQUAL,          ///< !=
    LESS,               ///< <
    LESS_EQUAL,         ///< <=
    GREATER,            ///< >
    GREATER_EQUAL,      ///< >=
    
    // 算术操作符
    PLUS,               ///< +
    MINUS,              ///< -
    MULTIPLY,           ///< *
    DIVIDE,             ///< /
    MODULO,             ///< %
    
    // 其他关键字
    TEXT,               ///< text
    SCRIPT,             ///< script
    INHERIT,            ///< inherit
    DELETE,             ///< delete
    INSERT,             ///< insert
    AFTER,              ///< after
    BEFORE,             ///< before
    REPLACE,            ///< replace
    AT_TOP,             ///< at top
    AT_BOTTOM,          ///< at bottom
    FROM,               ///< from
    AS,                 ///< as
    EXCEPT,             ///< except
    USE,                ///< use
    HTML5,              ///< html5
    
    // 文件结束
    END_OF_FILE,        ///< 文件结束
    
    // 未知
    UNKNOWN             ///< 未知类型
};

/**
 * @brief Token 结构体
 * 
 * 表示词法分析器识别出的一个词法单元
 */
struct Token {
    TokenType type;         ///< Token 类型
    std::string value;      ///< Token 值
    size_t line;            ///< 行号
    size_t column;          ///< 列号
    size_t position;        ///< 位置
    
    /**
     * @brief 默认构造函数
     */
    Token() : type(TokenType::UNKNOWN), line(0), column(0), position(0) {}
    
    /**
     * @brief 构造函数
     * @param t Token 类型
     * @param v Token 值
     * @param l 行号
     * @param c 列号
     * @param p 位置
     */
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    /**
     * @brief 检查是否为特定类型
     * @param t 要检查的类型
     * @return 是否为指定类型
     */
    bool is(TokenType t) const { return type == t; }
    
    /**
     * @brief 检查是否为特定值
     * @param v 要检查的值
     * @return 是否为指定值
     */
    bool is(const std::string& v) const { return value == v; }
    
    /**
     * @brief 检查是否为特定类型和值
     * @param t 要检查的类型
     * @param v 要检查的值
     * @return 是否为指定类型和值
     */
    bool is(TokenType t, const std::string& v) const { return type == t && value == v; }
    
    /**
     * @brief 检查是否为关键字
     * @return 是否为关键字
     */
    bool isKeyword() const;
    
    /**
     * @brief 检查是否为操作符
     * @return 是否为操作符
     */
    bool isOperator() const;
    
    /**
     * @brief 检查是否为分隔符
     * @return 是否为分隔符
     */
    bool isDelimiter() const;
    
    /**
     * @brief 获取 Token 类型名称
     * @return Token 类型名称
     */
    std::string getTypeName() const;
    
    /**
     * @brief 转换为字符串
     * @return 字符串表示
     */
    std::string toString() const;
};

/**
 * @brief Token 列表类型
 */
using TokenList = std::vector<Token>;

/**
 * @brief 输出操作符重载
 * @param os 输出流
 * @param token Token 对象
 * @return 输出流
 */
std::ostream& operator<<(std::ostream& os, const Token& token);

/**
 * @brief 输出操作符重载
 * @param os 输出流
 * @param tokenList Token 列表
 * @return 输出流
 */
std::ostream& operator<<(std::ostream& os, const TokenList& tokenList);

} // namespace CHTL