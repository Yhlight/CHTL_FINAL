#ifndef CHTL_JS_PARSER_H
#define CHTL_JS_PARSER_H

#include "../CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTLJSLexer/CHTLJSToken.h"
#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTL_JS {

class CHTLJSParser {
private:
    std::vector<CHTLJSToken> tokens;
    size_t currentTokenIndex;
    bool debugMode;
    bool strictMode;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 解析状态
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> variables;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> functions;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> classes;
    std::map<std::string, std::shared_ptr<CHTLJSBaseNode>> modules;
    
    // 作用域栈
    std::vector<std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>> scopeStack;
    
    // 解析选项
    bool allowUnquotedLiterals;
    bool allowUnorderedKeyValuePairs;
    bool allowOptionalKeyValuePairs;
    bool allowChainSyntax;
    bool allowDeclarationSyntax;

public:
    CHTLJSParser();
    virtual ~CHTLJSParser() = default;
    
    // 基本配置
    void setTokens(const std::vector<CHTLJSToken>& tokens);
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setAllowUnquotedLiterals(bool allow) { allowUnquotedLiterals = allow; }
    void setAllowUnorderedKeyValuePairs(bool allow) { allowUnorderedKeyValuePairs = allow; }
    void setAllowOptionalKeyValuePairs(bool allow) { allowOptionalKeyValuePairs = allow; }
    void setAllowChainSyntax(bool allow) { allowChainSyntax = allow; }
    void setAllowDeclarationSyntax(bool allow) { allowDeclarationSyntax = allow; }
    
    // 主要解析方法
    std::shared_ptr<CHTLJSBaseNode> parse();
    std::shared_ptr<CHTLJSBaseNode> parseProgram();
    std::shared_ptr<CHTLJSBaseNode> parseStatement();
    std::shared_ptr<CHTLJSBaseNode> parseExpression();
    std::shared_ptr<CHTLJSBaseNode> parseDeclaration();
    
    // 语句解析
    std::shared_ptr<CHTLJSBaseNode> parseExpressionStatement();
    std::shared_ptr<CHTLJSBaseNode> parseBlockStatement();
    std::shared_ptr<CHTLJSBaseNode> parseIfStatement();
    std::shared_ptr<CHTLJSBaseNode> parseForStatement();
    std::shared_ptr<CHTLJSBaseNode> parseWhileStatement();
    std::shared_ptr<CHTLJSBaseNode> parseDoWhileStatement();
    std::shared_ptr<CHTLJSBaseNode> parseSwitchStatement();
    std::shared_ptr<CHTLJSBaseNode> parseCaseStatement();
    std::shared_ptr<CHTLJSBaseNode> parseDefaultStatement();
    std::shared_ptr<CHTLJSBaseNode> parseBreakStatement();
    std::shared_ptr<CHTLJSBaseNode> parseContinueStatement();
    std::shared_ptr<CHTLJSBaseNode> parseReturnStatement();
    std::shared_ptr<CHTLJSBaseNode> parseThrowStatement();
    std::shared_ptr<CHTLJSBaseNode> parseTryStatement();
    std::shared_ptr<CHTLJSBaseNode> parseCatchStatement();
    std::shared_ptr<CHTLJSBaseNode> parseFinallyStatement();
    
    // 表达式解析
    std::shared_ptr<CHTLJSBaseNode> parsePrimaryExpression();
    std::shared_ptr<CHTLJSBaseNode> parseBinaryExpression();
    std::shared_ptr<CHTLJSBaseNode> parseUnaryExpression();
    std::shared_ptr<CHTLJSBaseNode> parseTernaryExpression();
    std::shared_ptr<CHTLJSBaseNode> parseCallExpression();
    std::shared_ptr<CHTLJSBaseNode> parseMemberExpression();
    std::shared_ptr<CHTLJSBaseNode> parseArrayExpression();
    std::shared_ptr<CHTLJSBaseNode> parseObjectExpression();
    std::shared_ptr<CHTLJSBaseNode> parseFunctionExpression();
    std::shared_ptr<CHTLJSBaseNode> parseArrowFunctionExpression();
    
