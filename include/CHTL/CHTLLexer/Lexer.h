#pragma once

#include "Token.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL 词法分析器
 * 
 * 负责将 CHTL 源代码转换为 Token 序列
 */
class Lexer {
public:
    /**
     * @brief 构造函数
     * @param source 源代码
     */
    explicit Lexer(const std::string& source);
    
    /**
     * @brief 析构函数
     */
    ~Lexer();
    
    /**
     * @brief 词法分析
     * @return Token 列表
     */
    TokenList tokenize();
    
    /**
     * @brief 获取下一个 Token
     * @return 下一个 Token
     */
    Token nextToken();
    
    /**
     * @brief 查看下一个 Token（不移动位置）
     * @return 下一个 Token
     */
    Token peekToken();
    
    /**
     * @brief 回退一个 Token
     */
    void ungetToken();
    
    /**
     * @brief 检查是否到达文件末尾
     * @return 是否到达文件末尾
     */
    bool isEOF() const;
    
    /**
     * @brief 获取当前行号
     * @return 当前行号
     */
    size_t getCurrentLine() const { return currentLine_; }
    
    /**
     * @brief 获取当前列号
     * @return 当前列号
     */
    size_t getCurrentColumn() const { return currentColumn_; }
    
    /**
     * @brief 获取当前位置
     * @return 当前位置
     */
    size_t getCurrentPosition() const { return currentPosition_; }

private:
    /**
     * @brief 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * @brief 跳过行注释
     */
    void skipLineComment();
    
    /**
     * @brief 跳过块注释
     */
    void skipBlockComment();
    
    /**
     * @brief 读取标识符
     * @return 标识符 Token
     */
    Token readIdentifier();
    
    /**
     * @brief 读取字符串
     * @return 字符串 Token
     */
    Token readString();
    
    /**
     * @brief 读取数字
     * @return 数字 Token
     */
    Token readNumber();
    
    /**
     * @brief 读取符号
     * @return 符号 Token
     */
    Token readSymbol();
    
    /**
     * @brief 读取生成器注释
     * @return 生成器注释 Token
     */
    Token readGeneratorComment();
    
    /**
     * @brief 读取模板关键字
     * @return 模板关键字 Token
     */
    Token readTemplateKeyword();
    
    /**
     * @brief 读取自定义关键字
     * @return 自定义关键字 Token
     */
    Token readCustomKeyword();
    
    /**
     * @brief 读取原始嵌入关键字
     * @return 原始嵌入关键字 Token
     */
    Token readOriginKeyword();
    
    /**
     * @brief 读取导入关键字
     * @return 导入关键字 Token
     */
    Token readImportKeyword();
    
    /**
     * @brief 读取配置关键字
     * @return 配置关键字 Token
     */
    Token readConfigurationKeyword();
    
    /**
     * @brief 读取命名空间关键字
     * @return 命名空间关键字 Token
     */
    Token readNamespaceKeyword();
    
    /**
     * @brief 读取类型关键字
     * @return 类型关键字 Token
     */
    Token readTypeKeyword();
    
    /**
     * @brief 读取操作符关键字
     * @return 操作符关键字 Token
     */
    Token readOperatorKeyword();
    
    /**
     * @brief 检查是否为关键字
     * @param word 要检查的单词
     * @return 是否为关键字
     */
    bool isKeyword(const std::string& word) const;
    
    /**
     * @brief 检查是否为类型关键字
     * @param word 要检查的单词
     * @return 是否为类型关键字
     */
    bool isTypeKeyword(const std::string& word) const;
    
    /**
     * @brief 检查是否为操作符关键字
     * @param word 要检查的单词
     * @return 是否为操作符关键字
     */
    bool isOperatorKeyword(const std::string& word) const;
    
    /**
     * @brief 检查是否为字母
     * @param c 要检查的字符
     * @return 是否为字母
     */
    bool isAlpha(char c) const;
    
    /**
     * @brief 检查是否为数字
     * @param c 要检查的字符
     * @return 是否为数字
     */
    bool isDigit(char c) const;
    
    /**
     * @brief 检查是否为字母或数字
     * @param c 要检查的字符
     * @return 是否为字母或数字
     */
    bool isAlphaNumeric(char c) const;
    
    /**
     * @brief 检查是否为空白字符
     * @param c 要检查的字符
     * @return 是否为空白字符
     */
    bool isWhitespace(char c) const;
    
    /**
     * @brief 获取当前字符
     * @return 当前字符
     */
    char currentChar() const;
    
    /**
     * @brief 获取下一个字符
     * @return 下一个字符
     */
    char nextChar();
    
    /**
     * @brief 回退一个字符
     */
    void ungetChar();
    
    /**
     * @brief 创建 Token
     * @param type Token 类型
     * @param value Token 值
     * @return Token 对象
     */
    Token createToken(TokenType type, const std::string& value);

private:
    std::string source_;        ///< 源代码
    size_t currentPosition_;    ///< 当前位置
    size_t currentLine_;        ///< 当前行号
    size_t currentColumn_;      ///< 当前列号
    TokenList tokens_;          ///< Token 列表
    size_t tokenIndex_;         ///< 当前 Token 索引
};

} // namespace CHTL