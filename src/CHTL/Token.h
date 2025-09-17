#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 字面量（无修饰字符串）
    
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
    AMPERSAND,      // &
    ARROW,          // ->
    QUESTION,       // ?
    EXCLAMATION,    // !
    UNDERSCORE,     // _
    DOLLAR,         // $
    
    // 运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    AND,            // &&
    OR,             // ||
    NOT,            // !
    LESS,           // <
    GREATER,        // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    
    // 关键字
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 类型关键字
    STYLE,          // @Style
    ELEMENT,        // @Element
    VAR,            // @Var
    HTML,           // @Html
    JAVASCRIPT,     // @JavaScript
    CHTL,           // @Chtl
    CJMOD,          // @CJmod
    CONFIG,         // @Config
    
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
    TEXT,           // text
    SCRIPT,         // script
    
    // 特殊关键字
    VIR,            // Vir
    LISTEN,         // Listen
    ANIMATE,        // Animate
    ROUTER,         // Router
    SCRIPT_LOADER,  // ScriptLoader
    UTIL,           // util
    CHANGE,         // change
    THEN,           // then
    
    // 注释
    COMMENT,        // //
    MULTILINE_COMMENT, // /**/
    GENERATOR_COMMENT, // #
    
    // 特殊标记
    PLACEHOLDER,    // 占位符
    END_OF_FILE,    // 文件结束
    UNKNOWN         // 未知
};

class Token {
public:
    Token(TokenType type, const std::string& value, size_t line = 0, size_t column = 0);
    
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    bool isType(TokenType type) const { return type_ == type; }
    bool isKeyword() const;
    bool isOperator() const;
    bool isSymbol() const;
    
    std::string toString() const;
    
private:
    TokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

using TokenPtr = std::shared_ptr<Token>;
using TokenList = std::vector<TokenPtr>;

// 全局映射表
class GlobalMap {
public:
    static void initialize();
    static TokenType getKeywordType(const std::string& keyword);
    static TokenType getOperatorType(const std::string& op);
    static bool isKeyword(const std::string& word);
    static bool isOperator(const std::string& op);
    static std::string getTokenTypeName(TokenType type);
    
private:
    static std::unordered_map<std::string, TokenType> keywordMap_;
    static std::unordered_map<std::string, TokenType> operatorMap_;
};

} // namespace CHTL

#endif // CHTL_TOKEN_H