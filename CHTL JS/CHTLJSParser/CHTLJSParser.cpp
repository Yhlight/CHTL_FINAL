#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSSpecialNode.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const std::vector<CHTLJS_Token>& tokens, 
                           std::shared_ptr<CHTLJSContext> context)
    : tokens_(tokens), position_(0), context_(context) {
    if (!context_) {
        context_ = std::make_shared<CHTLJSContext>();
    }
}

CHTLJSParser::~CHTLJSParser() {
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parse() {
    return parseProgram();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseProgram() {
    auto program = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BLOCK_STATEMENT);
    
    while (hasMoreTokens() && currentToken().type != CHTLJS_TokenType::EOF_TOKEN) {
        auto statement = parseStatement();
        if (statement) {
            program->addChild(statement);
        }
    }
    
    return program;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseStatement() {
    if (!hasMoreTokens()) {
        return nullptr;
    }
    
    const CHTLJS_Token& token = currentToken();
    
    // CHTL JS特有语法
    switch (token.type) {
        case CHTLJS_TokenType::SCRIPT_LOADER:
            return parseScriptLoader();
        case CHTLJS_TokenType::LISTEN:
            return parseListen();
        case CHTLJS_TokenType::ANIMATE:
            return parseAnimate();
        case CHTLJS_TokenType::ROUTER:
            return parseRouter();
        case CHTLJS_TokenType::VIR:
            return parseVir();
        case CHTLJS_TokenType::INEVERAWAY:
            return parseINeverAway();
        case CHTLJS_TokenType::UTIL:
            return parseUtilThen();
        case CHTLJS_TokenType::PRINTMYLOVE:
            return parsePrintMylove();
        case CHTLJS_TokenType::CHTL_SELECTOR:
            return parseCHTLSelector();
        case CHTLJS_TokenType::RESPONSIVE_GET:
            return parseResponsiveGet();
        case CHTLJS_TokenType::RESPONSIVE_SET:
            return parseResponsiveSet();
    }
    
    // 传统语句
    switch (token.type) {
        case CHTLJS_TokenType::LEFT_BRACE:
            return parseBlockStatement();
        case CHTLJS_TokenType::IDENTIFIER:
            // 可能是变量声明或函数声明
            if (peekToken().type == CHTLJS_TokenType::ASSIGN) {
                return parseVariableDeclaration();
            } else if (peekToken().type == CHTLJS_TokenType::LEFT_PAREN) {
                return parseFunctionDeclaration();
            } else {
                return parseExpressionStatement();
            }
        case CHTLJS_TokenType::SEMICOLON:
            advance(); // 空语句
            return nullptr;
        default:
            return parseExpressionStatement();
    }
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExpression() {
    return parseAssignmentExpression();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAssignmentExpression() {
    auto left = parseLogicalOrExpression();
    
    if (isAssignmentOperator(currentToken().type)) {
        CHTLJS_Token operatorToken = consume(currentToken().type);
        auto right = parseAssignmentExpression();
        
        auto assignment = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::ASSIGNMENT);
        assignment->addChild(left);
        assignment->addChild(right);
        assignment->setAttribute("operator", operatorToken.value);
        
        return assignment;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseLogicalOrExpression() {
    auto left = parseLogicalAndExpression();
    
    while (match(CHTLJS_TokenType::OR)) {
        CHTLJS_Token operatorToken = consume(CHTLJS_TokenType::OR);
        auto right = parseLogicalAndExpression();
        
        auto logicalOr = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BINARY_EXPRESSION);
        logicalOr->addChild(left);
        logicalOr->addChild(right);
        logicalOr->setAttribute("operator", operatorToken.value);
        
        left = logicalOr;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseLogicalAndExpression() {
    auto left = parseEqualityExpression();
    
    while (match(CHTLJS_TokenType::AND)) {
        CHTLJS_Token operatorToken = consume(CHTLJS_TokenType::AND);
        auto right = parseEqualityExpression();
        
        auto logicalAnd = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BINARY_EXPRESSION);
        logicalAnd->addChild(left);
        logicalAnd->addChild(right);
        logicalAnd->setAttribute("operator", operatorToken.value);
        
        left = logicalAnd;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseEqualityExpression() {
    auto left = parseRelationalExpression();
    
    while (match({CHTLJS_TokenType::EQUAL, CHTLJS_TokenType::NOT_EQUAL})) {
        CHTLJS_Token operatorToken = consume(currentToken().type);
        auto right = parseRelationalExpression();
        
        auto equality = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BINARY_EXPRESSION);
        equality->addChild(left);
        equality->addChild(right);
        equality->setAttribute("operator", operatorToken.value);
        
        left = equality;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseRelationalExpression() {
    auto left = parseAdditiveExpression();
    
    while (match({CHTLJS_TokenType::LESS, CHTLJS_TokenType::GREATER, 
                  CHTLJS_TokenType::LESS_EQUAL, CHTLJS_TokenType::GREATER_EQUAL})) {
        CHTLJS_Token operatorToken = consume(currentToken().type);
        auto right = parseAdditiveExpression();
        
        auto relational = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BINARY_EXPRESSION);
        relational->addChild(left);
        relational->addChild(right);
        relational->setAttribute("operator", operatorToken.value);
        
        left = relational;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();
    
    while (match({CHTLJS_TokenType::PLUS, CHTLJS_TokenType::MINUS})) {
        CHTLJS_Token operatorToken = consume(currentToken().type);
        auto right = parseMultiplicativeExpression();
        
        auto additive = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BINARY_EXPRESSION);
        additive->addChild(left);
        additive->addChild(right);
        additive->setAttribute("operator", operatorToken.value);
        
        left = additive;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();
    
    while (match({CHTLJS_TokenType::MULTIPLY, CHTLJS_TokenType::DIVIDE, CHTLJS_TokenType::MODULO, CHTLJS_TokenType::POWER})) {
        CHTLJS_Token operatorToken = consume(currentToken().type);
        auto right = parseUnaryExpression();
        
        auto multiplicative = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BINARY_EXPRESSION);
        multiplicative->addChild(left);
        multiplicative->addChild(right);
        multiplicative->setAttribute("operator", operatorToken.value);
        
        left = multiplicative;
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseUnaryExpression() {
    if (isUnaryOperator(currentToken().type)) {
        CHTLJS_Token operatorToken = consume(currentToken().type);
        auto operand = parseUnaryExpression();
        
        auto unary = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::UNARY_EXPRESSION);
        unary->addChild(operand);
        unary->setAttribute("operator", operatorToken.value);
        
        return unary;
    }
    
    return parsePostfixExpression();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePostfixExpression() {
    auto left = parsePrimaryExpression();
    
    while (match({CHTLJS_TokenType::LEFT_BRACKET, CHTLJS_TokenType::DOT, CHTLJS_TokenType::LEFT_PAREN})) {
        if (match(CHTLJS_TokenType::LEFT_BRACKET)) {
            // 数组访问
            consume(CHTLJS_TokenType::LEFT_BRACKET);
            auto index = parseExpression();
            consume(CHTLJS_TokenType::RIGHT_BRACKET);
            
            auto memberAccess = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::MEMBER_EXPRESSION);
            memberAccess->addChild(left);
            memberAccess->addChild(index);
            memberAccess->setAttribute("computed", "true");
            
            left = memberAccess;
        } else if (match(CHTLJS_TokenType::DOT)) {
            // 属性访问
            consume(CHTLJS_TokenType::DOT);
            CHTLJS_Token property = consume(CHTLJS_TokenType::IDENTIFIER);
            
            auto memberAccess = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::MEMBER_EXPRESSION);
            memberAccess->addChild(left);
            memberAccess->setAttribute("property", property.value);
            memberAccess->setAttribute("computed", "false");
            
            left = memberAccess;
        } else if (match(CHTLJS_TokenType::LEFT_PAREN)) {
            // 函数调用
            auto args = parseArgumentList();
            
            auto call = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::FUNCTION_CALL);
            call->addChild(left);
            for (auto arg : args) {
                call->addChild(arg);
            }
            
            left = call;
        }
    }
    
    return left;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePrimaryExpression() {
    const CHTLJS_Token& token = currentToken();
    
    switch (token.type) {
        case CHTLJS_TokenType::IDENTIFIER:
            return parseIdentifier();
        case CHTLJS_TokenType::STRING_LITERAL:
            return parseStringLiteral();
        case CHTLJS_TokenType::NUMBER_LITERAL:
            return parseNumberLiteral();
        case CHTLJS_TokenType::BOOLEAN_LITERAL:
            return parseBooleanLiteral();
        case CHTLJS_TokenType::LEFT_PAREN:
            consume(CHTLJS_TokenType::LEFT_PAREN);
            auto expr = parseExpression();
            consume(CHTLJS_TokenType::RIGHT_PAREN);
            return expr;
        case CHTLJS_TokenType::LEFT_BRACKET:
            return parseArrayLiteral();
        case CHTLJS_TokenType::LEFT_BRACE:
            return parseObjectLiteral();
        case CHTLJS_TokenType::RESPONSIVE_GET:
            return parseResponsiveGet();
        case CHTLJS_TokenType::CHTL_SELECTOR:
            return parseCHTLSelector();
        case CHTLJS_TokenType::CHTL_RESPONSIVE:
            return parseCHTLResponsive();
        default:
            error("Unexpected token in primary expression: " + token.value);
            return nullptr;
    }
}

bool CHTLJSParser::hasMoreTokens() const {
    return position_ < tokens_.size() && currentToken().type != CHTLJS_TokenType::EOF_TOKEN;
}

const CHTLJS_Token& CHTLJSParser::currentToken() const {
    if (position_ >= tokens_.size()) {
        static CHTLJS_Token eof(CHTLJS_TokenType::EOF_TOKEN, "", 0, 0);
        return eof;
    }
    return tokens_[position_];
}

const CHTLJS_Token& CHTLJSParser::peekToken(int offset) const {
    if (position_ + offset >= tokens_.size()) {
        static CHTLJS_Token eof(CHTLJS_TokenType::EOF_TOKEN, "", 0, 0);
        return eof;
    }
    return tokens_[position_ + offset];
}

void CHTLJSParser::advance() {
    if (position_ < tokens_.size()) {
        position_++;
    }
}

bool CHTLJSParser::match(CHTLJS_TokenType type) {
    return hasMoreTokens() && currentToken().type == type;
}

bool CHTLJSParser::match(const std::vector<CHTLJS_TokenType>& types) {
    if (!hasMoreTokens()) return false;
    
    CHTLJS_TokenType current = currentToken().type;
    for (CHTLJS_TokenType type : types) {
        if (current == type) return true;
    }
    return false;
}

CHTLJS_Token CHTLJSParser::consume(CHTLJS_TokenType type) {
    if (!match(type)) {
        error("Expected token type " + std::to_string(static_cast<int>(type)));
    }
    
    CHTLJS_Token token = currentToken();
    advance();
    return token;
}

CHTLJS_Token CHTLJSParser::consume(const std::vector<CHTLJS_TokenType>& types) {
    if (!match(types)) {
        error("Expected one of the specified token types");
    }
    
    CHTLJS_Token token = currentToken();
    advance();
    return token;
}

bool CHTLJSParser::expect(CHTLJS_TokenType type, const std::string& expected) {
    if (!match(type)) {
        error("Expected " + expected + " but found " + currentToken().value);
        return false;
    }
    return true;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseScriptLoader() {
    consume(CHTLJS_TokenType::SCRIPT_LOADER);
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::SCRIPT_LOADER);
    
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

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseListen() {
    consume(CHTLJS_TokenType::LISTEN);
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LISTEN);
    
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

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimate() {
    consume(CHTLJS_TokenType::ANIMATE);
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::ANIMATE);
    
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

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseRouter() {
    consume(CHTLJS_TokenType::ROUTER);
    consume(CHTLJS_TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::ROUTER);
    
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

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVir() {
    consume(CHTLJS_TokenType::VIR);
    
    CHTLJS_Token nameToken = consume(CHTLJS_TokenType::IDENTIFIER);
    consume(CHTLJS_TokenType::ASSIGN);
    
    auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::VIR);
    node->setAttribute("name", nameToken.value);
    
    // 解析右侧的表达式
    auto value = parseExpression();
    if (value) {
        node->addChild(value);
    }
    
    return node;
}

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
    consume(TokenType::UTIL);
    
    auto node = std::make_shared<UtilThenNode>();
    
    // 解析util表达式
    auto expression = parseExpression();
    if (expression) {
        node->addParameter("expression", expression);
    }
    
    // 解析 -> change
    if (match(TokenType::ARROW)) {
        consume(TokenType::ARROW);
        consume(TokenType::IDENTIFIER); // change
    }
    
    // 解析change块
    if (match(TokenType::LEFT_BRACE)) {
        auto changeBlock = parseBlock();
        if (changeBlock) {
            node->addParameter("change", changeBlock);
        }
    }
    
    // 解析 -> then
    if (match(TokenType::ARROW)) {
        consume(TokenType::ARROW);
        consume(TokenType::IDENTIFIER); // then
    }
    
    // 解析then块
    if (match(TokenType::LEFT_BRACE)) {
        auto thenBlock = parseBlock();
        if (thenBlock) {
            node->addParameter("then", thenBlock);
        }
    }
    
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parsePrintMylove() {
    consume(TokenType::PRINTMYLOVE);
    consume(TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<PrintMyloveNode>();
    parseParameters(node);
    
    consume(TokenType::RIGHT_BRACE);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseSelector() {
    Token token = consume(TokenType::SELECTOR_START);
    return std::make_shared<SelectorNode>(token.getValue());
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseResponsiveValue() {
    Token token = consume(TokenType::RESPONSIVE_START);
    return std::make_shared<ResponsiveValueNode>(token.getValue());
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseBlock() {
    consume(TokenType::LEFT_BRACE);
    
    auto block = std::make_shared<BlockNode>();
    
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
        auto statement = parseStatement();
        if (statement) {
            block->addStatement(statement);
        }
    }
    
    consume(TokenType::RIGHT_BRACE);
    return block;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseFunctionCall() {
    Token nameToken = consume(TokenType::IDENTIFIER);
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        auto node = std::make_shared<CHTLJSFunctionNode>(
            CHTLJSBaseNode::NodeType::FUNCTION_CALL, 
            nameToken.getValue()
        );
        parseParameters(node);
        
        consume(TokenType::RIGHT_BRACE);
        return node;
    }
    
    return std::make_shared<ExpressionNode>(nameToken.getValue());
}

void CHTLJSParser::parseParameters(std::shared_ptr<CHTLJSBaseNode> functionNode) {
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
        if (match(TokenType::IDENTIFIER)) {
            Token keyToken = consume(TokenType::IDENTIFIER);
            consume(TokenType::COLON);
            
            if (match(TokenType::STRING)) {
                std::string value = parseStringValue();
                auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(functionNode);
                if (funcNode) {
                    funcNode->addParameter(keyToken.getValue(), value);
                }
            } else {
                auto value = parseNodeValue();
                auto funcNode = std::dynamic_pointer_cast<CHTLJSFunctionNode>(functionNode);
                if (funcNode && value) {
                    funcNode->addParameter(keyToken.getValue(), value);
                }
            }
            
            if (match(TokenType::COMMA)) {
                consume(TokenType::COMMA);
            }
        } else {
            advance();
        }
    }
}

std::string CHTLJSParser::parseStringValue() {
    Token token = consume(TokenType::STRING);
    return token.getValue();
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseNodeValue() {
    if (match(TokenType::LEFT_BRACE)) {
        return parseBlock();
    } else if (match(TokenType::SELECTOR_START)) {
        return parseSelector();
    } else if (match(TokenType::RESPONSIVE_START)) {
        return parseResponsiveValue();
    } else {
        return parseExpression();
    }
}

void CHTLJSParser::error(const std::string& message) {
    error(message, currentToken());
}

void CHTLJSParser::error(const std::string& message, const Token& token) {
    std::string errorMsg = "Parse error at line " + std::to_string(token.getLine()) + 
                          ", column " + std::to_string(token.getColumn()) + 
                          ": " + message;
    throw std::runtime_error(errorMsg);
}

} // namespace CHTLJS