    // 声明解析
    std::shared_ptr<CHTLJSBaseNode> parseVariableDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseFunctionDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseClassDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseInterfaceDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseEnumDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseNamespaceDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseModuleDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseImportDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseExportDeclaration();
    
    // CHTL JS特有语法解析
    std::shared_ptr<CHTLJSBaseNode> parseEnhancedSelector();
    std::shared_ptr<CHTLJSBaseNode> parseResponsiveValue();
    std::shared_ptr<CHTLJSBaseNode> parseVirtualObject();
    std::shared_ptr<CHTLJSBaseNode> parseListenExpression();
    std::shared_ptr<CHTLJSBaseNode> parseDelegateExpression();
    std::shared_ptr<CHTLJSBaseNode> parseAnimateExpression();
    std::shared_ptr<CHTLJSBaseNode> parseRouterExpression();
    std::shared_ptr<CHTLJSBaseNode> parseFileloaderExpression();
    
    // 特殊语法解析
    std::shared_ptr<CHTLJSBaseNode> parseDeclarationSyntax(const std::string& keyword);
    std::shared_ptr<CHTLJSBaseNode> parseKeyValuePairs();
    std::shared_ptr<CHTLJSBaseNode> parseKeyValuePair();
    std::shared_ptr<CHTLJSBaseNode> parseUnorderedPairs();
    std::shared_ptr<CHTLJSBaseNode> parseOptionalPairs();
    std::shared_ptr<CHTLJSBaseNode> parseChainExpression();
    std::shared_ptr<CHTLJSBaseNode> parseUnquotedLiteral();
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void clearErrors();
    void clearWarnings();
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    // 状态管理
    void reset();
    void resetPosition();
    size_t getCurrentTokenIndex() const { return currentTokenIndex; }
    bool isAtEnd() const;
    
    // 调试信息
    std::string getDebugInfo() const;
    void printDebugInfo() const;
    
    // 验证
    bool validateAST(std::shared_ptr<CHTLJSBaseNode> ast) const;
    std::vector<std::string> validateNode(std::shared_ptr<CHTLJSBaseNode> node) const;

private:
    // Token处理
    CHTLJSToken getCurrentToken() const;
    CHTLJSToken getNextToken();
    CHTLJSToken peekToken() const;
    CHTLJSToken peekToken(int offset) const;
    void advance();
    void advance(int count);
    bool match(CHTLJSTokenType type) const;
    bool match(const std::vector<CHTLJSTokenType>& types) const;
    bool matchKeyword(const std::string& keyword) const;
    bool matchOperator(const std::string& op) const;
    bool matchPunctuation(char punct) const;
    
    // 语法检查
    bool isStatementStart() const;
    bool isExpressionStart() const;
    bool isDeclarationStart() const;
    bool isCHTLJSSyntaxStart() const;
    bool isPrimaryExpressionStart() const;
    bool isBinaryExpressionStart() const;
    bool isUnaryExpressionStart() const;
    bool isCallExpressionStart() const;
    bool isMemberExpressionStart() const;
    bool isArrayExpressionStart() const;
    bool isObjectExpressionStart() const;
    bool isFunctionExpressionStart() const;
    bool isArrowFunctionExpressionStart() const;
    
    // 表达式优先级处理
    std::shared_ptr<CHTLJSBaseNode> parseExpressionWithPrecedence(int precedence);
    int getOperatorPrecedence(CHTLJSTokenType type) const;
    bool isLeftAssociative(CHTLJSTokenType type) const;
    
    // 作用域管理
    void enterScope();
    void exitScope();
    void addToScope(const std::string& name, std::shared_ptr<CHTLJSBaseNode> node);
    std::shared_ptr<CHTLJSBaseNode> findInScope(const std::string& name) const;
    bool isInScope(const std::string& name) const;
    
