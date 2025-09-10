#include "CHTLJSParser.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace CHTL_JS {

// 运算符优先级表
const std::map<CHTLJSTokenType, int> CHTLJSParser::OPERATOR_PRECEDENCE = {
    {CHTLJSTokenType::COMMA, 1},
    {CHTLJSTokenType::ASSIGN, 2},
    {CHTLJSTokenType::TERNARY, 3},
    {CHTLJSTokenType::OR, 4},
    {CHTLJSTokenType::AND, 5},
    {CHTLJSTokenType::BITWISE_OR, 6},
    {CHTLJSTokenType::BITWISE_XOR, 7},
    {CHTLJSTokenType::BITWISE_AND, 8},
    {CHTLJSTokenType::EQUAL, 9},
    {CHTLJSTokenType::NOT_EQUAL, 9},
    {CHTLJSTokenType::LESS_THAN, 10},
    {CHTLJSTokenType::GREATER_THAN, 10},
    {CHTLJSTokenType::LESS_EQUAL, 10},
    {CHTLJSTokenType::GREATER_EQUAL, 10},
    {CHTLJSTokenType::LEFT_SHIFT, 11},
    {CHTLJSTokenType::RIGHT_SHIFT, 11},
    {CHTLJSTokenType::PLUS, 12},
    {CHTLJSTokenType::MINUS, 12},
    {CHTLJSTokenType::MULTIPLY, 13},
    {CHTLJSTokenType::DIVIDE, 13},
    {CHTLJSTokenType::MODULO, 13},
    {CHTLJSTokenType::POWER, 14},
    {CHTLJSTokenType::DOT, 15},
    {CHTLJSTokenType::ARROW, 15}
};

const std::map<CHTLJSTokenType, bool> CHTLJSParser::OPERATOR_ASSOCIATIVITY = {
    {CHTLJSTokenType::COMMA, true},
    {CHTLJSTokenType::ASSIGN, false},
    {CHTLJSTokenType::TERNARY, false},
    {CHTLJSTokenType::OR, true},
    {CHTLJSTokenType::AND, true},
    {CHTLJSTokenType::BITWISE_OR, true},
    {CHTLJSTokenType::BITWISE_XOR, true},
    {CHTLJSTokenType::BITWISE_AND, true},
    {CHTLJSTokenType::EQUAL, true},
    {CHTLJSTokenType::NOT_EQUAL, true},
    {CHTLJSTokenType::LESS_THAN, true},
    {CHTLJSTokenType::GREATER_THAN, true},
    {CHTLJSTokenType::LESS_EQUAL, true},
    {CHTLJSTokenType::GREATER_EQUAL, true},
    {CHTLJSTokenType::LEFT_SHIFT, true},
    {CHTLJSTokenType::RIGHT_SHIFT, true},
    {CHTLJSTokenType::PLUS, true},
    {CHTLJSTokenType::MINUS, true},
    {CHTLJSTokenType::MULTIPLY, true},
    {CHTLJSTokenType::DIVIDE, true},
    {CHTLJSTokenType::MODULO, true},
    {CHTLJSTokenType::POWER, false},
    {CHTLJSTokenType::DOT, true},
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
    } else if (token.getType() == CHTLJSTokenType::UTIL) {
        return parseUtilExpression();
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
    if (!matchKeyword("util")) {
        reportUnexpectedToken(getCurrentToken(), "util");
        return nullptr;
    }
    
    advance(); // 跳过 util
    
    // 解析条件表达式
    auto condition = parseExpression();
    if (!condition) {
        reportUnexpectedToken(getCurrentToken(), "condition");
        return nullptr;
    }
    
    if (!match(CHTLJSTokenType::ARROW)) {
        reportUnexpectedToken(getCurrentToken(), "->");
        return nullptr;
    }
    
    advance(); // 跳过 ->
    
    // 解析change代码块
    auto changeKeyword = getCurrentToken();
    if (changeKeyword.getValue() != "change") {
        reportUnexpectedToken(changeKeyword, "change");
        return nullptr;
    }
    
    advance(); // 跳过 change
    
    auto changeCode = parseExpression();
    if (!changeCode) {
        reportUnexpectedToken(getCurrentToken(), "change code");
        return nullptr;
    }
    
    if (!match(CHTLJSTokenType::ARROW)) {
        reportUnexpectedToken(getCurrentToken(), "->");
        return nullptr;
    }
    
    advance(); // 跳过 ->
    
    // 解析then代码块
    auto thenKeyword = getCurrentToken();
    if (thenKeyword.getValue() != "then") {
        reportUnexpectedToken(thenKeyword, "then");
        return nullptr;
    }
    
    advance(); // 跳过 then
    
    auto thenCode = parseExpression();
    if (!thenCode) {
        reportUnexpectedToken(getCurrentToken(), "then code");
        return nullptr;
    }
    
    // 创建util...then节点
    auto util = createNode(CHTLJSNodeType::UTIL_EXPRESSION);
    util->addChild(condition);
    util->addChild(changeCode);
    util->addChild(thenCode);
    
    return util;
}
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseForStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseWhileStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDoWhileStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseSwitchStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseCaseStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseDefaultStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseBreakStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseContinueStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseReturnStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseThrowStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseTryStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseCatchStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFinallyStatement() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseTernaryExpression() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseCallExpression() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseMemberExpression() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseArrayExpression() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseObjectExpression() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFunctionExpression() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseArrowFunctionExpression() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVariableDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFunctionDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseClassDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseInterfaceDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEnumDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseNamespaceDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseModuleDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseImportDeclaration() { return nullptr; }
std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExportDeclaration() { return nullptr; }

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
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::IF ||
           token.getType() == CHTLJSTokenType::FOR ||
           token.getType() == CHTLJSTokenType::WHILE ||
           token.getType() == CHTLJSTokenType::DO ||
           token.getType() == CHTLJSTokenType::SWITCH ||
           token.getType() == CHTLJSTokenType::TRY ||
           token.getType() == CHTLJSTokenType::BREAK ||
           token.getType() == CHTLJSTokenType::CONTINUE ||
           token.getType() == CHTLJSTokenType::RETURN ||
           token.getType() == CHTLJSTokenType::THROW;
}

