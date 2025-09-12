#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * Token类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 字面量（无引号字符串）
    
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
    
    // 操作符
    COLON,          // :
    EQUAL,          // =
    ARROW,          // ->
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
    
    // 关键字
    TEXT,           // text
    STYLE,          // style
    SCRIPT,         // script
    TEMPLATE,       // [Template]
    CUSTOM,         // [Custom]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    ORIGIN,         // [Origin]
    CONFIGURATION,  // [Configuration]
    INFO,           // [Info]
    EXPORT,         // [Export]
    
    // 模板关键字
    TEMPLATE_STYLE,     // @Style
    TEMPLATE_ELEMENT,   // @Element
    TEMPLATE_VAR,       // @Var
    CUSTOM_STYLE,       // @Style (Custom)
    CUSTOM_ELEMENT,     // @Element (Custom)
    CUSTOM_VAR,         // @Var (Custom)
    
    // 原始嵌入类型
    ORIGIN_HTML,        // @Html
    ORIGIN_STYLE,       // @Style (Origin)
    ORIGIN_JAVASCRIPT,  // @JavaScript
    
    // 导入类型
    IMPORT_HTML,        // @Html (Import)
    IMPORT_STYLE,       // @Style (Import)
    IMPORT_JAVASCRIPT,  // @JavaScript (Import)
    IMPORT_CHTL,        // @Chtl
    IMPORT_CJMOD,       // @CJmod
    
    // 其他关键字
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
    
    // 注释
    COMMENT_LINE,   // //
    COMMENT_BLOCK,  // /* */
    COMMENT_GEN,    // --
    
    // 特殊
    NEWLINE,        // 换行
    WHITESPACE,     // 空白字符
    EOF_TOKEN,      // 文件结束
    
    // 未知
    UNKNOWN         // 未知token
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
    
    Token(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool isKeyword() const;
    bool isOperator() const;
    bool isDelimiter() const;
    std::string toString() const;
};

/**
 * 词法分析器
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer();
    
    /**
     * 设置源代码
     * @param source 源代码
     */
    void setSource(const std::string& source);
    
    /**
     * 词法分析
     * @return Token列表
     */
    std::vector<Token> tokenize();
    
    /**
     * 获取下一个Token
     * @return Token指针
     */
    std::unique_ptr<Token> nextToken();
    
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
    std::map<std::string, TokenType> keywords;
    
    // 操作符映射表
    std::map<std::string, TokenType> operators;
    
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
     * 跳过注释
     */
    void skipComment();
    
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
     * 读取方括号关键字
     * @return 关键字字符串
     */
    std::string readBracketKeyword();
    
    /**
     * 读取@关键字
     * @return 关键字字符串
     */
    std::string readAtKeyword();
    
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
    std::unique_ptr<Token> createToken(TokenType type, const std::string& value);
};

} // namespace CHTL

#endif // CHTL_LEXER_H