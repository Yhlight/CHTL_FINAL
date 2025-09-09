#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {

enum class CHTLJSTokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    
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
    ARROW,          // ->
    
    // CHTL JS关键字
    FILELOADER,     // fileloader
    LISTEN,         // listen
    DELEGATE,       // delegate
    ANIMATE,        // animate
    VIR,            // vir
    ROUTER,         // router
    UTIL,           // util
    INEVERAWAY,     // iNeverAway
    
    // 位置关键字
    AFTER,          // after
    BEFORE,         // before
    REPLACE,        // replace
    AT_TOP,         // at top
    AT_BOTTOM,      // at bottom
    
    // 选择器
    SELECTOR,       // {{selector}}
    
    // 位置标记
    NEWLINE,        // 换行
    WHITESPACE,     // 空白
    EOF_TOKEN,      // 文件结束
    
    // 错误
    ERROR           // 错误标记
};

class CHTLJSToken {
public:
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken(CHTLJSTokenType type, std::string value, size_t line, size_t column, size_t position);
    std::string toString() const;
    
    // 静态方法
    static std::string tokenTypeToString(CHTLJSTokenType type);
    static bool isKeyword(const std::string& value);
    static CHTLJSTokenType getKeywordType(const std::string& value);
    static bool isOperator(const std::string& value);
    static CHTLJSTokenType getOperatorType(const std::string& value);
    static bool isDelimiter(const std::string& value);
    static CHTLJSTokenType getDelimiterType(const std::string& value);
    
private:
    static const std::map<std::string, CHTLJSTokenType> KEYWORDS;
    static const std::map<std::string, CHTLJSTokenType> OPERATORS;
    static const std::map<std::string, CHTLJSTokenType> DELIMITERS;
};

} // namespace CHTL