bool CHTLJSParser::isExpressionStart() const {
    return isPrimaryExpressionStart();
}

bool CHTLJSParser::isDeclarationStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::CONST ||
           token.getType() == CHTLJSTokenType::LET ||
           token.getType() == CHTLJSTokenType::VAR ||
           token.getType() == CHTLJSTokenType::FUNCTION ||
           token.getType() == CHTLJSTokenType::CLASS;
}

bool CHTLJSParser::isCHTLJSSyntaxStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::ENHANCED_SELECTOR ||
           token.getType() == CHTLJSTokenType::VIRTUAL_OBJECT ||
           token.getType() == CHTLJSTokenType::LISTEN ||
           token.getType() == CHTLJSTokenType::DELEGATE ||
           token.getType() == CHTLJSTokenType::ANIMATE ||
           token.getType() == CHTLJSTokenType::ROUTER ||
           token.getType() == CHTLJSTokenType::FILELOADER ||
           token.getType() == CHTLJSTokenType::UTIL;
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
    return token.getType() == CHTLJSTokenType::PLUS ||
           token.getType() == CHTLJSTokenType::MINUS ||
           token.getType() == CHTLJSTokenType::MULTIPLY ||
           token.getType() == CHTLJSTokenType::DIVIDE ||
           token.getType() == CHTLJSTokenType::MODULO ||
           token.getType() == CHTLJSTokenType::POWER ||
           token.getType() == CHTLJSTokenType::EQUAL ||
           token.getType() == CHTLJSTokenType::NOT_EQUAL ||
           token.getType() == CHTLJSTokenType::LESS_THAN ||
           token.getType() == CHTLJSTokenType::GREATER_THAN ||
           token.getType() == CHTLJSTokenType::LESS_EQUAL ||
           token.getType() == CHTLJSTokenType::GREATER_EQUAL ||
           token.getType() == CHTLJSTokenType::AND ||
           token.getType() == CHTLJSTokenType::OR ||
           token.getType() == CHTLJSTokenType::DOT ||
           token.getType() == CHTLJSTokenType::ARROW;
}

bool CHTLJSParser::isUnaryExpressionStart() const {
    auto token = getCurrentToken();
    return token.getType() == CHTLJSTokenType::NOT ||
           token.getType() == CHTLJSTokenType::MINUS ||
           token.getType() == CHTLJSTokenType::PLUS ||
           token.getType() == CHTLJSTokenType::BITWISE_NOT;
}

bool CHTLJSParser::isCallExpressionStart() const {
    return false; // 简化实现
}

bool CHTLJSParser::isMemberExpressionStart() const {
    return false; // 简化实现
}

bool CHTLJSParser::isArrayExpressionStart() const {
    return match(CHTLJSTokenType::LEFT_BRACKET);
}

bool CHTLJSParser::isObjectExpressionStart() const {
    return match(CHTLJSTokenType::LEFT_BRACE);
}

bool CHTLJSParser::isFunctionExpressionStart() const {
    return matchKeyword("function");
}

bool CHTLJSParser::isArrowFunctionExpressionStart() const {
    return false; // 简化实现
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

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createBinaryExpressionNode(const std::string& operator, std::shared_ptr<CHTLJSBaseNode> left, std::shared_ptr<CHTLJSBaseNode> right) {
    auto node = createNode(CHTLJSNodeType::BINARY_EXPRESSION, "binary", operator);
    node->addChild(left);
    node->addChild(right);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::createUnaryExpressionNode(const std::string& operator, std::shared_ptr<CHTLJSBaseNode> operand) {
    auto node = createNode(CHTLJSNodeType::UNARY_EXPRESSION, "unary", operator);
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
    } else if (keyword == "util") {
        return parseUtilDeclaration();
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
        if (match(CHTLJSTokenType::QUESTION)) {
            advance(); // 跳过 ?
            skipWhitespace();
        }
        
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
        } else if (token.getType() == CHTLJSTokenType::DOT) {
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
        } else if (token.getType() == CHTLJSTokenType::DOT) {
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
    
    auto chain = createNode(CHTLJSNodeType::CHAIN_EXPRESSION);
    
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