#ifndef CHTLJSPARSER_H
#define CHTLJSPARSER_H

#include "../CHTLJSLexer/Token.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include <vector>
#include <memory>
#include <functional>

namespace CHTLJS {

/**
 * CHTL JS解析器
 * 负责将令牌序列解析为AST
 * 严格遵循CHTL.md中定义的CHTL JS语法规范
 */
class CHTLJSParser {
public:
    CHTLJSParser(const std::vector<CHTLJS_Token>& tokens, 
                 std::shared_ptr<CHTLJSContext> context = nullptr);
    ~CHTLJSParser();
    
    // 主要解析方法
    std::shared_ptr<CHTLJSBaseNode> parse();
    std::shared_ptr<CHTLJSBaseNode> parseProgram();
    std::shared_ptr<CHTLJSBaseNode> parseStatement();
    std::shared_ptr<CHTLJSBaseNode> parseExpression();
    std::shared_ptr<CHTLJSBaseNode> parsePrimaryExpression();
    
    // 状态查询
    bool hasMoreTokens() const;
    const CHTLJS_Token& currentToken() const;
    const CHTLJS_Token& peekToken(int offset = 1) const;
    
    // 上下文管理
    void setContext(std::shared_ptr<CHTLJSContext> context);
    std::shared_ptr<CHTLJSContext> getContext() const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, int, int)> handler);
    void reportError(const std::string& message, int line = 0, int column = 0);
    
private:
    std::vector<CHTLJS_Token> tokens_;
    size_t position_;
    std::shared_ptr<CHTLJSContext> context_;
    std::function<void(const std::string&, int, int)> error_handler_;
    
    // 辅助方法
    void advance();
    bool match(CHTLJS_TokenType type);
    bool match(const std::vector<CHTLJS_TokenType>& types);
    CHTLJS_Token consume(CHTLJS_TokenType type);
    CHTLJS_Token consume(const std::vector<CHTLJS_TokenType>& types);
    bool expect(CHTLJS_TokenType type, const std::string& expected);
    
    // 语句解析
    std::shared_ptr<CHTLJSBaseNode> parseBlockStatement();
    std::shared_ptr<CHTLJSBaseNode> parseExpressionStatement();
    std::shared_ptr<CHTLJSBaseNode> parseIfStatement();
    std::shared_ptr<CHTLJSBaseNode> parseWhileStatement();
    std::shared_ptr<CHTLJSBaseNode> parseForStatement();
    std::shared_ptr<CHTLJSBaseNode> parseReturnStatement();
    std::shared_ptr<CHTLJSBaseNode> parseVariableDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseFunctionDeclaration();
    
    // CHTL JS特有语法解析
    std::shared_ptr<CHTLJSBaseNode> parseScriptLoader();
    std::shared_ptr<CHTLJSBaseNode> parseListen();
    std::shared_ptr<CHTLJSBaseNode> parseAnimate();
    std::shared_ptr<CHTLJSBaseNode> parseRouter();
    std::shared_ptr<CHTLJSBaseNode> parseVir();
    std::shared_ptr<CHTLJSBaseNode> parseUtilThen();
    std::shared_ptr<CHTLJSBaseNode> parsePrintMylove();
    std::shared_ptr<CHTLJSBaseNode> parseINeverAway();
    
    // 表达式解析
    std::shared_ptr<CHTLJSBaseNode> parseAssignmentExpression();
    std::shared_ptr<CHTLJSBaseNode> parseLogicalOrExpression();
    std::shared_ptr<CHTLJSBaseNode> parseLogicalAndExpression();
    std::shared_ptr<CHTLJSBaseNode> parseEqualityExpression();
    std::shared_ptr<CHTLJSBaseNode> parseRelationalExpression();
    std::shared_ptr<CHTLJSBaseNode> parseAdditiveExpression();
    std::shared_ptr<CHTLJSBaseNode> parseMultiplicativeExpression();
    std::shared_ptr<CHTLJSBaseNode> parseUnaryExpression();
    std::shared_ptr<CHTLJSBaseNode> parsePostfixExpression();
    std::shared_ptr<CHTLJSBaseNode> parseCallExpression();
    std::shared_ptr<CHTLJSBaseNode> parseMemberExpression();
    
    // CHTL JS特殊表达式解析
    std::shared_ptr<CHTLJSBaseNode> parseResponsiveGet();
    std::shared_ptr<CHTLJSBaseNode> parseResponsiveSet();
    std::shared_ptr<CHTLJSBaseNode> parseCHTLSelector();
    std::shared_ptr<CHTLJSBaseNode> parseCHTLResponsive();
    std::shared_ptr<CHTLJSBaseNode> parseBindExpression();
    std::shared_ptr<CHTLJSBaseNode> parseArrowExpression();
    
    // 字面量和标识符解析
    std::shared_ptr<CHTLJSBaseNode> parseLiteral();
    std::shared_ptr<CHTLJSBaseNode> parseIdentifier();
    std::shared_ptr<CHTLJSBaseNode> parseStringLiteral();
    std::shared_ptr<CHTLJSBaseNode> parseNumberLiteral();
    std::shared_ptr<CHTLJSBaseNode> parseBooleanLiteral();
    
    // 参数和参数列表解析
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseArgumentList();
    std::vector<std::string> parseParameterList();
    
    // 对象和数组解析
    std::shared_ptr<CHTLJSBaseNode> parseObjectLiteral();
    std::shared_ptr<CHTLJSBaseNode> parseArrayLiteral();
    
    // 条件表达式解析
    std::shared_ptr<CHTLJSBaseNode> parseConditionalExpression();
    
    // 错误处理
    void error(const std::string& message);
    void error(const std::string& message, const CHTLJS_Token& token);
    void logError(const std::string& message, int line = 0, int column = 0);
    
    // 工具方法
    bool isAssignmentOperator(CHTLJS_TokenType type);
    bool isBinaryOperator(CHTLJS_TokenType type);
    bool isUnaryOperator(CHTLJS_TokenType type);
    int getOperatorPrecedence(CHTLJS_TokenType type);
    bool isRightAssociative(CHTLJS_TokenType type);
    
    // 语法验证
    bool validateStatement(const std::shared_ptr<CHTLJSBaseNode>& node);
    bool validateExpression(const std::shared_ptr<CHTLJSBaseNode>& node);
    bool validateIdentifier(const std::string& identifier);
};

} // namespace CHTLJS

#endif // CHTLJSPARSER_H