#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <memory>
#include <vector>
#include <functional>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const TokenList& tokens);
    
    // 主要接口
    NodePtr parse();
    NodePtr parseDocument();
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
    // 状态查询
    bool hasMoreTokens() const;
    size_t getCurrentPosition() const { return currentPos_; }
    
private:
    TokenList tokens_;
    size_t currentPos_;
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    
    // Token处理
    TokenPtr currentToken() const;
    TokenPtr peekToken(size_t offset = 1) const;
    void advance();
    bool match(TokenType type) const;
    bool match(const std::string& value) const;
    void consume(TokenType type, const std::string& expected = "");
    void consume(const std::string& value, const std::string& expected = "");
    
    // 解析方法
    NodePtr parseElement();
    NodePtr parseText();
    NodePtr parseTemplate();
    NodePtr parseCustom();
    NodePtr parseStyle();
    NodePtr parseScript();
    NodePtr parseOrigin();
    NodePtr parseImport();
    NodePtr parseConfiguration();
    NodePtr parseNamespace();
    NodePtr parseOperator();
    
    // 属性解析
    void parseAttributes(ElementNode* element);
    void parseInlineStyles(ElementNode* element);
    
    // 块解析
    void parseBlock(NodePtr node);
    void parseStyleBlock(NodePtr node);
    void parseScriptBlock(NodePtr node);
    
    // 表达式解析
    std::string parseExpression();
    std::string parseConditionalExpression();
    std::string parseArithmeticExpression();
    
    // 模板解析
    NodePtr parseTemplateBlock(TokenType templateType);
    void parseTemplateContent(NodePtr templateNode);
    
    // 自定义解析
    NodePtr parseCustomBlock(TokenType customType);
    void parseCustomContent(NodePtr customNode);
    
    // 导入解析
    NodePtr parseImportStatement();
    std::string parseImportPath();
    
    // 配置解析
    NodePtr parseConfigurationBlock();
    void parseConfigurationContent(NodePtr configNode);
    
    // 命名空间解析
    NodePtr parseNamespaceBlock();
    void parseNamespaceContent(NodePtr namespaceNode);
    
    // 操作解析
    NodePtr parseOperatorBlock();
    void parseOperatorContent(NodePtr operatorNode);
    
    // 辅助方法
    bool isElementStart() const;
    bool isTemplateStart() const;
    bool isCustomStart() const;
    bool isStyleStart() const;
    bool isScriptStart() const;
    bool isOriginStart() const;
    bool isImportStart() const;
    bool isConfigurationStart() const;
    bool isNamespaceStart() const;
    bool isOperatorStart() const;
    
    // 错误报告
    void reportError(const std::string& message);
    void reportUnexpectedToken(const std::string& expected);
};

} // namespace CHTL

#endif // CHTL_PARSER_H