#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL_JS {

/**
 * @brief CHTL JS词法单元类型枚举
 */
enum class CHTLJSTokenType {
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
    
    // CHTL JS关键字
    VIR,            // vir
    LISTEN,         // listen
    DELEGATE,       // delegate
    ANIMATE,        // animate
    ROUTER,         // router
    FILELOADER,     // fileloader
    INEVERAWAY,     // iNeverAway
    UTIL,           // util
    CHANGE,         // change
    THEN,           // then
    
    // 特殊符号
    ARROW,          // ->
    AMPERSAND,      // &
    QUESTION,       // ?
    PIPE,           // ||
    HASH,           // #
    DOLLAR,         // $
    UNDERSCORE,     // _
    
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
 * @brief CHTL JS词法单元结构
 */
struct CHTLJSToken {
    CHTLJSTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLJSToken(CHTLJSTokenType t = CHTLJSTokenType::UNKNOWN, 
                const std::string& v = "", 
                size_t l = 0, 
                size_t c = 0, 
                size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
};

/**
 * @brief CHTL JS词法分析器
 */
class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer() = default;

    /**
     * @brief 词法分析主函数
     * @param input 输入字符串
     * @return 词法单元列表
     */
    std::vector<CHTLJSToken> tokenize(const std::string& input);
    
    /**
     * @brief 从文件读取并词法分析
     * @param filePath 文件路径
     * @return 词法单元列表
     */
    std::vector<CHTLJSToken> tokenizeFile(const std::string& filePath);

    /**
     * @brief 获取关键字映射表
     * @return 关键字到TokenType的映射
     */
    const std::unordered_map<std::string, CHTLJSTokenType>& getKeywordMap() const;

    /**
     * @brief 添加自定义关键字
     * @param keyword 关键字
     * @param type 对应的TokenType
     */
    void addKeyword(const std::string& keyword, CHTLJSTokenType type);

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
    std::unordered_map<std::string, CHTLJSTokenType> m_keywords;
    
    // 辅助函数
    void initializeKeywords();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    
    CHTLJSToken readIdentifier();
    CHTLJSToken readString(char delimiter);
    CHTLJSToken readNumber();
    CHTLJSToken readLiteral();
    CHTLJSToken readOperator();
    
    char currentChar() const;
    char peekChar() const;
    void advance();
    bool isAtEnd() const;
    
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
};

} // namespace CHTL_JS