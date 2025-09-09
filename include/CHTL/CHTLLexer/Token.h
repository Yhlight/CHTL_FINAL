#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 令牌类型枚举
 * 
 * 定义了CHTL语言中所有可能的令牌类型
 */
enum class TokenType {
    // 基础令牌
    IDENTIFIER,         // 标识符
    STRING,             // 字符串
    NUMBER,             // 数字
    LITERAL,            // 无修饰字面量
    
    // 分隔符
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    COLON,              // :
    EQUALS,             // =
    
    // 注释
    SINGLE_COMMENT,     // //
    MULTI_COMMENT,      // /* */
    GENERATOR_COMMENT,  // --
    
    // 关键字
    TEXT,               // text
    STYLE,              // style
    SCRIPT,             // script
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    IMPORT,             // [Import]
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    INFO,               // [Info]
    EXPORT,             // [Export]
    
    // 模板关键字
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CONFIG,          // @Config
    
    // 操作符
    DELETE,             // delete
    INSERT,             // insert
    INHERIT,            // inherit
    EXCEPT,             // except
    USE,                // use
    FROM,               // from
    AS,                 // as
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    AT_TOP,             // at top
    AT_BOTTOM,          // at bottom
    
    // 条件表达式
    QUESTION,           // ?
    LOGICAL_AND,        // &&
    LOGICAL_OR,         // ||
    GREATER,            // >
    LESS,               // <
    GREATER_EQUAL,      // >=
    LESS_EQUAL,         // <=
    EQUAL,              // ==
    NOT_EQUAL,          // !=
    
    // 特殊令牌
    AMPERSAND,          // &
    HASH,               // #
    DOLLAR,             // $
    PERCENT,            // %
    PLUS,               // +
    MINUS,              // -
    MULTIPLY,           // *
    DIVIDE,             // /
    MODULO,             // %
    
    // 文件结束
    END_OF_FILE,        // EOF
    
    // 错误
    ERROR,              // 错误令牌
    
    // CHTL JS 特定令牌
    VIR,                // vir
    LISTEN,             // listen
    ANIMATE,            // animate
    ROUTER,             // router
    FILELOADER,         // fileloader
    DELEGATE,           // delegate
    INEVERAWAY,         // iNeverAway
    UTIL,               // util
    CHANGE,             // change
    THEN,               // then
    PRINTMYLOVE,        // printMylove
};

/**
 * @brief 令牌类
 * 
 * 表示词法分析过程中识别出的一个令牌
 */
class Token {
public:
    Token(TokenType type, const std::string& value, size_t line, size_t column);
    ~Token() = default;

    /**
     * @brief 获取令牌类型
     * @return 令牌类型
     */
    TokenType getType() const { return m_type; }

    /**
     * @brief 获取令牌值
     * @return 令牌值
     */
    const std::string& getValue() const { return m_value; }

    /**
     * @brief 获取行号
     * @return 行号
     */
    size_t getLine() const { return m_line; }

    /**
     * @brief 获取列号
     * @return 列号
     */
    size_t getColumn() const { return m_column; }

    /**
     * @brief 设置令牌值
     * @param value 新的令牌值
     */
    void setValue(const std::string& value) { m_value = value; }

    /**
     * @brief 检查是否为特定类型的令牌
     * @param type 要检查的令牌类型
     * @return 是否匹配
     */
    bool isType(TokenType type) const { return m_type == type; }

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
     * @brief 获取令牌的字符串表示
     * @return 字符串表示
     */
    std::string toString() const;

    /**
     * @brief 获取令牌类型的字符串表示
     * @return 类型字符串
     */
    static std::string typeToString(TokenType type);

private:
    TokenType m_type;
    std::string m_value;
    size_t m_line;
    size_t m_column;
};

/**
 * @brief 令牌流类
 * 
 * 管理令牌序列，提供令牌访问和操作功能
 */
class TokenStream {
public:
    TokenStream();
    ~TokenStream() = default;

    /**
     * @brief 添加令牌
     * @param token 要添加的令牌
     */
    void addToken(std::unique_ptr<Token> token);

    /**
     * @brief 获取当前令牌
     * @return 当前令牌指针
     */
    Token* current() const;

    /**
     * @brief 获取下一个令牌
     * @return 下一个令牌指针
     */
    Token* next();

    /**
     * @brief 获取指定位置的令牌
     * @param index 位置索引
     * @return 令牌指针
     */
    Token* at(size_t index) const;

    /**
     * @brief 检查是否还有更多令牌
     * @return 是否还有更多令牌
     */
    bool hasMore() const;

    /**
     * @brief 获取令牌总数
     * @return 令牌总数
     */
    size_t size() const;

    /**
     * @brief 获取当前位置
     * @return 当前位置索引
     */
    size_t getPosition() const;

    /**
     * @brief 设置位置
     * @param position 新的位置
     */
    void setPosition(size_t position);

    /**
     * @brief 重置到开始位置
     */
    void reset();

    /**
     * @brief 清空所有令牌
     */
    void clear();

    /**
     * @brief 检查是否为空
     * @return 是否为空
     */
    bool empty() const;

private:
    std::vector<std::unique_ptr<Token>> m_tokens;
    size_t m_position;
};

} // namespace CHTL