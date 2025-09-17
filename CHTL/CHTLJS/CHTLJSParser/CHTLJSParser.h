#ifndef CHTLJS_PARSER_H
#define CHTLJS_PARSER_H

#include "ASTNode.h"
#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSLexer/CHTLJSLexer.h"
#include <memory>
#include <vector>

namespace CHTLJS {

class CHTLJSParser {
private:
    TokenList tokens;
    size_t currentIndex;
    
    Token getCurrent() const;
    Token peek(size_t offset = 1) const;
    bool hasNext() const;
    bool hasNext(size_t offset) const;
    void advance();
    bool match(TokenType type) const;
    bool match(const std::vector<TokenType>& types) const;
    Token consume(TokenType type, const std::string& message);
    bool check(TokenType type) const;
    bool isAtEnd() const;
    
    void error(const std::string& message) const;
    void synchronize();
    
    // 解析方法
    std::shared_ptr<ASTNode> parseProgram();
    std::shared_ptr<ASTNode> parseStatement();
    std::shared_ptr<ASTNode> parseExpression();
    std::shared_ptr<ASTNode> parseExpressionStatement();
    std::shared_ptr<ASTNode> parseBlockStatement();
    std::shared_ptr<ASTNode> parseVariableDeclaration();
    std::shared_ptr<ASTNode> parseFunctionDeclaration();
    std::shared_ptr<ASTNode> parseIfStatement();
    std::shared_ptr<ASTNode> parseWhileStatement();
    std::shared_ptr<ASTNode> parseForStatement();
    std::shared_ptr<ASTNode> parseReturnStatement();
    std::shared_ptr<ASTNode> parseBreakStatement();
    std::shared_ptr<ASTNode> parseContinueStatement();
    std::shared_ptr<ASTNode> parseTryStatement();
    std::shared_ptr<ASTNode> parseThrowStatement();
    
    // 表达式解析
    std::shared_ptr<ASTNode> parseAssignment();
    std::shared_ptr<ASTNode> parseConditional();
    std::shared_ptr<ASTNode> parseLogicalOr();
    std::shared_ptr<ASTNode> parseLogicalAnd();
    std::shared_ptr<ASTNode> parseEquality();
    std::shared_ptr<ASTNode> parseComparison();
    std::shared_ptr<ASTNode> parseAddition();
    std::shared_ptr<ASTNode> parseMultiplication();
    std::shared_ptr<ASTNode> parseUnary();
    std::shared_ptr<ASTNode> parsePrimary();
    std::shared_ptr<ASTNode> parseCall();
    std::shared_ptr<ASTNode> parseMember();
    
    // CHTL JS 特有解析
    std::shared_ptr<ASTNode> parseVirDeclaration();
    std::shared_ptr<ASTNode> parseListenExpression();
    std::shared_ptr<ASTNode> parseAnimateExpression();
    std::shared_ptr<ASTNode> parseRouterExpression();
    std::shared_ptr<ASTNode> parseScriptLoaderExpression();
    std::shared_ptr<ASTNode> parseDelegateExpression();
    std::shared_ptr<ASTNode> parseSelectorExpression();
    std::shared_ptr<ASTNode> parseReactiveValueExpression();
    
    // 辅助方法
    std::shared_ptr<ASTNode> parseObjectExpression();
    std::shared_ptr<ASTNode> parseArrayExpression();
    std::shared_ptr<ASTNode> parseProperty();
    std::vector<std::shared_ptr<ASTNode>> parseArgumentList();
    std::vector<std::string> parseParameterList();
    
    // 工具方法
    std::string getOperatorString(TokenType type) const;
    bool isAssignmentOperator(TokenType type) const;
    bool isLogicalOperator(TokenType type) const;
    bool isEqualityOperator(TokenType type) const;
    bool isComparisonOperator(TokenType type) const;
    bool isAdditionOperator(TokenType type) const;
    bool isMultiplicationOperator(TokenType type) const;
    bool isUnaryOperator(TokenType type) const;
    
public:
    CHTLJSParser(const TokenList& tokenList);
    CHTLJSParser(const std::string& source);
    ~CHTLJSParser() = default;
    
    std::shared_ptr<ASTNode> parse();
    std::shared_ptr<ASTNode> parse(const std::string& source);
    
    void reset();
    void setTokens(const TokenList& tokenList);
    
    TokenList getTokens() const;
    size_t getCurrentIndex() const;
    bool hasErrors() const;
};

} // namespace CHTLJS

#endif // CHTLJS_PARSER_H