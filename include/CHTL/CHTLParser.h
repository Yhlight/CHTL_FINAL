#pragma once

#include "CHTLLexer.h"
#include "CHTLNode.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL 语法分析器
 * 
 * 负责将 Token 序列转换为抽象语法树（AST）
 */
class CHTLParser {
public:
    explicit CHTLParser(std::shared_ptr<CHTLContext> context);
    ~CHTLParser() = default;

    // 语法分析
    std::shared_ptr<BaseNode> parse(const std::vector<Token>& tokens);
    
    // 重置状态
    void reset();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<Token> m_tokens;
    size_t m_current;
    
    // 辅助方法
    const Token& current() const;
    const Token& peek(size_t offset = 1) const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool check(const std::vector<TokenType>& types) const;
    const Token& advance();
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& message);
    void synchronize();
    
    // 语法规则
    std::shared_ptr<BaseNode> parseProgram();
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseText();
    std::shared_ptr<BaseNode> parseComment();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseConfiguration();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseOperator();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<BaseNode> node);
    void parseStyleBlock(std::shared_ptr<BaseNode> node);
    void parseScriptBlock(std::shared_ptr<BaseNode> node);
    
    // 模板解析
    std::shared_ptr<TemplateNode> parseStyleTemplate();
    std::shared_ptr<TemplateNode> parseElementTemplate();
    std::shared_ptr<TemplateNode> parseVarTemplate();
    
    // 自定义解析
    std::shared_ptr<CustomNode> parseCustomStyle();
    std::shared_ptr<CustomNode> parseCustomElement();
    std::shared_ptr<CustomNode> parseCustomVar();
    
    // 原始嵌入解析
    std::shared_ptr<OriginNode> parseOriginHtml();
    std::shared_ptr<OriginNode> parseOriginStyle();
    std::shared_ptr<OriginNode> parseOriginJavaScript();
    
    // 导入解析
    std::shared_ptr<ImportNode> parseImportHtml();
    std::shared_ptr<ImportNode> parseImportStyle();
    std::shared_ptr<ImportNode> parseImportJavaScript();
    std::shared_ptr<ImportNode> parseImportChtl();
    std::shared_ptr<ImportNode> parseImportCjmod();
    
    // 配置解析
    std::shared_ptr<ConfigNode> parseConfigurationBlock();
    void parseNameConfiguration(std::shared_ptr<ConfigNode> config);
    void parseOriginTypeConfiguration(std::shared_ptr<ConfigNode> config);
    
    // 命名空间解析
    std::shared_ptr<NamespaceNode> parseNamespaceBlock();
    
    // 操作符解析
    std::shared_ptr<OperatorNode> parseDeleteOperator();
    std::shared_ptr<OperatorNode> parseInsertOperator();
    std::shared_ptr<OperatorNode> parseUseOperator();
    std::shared_ptr<OperatorNode> parseInheritOperator();
    
    // 表达式解析
    std::string parseExpression();
    
    // 节点类型转换
    NodeType getTemplateNodeType(const std::string& type) const;
    NodeType getCustomNodeType(const std::string& type) const;
    std::string parseConditionalExpression();
    std::string parseLogicalExpression();
    std::string parseEqualityExpression();
    std::string parseComparisonExpression();
    std::string parseAdditionExpression();
    std::string parseMultiplicationExpression();
    std::string parseUnaryExpression();
    std::string parsePrimaryExpression();
    
    // 工具方法
    std::string parseString();
    std::string parseLiteral();
    std::string parseIdentifier();
    void skipNewlines();
    
    // 错误处理
    void error(const Token& token, const std::string& message);
    void error(const std::string& message);
};

} // namespace CHTL