#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

// CHTL Token类型枚举
enum class CHTLTokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    UNQUOTED_LITERAL, // 无引号字面量
    
    // 符号
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    SEMICOLON,      // ;
    COLON,          // :
    EQUAL,          // =
    COMMA,          // ,
    DOT,            // .
    ARROW,          // ->
    AMPERSAND,      // &
    QUESTION,       // ?
    PIPE,           // |
    
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
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    
    // 注释
    LINE_COMMENT,   // //
    BLOCK_COMMENT,  // /* */
    GENERATOR_COMMENT, // --
    
    // 关键字
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    ORIGIN,         // @Html, @Style, @JavaScript
    IMPORT,         // import
    NAMESPACE,      // namespace
    CONSTRAINT,     // constraint
    CONFIGURATION,  // [Configuration]
    USE,            // use
    
    // CHTL JS关键字
    FILELOADER,     // fileloader
    LISTEN,         // listen
    DELEGATE,       // delegate
    ANIMATE,        // animate
    VIR,            // vir
    ROUTER,         // router
    
    // 特殊符号
    AT,             // @
    HASH,           // #
    DOLLAR,         // $
    TILDE,          // ~
    CARET,          // ^
    
    // 模板相关
    TEMPLATE_STYLE, // @Style
    TEMPLATE_ELEMENT, // @Element
    TEMPLATE_VAR,   // @Var
    INHERIT,        // inherit
    DELETE,         // delete
    INSERT,         // insert
    
    // 文件结束
    EOF_TOKEN,      // 文件结束
    
    // 错误
    ERROR           // 错误token
};

// CHTL Token结构
struct CHTLToken {
    CHTLTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLToken() : type(CHTLTokenType::ERROR), line(0), column(0), position(0) {}
    CHTLToken(CHTLTokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool isKeyword() const;
    bool isOperator() const;
    bool isSymbol() const;
    bool isComment() const;
    std::string toString() const;
};

// CHTL词法分析器
class CHTLLexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<CHTLToken> tokens;
    std::vector<std::string> errors;
    
    // 关键字映射
    std::map<std::string, CHTLTokenType> keywords;
    std::set<std::string> htmlElements;
    
    // 字符处理
    char current() const;
    char peek(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // Token识别
    CHTLToken scanIdentifier();
    CHTLToken scanString();
    CHTLToken scanNumber();
    CHTLToken scanUnquotedLiteral();
    CHTLToken scanComment();
    CHTLToken scanSymbol();
    CHTLToken scanOperator();
    
    // 特殊处理
    CHTLToken scanTemplateKeyword();
    CHTLToken scanOriginKeyword();
    CHTLToken scanCHTLJSKeyword();
    
    // 错误处理
    void addError(const std::string& message);
    CHTLToken createErrorToken(const std::string& message);
    
    // 初始化
    void initializeKeywords();
    void initializeHTMLElements();
    
public:
    CHTLLexer();
    explicit CHTLLexer(const std::string& source);
    
    // 主要方法
    std::vector<CHTLToken> tokenize();
    std::vector<CHTLToken> tokenize(const std::string& source);
    
    // 状态查询
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    size_t getTokenCount() const;
    size_t getCurrentPosition() const;
    
    // 调试
    void printTokens() const;
    void printErrors() const;
    
    // 重置
    void reset();
    void setSource(const std::string& source);
};

// 辅助函数
std::string tokenTypeToString(CHTLTokenType type);
bool isHTMLElement(const std::string& name);
bool isCHTLKeyword(const std::string& name);
bool isCHTLJSKeyword(const std::string& name);

} // namespace CHTL

#endif // CHTL_LEXER_H