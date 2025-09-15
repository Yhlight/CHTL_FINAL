#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSFunctionNode.h"
#include "../CHTLJSNode/CHTLJSSpecialNode.h"
#include <iostream>
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const std::vector<Token>& tokens)
    : tokens_(tokens), position_(0) {
}

CHTLJSParser::~CHTLJSParser() {
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parse() {
    auto block = std::make_shared<BlockNode>();
    
    while (hasMoreTokens()) {
        auto statement = parseStatement();
        if (statement) {
            block->addStatement(statement);
        }
    }
    
    return block;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseStatement() {
    if (!hasMoreTokens()) {
        return nullptr;
    }
    
    const Token& token = currentToken();
    
    switch (token.getType()) {
        case TokenType::SCRIPT_LOADER:
            return parseScriptLoader();
        case TokenType::LISTEN:
            return parseListen();
        case TokenType::ANIMATE:
            return parseAnimate();
        case TokenType::ROUTER:
            return parseRouter();
        case TokenType::VIR:
            return parseVir();
        case TokenType::INEVERAWAY:
            return parseINeverAway();
        case TokenType::UTIL:
            return parseUtilThen();
        case TokenType::PRINTMYLOVE:
            return parsePrintMylove();
        case TokenType::SELECTOR_START:
            return parseSelector();
        case TokenType::RESPONSIVE_START:
            return parseResponsiveValue();
        case TokenType::LEFT_BRACE:
            return parseBlock();
        default:
            return parseExpression();
    }
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseExpression() {
    if (!hasMoreTokens()) {
        return nullptr;
    }
    
    const Token& token = currentToken();
    
    if (token.getType() == TokenType::IDENTIFIER) {
        return parseFunctionCall();
    }
    
    if (token.getType() == TokenType::STRING || 
        token.getType() == TokenType::NUMBER || 
        token.getType() == TokenType::BOOLEAN) {
        auto expr = std::make_shared<ExpressionNode>(token.getValue());
        advance();
        return expr;
    }
    
    return nullptr;
}

bool CHTLJSParser::hasMoreTokens() const {
    return position_ < tokens_.size();
}

const Token& CHTLJSParser::currentToken() const {
    if (position_ >= tokens_.size()) {
        static Token eof(TokenType::EOF_TOKEN, "", 0, 0);
        return eof;
    }
    return tokens_[position_];
}

const Token& CHTLJSParser::peekToken() const {
    if (position_ + 1 >= tokens_.size()) {
        static Token eof(TokenType::EOF_TOKEN, "", 0, 0);
        return eof;
    }
    return tokens_[position_ + 1];
}

void CHTLJSParser::advance() {
    if (position_ < tokens_.size()) {
        position_++;
    }
}

bool CHTLJSParser::match(TokenType type) {
    return hasMoreTokens() && currentToken().getType() == type;
}

bool CHTLJSParser::match(const std::vector<TokenType>& types) {
    if (!hasMoreTokens()) return false;
    
    TokenType current = currentToken().getType();
    for (TokenType type : types) {
        if (current == type) return true;
    }
    return false;
}

Token CHTLJSParser::consume(TokenType type) {
    if (!match(type)) {
        error("Expected token type " + std::to_string(static_cast<int>(type)));
    }
    
    Token token = currentToken();
    advance();
    return token;
}

Token CHTLJSParser::consume(const std::vector<TokenType>& types) {
    if (!match(types)) {
        error("Expected one of the specified token types");
    }
    
    Token token = currentToken();
    advance();
    return token;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseScriptLoader() {
    consume(TokenType::SCRIPT_LOADER);
    consume(TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<ScriptLoaderNode>();
    parseParameters(node);
    
    consume(TokenType::RIGHT_BRACE);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseListen() {
    consume(TokenType::LISTEN);
    consume(TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<ListenNode>();
    parseParameters(node);
    
    consume(TokenType::RIGHT_BRACE);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseAnimate() {
    consume(TokenType::ANIMATE);
    consume(TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<AnimateNode>();
    parseParameters(node);
    
    consume(TokenType::RIGHT_BRACE);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseRouter() {
    consume(TokenType::ROUTER);
    consume(TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<RouterNode>();
    parseParameters(node);
    
    consume(TokenType::RIGHT_BRACE);
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseVir() {
    consume(TokenType::VIR);
    
    Token nameToken = consume(TokenType::IDENTIFIER);
    consume(TokenType::EQUAL);
    
    auto node = std::make_shared<VirNode>();
    node->setFunctionName(nameToken.getValue());
    
    // 解析右侧的表达式
    auto value = parseExpression();
    if (value) {
        node->addParameter("value", value);
    }
    
    return node;
}

std::shared_ptr<CHTLJSBaseNode> CHTLJSParser::parseINeverAway() {
    consume(TokenType::INEVERAWAY);
    consume(TokenType::LEFT_BRACE);
    
    auto node = std::make_shared<INeverAwayNode>();
    parseParameters(node);
    
    consume(TokenType::RIGHT_BRACE);
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