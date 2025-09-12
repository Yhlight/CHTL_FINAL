#pragma once

#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLNode.h"
#include "CHTL/CHTLContext.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * CHTL语法分析器
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();
    
    // 设置输入
    void setInput(const std::string& input);
    void setInput(std::istream& input);
    
    // 解析
    std::shared_ptr<CHTLNode> parse();
    std::vector<std::shared_ptr<CHTLNode>> parseAll();
    
    // 配置
    void setContext(std::shared_ptr<CHTLContext> context);
    void setDebugMode(bool enabled);
    
    // 错误处理
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    void clearErrors();

private:
    // 词法分析器
    CHTLLexer lexer_;
    
    // 上下文
    std::shared_ptr<CHTLContext> context_;
    
    // 当前token
    Token current_token_;
    bool token_consumed_;
    
    // 调试模式
    bool debug_mode_;
    
    // 错误处理
    std::vector<std::string> errors_;
    
    // Token管理
    void advance();
    void consume();
    bool match(TokenType type);
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type) const;
    bool check(const std::vector<TokenType>& types) const;
    
    // 错误报告
    void reportError(const std::string& message);
    void reportError(const std::string& message, const Token& token);
    
    // 解析函数
    std::shared_ptr<CHTLNode> parseElement();
    std::shared_ptr<CHTLNode> parseText();
    std::shared_ptr<CHTLNode> parseComment();
    std::shared_ptr<CHTLNode> parseStyle();
    std::shared_ptr<CHTLNode> parseScript();
    std::shared_ptr<CHTLNode> parseTemplate();
    std::shared_ptr<CHTLNode> parseCustom();
    std::shared_ptr<CHTLNode> parseOrigin();
    std::shared_ptr<CHTLNode> parseImport();
    std::shared_ptr<CHTLNode> parseNamespace();
    std::shared_ptr<CHTLNode> parseConfiguration();
    
    // 属性解析
    void parseAttributes(std::shared_ptr<ElementNode> element);
    void parseCSSProperties(std::shared_ptr<StyleNode> style);
    
    // 表达式解析
    std::string parseExpression();
    std::string parseConditionalExpression();
    std::string parseLogicalOrExpression();
    std::string parseLogicalAndExpression();
    std::string parseEqualityExpression();
    std::string parseRelationalExpression();
    std::string parseAdditiveExpression();
    std::string parseMultiplicativeExpression();
    std::string parseUnaryExpression();
    std::string parsePrimaryExpression();
    
    // 辅助函数
    std::string getTokenValue(TokenType type) const;
    bool isElementToken(TokenType type) const;
    bool isTemplateToken(TokenType type) const;
    bool isCustomToken(TokenType type) const;
    bool isOriginToken(TokenType type) const;
    bool isImportToken(TokenType type) const;
    bool isNamespaceToken(TokenType type) const;
    bool isConfigurationToken(TokenType type) const;
};

} // namespace CHTL