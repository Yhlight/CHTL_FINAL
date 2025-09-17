#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSSpecialNode.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace CHTLJS {

// 继续实现剩余的CHTL JS解析器方法

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseINeverAway() {
    consume(CHTLJS_TokenType::INEVERAWAY);
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::INEVERAWAY);
    
    // 解析参数
    while (hasMoreTokens() && !match(CHTLJS_TokenType::RIGHT_BRACE)) {
        if (match(CHTLJS_TokenType::IDENTIFIER)) {
            CHTLJS_Token keyToken = consume(CHTLJS_TokenType::IDENTIFIER);
            consume(CHTLJS_TokenType::COLON);
            
            if (match(CHTLJS_TokenType::STRING_LITERAL)) {
                CHTLJS_Token valueToken = consume(CHTLJS_TokenType::STRING_LITERAL);
                node->setAttribute(keyToken.value, valueToken.value);
            } else if (match(CHTLJS_TokenType::IDENTIFIER)) {
                CHTLJS_Token valueToken = consume(CHTLJS_TokenType::IDENTIFIER);
                node->setAttribute(keyToken.value, valueToken.value);
            } else if (match(CHTLJS_TokenType::LEFT_BRACE)) {
                auto blockValue = parseBlockStatement();
                node->addChild(blockValue);
            }
            
            if (match(CHTLJS_TokenType::COMMA)) {
                consume(CHTLJS_TokenType::COMMA);
            }
        } else {
            advance();
        }
    }
    
    consume(CHTLJS_TokenType::RIGHT_BRACE);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUtilThen() {
    consume(CHTLJS_TokenType::UTIL);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::UTIL_THEN);
    
    // 解析util表达式
    auto expression = parseExpression();
    if (expression) {
        node->addChild(expression);
    }
    
    // 解析 -> change
    if (match(CHTLJS_TokenType::ARROW_OPERATOR)) {
        consume(CHTLJS_TokenType::ARROW_OPERATOR);
        consume(CHTLJS_TokenType::CHANGE); // change
    }
    
    // 解析change块
    if (match(CHTLJS_TokenType::LEFT_BRACE)) {
        auto changeBlock = parseBlockStatement();
        if (changeBlock) {
            changeBlock->setAttribute("type", "change");
            node->addChild(changeBlock);
        }
    }
    
    // 解析 -> then
    if (match(CHTLJS_TokenType::ARROW_OPERATOR)) {
        consume(CHTLJS_TokenType::ARROW_OPERATOR);
        consume(CHTLJS_TokenType::THEN); // then
    }
    
    // 解析then块
    if (match(CHTLJS_TokenType::LEFT_BRACE)) {
        auto thenBlock = parseBlockStatement();
        if (thenBlock) {
            thenBlock->setAttribute("type", "then");
            node->addChild(thenBlock);
        }
    }
    
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePrintMylove() {
    consume(CHTLJS_TokenType::PRINTMYLOVE);
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::PRINTMYLOVE);
    
    // 解析参数
    while (hasMoreTokens() && !match(CHTLJS_TokenType::RIGHT_BRACE)) {
        if (match(CHTLJS_TokenType::IDENTIFIER)) {
            CHTLJS_Token keyToken = consume(CHTLJS_TokenType::IDENTIFIER);
            consume(CHTLJS_TokenType::COLON);
            
            if (match(CHTLJS_TokenType::STRING_LITERAL)) {
                CHTLJS_Token valueToken = consume(CHTLJS_TokenType::STRING_LITERAL);
                node->setAttribute(keyToken.value, valueToken.value);
            } else if (match(CHTLJS_TokenType::IDENTIFIER)) {
                CHTLJS_Token valueToken = consume(CHTLJS_TokenType::IDENTIFIER);
                node->setAttribute(keyToken.value, valueToken.value);
            }
            
            if (match(CHTLJS_TokenType::COMMA)) {
                consume(CHTLJS_TokenType::COMMA);
            }
        } else {
            advance();
        }
    }
    
    consume(CHTLJS_TokenType::RIGHT_BRACE);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseResponsiveGet() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::RESPONSIVE_GET);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::RESPONSIVE_GET);
    node->setAttribute("variable", token.value);
    
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseResponsiveSet() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::RESPONSIVE_SET);
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::RESPONSIVE_SET);
    node->setAttribute("variable", token.value);
    
    // 解析赋值表达式
    auto value = parseExpression();
    if (value) {
        node->addChild(value);
    }
    
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseCHTLSelector() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::CHTL_SELECTOR);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::CHTL_SELECTOR);
    node->setAttribute("selector", token.value);
    
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseCHTLResponsive() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::CHTL_RESPONSIVE);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::CHTL_RESPONSIVE);
    node->setAttribute("variable", token.value);
    
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseBlockStatement() {
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto block = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BLOCK_STATEMENT);
    
    while (hasMoreTokens() && !match(CHTLJS_TokenType::RIGHT_BRACE)) {
        auto statement = parseStatement();
        if (statement) {
            block->addChild(statement);
        }
    }
    
    consume(CHTLJS_TokenType::RIGHT_BRACE);
    return block;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExpressionStatement() {
    auto expr = parseExpression();
    
    if (match(CHTLJS_TokenType::SEMICOLON)) {
        consume(CHTLJS_TokenType::SEMICOLON);
    }
    
    auto statement = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::EXPRESSION_STATEMENT);
    if (expr) {
        statement->addChild(expr);
    }
    
    return statement;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVariableDeclaration() {
    CHTLJS_Token nameToken = consume(CHTLJS_TokenType::IDENTIFIER);
    consume(CHTLJS_TokenType::ASSIGN);
    
    auto declaration = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::VARIABLE_DECLARATION);
    declaration->setAttribute("name", nameToken.value);
    
    auto value = parseExpression();
    if (value) {
        declaration->addChild(value);
    }
    
    if (match(CHTLJS_TokenType::SEMICOLON)) {
        consume(CHTLJS_TokenType::SEMICOLON);
    }
    
    return declaration;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFunctionDeclaration() {
    CHTLJS_Token nameToken = consume(CHTLJS_TokenType::IDENTIFIER);
    
    auto declaration = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::FUNCTION_DECLARATION);
    declaration->setAttribute("name", nameToken.value);
    
    // 解析参数列表
    auto params = parseParameterList();
    for (const auto& param : params) {
        declaration->setAttribute("param_" + std::to_string(declaration->getChildren().size()), param);
    }
    
    // 解析函数体
    auto body = parseBlockStatement();
    if (body) {
        declaration->addChild(body);
    }
    
    return declaration;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseIdentifier() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::IDENTIFIER);
    
    auto identifier = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::IDENTIFIER);
    identifier->setAttribute("name", token.value);
    
    return identifier;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseStringLiteral() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::STRING_LITERAL);
    
    auto literal = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LITERAL);
    literal->setAttribute("type", "string");
    literal->setAttribute("value", token.value);
    
    return literal;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseNumberLiteral() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::NUMBER_LITERAL);
    
    auto literal = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LITERAL);
    literal->setAttribute("type", "number");
    literal->setAttribute("value", token.value);
    
    return literal;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseBooleanLiteral() {
    CHTLJS_Token token = consume(CHTLJS_TokenType::BOOLEAN_LITERAL);
    
    auto literal = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LITERAL);
    literal->setAttribute("type", "boolean");
    literal->setAttribute("value", token.value);
    
    return literal;
}

