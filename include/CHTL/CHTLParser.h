#pragma once

#include "CHTLLexer.h"
#include "CHTLNode.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL语法分析器
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;

    /**
     * @brief 解析源代码为AST
     * @param tokens 词法单元列表
     * @return AST根节点
     */
    std::shared_ptr<CHTLNode> parse(const std::vector<Token>& tokens);

    /**
     * @brief 解析源代码为AST
     * @param source_code 源代码
     * @return AST根节点
     */
    std::shared_ptr<CHTLNode> parse(const std::string& source_code);

    /**
     * @brief 重置解析器状态
     */
    void reset();

    /**
     * @brief 设置调试模式
     * @param enabled 是否启用调试模式
     */
    void setDebugMode(bool enabled);

    /**
     * @brief 获取错误信息
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const { return errors_; }

    /**
     * @brief 清空错误信息
     */
    void clearErrors();

private:
    /**
     * @brief 解析根节点
     * @return 根节点
     */
    std::shared_ptr<CHTLNode> parseRoot();

    /**
     * @brief 解析元素
     * @return 元素节点
     */
    std::shared_ptr<CHTLNode> parseElement();

    /**
     * @brief 解析文本节点
     * @return 文本节点
     */
    std::shared_ptr<CHTLNode> parseText();

    /**
     * @brief 解析注释
     * @return 注释节点
     */
    std::shared_ptr<CHTLNode> parseComment();

    /**
     * @brief 解析模板
     * @return 模板节点
     */
    std::shared_ptr<CHTLNode> parseTemplate();

    /**
     * @brief 解析自定义
     * @return 自定义节点
     */
    std::shared_ptr<CHTLNode> parseCustom();

    /**
     * @brief 解析原始嵌入
     * @return 原始嵌入节点
     */
    std::shared_ptr<CHTLNode> parseOrigin();

    /**
     * @brief 解析导入
     * @return 导入节点
     */
    std::shared_ptr<CHTLNode> parseImport();

    /**
     * @brief 解析配置
     * @return 配置节点
     */
    std::shared_ptr<CHTLNode> parseConfiguration();

    /**
     * @brief 解析命名空间
     * @return 命名空间节点
     */
    std::shared_ptr<CHTLNode> parseNamespace();

    /**
     * @brief 解析样式块
     * @return 样式节点
     */
    std::shared_ptr<CHTLNode> parseStyle();

    /**
     * @brief 解析脚本块
     * @return 脚本节点
     */
    std::shared_ptr<CHTLNode> parseScript();

    /**
     * @brief 解析属性
     * @param node 节点
     */
    void parseAttributes(std::shared_ptr<CHTLNode> node);

    /**
     * @brief 解析属性值
     * @return 属性值
     */
    AttributeValue parseAttributeValue();

    /**
     * @brief 解析表达式
     * @return 表达式节点
     */
    std::shared_ptr<CHTLNode> parseExpression();

    /**
     * @brief 解析条件表达式
     * @return 条件表达式节点
     */
    std::shared_ptr<CHTLNode> parseConditionalExpression();

    /**
     * @brief 解析引用
     * @return 引用节点
     */
    std::shared_ptr<CHTLNode> parseReference();

    /**
     * @brief 解析变量
     * @return 变量节点
     */
    std::shared_ptr<CHTLNode> parseVariable();

    /**
     * @brief 解析函数调用
     * @return 函数调用节点
     */
    std::shared_ptr<CHTLNode> parseFunctionCall();

    /**
     * @brief 解析操作
     * @return 操作节点
     */
    std::shared_ptr<CHTLNode> parseOperation();

    /**
     * @brief 解析删除操作
     * @return 删除操作节点
     */
    std::shared_ptr<CHTLNode> parseDeleteOperation();

    /**
     * @brief 解析插入操作
     * @return 插入操作节点
     */
    std::shared_ptr<CHTLNode> parseInsertOperation();

    /**
     * @brief 解析继承操作
     * @return 继承操作节点
     */
    std::shared_ptr<CHTLNode> parseInheritOperation();

    /**
     * @brief 解析选择器
     * @return 选择器字符串
     */
    std::string parseSelector();

    /**
     * @brief 解析路径
     * @return 路径字符串
     */
    std::string parsePath();

    /**
     * @brief 解析标识符
     * @return 标识符字符串
     */
    std::string parseIdentifier();

    /**
     * @brief 解析字符串
     * @return 字符串
     */
    std::string parseString();

    /**
     * @brief 解析数字
     * @return 数字值
     */
    double parseNumber();

    /**
     * @brief 检查当前词法单元类型
     * @param type 期望的类型
     * @return 是否匹配
     */
    bool check(TokenType type) const;

    /**
     * @brief 检查当前词法单元类型并消费
     * @param type 期望的类型
     * @return 是否匹配
     */
    bool match(TokenType type);

    /**
     * @brief 消费当前词法单元
     * @return 当前词法单元
     */
    Token consume();

    /**
     * @brief 消费指定类型的词法单元
     * @param type 期望的类型
     * @return 词法单元
     */
    Token consume(TokenType type);

    /**
     * @brief 获取当前词法单元
     * @return 当前词法单元
     */
    const Token& current() const;

    /**
     * @brief 获取前一个词法单元
     * @return 前一个词法单元
     */
    const Token& previous() const;

    /**
     * @brief 检查是否到达末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;

    /**
     * @brief 同步到下一个语句
     */
    void synchronize();

    /**
     * @brief 报告错误
     * @param message 错误信息
     */
    void reportError(const std::string& message);

    /**
     * @brief 报告错误
     * @param token 出错的词法单元
     * @param message 错误信息
     */
    void reportError(const Token& token, const std::string& message);

    /**
     * @brief 报告错误并抛出异常
     * @param message 错误信息
     */
    void error(const std::string& message);

    /**
     * @brief 报告错误并抛出异常
     * @param token 出错的词法单元
     * @param message 错误信息
     */
    void error(const Token& token, const std::string& message);

private:
    std::vector<Token> tokens_;
    size_t current_token_;
    bool debug_mode_;
    std::vector<std::string> errors_;
};

} // namespace CHTL