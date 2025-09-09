#pragma once

#include "CHTLLexer.h"
#include "CHTLNode.h"
#include <memory>
#include <stack>

namespace CHTL {

/**
 * @brief CHTL语法分析器
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser() = default;

    /**
     * @brief 解析词法单元列表为AST
     * @param tokens 词法单元列表
     * @return AST根节点
     */
    std::shared_ptr<CHTLNode> parse(const std::vector<Token>& tokens);
    
    /**
     * @brief 从文件解析
     * @param filePath 文件路径
     * @return AST根节点
     */
    std::shared_ptr<CHTLNode> parseFile(const std::string& filePath);

    /**
     * @brief 获取解析错误
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * @brief 是否有错误
     * @return 是否有错误
     */
    bool hasErrors() const;

private:
    // 解析状态
    std::vector<Token> m_tokens;
    size_t m_current;
    std::vector<std::string> m_errors;
    
    // 解析栈
    std::stack<std::shared_ptr<CHTLNode>> m_nodeStack;
    
    // 当前命名空间
    std::string m_currentNamespace;
    
    // 辅助函数
    bool isAtEnd() const;
    Token current() const;
    Token peek() const;
    Token previous() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token advance();
    void consume(TokenType type, const std::string& message);
    
    // 错误处理
    void error(const std::string& message);
    void errorAtCurrent(const std::string& message);
    void errorAt(const Token& token, const std::string& message);
    
    // 解析函数
    std::shared_ptr<CHTLNode> parseRoot();
    std::shared_ptr<CHTLNode> parseElement();
    std::shared_ptr<CHTLNode> parseText();
    std::shared_ptr<CHTLNode> parseStyle();
    std::shared_ptr<CHTLNode> parseScript();
    std::shared_ptr<CHTLNode> parseTemplate();
    std::shared_ptr<CHTLNode> parseCustom();
    std::shared_ptr<CHTLNode> parseImport();
    std::shared_ptr<CHTLNode> parseOrigin();
    std::shared_ptr<CHTLNode> parseConfiguration();
    std::shared_ptr<CHTLNode> parseNamespace();
    std::shared_ptr<CHTLNode> parseOperator();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<CHTLNode> node);
    AttributeValue parseAttributeValue();
    
    // 样式解析
    void parseStyleProperties(std::shared_ptr<StyleNode> styleNode);
    void parseStyleRules(std::shared_ptr<StyleNode> styleNode);
    std::string parseStyleSelector();
    std::string parseStyleProperty();
    std::string parseStyleValue();
    
    // 条件表达式解析
    std::string parseConditionalExpression();
    std::string parseLogicalExpression();
    std::string parseComparisonExpression();
    std::string parsePrimaryExpression();
    
    // 模板解析
    void parseTemplateContent(std::shared_ptr<TemplateNode> templateNode);
    void parseTemplateInheritance(std::shared_ptr<TemplateNode> templateNode);
    
    // 自定义解析
    void parseCustomSpecialization(std::shared_ptr<CHTLNode> customNode);
    void parseCustomDeletion(std::shared_ptr<CHTLNode> customNode);
    void parseCustomInsertion(std::shared_ptr<CHTLNode> customNode);
    
    // 导入解析
    void parseImportPath(std::shared_ptr<ImportNode> importNode);
    void parseImportAlias(std::shared_ptr<ImportNode> importNode);
    
    // 命名空间解析
    void parseNamespaceContent(std::shared_ptr<CHTLNode> namespaceNode);
    
    // 配置解析
    void parseConfigurationContent(std::shared_ptr<CHTLNode> configNode);
    void parseNameConfiguration(std::shared_ptr<CHTLNode> configNode);
    
    // 工具函数
    std::string parseString();
    std::string parseLiteral();
    std::string parseIdentifier();
    bool isElementName(const std::string& name) const;
    bool isTemplateKeyword(const std::string& name) const;
    bool isCustomKeyword(const std::string& name) const;
    bool isImportKeyword(const std::string& name) const;
    bool isOriginKeyword(const std::string& name) const;
    bool isConfigurationKeyword(const std::string& name) const;
    bool isNamespaceKeyword(const std::string& name) const;
    bool isOperatorKeyword(const std::string& name) const;
};

} // namespace CHTL