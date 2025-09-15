#ifndef CHTLPARSER_H
#define CHTLPARSER_H

#include "CHTLASTNode.h"
#include "../CHTLLexer/CHTLToken.h"
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

/**
 * CHTL语法分析器
 * 将Token序列解析为AST
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();

    // 解析方法
    std::shared_ptr<ProgramNode> parse(const std::vector<CHTLToken>& tokens);
    void setTokens(const std::vector<CHTLToken>& tokens);
    
    // 状态管理
    void reset();
    bool hasMoreTokens() const;
    CHTLToken currentToken() const;
    CHTLToken peekToken() const;
    void advance();
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;

private:
    std::vector<CHTLToken> tokens;
    size_t position;
    std::vector<std::string> errors;
    
    // 解析方法
    std::shared_ptr<CHTLASTNode> parseStatement();
    std::shared_ptr<CHTLASTNode> parseElement();
    std::shared_ptr<CHTLASTNode> parseText();
    std::shared_ptr<CHTLASTNode> parseAttribute();
    std::shared_ptr<CHTLASTNode> parseStyleBlock();
    std::shared_ptr<CHTLASTNode> parseScriptBlock();
    std::shared_ptr<CHTLASTNode> parseTemplate();
    std::shared_ptr<CHTLASTNode> parseCustom();
    std::shared_ptr<CHTLASTNode> parseOrigin();
    std::shared_ptr<CHTLASTNode> parseImport();
    std::shared_ptr<CHTLASTNode> parseConfiguration();
    std::shared_ptr<CHTLASTNode> parseNamespace();
    std::shared_ptr<CHTLASTNode> parseInfo();
    std::shared_ptr<CHTLASTNode> parseExport();
    std::shared_ptr<CHTLASTNode> parseUseStatement();
    std::shared_ptr<CHTLASTNode> parseExpression();
    std::shared_ptr<CHTLASTNode> parseConditionalExpression();
    std::shared_ptr<CHTLASTNode> parseBinaryExpression();
    std::shared_ptr<CHTLASTNode> parsePrimary();
    std::shared_ptr<CHTLASTNode> parseLiteral();
    std::shared_ptr<CHTLASTNode> parseIdentifier();
    std::shared_ptr<CHTLASTNode> parsePropertyReference();
    std::shared_ptr<CHTLASTNode> parseFunctionCall();
    
    // 样式解析
    std::shared_ptr<CHTLASTNode> parseStyleRule();
    std::shared_ptr<CHTLASTNode> parseStyleSelector();
    std::shared_ptr<CHTLASTNode> parseStyleProperty();
    std::shared_ptr<CHTLASTNode> parseStyleValue();
    
    // 模板解析
    std::shared_ptr<CHTLASTNode> parseTemplateStyle();
    std::shared_ptr<CHTLASTNode> parseTemplateElement();
    std::shared_ptr<CHTLASTNode> parseTemplateVar();
    
    // 自定义解析
    std::shared_ptr<CHTLASTNode> parseCustomStyle();
    std::shared_ptr<CHTLASTNode> parseCustomElement();
    std::shared_ptr<CHTLASTNode> parseCustomVar();
    
    // 辅助方法
    bool match(CHTLTokenType type) const;
    bool match(const std::vector<CHTLTokenType>& types) const;
    bool check(CHTLTokenType type) const;
    CHTLToken consume(CHTLTokenType type, const std::string& message);
    CHTLToken consume(const std::string& message);
    bool isAtEnd() const;
    
    // 错误报告
    void reportError(const std::string& message);
    void reportError(const CHTLToken& token, const std::string& message);
    
    // 同步恢复
    void synchronize();
    
    // 上下文管理
    std::map<std::string, std::shared_ptr<CHTLASTNode>> templates;
    std::map<std::string, std::shared_ptr<CHTLASTNode>> customs;
    std::map<std::string, std::string> variables;
    std::set<std::string> currentNamespace;
    
    // 样式处理
    std::string generateGlobalStyles();
    void addGlobalStyle(const std::string& style);
    std::vector<std::string> globalStyles;
    
    // 属性运算处理
    std::shared_ptr<CHTLASTNode> parseArithmeticExpression();
    std::shared_ptr<CHTLASTNode> parsePropertyExpression();
    bool isArithmeticOperator(const std::string& op) const;
    bool isComparisonOperator(const std::string& op) const;
    bool isLogicalOperator(const std::string& op) const;
    
    // 选择器处理
    std::string resolveSelector(const std::string& selector) const;
    std::string generateUniqueClass(const std::string& baseName) const;
    std::string generateUniqueId(const std::string& baseName) const;
    
    // 变量处理
    std::string resolveVariable(const std::string& name) const;
    void addVariable(const std::string& name, const std::string& value);
    
    // 模板处理
    std::shared_ptr<CHTLASTNode> resolveTemplate(const std::string& name) const;
    void addTemplate(const std::string& name, std::shared_ptr<CHTLASTNode> template_);
    
    // 自定义处理
    std::shared_ptr<CHTLASTNode> resolveCustom(const std::string& name) const;
    void addCustom(const std::string& name, std::shared_ptr<CHTLASTNode> custom);
};

} // namespace CHTL

#endif // CHTLPARSER_H