#pragma once

#include "Token.h"
#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>

namespace CHTL {

/**
 * @brief CHTL词法分析器
 * 
 * 负责将CHTL源代码转换为令牌序列
 */
class Lexer {
public:
    Lexer();
    ~Lexer() = default;

    /**
     * @brief 词法分析
     * @param source 源代码字符串
     * @return 令牌流
     */
    std::unique_ptr<TokenStream> tokenize(const std::string& source);

    /**
     * @brief 从文件进行词法分析
     * @param filename 文件名
     * @return 令牌流
     */
    std::unique_ptr<TokenStream> tokenizeFile(const std::string& filename);

    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);

    /**
     * @brief 获取错误信息
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;

    /**
     * @brief 清空错误信息
     */
    void clearErrors();

private:
    std::string m_source;
    size_t m_position;
    size_t m_line;
    size_t m_column;
    bool m_debugMode;
    std::vector<std::string> m_errors;

    // 关键字集合
    std::unordered_set<std::string> m_keywords;
    std::unordered_set<std::string> m_operators;
    std::unordered_set<std::string> m_delimiters;

    /**
     * @brief 初始化关键字集合
     */
    void initializeKeywords();

    /**
     * @brief 获取下一个字符
     * @return 下一个字符
     */
    char peek() const;

    /**
     * @brief 获取指定偏移的字符
     * @param offset 偏移量
     * @return 字符
     */
    char peek(size_t offset) const;

    /**
     * @brief 消费当前字符并前进
     * @return 当前字符
     */
    char consume();

    /**
     * @brief 消费指定数量的字符
     * @param count 字符数量
     */
    void consume(size_t count);

    /**
     * @brief 检查是否到达文件末尾
     * @return 是否到达文件末尾
     */
    bool isAtEnd() const;

    /**
     * @brief 跳过空白字符
     */
    void skipWhitespace();

    /**
     * @brief 跳过注释
     * @return 是否跳过了注释
     */
    bool skipComment();

    /**
     * @brief 跳过单行注释
     */
    void skipSingleLineComment();

    /**
     * @brief 跳过多行注释
     */
    void skipMultiLineComment();

    /**
     * @brief 跳过生成器注释
     */
    void skipGeneratorComment();

    /**
     * @brief 识别标识符
     * @return 标识符令牌
     */
    std::unique_ptr<Token> scanIdentifier();

    /**
     * @brief 识别字符串
     * @return 字符串令牌
     */
    std::unique_ptr<Token> scanString();

    /**
     * @brief 识别数字
     * @return 数字令牌
     */
    std::unique_ptr<Token> scanNumber();

    /**
     * @brief 识别无修饰字面量
     * @return 字面量令牌
     */
    std::unique_ptr<Token> scanLiteral();

    /**
     * @brief 识别操作符
     * @return 操作符令牌
     */
    std::unique_ptr<Token> scanOperator();

    /**
     * @brief 识别分隔符
     * @return 分隔符令牌
     */
    std::unique_ptr<Token> scanDelimiter();

    /**
     * @brief 识别特殊字符
     * @return 特殊字符令牌
     */
    std::unique_ptr<Token> scanSpecial();

    /**
     * @brief 检查是否为字母
     * @param c 字符
     * @return 是否为字母
     */
    bool isAlpha(char c) const;

    /**
     * @brief 检查是否为数字
     * @param c 字符
     * @return 是否为数字
     */
    bool isDigit(char c) const;

    /**
     * @brief 检查是否为字母或数字
     * @param c 字符
     * @return 是否为字母或数字
     */
    bool isAlphaNumeric(char c) const;

    /**
     * @brief 检查是否为空白字符
     * @param c 字符
     * @return 是否为空白字符
     */
    bool isWhitespace(char c) const;

    /**
     * @brief 检查是否为操作符字符
     * @param c 字符
     * @return 是否为操作符字符
     */
    bool isOperatorChar(char c) const;

    /**
     * @brief 检查是否为分隔符字符
     * @param c 字符
     * @return 是否为分隔符字符
     */
    bool isDelimiterChar(char c) const;

    /**
     * @brief 检查是否为特殊字符
     * @param c 字符
     * @return 是否为特殊字符
     */
    bool isSpecialChar(char c) const;

    /**
     * @brief 检查字符串是否为关键字
     * @param str 字符串
     * @return 是否为关键字
     */
    bool isKeyword(const std::string& str) const;

    /**
     * @brief 检查字符串是否为操作符
     * @param str 字符串
     * @return 是否为操作符
     */
    bool isOperator(const std::string& str) const;

    /**
     * @brief 检查字符串是否为分隔符
     * @param str 字符串
     * @return 是否为分隔符
     */
    bool isDelimiter(const std::string& str) const;

    /**
     * @brief 获取关键字对应的令牌类型
     * @param keyword 关键字
     * @return 令牌类型
     */
    TokenType getKeywordType(const std::string& keyword) const;

    /**
     * @brief 获取操作符对应的令牌类型
     * @param op 操作符
     * @return 令牌类型
     */
    TokenType getOperatorType(const std::string& op) const;

    /**
     * @brief 获取分隔符对应的令牌类型
     * @param delim 分隔符
     * @return 令牌类型
     */
    TokenType getDelimiterType(const std::string& delim) const;

    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);

    /**
     * @brief 创建令牌
     * @param type 令牌类型
     * @param value 令牌值
     * @return 令牌指针
     */
    std::unique_ptr<Token> createToken(TokenType type, const std::string& value);
};

} // namespace CHTL