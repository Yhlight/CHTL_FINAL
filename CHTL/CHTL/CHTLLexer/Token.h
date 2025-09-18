#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>

namespace CHTL {

/**
 * Token类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,         // 标识符
    STRING,             // 字符串
    NUMBER,             // 数字
    LITERAL,            // 字面量（无修饰字符串）
    
    // 分隔符
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    SEMICOLON,          // ;
    COLON,              // :
    EQUALS,             // =
    COMMA,              // ,
    DOT,                // .
    HASH,               // #
    
    // 注释
    LINE_COMMENT,       // //
    BLOCK_COMMENT,      // /* */
    GENERATOR_COMMENT,  // # 注释
    
    // 关键字
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    IMPORT,             // [Import]
    NAMESPACE,          // [Namespace]
    CONFIGURATION,      // [Configuration]
    INFO,               // [Info]
    EXPORT,             // [Export]
    
    // 样式相关
    STYLE,              // style
    SCRIPT,             // script
    TEXT,               // text
    
    // 模板相关
    AT_STYLE,           // @Style
    AT_ELEMENT,         // @Element
    AT_VAR,             // @Var
    AT_HTML,            // @Html
    AT_JAVASCRIPT,      // @JavaScript
    AT_CHTL,            // @Chtl
    AT_CONFIG,          // @Config
    
    // 操作符
    INHERIT,            // inherit
    DELETE,             // delete
    INSERT,             // insert
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    AT_TOP,             // at top
    AT_BOTTOM,          // at bottom
    FROM,               // from
    AS,                 // as
    EXCEPT,             // except
    USE,                // use
    
    // 特殊
    END_OF_FILE,        // 文件结束
    UNKNOWN             // 未知
};

/**
 * Token结构
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
    
    bool operator==(const Token& other) const {
        return type == other.type && value == other.value;
    }
    
    bool operator!=(const Token& other) const {
        return !(*this == other);
    }
};

/**
 * 关键字映射表
 */
class KeywordMap {
private:
    static std::unordered_map<std::string, TokenType> s_keywords;
    static bool s_initialized;
    
public:
    static void initialize();
    static TokenType getKeywordType(const std::string& keyword);
    static bool isKeyword(const std::string& keyword);
    static std::string getKeywordName(TokenType type);
};

/**
 * Token工具类
 */
class TokenUtils {
public:
    static bool isIdentifier(const std::string& str);
    static bool isString(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isLiteral(const std::string& str);
    static bool isWhitespace(char c);
    static bool isNewline(char c);
    static bool isOperator(char c);
    static bool isDelimiter(char c);
    
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
};

} // namespace CHTL

#endif // TOKEN_H