std::vector<std::shared_ptr<CHTLJSBaseNode>> CHTLJSParser::parseArgumentList() {
    std::vector<std::shared_ptr<CHTLJSBaseNode>> arguments;
    
    consume(CHTLJS_TokenType::LEFT_PAREN);
    
    while (hasMoreTokens() && !match(CHTLJS_TokenType::RIGHT_PAREN)) {
        auto arg = parseExpression();
        if (arg) {
            arguments.push_back(arg);
        }
        
        if (match(CHTLJS_TokenType::COMMA)) {
            consume(CHTLJS_TokenType::COMMA);
        } else {
            break;
        }
    }
    
    consume(CHTLJS_TokenType::RIGHT_PAREN);
    return arguments;
}

std::vector<std::string> CHTLJSParser::parseParameterList() {
    std::vector<std::string> parameters;
    
    consume(CHTLJS_TokenType::LEFT_PAREN);
    
    while (hasMoreTokens() && !match(CHTLJS_TokenType::RIGHT_PAREN)) {
        if (match(CHTLJS_TokenType::IDENTIFIER)) {
            CHTLJS_Token param = consume(CHTLJS_TokenType::IDENTIFIER);
            parameters.push_back(param.value);
        }
        
        if (match(CHTLJS_TokenType::COMMA)) {
            consume(CHTLJS_TokenType::COMMA);
        } else {
            break;
        }
    }
    
    consume(CHTLJS_TokenType::RIGHT_PAREN);
    return parameters;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseArrayLiteral() {
    consume(CHTLJS_TokenType::LEFT_BRACKET);
    
    auto array = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LITERAL);
    array->setAttribute("type", "array");
    
    while (hasMoreTokens() && !match(CHTLJS_TokenType::RIGHT_BRACKET)) {
        auto element = parseExpression();
        if (element) {
            array->addChild(element);
        }
        
        if (match(CHTLJS_TokenType::COMMA)) {
            consume(CHTLJS_TokenType::COMMA);
        } else {
            break;
        }
    }
    
    consume(CHTLJS_TokenType::RIGHT_BRACKET);
    return array;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseObjectLiteral() {
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto object = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LITERAL);
    object->setAttribute("type", "object");
    
    while (hasMoreTokens() && !match(CHTLJS_TokenType::RIGHT_BRACE)) {
        if (match(CHTLJS_TokenType::IDENTIFIER) || match(CHTLJS_TokenType::STRING_LITERAL)) {
            CHTLJS_Token key = consume(currentToken().type);
            consume(CHTLJS_TokenType::COLON);
            auto value = parseExpression();
            
            if (value) {
                value->setAttribute("key", key.value);
                object->addChild(value);
            }
        }
        
        if (match(CHTLJS_TokenType::COMMA)) {
            consume(CHTLJS_TokenType::COMMA);
        } else {
            break;
        }
    }
    
    consume(CHTLJS_TokenType::RIGHT_BRACE);
    return object;
}

