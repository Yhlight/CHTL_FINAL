#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL词法分析器
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer();
    
    /**
     * 词法分析
     * @param code 输入代码
     * @return Token列表
     */
    std::vector<Token> tokenize(const std::string& code);
    
    /**
     * 重置词法分析器状态
     */
    void reset();
    
    /**
     * 设置是否保留注释
     * @param keep 是否保留
     */
    void setKeepComments(bool keep);
    
    /**
     * 设置是否保留空白字符
     * @param keep 是否保留
     */
    void setKeepWhitespace(bool keep);

private:
    /**
     * 获取下一个Token
     * @return Token
     */
    Token getNextToken();
    
    /**
     * 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * 跳过注释
     */
    void skipComment();
    
    /**
     * 跳过行注释
     */
    void skipLineComment();
    
    /**
     * 跳过块注释
     */
    void skipBlockComment();
    
    /**
     * 处理生成器注释
     * @return Token
     */
    Token processGeneratorComment();
    
    /**
     * 处理方括号关键字
     * @return Token
     */
    Token processBracketKeyword();
    
    /**
     * 处理标识符
     * @return Token
     */
    Token processIdentifier();
    
    /**
     * 处理字符串
     * @return Token
     */
    Token processString();
    
    /**
     * 处理字面量（无修饰字符串）
     * @return Token
     */
    Token processLiteral();
    
    /**
     * 处理数字
     * @return Token
     */
    Token processNumber();
    
    /**
     * 处理操作符
     * @return Token
     */
    Token processOperator();
    
    /**
     * 处理分隔符
     * @return Token
     */
    Token processDelimiter();
    
    /**
     * 检查是否为关键字
     * @param str 字符串
     * @return 是否为关键字
     */
    bool isKeyword(const std::string& str);
    
    /**
     * 获取当前位置的字符
     * @return 字符
     */
    char current() const;
    
    /**
     * 获取指定偏移位置的字符
     * @param offset 偏移
     * @return 字符
     */
    char peek(size_t offset = 1) const;
    
    /**
     * 前进到下一个字符
     */
    void advance();
    
    /**
     * 检查是否到达文件末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * 获取当前位置信息
     * @return 位置信息
     */
    std::pair<size_t, size_t> getPosition() const;

private:
    std::string m_code;
    size_t m_position;
    size_t m_line;
    size_t m_column;
    bool m_keepComments;
    bool m_keepWhitespace;
    
    // 状态信息
    bool m_inString;
    bool m_inComment;
    char m_stringDelimiter;
};

} // namespace CHTL

#endif // CHTL_LEXER_H