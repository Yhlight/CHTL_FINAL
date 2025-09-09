#pragma once

#include "Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <memory>
#include <vector>

namespace CHTL {

class CHTLParser {
private:
    TokenStream tokens;
    size_t current;
    
    // 解析状态
    bool inTemplate;
    bool inCustom;
    bool inOrigin;
    bool inImport;
    bool inNamespace;
    bool inConfiguration;
    
    // 上下文栈
    std::vector<std::shared_ptr<BaseNode>> nodeStack;
    
    // 辅助方法
    Token peek(size_t offset = 0) const;
    Token consume();
    bool match(TokenType type) const;
    bool match(const std::string& value) const;
    bool match(TokenType type, const std::string& value) const;
    bool check(TokenType type) const;
    bool check(const std::string& value) const;
    
    // 错误处理
    void throwError(const std::string& message) const;
    void synchronize();
    
    // 解析方法
    std::shared_ptr<BaseNode> parseElement();
    std::shared_ptr<BaseNode> parseText();
    std::shared_ptr<BaseNode> parseComment();
    std::shared_ptr<BaseNode> parseTemplate();
    std::shared_ptr<BaseNode> parseCustom();
    std::shared_ptr<BaseNode> parseOrigin();
    std::shared_ptr<BaseNode> parseImport();
    std::shared_ptr<BaseNode> parseNamespace();
    std::shared_ptr<BaseNode> parseConfiguration();
    std::shared_ptr<BaseNode> parseStyle();
    std::shared_ptr<BaseNode> parseScript();
    std::shared_ptr<BaseNode> parseOperator();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<BaseNode> node);
    std::pair<std::string, std::string> parseAttribute();
    
    // 表达式解析
    std::string parseExpression();
    std::string parseConditionalExpression();
    std::string parseLogicalExpression();
    std::string parseEqualityExpression();
    std::string parseComparisonExpression();
    std::string parseAdditiveExpression();
    std::string parseMultiplicativeExpression();
    std::string parsePrimaryExpression();
    
    // 模板解析
    std::shared_ptr<BaseNode> parseTemplateStyle();
    std::shared_ptr<BaseNode> parseTemplateElement();
    std::shared_ptr<BaseNode> parseTemplateVar();
    
    // 自定义解析
    std::shared_ptr<BaseNode> parseCustomStyle();
    std::shared_ptr<BaseNode> parseCustomElement();
    std::shared_ptr<BaseNode> parseCustomVar();
    
    // 原始嵌入解析
    std::shared_ptr<BaseNode> parseOriginHTML();
    std::shared_ptr<BaseNode> parseOriginStyle();
    std::shared_ptr<BaseNode> parseOriginJavaScript();
    
    // 导入解析
    std::shared_ptr<BaseNode> parseImportFile();
    std::shared_ptr<BaseNode> parseImportTemplate();
    std::shared_ptr<BaseNode> parseImportCustom();
    std::shared_ptr<BaseNode> parseImportOrigin();
    
    // 配置解析
    std::shared_ptr<BaseNode> parseConfigurationGroup();
    std::shared_ptr<BaseNode> parseNameGroup();
    std::shared_ptr<BaseNode> parseOriginTypeGroup();
    
    // 操作符解析
    std::shared_ptr<BaseNode> parseDeleteOperator();
    std::shared_ptr<BaseNode> parseInsertOperator();
    std::shared_ptr<BaseNode> parseInheritOperator();
    
    // 工具方法
    void pushNode(std::shared_ptr<BaseNode> node);
    void popNode();
    std::shared_ptr<BaseNode> getCurrentNode() const;
    
    // 验证方法
    bool isValidElementName(const std::string& name) const;
    bool isValidAttributeName(const std::string& name) const;
    bool isValidTemplateName(const std::string& name) const;
    
public:
    CHTLParser(const TokenStream& tokens);
    ~CHTLParser() = default;
    
    // 主要解析方法
    std::shared_ptr<BaseNode> parse();
    std::vector<std::shared_ptr<BaseNode>> parseAll();
    
    // 状态查询
    bool isAtEnd() const;
    size_t getCurrentPosition() const;
    
    // 错误恢复
    void reset();
    void setPosition(size_t position);
};

} // namespace CHTL