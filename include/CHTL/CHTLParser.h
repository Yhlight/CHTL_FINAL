#pragma once

#include "CHTL/Token.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLContext.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL语法分析器类
 * 
 * 负责将Token序列转换为抽象语法树(AST)
 */
class CHTLParser {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit CHTLParser(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~CHTLParser();
    
    /**
     * @brief 解析Token序列
     * @param tokens Token列表
     * @return AST根节点
     */
    std::shared_ptr<BaseNode> parse(const TokenList& tokens);
    
    /**
     * @brief 解析字符串
     * @param sourceCode 源代码字符串
     * @return AST根节点
     */
    std::shared_ptr<BaseNode> parse(const std::string& sourceCode);
    
    /**
     * @brief 重置解析器状态
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
     * @brief 解析文档根节点
     * @return 文档根节点
     */
    std::shared_ptr<BaseNode> parseDocument();
    
    /**
     * @brief 解析元素节点
     * @return 元素节点
     */
    std::shared_ptr<BaseNode> parseElement();
    
    /**
     * @brief 解析文本节点
     * @return 文本节点
     */
    std::shared_ptr<BaseNode> parseText();
    
    /**
     * @brief 解析注释节点
     * @return 注释节点
     */
    std::shared_ptr<BaseNode> parseComment();
    
    /**
     * @brief 解析模板节点
     * @return 模板节点
     */
    std::shared_ptr<BaseNode> parseTemplate();
    
    /**
     * @brief 解析自定义节点
     * @return 自定义节点
     */
    std::shared_ptr<BaseNode> parseCustom();
    
    /**
     * @brief 解析原始嵌入节点
     * @return 原始嵌入节点
     */
    std::shared_ptr<BaseNode> parseOrigin();
    
    /**
     * @brief 解析导入节点
     * @return 导入节点
     */
    std::shared_ptr<BaseNode> parseImport();
    
    /**
     * @brief 解析配置节点
     * @return 配置节点
     */
    std::shared_ptr<BaseNode> parseConfiguration();
    
    /**
     * @brief 解析命名空间节点
     * @return 命名空间节点
     */
    std::shared_ptr<BaseNode> parseNamespace();
    
    /**
     * @brief 解析模块节点
     * @return 模块节点
     */
    std::shared_ptr<BaseNode> parseModule();
    
    /**
     * @brief 解析样式节点
     * @return 样式节点
     */
    std::shared_ptr<BaseNode> parseStyle();
    
    /**
     * @brief 解析脚本节点
     * @return 脚本节点
     */
    std::shared_ptr<BaseNode> parseScript();
    
    /**
     * @brief 解析属性列表
     * @param element 元素节点
     */
    void parseAttributes(std::shared_ptr<BaseNode> element);
    
    /**
     * @brief 解析属性
     * @return 属性键值对
     */
    std::pair<std::string, std::string> parseAttribute();
    
    /**
     * @brief 解析样式属性
     * @param styleNode 样式节点
     */
    void parseStyleProperties(std::shared_ptr<BaseNode> styleNode);
    
    /**
     * @brief 解析样式属性
     * @return 样式属性键值对
     */
    std::pair<std::string, std::string> parseStyleProperty();
    
    /**
     * @brief 解析选择器
     * @return 选择器字符串
     */
    std::string parseSelector();
    
    /**
     * @brief 解析表达式
     * @return 表达式字符串
     */
    std::string parseExpression();
    
    /**
     * @brief 解析字符串
     * @return 字符串值
     */
    std::string parseString();
    
    /**
     * @brief 解析字面量
     * @return 字面量值
     */
    std::string parseLiteral();
    
    /**
     * @brief 跳过空白Token
     */
    void skipWhitespace();
    
    /**
     * @brief 检查当前Token类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool checkToken(TokenType type) const;
    
    /**
     * @brief 检查当前Token值
     * @param value Token值
     * @return 是否匹配
     */
    bool checkToken(const std::string& value) const;
    
    /**
     * @brief 匹配并消费Token
     * @param type Token类型
     * @return 是否匹配
     */
    bool matchToken(TokenType type);
    
    /**
     * @brief 匹配并消费Token
     * @param value Token值
     * @return 是否匹配
     */
    bool matchToken(const std::string& value);
    
    /**
     * @brief 获取当前Token
     * @return 当前Token
     */
    const Token& currentToken() const;
    
    /**
     * @brief 获取下一个Token
     * @return 下一个Token
     */
    const Token& peekToken() const;
    
    /**
     * @brief 移动到下一个Token
     * @return 是否成功移动
     */
    bool nextToken();
    
    /**
     * @brief 检查是否到达末尾
     * @return 是否到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * @brief 添加警告信息
     * @param message 警告信息
     */
    void addWarning(const std::string& message);

private:
    std::shared_ptr<CHTLContext> m_context;
    TokenList m_tokens;
    size_t m_currentIndex;
    bool m_debugMode;
};

} // namespace CHTL