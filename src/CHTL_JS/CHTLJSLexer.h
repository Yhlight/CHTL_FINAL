#ifndef CHTL_JS_LEXER_H
#define CHTL_JS_LEXER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * CHTL JS Token类型枚举
 */
enum class CHTLJSTokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 字面量（无修饰字符串）
    
    // 分隔符
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    ARROW,          // ->
    
    // 操作符
    COLON,          // :
    EQUAL,          // =
    QUESTION,       // ?
    EXCLAMATION,    // !
    AMPERSAND,      // &
    PIPE,           // |
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    POWER,          // **
    LESS,           // <
    GREATER,        // >
    LESS_EQUAL,     // <=
    GREATER_EQUAL,  // >=
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    AND,            // &&
    OR,             // ||
    
    // CHTL JS关键字
    VIR,            // vir
    LISTEN,         // listen
    ANIMATE,        // animate
    FILELOADER,     // fileloader
    ROUTER,         // router
    INEVERAWAY,     // iNeverAway
    PRINTMYLOVE,    // printMylove
    UTIL,           // util
    THEN,           // then
    CHANGE,         // change
    DELEGATE,       // delegate
    TARGET,         // target
    DURATION,       // duration
    EASING,         // easing
    BEGIN,          // begin
    WHEN,           // when
    AT,             // at
    END,            // end
    LOOP,           // loop
    DIRECTION,      // direction
    DELAY,          // delay
    CALLBACK,       // callback
    
    // 增强选择器
    ENHANCED_SELECTOR,  // {{选择器}}
    
    // 响应式值
    REACTIVE_VALUE,     // $变量名$
    
    // 特殊
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    EOF_TOKEN,      // 文件结束
    UNKNOWN         // 未知token
};

/**
 * CHTL JS Token结构
 */
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken(CHTLJSTokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool isKeyword() const;
    bool isOperator() const;
    bool isDelimiter() const;
    std::string toString() const;
};

/**
 * CHTL JS词法分析器
 */
class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer();
    
    /**
     * 设置源代码
     * @param source 源代码
     */
    void setSource(const std::string& source);
    
    /**
     * 词法分析
     * @return Token列表
     */
    std::vector<CHTLJSToken> tokenize();
    
    /**
     * 获取下一个Token
     * @return Token指针
     */
    std::unique_ptr<CHTLJSToken> nextToken();
    
    /**
     * 重置词法分析器
     */
    void reset();
    
    /**
     * 获取当前位置
     * @return 当前位置
     */
    size_t getCurrentPosition() const;
    
    /**
     * 获取当前行号
     * @return 当前行号
     */
    size_t getCurrentLine() const;
    
    /**
     * 获取当前列号
     * @return 当前列号
     */
    size_t getCurrentColumn() const;

private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    
    // 关键字映射表
    std::map<std::string, CHTLJSTokenType> keywords;
    
    // 操作符映射表
    std::map<std::string, CHTLJSTokenType> operators;
    
    /**
     * 初始化关键字映射表
     */
    void initializeKeywords();
    
    /**
     * 初始化操作符映射表
     */
    void initializeOperators();
    
    /**
     * 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * 读取标识符
     * @return 标识符字符串
     */
    std::string readIdentifier();
    
    /**
     * 读取字符串
     * @return 字符串内容
     */
    std::string readString();
    
    /**
     * 读取字面量
     * @return 字面量内容
     */
    std::string readLiteral();
    
    /**
     * 读取数字
     * @return 数字字符串
     */
    std::string readNumber();
    
    /**
     * 读取操作符
     * @return 操作符字符串
     */
    std::string readOperator();
    
    /**
     * 读取增强选择器
     * @return 选择器字符串
     */
    std::string readEnhancedSelector();
    
    /**
     * 读取响应式值
     * @return 响应式值字符串
     */
    std::string readReactiveValue();
    
    /**
     * 检查是否为字母
     * @param c 字符
     * @return 是否为字母
     */
    bool isAlpha(char c) const;
    
    /**
     * 检查是否为数字
     * @param c 字符
     * @return 是否为数字
     */
    bool isDigit(char c) const;
    
    /**
     * 检查是否为字母或数字
     * @param c 字符
     * @return 是否为字母或数字
     */
    bool isAlphaNumeric(char c) const;
    
    /**
     * 检查是否为空白字符
     * @param c 字符
     * @return 是否为空白字符
     */
    bool isWhitespace(char c) const;
    
    /**
     * 检查是否为操作符字符
     * @param c 字符
     * @return 是否为操作符字符
     */
    bool isOperatorChar(char c) const;
    
    /**
     * 获取当前字符
     * @return 当前字符
     */
    char currentChar() const;
    
    /**
     * 获取下一个字符
     * @return 下一个字符
     */
    char nextChar() const;
    
    /**
     * 前进一个字符
     */
    void advance();
    
    /**
     * 回退一个字符
     */
    void retreat();
    
    /**
     * 检查是否到达文件末尾
     * @return 是否到达文件末尾
     */
    bool isEOF() const;
    
    /**
     * 创建Token
     * @param type Token类型
     * @param value Token值
     * @return Token指针
     */
    std::unique_ptr<CHTLJSToken> createToken(CHTLJSTokenType type, const std::string& value);
};

} // namespace CHTL

#endif // CHTL_JS_LEXER_H