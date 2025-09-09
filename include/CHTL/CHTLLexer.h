#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 词法单元类型枚举
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,     // 标识符
    STRING,         // 字符串
    NUMBER,         // 数字
    LITERAL,        // 无修饰字面量
    
    // 分隔符
    LBRACE,         // {
    RBRACE,         // }
    LPAREN,         // (
    RPAREN,         // )
    LBRACKET,       // [
    RBRACKET,       // ]
    SEMICOLON,      // ;
    COMMA,          // ,
    DOT,            // .
    COLON,          // :
    EQUAL,          // =
    
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
    ORIGIN,         // [Origin]
    IMPORT,         // [Import]
    NAMESPACE,      // [Namespace]
    CONFIGURATION,  // [Configuration]
    USE,            // use
    INHERIT,        // inherit
    DELETE,         // delete
    INSERT,         // insert
    FROM,           // from
    AS,             // as
    EXCEPT,         // except
    
    // 特殊符号
    AT,             // @
    AMPERSAND,      // &
    QUESTION,       // ?
    PIPE,           // |
    ARROW,          // ->
    HASH,           // #
    
    // 逻辑运算符
    AND,            // &&
    OR,             // ||
    NOT,            // !
    GT,             // >
    LT,             // <
    GE,             // >=
    LE,             // <=
    EQ,             // ==
    NE,             // !=
    
    // 特殊标记
    EOF_TOKEN,      // 文件结束
    UNKNOWN         // 未知
};

/**
 * @brief 词法单元结构
 */
struct Token {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    Token(TokenType t = TokenType::UNKNOWN, 
          const std::string& v = "", 
          size_t l = 0, 
          size_t c = 0, 
          size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
};

/**
 * @brief CHTL词法分析器
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;

    /**
     * @brief 词法分析主函数
     * @param input 输入字符串
     * @return 词法单元列表
     */
    std::vector<Token> tokenize(const std::string& input);
    
    /**
     * @brief 从文件读取并词法分析
     * @param filePath 文件路径
     * @return 词法单元列表
     */
    std::vector<Token> tokenizeFile(const std::string& filePath);

    /**
     * @brief 获取关键字映射表
     * @return 关键字到TokenType的映射
     */
    const std::unordered_map<std::string, TokenType>& getKeywordMap() const;

    /**
     * @brief 添加自定义关键字
     * @param keyword 关键字
     * @param type 对应的TokenType
     */
    void addKeyword(const std::string& keyword, TokenType type);

    /**
     * @brief 重置词法分析器状态
     */
    void reset();

private:
    // 内部状态
    std::string m_input;
    size_t m_position;
    size_t m_line;
    size_t m_column;
    
    // 关键字映射表
    std::unordered_map<std::string, TokenType> m_keywords;
    
    // 辅助函数
    void initializeKeywords();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    void skipGeneratorComment();
    
    Token readIdentifier();
    Token readString(char delimiter);
    Token readNumber();
    Token readLiteral();
    Token readOperator();
    
    char currentChar() const;
    char peekChar() const;
    void advance();
    bool isAtEnd() const;
    
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
};

} // namespace CHTL