    // 类型推断
    std::string inferType(std::shared_ptr<CHTLJSBaseNode> node) const;
    std::string inferExpressionType(std::shared_ptr<CHTLJSBaseNode> node) const;
    std::string inferStatementType(std::shared_ptr<CHTLJSBaseNode> node) const;
    std::string inferDeclarationType(std::shared_ptr<CHTLJSBaseNode> node) const;
    
    // 错误处理辅助
    void reportError(const std::string& message, const CHTLJSToken& token);
    void reportWarning(const std::string& message, const CHTLJSToken& token);
    void reportUnexpectedToken(const CHTLJSToken& token, const std::string& expected);
    void reportUnexpectedEndOfFile(const std::string& expected);
    std::string formatError(const std::string& message, const CHTLJSToken& token) const;
    
    // 调试辅助
    std::string getContext() const;
    std::string getTokenContext(const CHTLJSToken& token) const;
    void printContext() const;
    
    // 验证辅助
    bool validateNodeType(std::shared_ptr<CHTLJSBaseNode> node, CHTLJSNodeType expectedType) const;
    bool validateNodeStructure(std::shared_ptr<CHTLJSBaseNode> node) const;
    bool validateNodeSemantics(std::shared_ptr<CHTLJSBaseNode> node) const;
    
    // 解析辅助
    std::shared_ptr<CHTLJSBaseNode> createNode(CHTLJSNodeType type, const std::string& name = "", const std::string& value = "");
    std::shared_ptr<CHTLJSBaseNode> createLiteralNode(const std::string& value, const std::string& type = "string");
    std::shared_ptr<CHTLJSBaseNode> createIdentifierNode(const std::string& name);
    std::shared_ptr<CHTLJSBaseNode> createBinaryExpressionNode(const std::string& op, std::shared_ptr<CHTLJSBaseNode> left, std::shared_ptr<CHTLJSBaseNode> right);
    std::shared_ptr<CHTLJSBaseNode> createUnaryExpressionNode(const std::string& op, std::shared_ptr<CHTLJSBaseNode> operand);
    std::shared_ptr<CHTLJSBaseNode> createCallExpressionNode(std::shared_ptr<CHTLJSBaseNode> callee, std::vector<std::shared_ptr<CHTLJSBaseNode>> arguments);
    std::shared_ptr<CHTLJSBaseNode> createMemberExpressionNode(std::shared_ptr<CHTLJSBaseNode> object, std::shared_ptr<CHTLJSBaseNode> property, bool computed = false);
    
    // 特殊语法解析辅助
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseParameterList();
    std::shared_ptr<CHTLJSBaseNode> parseCHTLJSSyntax();
    void skipWhitespace();
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseArgumentList();
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseArrayElements();
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseObjectProperties();
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseKeyValuePairList();
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseUnorderedPairList();
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseOptionalPairList();
    std::vector<std::shared_ptr<CHTLJSBaseNode>> parseChainElementList();
    
    // 声明式语法解析辅助
    std::shared_ptr<CHTLJSBaseNode> parseListenDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseDelegateDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseAnimateDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseRouterDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseFileloaderDeclaration();
    std::shared_ptr<CHTLJSBaseNode> parseUtilDeclaration();
    
    // 无修饰字面量解析辅助
    bool isUnquotedLiteralStart() const;
    std::string parseUnquotedLiteralValue();
    bool isValidUnquotedLiteral(const std::string& literal) const;
    
    // 链式语法解析辅助
    bool isChainSyntaxStart() const;
    std::vector<std::string> parseChainElements();
    std::shared_ptr<CHTLJSBaseNode> createChainExpressionNode(const std::vector<std::string>& elements);
    
    // 运算符优先级表
    static const std::map<CHTLJSTokenType, int> OPERATOR_PRECEDENCE;
    static const std::map<CHTLJSTokenType, bool> OPERATOR_ASSOCIATIVITY;
};

} // namespace CHTL_JS

#endif // CHTL_JS_PARSER_H