#pragma once

#include "CHTL/Token.h"
#include "CHTL/CHTLContext.h"
#include <string>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL词法分析器类
 * 
 * 负责将CHTL源代码转换为Token序列
 */
class CHTLLexer {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit CHTLLexer(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~CHTLLexer();
    
    /**
     * @brief 词法分析
     * @param sourceCode 源代码
     * @return Token列表
     */
    TokenList tokenize(const std::string& sourceCode);
    
    /**
     * @brief 重置词法分析器状态
     */
    void reset();
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;

private:
    /**
     * @brief 跳过空白字符
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 跳过后的位置
     */
    size_t skipWhitespace(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 跳过注释
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 跳过后的位置
     */
    size_t skipComment(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 跳过单行注释
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 跳过后的位置
     */
    size_t skipSingleLineComment(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 跳过多行注释
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 跳过后的位置
     */
    size_t skipMultiLineComment(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 跳过生成器注释
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 跳过后的位置
     */
    size_t skipGeneratorComment(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 跳过字符串
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @param quote 引号字符
     * @return 跳过后的位置
     */
    size_t skipString(const std::string& sourceCode, size_t pos, char quote) const;
    
    /**
     * @brief 识别标识符
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 标识符结束位置
     */
    size_t readIdentifier(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 识别数字
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 数字结束位置
     */
    size_t readNumber(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 识别字符串
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 字符串结束位置
     */
    size_t readString(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 识别无修饰字面量
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 字面量结束位置
     */
    size_t readLiteral(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 识别关键字
     * @param word 单词
     * @return 对应的Token类型，不是关键字返回IDENTIFIER
     */
    TokenType identifyKeyword(const std::string& word) const;
    
    /**
     * @brief 识别符号
     * @param sourceCode 源代码
     * @param pos 当前位置
     * @return 符号Token类型，不是符号返回UNKNOWN
     */
    TokenType identifySymbol(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 计算行号和列号
     * @param sourceCode 源代码
     * @param pos 位置
     * @return 行号和列号的pair
     */
    std::pair<size_t, size_t> getLineColumn(const std::string& sourceCode, size_t pos) const;
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     * @param line 行号
     * @param column 列号
     */
    void addError(const std::string& message, size_t line, size_t column);

private:
    std::shared_ptr<CHTLContext> m_context;
    bool m_debugMode;
    
    // 关键字映射
    std::unordered_map<std::string, TokenType> m_keywords;
    
    // 符号映射
    std::unordered_map<std::string, TokenType> m_symbols;
};

} // namespace CHTL