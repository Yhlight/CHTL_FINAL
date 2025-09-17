#ifndef CHTLJSPARSER_H
#define CHTLJSPARSER_H

#include "ASTNode.h"
#include "../CHTLJSLexer/Token.h"
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS语法分析器
 * 将Token序列解析为AST
 */
class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser();

    // 解析方法
    std::shared_ptr<ProgramNode> parse(const std::vector<Token>& tokens);
    void setTokens(const std::vector<Token>& tokens);
    
    // 状态管理
    void reset();
    bool hasMoreTokens() const;
    Token currentToken() const;
    Token peekToken() const;
    void advance();
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;

private:
    std::vector<Token> tokens;
    size_t position;
    std::vector<std::string> errors;
    
    // 解析方法
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parseObjectLiteral();
    std::shared_ptr<ASTNode> parseFunctionCall();
    std::shared_ptr<ASTNode> parseVirtualObject();
    std::shared_ptr<ASTNode> parseListen();
    std::shared_ptr<ASTNode> parseAnimate();
    std::shared_ptr<ASTNode> parseRouter();
    std::shared_ptr<ASTNode> parseScriptLoader();
    std::shared_ptr<ASTNode> parseDelegate();
    std::shared_ptr<ASTNode> parseINeverAway();
    std::shared_ptr<ASTNode> parseUtilExpression();
    std::shared_ptr<ASTNode> parseSelector();
    std::shared_ptr<ASTNode> parseEventBinding();
    std::shared_ptr<ASTNode> parsePropertyAccess();
    
    // 辅助方法
    bool match(TokenType type) const;
    bool match(const std::vector<TokenType>& types) const;
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    Token consume(const std::string& message);
    bool isAtEnd() const;
    
    // 错误报告
    void reportError(const std::string& message);
    void reportError(const Token& token, const std::string& message);
    
    // 同步恢复
    void synchronize();
};

} // namespace CHTLJS

#endif // CHTLJSPARSER_H