void CHTLJSParser::setContext(std::shared_ptr<CHTLJSContext> context) {
    context_ = context;
}

std::shared_ptr<CHTLJSContext> CHTLJSParser::getContext() const {
    return context_;
}

void CHTLJSParser::setErrorHandler(std::function<void(const std::string&, int, int)> handler) {
    error_handler_ = handler;
}

void CHTLJSParser::reportError(const std::string& message, int line, int column) {
    logError(message, line, column);
    
    if (error_handler_) {
        error_handler_(message, line, column);
    }
}

void CHTLJSParser::error(const std::string& message) {
    error(message, currentToken());
}

void CHTLJSParser::error(const std::string& message, const CHTLJS_Token& token) {
    std::string errorMsg = "Parse error at line " + std::to_string(token.line) + 
                          ", column " + std::to_string(token.column) + 
                          ": " + message;
    throw std::runtime_error(errorMsg);
}

void CHTLJSParser::logError(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << "CHTLJSParser Error";
    if (line > 0) {
        ss << " at line " << line;
    }
    if (column > 0) {
        ss << ", column " << column;
    }
    ss << ": " << message;
    
    // 这里可以添加错误日志记录
}

bool CHTLJSParser::isAssignmentOperator(CHTLJS_TokenType type) {
    return type == CHTLJS_TokenType::ASSIGN;
}

bool CHTLJSParser::isBinaryOperator(CHTLJS_TokenType type) {
    return type == CHTLJS_TokenType::PLUS || 
           type == CHTLJS_TokenType::MINUS || 
           type == CHTLJS_TokenType::MULTIPLY || 
           type == CHTLJS_TokenType::DIVIDE || 
           type == CHTLJS_TokenType::MODULO || 
           type == CHTLJS_TokenType::POWER ||
           type == CHTLJS_TokenType::EQUAL ||
           type == CHTLJS_TokenType::NOT_EQUAL ||
           type == CHTLJS_TokenType::LESS ||
           type == CHTLJS_TokenType::GREATER ||
           type == CHTLJS_TokenType::LESS_EQUAL ||
           type == CHTLJS_TokenType::GREATER_EQUAL ||
           type == CHTLJS_TokenType::AND ||
           type == CHTLJS_TokenType::OR;
}

bool CHTLJSParser::isUnaryOperator(CHTLJS_TokenType type) {
    return type == CHTLJS_TokenType::NOT || 
           type == CHTLJS_TokenType::MINUS || 
           type == CHTLJS_TokenType::PLUS;
}

int CHTLJSParser::getOperatorPrecedence(CHTLJS_TokenType type) {
    switch (type) {
        case CHTLJS_TokenType::OR:
            return 1;
        case CHTLJS_TokenType::AND:
            return 2;
        case CHTLJS_TokenType::EQUAL:
        case CHTLJS_TokenType::NOT_EQUAL:
            return 3;
        case CHTLJS_TokenType::LESS:
        case CHTLJS_TokenType::GREATER:
        case CHTLJS_TokenType::LESS_EQUAL:
        case CHTLJS_TokenType::GREATER_EQUAL:
            return 4;
        case CHTLJS_TokenType::PLUS:
        case CHTLJS_TokenType::MINUS:
            return 5;
        case CHTLJS_TokenType::MULTIPLY:
        case CHTLJS_TokenType::DIVIDE:
        case CHTLJS_TokenType::MODULO:
            return 6;
        case CHTLJS_TokenType::POWER:
            return 7;
        default:
            return 0;
    }
}

bool CHTLJSParser::isRightAssociative(CHTLJS_TokenType type) {
    return type == CHTLJS_TokenType::ASSIGN || type == CHTLJS_TokenType::POWER;
}

bool CHTLJSParser::validateStatement(const std::shared_ptr<CHTLJSBaseNode>& node) {
    if (!node) return false;
    
    // 验证语句的合法性
    return true;
}

bool CHTLJSParser::validateExpression(const std::shared_ptr<CHTLJSBaseNode>& node) {
    if (!node) return false;
    
    // 验证表达式的合法性
    return true;
}

bool CHTLJSParser::validateIdentifier(const std::string& identifier) {
    if (identifier.empty()) return false;
    
    // 验证标识符的合法性
    if (!std::isalpha(identifier[0]) && identifier[0] != '_') {
        return false;
    }
    
    for (size_t i = 1; i < identifier.length(); ++i) {
        if (!std::isalnum(identifier[i]) && identifier[i] != '_') {
            return false;
        }
    }
    
    return true;
}

} // namespace CHTLJS