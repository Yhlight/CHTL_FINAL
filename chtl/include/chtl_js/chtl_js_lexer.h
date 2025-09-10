#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace chtl {
namespace chtl_js {

// CHTL JS Token类型
enum class CHTLJSTokenType {
    // 基本类型
    IDENTIFIER,
    STRING_LITERAL,
    NUMBER_LITERAL,
    BOOLEAN_LITERAL,
    
    // 运算符
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    ASSIGN,         // =
    EQUAL,          // ==
    NOT_EQUAL,      // !=
    LESS,           // <
    LESS_EQUAL,     // <=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    AND,            // &&
    OR,             // ||
    NOT,            // !
    
    // 分隔符
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    ARROW,          // ->
    COLON,          // :
    QUESTION,       // ?
    
    // CHTL JS特有语法
    FILELOADER,     // fileloader
    LISTEN,         // listen
    ANIMATE,        // animate
    VIR,            // vir
    ROUTER,         // router
    INEVERAWAY,     // iNeverAway
    UTIL,           // util
    CHANGE,         // change
    THEN,           // then
    
    // 增强选择器
    SELECTOR_START, // {{
    SELECTOR_END,   // }}
    SELF_REF,       // {{&}}
    
    // 控制流
    IF,
    ELSE,
    FOR,
    WHILE,
    SWITCH,
    CASE,
    DEFAULT,
    BREAK,
    CONTINUE,
    RETURN,
    
    // 函数和类
    FUNCTION,
    CLASS,
    NEW,
    THIS,
    SUPER,
    
    // 异常处理
    TRY,
    CATCH,
    FINALLY,
    THROW,
    
    // 其他
    VAR,
    LET,
    CONST,
    ASYNC,
    AWAIT,
    YIELD,
    
    // 特殊
    EOF_TOKEN,
    UNKNOWN
};

// CHTL JS Token
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken(CHTLJSTokenType t, const std::string& v, size_t l = 1, size_t c = 1, size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
};

// CHTL JS词法分析器
class CHTLJSLexer {
public:
    explicit CHTLJSLexer(const std::string& source);
    
    // 主要接口
    std::vector<CHTLJSToken> tokenize();
    CHTLJSToken next_token();
    CHTLJSToken peek_token(size_t offset = 1);
    
    // 状态查询
    bool is_at_end() const;
    size_t get_position() const;
    size_t get_line() const;
    size_t get_column() const;
    
    // 重置
    void reset();
    
private:
    std::string source_;
    size_t position_;
    size_t line_;
    size_t column_;
    size_t start_;
    
    // 关键字映射
    std::unordered_map<std::string, CHTLJSTokenType> keywords_;
    
    // 初始化关键字
    void initialize_keywords();
    
    // 字符处理
    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void skip_whitespace();
    void skip_comment();
    
    // 字符判断
    bool is_alpha(char c) const;
    bool is_digit(char c) const;
    bool is_alnum(char c) const;
    bool is_whitespace(char c) const;
    
    // Token识别
    CHTLJSToken scan_identifier();
    CHTLJSToken scan_string();
    CHTLJSToken scan_number();
    CHTLJSToken scan_operator();
    CHTLJSToken scan_selector();
    
    // 辅助方法
    bool match_string(const std::string& str);
    void skip_until_newline();
    std::string get_lexeme() const;
    CHTLJSToken make_token(CHTLJSTokenType type);
    CHTLJSToken make_token(CHTLJSTokenType type, const std::string& value);
};

} // namespace chtl_js
} // namespace chtl