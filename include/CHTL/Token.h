#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief Token类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    COMMENT,        // 注释
    
    // 符号
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    SEMICOLON,      // ;
    COLON,          // :
    EQUALS,         // =
    COMMA,          // ,
    DOT,            // .
    HASH,           // #
    AT,             // @
    DOLLAR,         // $
    AMPERSAND,      // &
    PIPE,           // |
    QUESTION,       // ?
    EXCLAMATION,    // !
    TILDE,          // ~
    CARET,          // ^
    LESS_THAN,      // <
    GREATER_THAN,   // >
    PLUS,           // +
    MINUS,          // -
    ASTERISK,       // *
    SLASH,          // /
    PERCENT,        // %
    BACKSLASH,      // \
    
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
    MODULE,         // [Module]
    INFO,           // [Info]
    EXPORT,         // [Export]
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
    
    // CHTL JS关键字
    VIR,            // vir
    LISTEN,         // listen
    ANIMATE,        // animate
    ROUTER,         // router
    DELEGATE,       // delegate
    FILELOADER,     // fileloader
    
    // 特殊Token
    END_OF_FILE,    // 文件结束
    UNKNOWN         // 未知Token
};

/**
 * @brief Token类
 * 
 * 表示词法分析器识别出的一个Token
 */
class Token {
public:
    /**
     * @brief 构造函数
     * @param type Token类型
     * @param value Token值
     * @param line 行号
     * @param column 列号
     */
    Token(TokenType type, const std::string& value, size_t line = 0, size_t column = 0);
    
    /**
     * @brief 获取Token类型
     * @return Token类型
     */
    TokenType getType() const;
    
    /**
     * @brief 获取Token值
     * @return Token值
     */
    const std::string& getValue() const;
    
    /**
     * @brief 获取行号
     * @return 行号
     */
    size_t getLine() const;
    
    /**
     * @brief 获取列号
     * @return 列号
     */
    size_t getColumn() const;
    
    /**
     * @brief 设置位置信息
     * @param line 行号
     * @param column 列号
     */
    void setPosition(size_t line, size_t column);
    
    /**
     * @brief 检查Token类型
     * @param type 要检查的类型
     * @return 是否匹配
     */
    bool isType(TokenType type) const;
    
    /**
     * @brief 检查是否为关键字
     * @return 是否为关键字
     */
    bool isKeyword() const;
    
    /**
     * @brief 检查是否为符号
     * @return 是否为符号
     */
    bool isSymbol() const;
    
    /**
     * @brief 检查是否为运算符
     * @return 是否为运算符
     */
    bool isOperator() const;
    
    /**
     * @brief 转换为字符串
     * @return 字符串表示
     */
    std::string toString() const;
    
    /**
     * @brief 获取Token类型名称
     * @return 类型名称
     */
    std::string getTypeName() const;

private:
    TokenType m_type;
    std::string m_value;
    size_t m_line;
    size_t m_column;
};

/**
 * @brief Token列表类
 * 
 * 管理Token序列，提供便捷的访问方法
 */
class TokenList {
public:
    /**
     * @brief 构造函数
     */
    TokenList();
    
    /**
     * @brief 添加Token
     * @param token Token对象
     */
    void addToken(const Token& token);
    
    /**
     * @brief 添加Token
     * @param type Token类型
     * @param value Token值
     * @param line 行号
     * @param column 列号
     */
    void addToken(TokenType type, const std::string& value, size_t line = 0, size_t column = 0);
    
    /**
     * @brief 获取Token数量
     * @return Token数量
     */
    size_t size() const;
    
    /**
     * @brief 检查是否为空
     * @return 是否为空
     */
    bool empty() const;
    
    /**
     * @brief 获取指定位置的Token
     * @param index 索引
     * @return Token引用
     */
    const Token& operator[](size_t index) const;
    
    /**
     * @brief 获取指定位置的Token
     * @param index 索引
     * @return Token引用
     */
    Token& operator[](size_t index);
    
    /**
     * @brief 获取当前Token（不移动位置）
     * @return 当前Token
     */
    const Token& current() const;
    
    /**
     * @brief 获取下一个Token（不移动位置）
     * @return 下一个Token
     */
    const Token& peek() const;
    
    /**
     * @brief 移动到下一个Token
     * @return 是否成功移动
     */
    bool next();
    
    /**
     * @brief 检查是否到达末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * @brief 重置位置到开始
     */
    void reset();
    
    /**
     * @brief 清空Token列表
     */
    void clear();
    
    /**
     * @brief 查找指定类型的Token
     * @param type Token类型
     * @param startIndex 开始索引
     * @return 找到的索引，未找到返回-1
     */
    int find(TokenType type, size_t startIndex = 0) const;
    
    /**
     * @brief 查找指定值的Token
     * @param value Token值
     * @param startIndex 开始索引
     * @return 找到的索引，未找到返回-1
     */
    int find(const std::string& value, size_t startIndex = 0) const;

private:
    std::vector<Token> m_tokens;
    size_t m_currentIndex;
};

} // namespace CHTL