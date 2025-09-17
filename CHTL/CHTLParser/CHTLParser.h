#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/CHTLBaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OperatorNode.h"
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL语法分析器
 * 负责将令牌序列解析为AST
 */
class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    ~CHTLParser();
    
    // 主要解析方法
    std::shared_ptr<CHTLBaseNode> parse();
    std::shared_ptr<CHTLBaseNode> parseDocument();
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, int, int)> handler);
    void reportError(const std::string& message);

private:
    const std::vector<Token>& tokens_;
    size_t current_token_;
    std::function<void(const std::string&, int, int)> error_handler_;
    
    // 辅助方法
    Token currentToken() const;
    Token peekToken() const;
    void advance();
    bool match(TokenType type);
    bool match(const std::string& value);
    void expect(TokenType type, const std::string& message);
    void expect(const std::string& value, const std::string& message);
    
    // 解析方法
    std::shared_ptr<CHTLBaseNode> parseStatement();
    std::shared_ptr<CHTLBaseNode> parseElement();
    std::shared_ptr<CHTLBaseNode> parseText();
    std::shared_ptr<CHTLBaseNode> parseComment();
    std::shared_ptr<CHTLBaseNode> parseStyle();
    std::shared_ptr<CHTLBaseNode> parseScript();
    std::shared_ptr<CHTLBaseNode> parseTemplate();
    std::shared_ptr<CHTLBaseNode> parseCustom();
    std::shared_ptr<CHTLBaseNode> parseOrigin();
    std::shared_ptr<CHTLBaseNode> parseImport();
    std::shared_ptr<CHTLBaseNode> parseNamespace();
    std::shared_ptr<CHTLBaseNode> parseConfiguration();
    std::shared_ptr<CHTLBaseNode> parseOperator();
    std::shared_ptr<CHTLBaseNode> parseSelector();
    std::shared_ptr<CHTLBaseNode> parseResponsiveValue();
    std::shared_ptr<CHTLBaseNode> parseGeneratorComment();
    std::shared_ptr<CHTLBaseNode> parseExpression();
    std::shared_ptr<CHTLBaseNode> parseBlock();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<ElementNode> element);
    void parseStyleProperties(std::shared_ptr<StyleNode> style);
    
    // 模板解析
    std::shared_ptr<TemplateNode> parseTemplateStyle();
    std::shared_ptr<TemplateNode> parseTemplateElement();
    std::shared_ptr<TemplateNode> parseTemplateVar();
    std::shared_ptr<CustomNode> parseCustomStyle();
    std::shared_ptr<CustomNode> parseCustomElement();
    std::shared_ptr<CustomNode> parseCustomVar();
    
    // 原始嵌入解析
    std::shared_ptr<OriginNode> parseOriginHTML();
    std::shared_ptr<OriginNode> parseOriginStyle();
    std::shared_ptr<OriginNode> parseOriginJavaScript();
    
    // 导入解析
    std::shared_ptr<ImportNode> parseImportHTML();
    std::shared_ptr<ImportNode> parseImportStyle();
    std::shared_ptr<ImportNode> parseImportJavaScript();
    std::shared_ptr<ImportNode> parseImportCHTL();
    std::shared_ptr<ImportNode> parseImportCJMOD();
    
    // 操作符解析
    std::shared_ptr<DeleteNode> parseDelete();
    std::shared_ptr<InsertNode> parseInsert();
    std::shared_ptr<UseNode> parseUse();
    
    // 表达式解析
    std::shared_ptr<ExpressionNode> parsePrimaryExpression();
    std::shared_ptr<ExpressionNode> parseBinaryExpression();
    std::shared_ptr<ExpressionNode> parseUnaryExpression();
    
    // 工具方法
    bool isAtEnd() const;
    bool isElementStart() const;
    bool isTemplateStart() const;
    bool isCustomStart() const;
    bool isOriginStart() const;
    bool isImportStart() const;
    bool isNamespaceStart() const;
    bool isConfigurationStart() const;
    bool isOperatorStart() const;
    bool isSpecialStart() const;
    
    // 跳过空白和注释
    void skipWhitespaceAndComments();
};

} // namespace CHTL

#endif // CHTL_PARSER_H