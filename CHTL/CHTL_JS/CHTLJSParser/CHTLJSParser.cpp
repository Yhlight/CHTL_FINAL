#include "CHTLJSParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL_JS {

// 运算符优先级表
const std::map<CHTLJSTokenType, int> CHTLJSParser::OPERATOR_PRECEDENCE = {
    {CHTLJSTokenType::COMMA, 1},
    {CHTLJSTokenType::ASSIGN, 2},
    {CHTLJSTokenType::ARROW, 3}
};

const std::map<CHTLJSTokenType, bool> CHTLJSParser::OPERATOR_ASSOCIATIVITY = {
    {CHTLJSTokenType::COMMA, true},
    {CHTLJSTokenType::ASSIGN, false},
    {CHTLJSTokenType::ARROW, true}
};

CHTLJSParser::CHTLJSParser() 
    : currentTokenIndex(0), debugMode(false), strictMode(false),
      allowUnquotedLiterals(true), allowUnorderedKeyValuePairs(true),
      allowOptionalKeyValuePairs(true), allowChainSyntax(true), allowDeclarationSyntax(true) {
    enterScope(); // 创建全局作用域
}

void CHTLJSParser::setTokens(const std::vector<CHTLJSToken>& tokens) {
    this->tokens = tokens;
    reset();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parse() {
    return parseProgram();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseProgram() {
    auto program = createNode(CHTLJSNodeType::MODULE_DECLARATION, "Program");
    
    while (!isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            program->addChild(statement);
        }
    }
    
    return program;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseStatement() {
    if (isAtEnd()) {
        return nullptr;
    }
    
    if (isDeclarationStart()) {
        return parseDeclaration();
    }
    
    if (isCHTLJSSyntaxStart()) {
        return parseCHTLJSSyntax();
    }
    
    if (isStatementStart()) {
        return parseStatement();
    }
    
    if (isExpressionStart()) {
        return parseExpressionStatement();
    }
    
    reportUnexpectedToken(getCurrentToken(), "statement");
    return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExpression() {
    return parseExpressionWithPrecedence(0);
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDeclaration() {
    auto token = getCurrentToken();
    
    if (matchKeyword("const")) {
        advance();
        return parseVariableDeclaration();
    } else if (matchKeyword("let")) {
        advance();
        return parseVariableDeclaration();
    } else if (matchKeyword("var")) {
        advance();
        return parseVariableDeclaration();
    } else if (matchKeyword("function")) {
        advance();
        return parseFunctionDeclaration();
    } else if (matchKeyword("class")) {
        advance();
        return parseClassDeclaration();
    } else if (matchKeyword("interface")) {
        advance();
        return parseInterfaceDeclaration();
    } else if (matchKeyword("enum")) {
        advance();
        return parseEnumDeclaration();
    } else if (matchKeyword("namespace")) {
        advance();
        return parseNamespaceDeclaration();
    } else if (matchKeyword("module")) {
        advance();
        return parseModuleDeclaration();
    } else if (matchKeyword("import")) {
        advance();
        return parseImportDeclaration();
    } else if (matchKeyword("export")) {
        advance();
        return parseExportDeclaration();
    }
    
    reportUnexpectedToken(token, "declaration");
    return nullptr;
}

// 语句解析实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExpressionStatement() {
    auto expression = parseExpression();
    if (!expression) {
        return nullptr;
    }
    
    auto statement = createNode(CHTLJSNodeType::EXPRESSION_STATEMENT);
    statement->addChild(expression);
    
    if (match(CHTLJSTokenType::SEMICOLON)) {
        advance();
    }
    
    return statement;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseBlockStatement() {
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    auto block = createNode(CHTLJSNodeType::BLOCK_STATEMENT);
    enterScope();
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        auto statement = parseStatement();
        if (statement) {
            block->addChild(statement);
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    exitScope();
    return block;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseIfStatement() {
    if (!matchKeyword("if")) {
        reportUnexpectedToken(getCurrentToken(), "if");
        return nullptr;
    }
    
    advance(); // 跳过 if
    
    if (!match(CHTLJSTokenType::LEFT_PAREN)) {
        reportUnexpectedToken(getCurrentToken(), "(");
        return nullptr;
    }
    
    advance(); // 跳过 (
    auto condition = parseExpression();
    if (!condition) {
        reportUnexpectedToken(getCurrentToken(), "condition");
        return nullptr;
    }
    
    if (!match(CHTLJSTokenType::RIGHT_PAREN)) {
        reportUnexpectedToken(getCurrentToken(), ")");
        return nullptr;
    }
    
    advance(); // 跳过 )
    auto consequent = parseStatement();
    if (!consequent) {
        reportUnexpectedToken(getCurrentToken(), "statement");
        return nullptr;
    }
    
    auto ifStatement = createNode(CHTLJSNodeType::IF_STATEMENT);
    ifStatement->addChild(condition);
    ifStatement->addChild(consequent);
    
    if (matchKeyword("else")) {
        advance(); // 跳过 else
        auto alternate = parseStatement();
        if (alternate) {
            ifStatement->addChild(alternate);
        }
    }
    
    return ifStatement;
}

// 表达式解析实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePrimaryExpression() {
    auto token = getCurrentToken();
    
    if (token.getType() == CHTLJSTokenType::LITERAL) {
        advance();
        return createLiteralNode(token.getValue());
    } else if (token.getType() == CHTLJSTokenType::IDENTIFIER) {
        advance();
        return createIdentifierNode(token.getValue());
    } else if (token.getType() == CHTLJSTokenType::STRING) {
        advance();
        return createLiteralNode(token.getValue(), "string");
    } else if (token.getType() == CHTLJSTokenType::NUMBER) {
        advance();
        return createLiteralNode(token.getValue(), "number");
    } else if (token.getType() == CHTLJSTokenType::BOOLEAN) {
        advance();
        return createLiteralNode(token.getValue(), "boolean");
    } else if (token.getType() == CHTLJSTokenType::ENHANCED_SELECTOR) {
        return parseEnhancedSelector();
    } else if (token.getType() == CHTLJSTokenType::VIRTUAL_OBJECT) {
        return parseVirtualObject();
    } else if (token.getType() == CHTLJSTokenType::LISTEN) {
        return parseListenExpression();
    } else if (token.getType() == CHTLJSTokenType::DELEGATE) {
        return parseDelegateExpression();
    } else if (token.getType() == CHTLJSTokenType::ANIMATE) {
        return parseAnimateExpression();
    } else if (token.getType() == CHTLJSTokenType::ROUTER) {
        return parseRouterExpression();
    } else if (token.getType() == CHTLJSTokenType::FILELOADER) {
        return parseFileloaderExpression();
    } else if (match(CHTLJSTokenType::LEFT_PAREN)) {
        advance(); // 跳过 (
        auto expression = parseExpression();
        if (!match(CHTLJSTokenType::RIGHT_PAREN)) {
            reportUnexpectedToken(getCurrentToken(), ")");
            return nullptr;
        }
        advance(); // 跳过 )
        return expression;
    } else if (isArrayExpressionStart()) {
        return parseArrayExpression();
    } else if (isObjectExpressionStart()) {
        return parseObjectExpression();
    } else if (isFunctionExpressionStart()) {
        return parseFunctionExpression();
    } else if (isArrowFunctionExpressionStart()) {
        return parseArrowFunctionExpression();
    }
    
    reportUnexpectedToken(token, "primary expression");
    return nullptr;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseBinaryExpression() {
    auto left = parsePrimaryExpression();
    if (!left) {
        return nullptr;
    }
    
    while (isBinaryExpressionStart()) {
        auto token = getCurrentToken();
        advance();
        
        auto right = parsePrimaryExpression();
        if (!right) {
            reportUnexpectedToken(getCurrentToken(), "expression");
            return nullptr;
        }
        
        left = createBinaryExpressionNode(token.getValue(), left, right);
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUnaryExpression() {
    auto token = getCurrentToken();
    
    if (isUnaryExpressionStart()) {
        advance();
        auto operand = parseUnaryExpression();
        if (!operand) {
            reportUnexpectedToken(getCurrentToken(), "expression");
            return nullptr;
        }
        return createUnaryExpressionNode(token.getValue(), operand);
    }
    
    return parsePrimaryExpression();
}

// CHTL JS特有语法解析实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEnhancedSelector() {
    auto token = getCurrentToken();
    if (token.getType() != CHTLJSTokenType::ENHANCED_SELECTOR) {
        reportUnexpectedToken(token, "enhanced selector");
        return nullptr;
    }
    
    advance();
    auto selector = createNode(CHTLJSNodeType::ENHANCED_SELECTOR, "selector", token.getValue());
    return selector;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVirtualObject() {
    if (!matchKeyword("vir")) {
        reportUnexpectedToken(getCurrentToken(), "vir");
        return nullptr;
    }
    
    advance(); // 跳过 vir
    
    auto nameToken = getCurrentToken();
    if (nameToken.getType() != CHTLJSTokenType::IDENTIFIER) {
        reportUnexpectedToken(nameToken, "identifier");
        return nullptr;
    }
    
    advance(); // 跳过标识符
    
    if (!match(CHTLJSTokenType::ASSIGN)) {
        reportUnexpectedToken(getCurrentToken(), "=");
        return nullptr;
    }
    
    advance(); // 跳过 =
    
    auto value = parseExpression();
    if (!value) {
        reportUnexpectedToken(getCurrentToken(), "expression");
        return nullptr;
    }
    
    auto virtualObject = createNode(CHTLJSNodeType::VIRTUAL_OBJECT, nameToken.getValue());
    virtualObject->addChild(value);
    return virtualObject;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseListenExpression() {
    if (!matchKeyword("listen")) {
        reportUnexpectedToken(getCurrentToken(), "listen");
        return nullptr;
    }
    
    advance(); // 跳过 listen
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto listen = createNode(CHTLJSNodeType::LISTEN_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        auto eventToken = getCurrentToken();
        if (eventToken.getType() != CHTLJSTokenType::IDENTIFIER) {
            reportUnexpectedToken(eventToken, "event name");
            return nullptr;
        }
        
        advance(); // 跳过事件名
        
        if (!match(CHTLJSTokenType::COLON)) {
            reportUnexpectedToken(getCurrentToken(), ":");
            return nullptr;
        }
        
        advance(); // 跳过 :
        
        auto handler = parseExpression();
        if (!handler) {
            reportUnexpectedToken(getCurrentToken(), "handler");
            return nullptr;
        }
        
        auto eventHandler = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        eventHandler->setAttribute("event", eventToken.getValue());
        eventHandler->addChild(handler);
        listen->addChild(eventHandler);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return listen;
}

// 其他解析方法的简化实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDelegateExpression() {
    if (!matchKeyword("delegate")) {
        reportUnexpectedToken(getCurrentToken(), "delegate");
        return nullptr;
    }
    
    advance(); // 跳过 delegate
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto delegate = createNode(CHTLJSNodeType::DELEGATE_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        // 解析选择器
        auto selectorToken = getCurrentToken();
        if (selectorToken.getType() != CHTLJSTokenType::ENHANCED_SELECTOR) {
            reportUnexpectedToken(selectorToken, "enhanced selector");
            return nullptr;
        }
        
        advance(); // 跳过选择器
        
        if (!match(CHTLJSTokenType::COLON)) {
            reportUnexpectedToken(getCurrentToken(), ":");
            return nullptr;
        }
        
        advance(); // 跳过 :
        
        // 解析事件名
        auto eventToken = getCurrentToken();
        if (eventToken.getType() != CHTLJSTokenType::IDENTIFIER) {
            reportUnexpectedToken(eventToken, "event name");
            return nullptr;
        }
        
        advance(); // 跳过事件名
        
        if (!match(CHTLJSTokenType::ARROW)) {
            reportUnexpectedToken(getCurrentToken(), "->");
            return nullptr;
        }
        
        advance(); // 跳过 ->
        
        // 解析处理器
        auto handler = parseExpression();
        if (!handler) {
            reportUnexpectedToken(getCurrentToken(), "handler");
            return nullptr;
        }
        
        // 创建事件委托节点
        auto eventDelegate = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        eventDelegate->setAttribute("selector", selectorToken.getValue());
        eventDelegate->setAttribute("event", eventToken.getValue());
        eventDelegate->addChild(handler);
        delegate->addChild(eventDelegate);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return delegate;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimateExpression() {
    if (!matchKeyword("animate")) {
        reportUnexpectedToken(getCurrentToken(), "animate");
        return nullptr;
    }
    
    advance(); // 跳过 animate
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto animate = createNode(CHTLJSNodeType::ANIMATE_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        // 解析目标选择器
        auto targetToken = getCurrentToken();
        if (targetToken.getType() != CHTLJSTokenType::ENHANCED_SELECTOR) {
            reportUnexpectedToken(targetToken, "enhanced selector");
            return nullptr;
        }
        
        advance(); // 跳过选择器
        
        if (!match(CHTLJSTokenType::COLON)) {
            reportUnexpectedToken(getCurrentToken(), ":");
            return nullptr;
        }
        
        advance(); // 跳过 :
        
        // 解析动画配置
        auto config = parseUnorderedPairs();
        if (!config) {
            reportUnexpectedToken(getCurrentToken(), "animation config");
            return nullptr;
        }
        
        // 创建动画节点
        auto animation = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        animation->setAttribute("target", targetToken.getValue());
        animation->addChild(config);
        animate->addChild(animation);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return animate;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseRouterExpression() {
    if (!matchKeyword("router")) {
        reportUnexpectedToken(getCurrentToken(), "router");
        return nullptr;
    }
    
    advance(); // 跳过 router
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto router = createNode(CHTLJSNodeType::ROUTER_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        // 解析路由路径
        auto pathToken = getCurrentToken();
        if (pathToken.getType() != CHTLJSTokenType::STRING && pathToken.getType() != CHTLJSTokenType::LITERAL) {
            reportUnexpectedToken(pathToken, "route path");
            return nullptr;
        }
        
        advance(); // 跳过路径
        
        if (!match(CHTLJSTokenType::ARROW)) {
            reportUnexpectedToken(getCurrentToken(), "->");
            return nullptr;
        }
        
        advance(); // 跳过 ->
        
        // 解析处理器
        auto handler = parseExpression();
        if (!handler) {
            reportUnexpectedToken(getCurrentToken(), "handler");
            return nullptr;
        }
        
        // 创建路由节点
        auto route = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        route->setAttribute("path", pathToken.getValue());
        route->addChild(handler);
        router->addChild(route);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return router;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFileloaderExpression() {
    auto token = getCurrentToken();
    if (token.getType() != CHTLJSTokenType::FILELOADER) {
        return nullptr;
    }
    
    advance(); // 跳过 fileloader
    
    skipWhitespace();
    
    // 解析文件路径
    if (!match(CHTLJSTokenType::STRING) && !match(CHTLJSTokenType::LITERAL)) {
        addError("期望文件路径");
        return nullptr;
    }
    
    auto pathToken = getCurrentToken();
    std::string filePath = pathToken.getValue();
    advance();
    
    skipWhitespace();
    
    // 检查是否有配置对象
    std::shared_ptr<CHTLJSBaseNode> config = nullptr;
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        config = parseUnorderedPairs();
    }
    
    // 创建fileloader节点
    auto fileloader = createNode(CHTLJSNodeType::CALL_EXPRESSION, "fileloader");
    fileloader->setAttribute("path", filePath);
    
    if (config) {
        fileloader->addChild(config);
    }
    
    return fileloader;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUtilExpression() {
    // util...then表达式属于Chtholly模块，不是CHTL JS核心功能
    return nullptr;
}
// 这些方法不属于CHTL JS核心功能，由JS编译器处理
// CHTL JS只支持特定的声明式语法：fileloader, listen, delegate, animate, router, vir

// CHTL JS特有的成员表达式解析（支持->运算符）
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseMemberExpression() {
    auto left = parsePrimaryExpression();
    if (!left) {
        return nullptr;
    }
    
    while (match(CHTLJSTokenType::ARROW)) {
        auto token = getCurrentToken();
        advance();
        
        if (!match(CHTLJSTokenType::IDENTIFIER) && !match(CHTLJSTokenType::LITERAL)) {
            reportUnexpectedToken(getCurrentToken(), "identifier or literal");
            return nullptr;
        }
        
        auto property = parsePrimaryExpression();
        if (!property) {
            return nullptr;
        }
        
        auto member = createNode(CHTLJSNodeType::MEMBER_EXPRESSION);
        member->setValue("->");
        member->addChild(left);
        member->addChild(property);
        
        left = member;
    }
    
    return left;
}

// CHTL JS特有的调用表达式解析
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseCallExpression() {
    auto callee = parseMemberExpression();
    if (!callee) {
        return nullptr;
    }
    
    if (match(CHTLJSTokenType::LEFT_PAREN)) {
        advance(); // 跳过 (
        
        auto call = createNode(CHTLJSNodeType::CALL_EXPRESSION);
        call->addChild(callee);
        
        // 解析参数
        if (!match(CHTLJSTokenType::RIGHT_PAREN)) {
            while (true) {
                auto arg = parseExpression();
                if (arg) {
                    call->addChild(arg);
                }
                
                if (match(CHTLJSTokenType::COMMA)) {
                    advance(); // 跳过逗号
                } else {
                    break;
                }
            }
        }
        
        if (!match(CHTLJSTokenType::RIGHT_PAREN)) {
            reportUnexpectedToken(getCurrentToken(), ")");
            return nullptr;
        }
        
        advance(); // 跳过 )
        return call;
    }
    
    return callee;
}

// Token处理实现
CHTLJSToken CHTLJSParser::getCurrentToken() const {
    return currentTokenIndex < tokens.size() ? tokens[currentTokenIndex] : CHTLJSToken();
}

CHTLJSToken CHTLJSParser::getNextToken() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex++];
    }
    return CHTLJSToken();
}

CHTLJSToken CHTLJSParser::peekToken() const {
    return peekToken(1);
}

CHTLJSToken CHTLJSParser::peekToken(int offset) const {
    size_t index = currentTokenIndex + offset;
    return index < tokens.size() ? tokens[index] : CHTLJSToken();
}

void CHTLJSParser::advance() {
    if (currentTokenIndex < tokens.size()) {
        currentTokenIndex++;
    }
}

void CHTLJSParser::advance(int count) {
    for (int i = 0; i < count; i++) {
        advance();
    }
}

bool CHTLJSParser::match(CHTLJSTokenType type) const {
    return getCurrentToken().getType() == type;
}

bool CHTLJSParser::match(const std::vector<CHTLJSTokenType>& types) const {
    auto currentType = getCurrentToken().getType();
    return std::find(types.begin(), types.end(), currentType) != types.end();
}

bool CHTLJSParser::matchKeyword(const std::string& keyword) const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::IDENTIFIER && token.getValue() == keyword;
}

bool CHTLJSParser::matchOperator(const std::string& op) const {
    auto token = getCurrentToken();
    return token.getValue() == op;
}

bool CHTLJSParser::matchPunctuation(char punct) const {
    auto token = getCurrentToken();
    return token.getValue() == std::string(1, punct);
}

// 语法检查实现
bool CHTLJSParser::isStatementStart() const {
    // CHTL JS不支持传统语句，这些由JS编译器处理
    return false;
}

bool CHTLJSParser::isExpressionStart() const {
    return isPrimaryExpressionStart();
}

bool CHTLJSParser::isDeclarationStart() const {
    // CHTL JS不支持传统声明，这些由JS编译器处理
    return false;
}

bool CHTLJSParser::isCHTLJSSyntaxStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::ENHANCED_SELECTOR ||
           token.getType() == CHTLJSTokenType::VIRTUAL_OBJECT ||
           token.getType() == CHTLJSTokenType::LISTEN ||
           token.getType() == CHTLJSTokenType::DELEGATE ||
           token.getType() == CHTLJSTokenType::ANIMATE ||
           token.getType() == CHTLJSTokenType::ROUTER ||
           token.getType() == CHTLJSTokenType::FILELOADER;
}

bool CHTLJSParser::isPrimaryExpressionStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::LITERAL ||
           token.getType() == CHTLJSTokenType::IDENTIFIER ||
           token.getType() == CHTLJSTokenType::STRING ||
           token.getType() == CHTLJSTokenType::NUMBER ||
           token.getType() == CHTLJSTokenType::BOOLEAN ||
           token.getType() == CHTLJSTokenType::ENHANCED_SELECTOR ||
           token.getType() == CHTLJSTokenType::LEFT_PAREN ||
           isArrayExpressionStart() ||
           isObjectExpressionStart() ||
           isFunctionExpressionStart();
}

bool CHTLJSParser::isBinaryExpressionStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::ARROW;
}

bool CHTLJSParser::isUnaryExpressionStart() const {
    // CHTL JS不支持一元运算符，这些由JS编译器处理
    return false;
}

bool CHTLJSParser::isCallExpressionStart() const {
    // CHTL JS中调用表达式以标识符、字面量或增强选择器开始
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::IDENTIFIER ||
           token.getType() == CHTLJSTokenType::LITERAL ||
           token.getType() == CHTLJSTokenType::ENHANCED_SELECTOR;
}

bool CHTLJSParser::isMemberExpressionStart() const {
    // CHTL JS中成员表达式以标识符、字面量或增强选择器开始
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::IDENTIFIER ||
           token.getType() == CHTLJSTokenType::LITERAL ||
           token.getType() == CHTLJSTokenType::ENHANCED_SELECTOR;
}

bool CHTLJSParser::isArrayExpressionStart() const {
    // CHTL JS不支持数组表达式，由JS编译器处理
    return false;
}

bool CHTLJSParser::isObjectExpressionStart() const {
    // CHTL JS中对象表达式以{开始，用于无序键值对
    return match(CHTLJSTokenType::LEFT_BRACE);
}

bool CHTLJSParser::isFunctionExpressionStart() const {
    // CHTL JS不支持传统函数表达式，由JS编译器处理
    return false;
}

bool CHTLJSParser::isArrowFunctionExpressionStart() const {
    // CHTL JS不支持箭头函数表达式，由JS编译器处理
    return false;
}

// 表达式优先级处理实现
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExpressionWithPrecedence(int precedence) {
    auto left = parseUnaryExpression();
    if (!left) {
        return nullptr;
    }
    
    while (isBinaryExpressionStart()) {
        auto token = getCurrentToken();
        int tokenPrecedence = getOperatorPrecedence(token.getType());
        
        if (tokenPrecedence < precedence) {
            break;
        }
        
        advance();
        
        int nextPrecedence = isLeftAssociative(token.getType()) ? tokenPrecedence + 1 : tokenPrecedence;
        auto right = parseExpressionWithPrecedence(nextPrecedence);
        
        if (!right) {
            reportUnexpectedToken(getCurrentToken(), "expression");
            return nullptr;
        }
        
        left = createBinaryExpressionNode(token.getValue(), left, right);
    }
    
    return left;
}

int CHTLJSParser::getOperatorPrecedence(CHTLJSTokenType type) const {
    auto it = OPERATOR_PRECEDENCE.find(type);
    return it != OPERATOR_PRECEDENCE.end() ? it->second : 0;
}

bool CHTLJSParser::isLeftAssociative(CHTLJSTokenType type) const {
    auto it = OPERATOR_ASSOCIATIVITY.find(type);
    return it != OPERATOR_ASSOCIATIVITY.end() ? it->second : true;
}

// 作用域管理实现
void CHTLJSParser::enterScope() {
    scopeStack.push_back(std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>());
}

void CHTLJSParser::exitScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop_back();
    }
}

void CHTLJSParser::addToScope(const std::string& name, std::shared_ptr<CHTLJSBaseNode> node) {
    if (!scopeStack.empty()) {
        scopeStack.back()[name] = node;
    }
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::findInScope(const std::string& name) const {
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
        auto nodeIt = it->find(name);
        if (nodeIt != it->end()) {
            return nodeIt->second;
        }
    }
    return nullptr;
}

bool CHTLJSParser::isInScope(const std::string& name) const {
    return findInScope(name) != nullptr;
}

// 错误处理实现
void CHTLJSParser::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLJSParser::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLJSParser::clearErrors() {
    errors.clear();
}

void CHTLJSParser::clearWarnings() {
    warnings.clear();
}

void CHTLJSParser::reset() {
    currentTokenIndex = 0;
    clearErrors();
    clearWarnings();
    scopeStack.clear();
    enterScope();
}

void CHTLJSParser::resetPosition() {
    currentTokenIndex = 0;
}

bool CHTLJSParser::isAtEnd() const {
    return currentTokenIndex >= tokens.size();
}

std::string CHTLJSParser::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLJSParser Debug Info:\n";
    oss << "Current Token Index: " << currentTokenIndex << "\n";
    oss << "Total Tokens: " << tokens.size() << "\n";
    oss << "Debug Mode: " << (debugMode ? "ON" : "OFF") << "\n";
    oss << "Strict Mode: " << (strictMode ? "ON" : "OFF") << "\n";
    oss << "Errors: " << errors.size() << "\n";
    oss << "Warnings: " << warnings.size() << "\n";
    return oss.str();
}

void CHTLJSParser::printDebugInfo() const {
    std::cout << getDebugInfo() << std::endl;
}

// 验证实现
bool CHTLJSParser::validateAST(std::shared_ptr<CHTLJSBaseNode> ast) const {
    return ast && ast->isValid();
}

std::vector<std::string> CHTLJSParser::validateNode(std::shared_ptr<CHTLJSBaseNode> node) const {
    return node ? node->validate() : std::vector<std::string>{"Node is null"};
}

// 辅助方法实现
void CHTLJSParser::reportError(const std::string& message, const CHTLJSToken& token) {
    addError(formatError(message, token));
}

void CHTLJSParser::reportWarning(const std::string& message, const CHTLJSToken& token) {
    addWarning(formatError(message, token));
}

void CHTLJSParser::reportUnexpectedToken(const CHTLJSToken& token, const std::string& expected) {
    std::string message = "Unexpected token '" + token.getValue() + "', expected " + expected;
    reportError(message, token);
}

void CHTLJSParser::reportUnexpectedEndOfFile(const std::string& expected) {
    addError("Unexpected end of file, expected " + expected);
}

std::string CHTLJSParser::formatError(const std::string& message, const CHTLJSToken& token) const {
    std::ostringstream oss;
    oss << "Error at line " << token.getLine() << ", column " << token.getColumn() << ": " << message;
    return oss.str();
}

std::string CHTLJSParser::getContext() const {
    return "Current token: " + getCurrentToken().toDebugString();
}

std::string CHTLJSParser::getTokenContext(const CHTLJSToken& token) const {
    return token.toDebugString();
}

void CHTLJSParser::printContext() const {
    std::cout << getContext() << std::endl;
}

// 创建节点辅助方法
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createNode(CHTLJSNodeType type, const std::string& name, const std::string& value) {
    auto node = std::make_shared<CHTLJSBaseNode>(type, name, value);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createLiteralNode(const std::string& value, const std::string& type) {
    auto node = createNode(CHTLJSNodeType::LITERAL, "literal", value);
    node->setAttribute("literalType", type);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createIdentifierNode(const std::string& name) {
    return createNode(CHTLJSNodeType::IDENTIFIER, name);
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createBinaryExpressionNode(const std::string& op, std::shared_ptr<CHTLJSBaseNode> left, std::shared_ptr<CHTLJSBaseNode> right) {
    auto node = createNode(CHTLJSNodeType::BINARY_EXPRESSION, "binary", op);
    node->addChild(left);
    node->addChild(right);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createUnaryExpressionNode(const std::string& op, std::shared_ptr<CHTLJSBaseNode> operand) {
    auto node = createNode(CHTLJSNodeType::UNARY_EXPRESSION, "unary", op);
    node->addChild(operand);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createCallExpressionNode(std::shared_ptr<CHTLJSBaseNode> callee, std::vector<std::shared_ptr<CHTLJSBaseNode>> arguments) {
    auto node = createNode(CHTLJSNodeType::CALL_EXPRESSION, "call");
    node->addChild(callee);
    for (auto& arg : arguments) {
        node->addChild(arg);
    }
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createMemberExpressionNode(std::shared_ptr<CHTLJSBaseNode> object, std::shared_ptr<CHTLJSBaseNode> property, bool computed) {
    auto node = createNode(CHTLJSNodeType::MEMBER_EXPRESSION, "member");
    node->setAttribute("computed", computed ? "true" : "false");
    node->addChild(object);
    node->addChild(property);
    return node;
}

// 其他未实现的方法
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseCHTLJSSyntax() { return nullptr; }

// Minimal stubs for missing declarations to satisfy linker
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVariableDeclaration() {
    return createNode(CHTLJSNodeType::DECLARATION, "var");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFunctionDeclaration() {
    return createNode(CHTLJSNodeType::FUNCTION_DECLARATION, "function");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseClassDeclaration() {
    return createNode(CHTLJSNodeType::CLASS_DECLARATION, "class");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseInterfaceDeclaration() {
    return createNode(CHTLJSNodeType::INTERFACE_DECLARATION, "interface");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEnumDeclaration() {
    return createNode(CHTLJSNodeType::ENUM_DECLARATION, "enum");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseNamespaceDeclaration() {
    return createNode(CHTLJSNodeType::NAMESPACE_DECLARATION, "namespace");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseModuleDeclaration() {
    return createNode(CHTLJSNodeType::MODULE_DECLARATION, "module");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseImportDeclaration() {
    return createNode(CHTLJSNodeType::IMPORT_DECLARATION, "import");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExportDeclaration() {
    return createNode(CHTLJSNodeType::EXPORT_DECLARATION, "export");
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseObjectExpression() {
    return createNode(CHTLJSNodeType::OBJECT_EXPRESSION, "object");
}

void CHTLJSParser::skipWhitespace() {
    while (currentTokenIndex < tokens.size() && 
           (tokens[currentTokenIndex].getType() == CHTLJSTokenType::SINGLE_COMMENT ||
            tokens[currentTokenIndex].getType() == CHTLJSTokenType::MULTI_COMMENT)) {
        advance();
    }
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDeclarationSyntax(const std::string& keyword) {
    // CHTL JS只支持特定的声明式语法
    if (keyword == "listen") {
        return parseListenDeclaration();
    } else if (keyword == "delegate") {
        return parseDelegateDeclaration();
    } else if (keyword == "animate") {
        return parseAnimateDeclaration();
    } else if (keyword == "router") {
        return parseRouterDeclaration();
    } else if (keyword == "fileloader") {
        return parseFileloaderDeclaration();
    }
    
    return nullptr;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseKeyValuePairs() {
    auto pairs = parseKeyValuePairList();
    if (pairs.empty()) {
        return nullptr;
    }
    
    auto keyValuePairs = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
    for (const auto& pair : pairs) {
        keyValuePairs->addChild(pair);
    }
    
    return keyValuePairs;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUnorderedPairs() {
    auto pairs = parseUnorderedPairList();
    if (pairs.empty()) {
        return nullptr;
    }
    
    auto unorderedPairs = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
    for (const auto& pair : pairs) {
        unorderedPairs->addChild(pair);
    }
    
    return unorderedPairs;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseOptionalPairs() {
    auto pairs = parseOptionalPairList();
    if (pairs.empty()) {
        return nullptr;
    }
    
    auto optionalPairs = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
    for (const auto& pair : pairs) {
        optionalPairs->addChild(pair);
    }
    
    return optionalPairs;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseChainExpression() {
    // CHTL JS的链式访问通过->运算符实现，在parseMemberExpression中处理
    return nullptr;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUnquotedLiteral() {
    if (!isUnquotedLiteralStart()) {
        return nullptr;
    }
    
    auto value = parseUnquotedLiteralValue();
    if (value.empty()) {
        return nullptr;
    }
    
    return createNode(CHTLJSNodeType::LITERAL, value);
}
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseParameterList() { 
    // CHTL JS不支持参数列表，这些是JS的功能
    return {}; 
}
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseArgumentList() { 
    // CHTL JS不支持参数列表，这些是JS的功能
    return {}; 
}
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseArrayElements() { 
    // CHTL JS不支持数组，这些是JS的功能
    return {}; 
}
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseObjectProperties() { 
    // CHTL JS的对象属性解析由parseUnorderedPairList处理
    return parseUnorderedPairList(); 
}
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseKeyValuePairList() {
    std::vector<std::shared_ptr<CHTLJSBaseNode>> pairs;
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        return pairs;
    }
    
    advance(); // 跳过 {
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        skipWhitespace();
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            break;
        }
        
        auto pair = parseKeyValuePair();
        if (pair) {
            pairs.push_back(pair);
        }
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过逗号
        }
        
        skipWhitespace();
    }
    
    if (match(CHTLJSTokenType::RIGHT_BRACE)) {
        advance(); // 跳过 }
    }
    
    return pairs;
}
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseUnorderedPairList() {
    std::vector<std::shared_ptr<CHTLJSBaseNode>> pairs;
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        return pairs;
    }
    
    advance(); // 跳过 {
    
    while (!match(CHTLJSTokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            break;
        }
        
        // 解析键值对
        auto pair = parseKeyValuePair();
        if (pair) {
            pairs.push_back(pair);
        }
        
        // 检查是否有逗号分隔符
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过逗号
        }
        
        skipWhitespace();
    }
    
    if (match(CHTLJSTokenType::RIGHT_BRACE)) {
        advance(); // 跳过 }
    }
    
    return pairs;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseKeyValuePair() {
    // 解析键
    auto key = parsePrimaryExpression();
    if (!key) {
        return nullptr;
    }
    
    skipWhitespace();
    
    // 检查是否有冒号分隔符
    if (!match(CHTLJSTokenType::COLON)) {
        // 如果没有冒号，可能是无修饰字面量
        return key;
    }
    
    advance(); // 跳过冒号
    skipWhitespace();
    
    // 解析值
    auto value = parsePrimaryExpression();
    if (!value) {
        return nullptr;
    }
    
    // 创建键值对节点
    auto pair = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
    pair->setAttribute("key", key->getValue());
    pair->addChild(value);
    
    return pair;
}

std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseOptionalPairList() {
    std::vector<std::shared_ptr<CHTLJSBaseNode>> pairs;
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        return pairs;
    }
    
    advance(); // 跳过 {
    
    while (!match(CHTLJSTokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipWhitespace();
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            break;
        }
        
        // 检查是否是可选的键值对（以?开头）
        // CHTL JS不支持可选键值对，这些由JS编译器处理
        
        // 解析键值对
        auto pair = parseKeyValuePair();
        if (pair) {
            pair->setAttribute("optional", "true");
            pairs.push_back(pair);
        }
        
        // 检查是否有逗号分隔符
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过逗号
        }
        
        skipWhitespace();
    }
    
    if (match(CHTLJSTokenType::RIGHT_BRACE)) {
        advance(); // 跳过 }
    }
    
    return pairs;
}
std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseChainElementList() {
    std::vector<std::shared_ptr<CHTLJSBaseNode>> elements;
    
    while (!isAtEnd()) {
        auto token = getCurrentToken();
        
        if (token.getType() == CHTLJSTokenType::IDENTIFIER || 
            token.getType() == CHTLJSTokenType::LITERAL) {
            auto element = createNode(CHTLJSNodeType::IDENTIFIER, token.getValue());
            elements.push_back(element);
            advance();
        } else if (token.getType() == CHTLJSTokenType::ARROW) {
            advance(); // 跳过点号
        } else {
            break;
        }
    }
    
    return elements;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseListenDeclaration() {
    if (!matchKeyword("listen")) {
        reportUnexpectedToken(getCurrentToken(), "listen");
        return nullptr;
    }
    
    advance(); // 跳过 listen
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto listen = createNode(CHTLJSNodeType::LISTEN_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        auto eventToken = getCurrentToken();
        if (eventToken.getType() != CHTLJSTokenType::IDENTIFIER) {
            reportUnexpectedToken(eventToken, "event name");
            return nullptr;
        }
        
        advance(); // 跳过事件名
        
        if (!match(CHTLJSTokenType::COLON)) {
            reportUnexpectedToken(getCurrentToken(), ":");
            return nullptr;
        }
        
        advance(); // 跳过 :
        
        auto handler = parseExpression();
        if (!handler) {
            reportUnexpectedToken(getCurrentToken(), "handler");
            return nullptr;
        }
        
        auto eventHandler = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        eventHandler->setAttribute("event", eventToken.getValue());
        eventHandler->addChild(handler);
        listen->addChild(eventHandler);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return listen;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDelegateDeclaration() {
    if (!matchKeyword("delegate")) {
        reportUnexpectedToken(getCurrentToken(), "delegate");
        return nullptr;
    }
    
    advance(); // 跳过 delegate
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto delegate = createNode(CHTLJSNodeType::DELEGATE_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        // 解析选择器
        auto selectorToken = getCurrentToken();
        if (selectorToken.getType() != CHTLJSTokenType::ENHANCED_SELECTOR) {
            reportUnexpectedToken(selectorToken, "enhanced selector");
            return nullptr;
        }
        
        advance(); // 跳过选择器
        
        if (!match(CHTLJSTokenType::COLON)) {
            reportUnexpectedToken(getCurrentToken(), ":");
            return nullptr;
        }
        
        advance(); // 跳过 :
        
        // 解析事件名
        auto eventToken = getCurrentToken();
        if (eventToken.getType() != CHTLJSTokenType::IDENTIFIER) {
            reportUnexpectedToken(eventToken, "event name");
            return nullptr;
        }
        
        advance(); // 跳过事件名
        
        if (!match(CHTLJSTokenType::ARROW)) {
            reportUnexpectedToken(getCurrentToken(), "->");
            return nullptr;
        }
        
        advance(); // 跳过 ->
        
        // 解析处理器
        auto handler = parseExpression();
        if (!handler) {
            reportUnexpectedToken(getCurrentToken(), "handler");
            return nullptr;
        }
        
        // 创建事件委托节点
        auto eventDelegate = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        eventDelegate->setAttribute("selector", selectorToken.getValue());
        eventDelegate->setAttribute("event", eventToken.getValue());
        eventDelegate->addChild(handler);
        delegate->addChild(eventDelegate);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return delegate;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimateDeclaration() {
    if (!matchKeyword("animate")) {
        reportUnexpectedToken(getCurrentToken(), "animate");
        return nullptr;
    }
    
    advance(); // 跳过 animate
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto animate = createNode(CHTLJSNodeType::ANIMATE_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        // 解析目标选择器
        auto targetToken = getCurrentToken();
        if (targetToken.getType() != CHTLJSTokenType::ENHANCED_SELECTOR) {
            reportUnexpectedToken(targetToken, "enhanced selector");
            return nullptr;
        }
        
        advance(); // 跳过选择器
        
        if (!match(CHTLJSTokenType::COLON)) {
            reportUnexpectedToken(getCurrentToken(), ":");
            return nullptr;
        }
        
        advance(); // 跳过 :
        
        // 解析动画配置
        auto config = parseUnorderedPairs();
        if (!config) {
            reportUnexpectedToken(getCurrentToken(), "animation config");
            return nullptr;
        }
        
        // 创建动画节点
        auto animation = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        animation->setAttribute("target", targetToken.getValue());
        animation->addChild(config);
        animate->addChild(animation);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return animate;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseRouterDeclaration() {
    if (!matchKeyword("router")) {
        reportUnexpectedToken(getCurrentToken(), "router");
        return nullptr;
    }
    
    advance(); // 跳过 router
    
    if (!match(CHTLJSTokenType::LEFT_BRACE)) {
        reportUnexpectedToken(getCurrentToken(), "{");
        return nullptr;
    }
    
    advance(); // 跳过 {
    
    auto router = createNode(CHTLJSNodeType::ROUTER_EXPRESSION);
    
    while (!isAtEnd() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        // 解析路由路径
        auto pathToken = getCurrentToken();
        if (pathToken.getType() != CHTLJSTokenType::STRING && pathToken.getType() != CHTLJSTokenType::LITERAL) {
            reportUnexpectedToken(pathToken, "route path");
            return nullptr;
        }
        
        advance(); // 跳过路径
        
        if (!match(CHTLJSTokenType::ARROW)) {
            reportUnexpectedToken(getCurrentToken(), "->");
            return nullptr;
        }
        
        advance(); // 跳过 ->
        
        // 解析处理器
        auto handler = parseExpression();
        if (!handler) {
            reportUnexpectedToken(getCurrentToken(), "handler");
            return nullptr;
        }
        
        // 创建路由节点
        auto route = createNode(CHTLJSNodeType::OBJECT_EXPRESSION);
        route->setAttribute("path", pathToken.getValue());
        route->addChild(handler);
        router->addChild(route);
        
        if (match(CHTLJSTokenType::COMMA)) {
            advance(); // 跳过 ,
        }
    }
    
    if (!match(CHTLJSTokenType::RIGHT_BRACE)) {
        reportUnexpectedEndOfFile("}");
        return nullptr;
    }
    
    advance(); // 跳过 }
    return router;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFileloaderDeclaration() {
    if (!matchKeyword("fileloader")) {
        reportUnexpectedToken(getCurrentToken(), "fileloader");
        return nullptr;
    }
    
    advance(); // 跳过 fileloader
    
    // 解析文件路径
    if (!match(CHTLJSTokenType::STRING) && !match(CHTLJSTokenType::LITERAL)) {
        addError("期望文件路径");
        return nullptr;
    }
    
    auto pathToken = getCurrentToken();
    std::string filePath = pathToken.getValue();
    advance();
    
    skipWhitespace();
    
    // 检查是否有配置对象
    std::shared_ptr<CHTLJSBaseNode> config = nullptr;
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        config = parseUnorderedPairs();
    }
    
    // 创建fileloader节点
    auto fileloader = createNode(CHTLJSNodeType::CALL_EXPRESSION, "fileloader");
    fileloader->setAttribute("path", filePath);
    
    if (config) {
        fileloader->addChild(config);
    }
    
    return fileloader;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUtilDeclaration() { return nullptr; }
bool CHTLJSParser::isUnquotedLiteralStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::LITERAL;
}

std::string CHTLJSParser::parseUnquotedLiteralValue() {
    auto token = getCurrentToken();
    if (token.getType() == CHTLJSTokenType::LITERAL) {
        advance();
        return token.getValue();
    }
    return "";
}

bool CHTLJSParser::isValidUnquotedLiteral(const std::string& literal) const {
    // CHTL JS的无修饰字面量验证
    return !literal.empty();
}
bool CHTLJSParser::isChainSyntaxStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::IDENTIFIER || 
           token.getType() == CHTLJSTokenType::LITERAL;
}
std::vector<std::string> CHTLJSParser::parseChainElements() {
    std::vector<std::string> elements;
    
    while (!isAtEnd()) {
        auto token = getCurrentToken();
        
        if (token.getType() == CHTLJSTokenType::IDENTIFIER || 
            token.getType() == CHTLJSTokenType::LITERAL) {
            elements.push_back(token.getValue());
            advance();
        } else if (token.getType() == CHTLJSTokenType::ARROW) {
            advance(); // 跳过点号
        } else {
            break;
        }
    }
    
    return elements;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createChainExpressionNode(const std::vector<std::string>& elements) {
    if (elements.empty()) {
        return nullptr;
    }
    
    auto chain = createNode(CHTLJSNodeType::CALL_EXPRESSION);
    
    for (const auto& element : elements) {
        auto elementNode = createNode(CHTLJSNodeType::IDENTIFIER, element);
        chain->addChild(elementNode);
    }
    
    return chain;
}
std::string CHTLJSParser::inferType(std::shared_ptr<CHTLJSBaseNode> node) const { return "unknown"; }
std::string CHTLJSParser::inferExpressionType(std::shared_ptr<CHTLJSBaseNode> node) const { return "unknown"; }
std::string CHTLJSParser::inferStatementType(std::shared_ptr<CHTLJSBaseNode> node) const { return "unknown"; }
std::string CHTLJSParser::inferDeclarationType(std::shared_ptr<CHTLJSBaseNode> node) const { return "unknown"; }
bool CHTLJSParser::validateNodeType(std::shared_ptr<CHTLJSBaseNode> node, CHTLJSNodeType expectedType) const { return true; }
bool CHTLJSParser::validateNodeStructure(std::shared_ptr<CHTLJSBaseNode> node) const { return true; }
bool CHTLJSParser::validateNodeSemantics(std::shared_ptr<CHTLJSBaseNode> node) const { return true; }

} // namespace CHTL_JS