#pragma once

#include "Token.h"
#include "ASTNode.h"
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

/**
 * CHTL语法分析器
 * 负责将Token序列转换为AST
 */
class CHTLParser {
public:
    explicit CHTLParser(const std::vector<Token>& tokens);
    ~CHTLParser() = default;
    
    // 语法分析主函数
    std::shared_ptr<ASTNode> parse();
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
private:
    // 输入
    std::vector<Token> tokens_;
    size_t currentToken_;
    
    // 错误处理
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    
    // 核心解析方法
    std::shared_ptr<ASTNode> parseRoot();
    std::shared_ptr<ASTNode> parseElement();
    std::shared_ptr<ASTNode> parseSpecialStructure();
    std::shared_ptr<ASTNode> parseHTMLElement();
    std::shared_ptr<ASTNode> parseText();
    std::shared_ptr<ASTNode> parseStyle();
    std::shared_ptr<ASTNode> parseScript();
    std::shared_ptr<ASTNode> parseTemplate();
    std::shared_ptr<ASTNode> parseCustom();
    std::shared_ptr<ASTNode> parseOrigin();
    std::shared_ptr<ASTNode> parseImport();
    std::shared_ptr<ASTNode> parseNamespace();
    std::shared_ptr<ASTNode> parseConfiguration();
    std::shared_ptr<ASTNode> parseConstraint();
    std::shared_ptr<ASTNode> parseUse();
    std::shared_ptr<ASTNode> parseTemplateReference();
    void parseStyleRules(std::shared_ptr<StyleNode> styleNode);
    void parseStyleRuleProperties(std::shared_ptr<StyleRuleNode> ruleNode);
    std::string parseSelector();
    
    // 辅助方法
    Token& current();
    Token& peek();
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type);
    bool check(const std::vector<TokenType>& types);
    Token& advance();
    bool isAtEnd();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<ASTNode> node);
    void parseStyleProperties(std::shared_ptr<StyleNode> styleNode);
    
    // 表达式解析
    std::string parseExpression();
    std::string parseConditionalExpression();
    std::string parseLogicalExpression();
    std::string parseEqualityExpression();
    std::string parseRelationalExpression();
    std::string parseArithmeticExpression();
    std::string parsePrimaryExpression();
    
    // 错误报告
    void reportError(const std::string& message);
    void synchronize();
    
    // 工具方法
    bool isElementToken(const Token& token);
    bool isStylePropertyToken(const Token& token);
    std::string tokenTypeToString(TokenType type);
};

} // namespace CHTL