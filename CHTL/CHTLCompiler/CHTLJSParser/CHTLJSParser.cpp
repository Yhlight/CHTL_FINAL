#include "CHTLJSParser.h"
#include <sstream>

namespace CHTL {

CHTLJSNode::CHTLJSNode(CHTLJSNodeType t, const std::string& v, size_t l, size_t c)
    : type(t), value(v), line(l), column(c) {
}

void CHTLJSNode::addChild(std::shared_ptr<CHTLJSNode> child) {
    children.push_back(child);
}

std::string CHTLJSNode::toString() const {
    std::stringstream ss;
    ss << "CHTLJSNode(" << static_cast<int>(type) << ", \"" << value << "\")";
    return ss.str();
}

CHTLJSParser::CHTLJSParser(const std::vector<CHTLJSToken>& tokens)
    : tokens(tokens), current(0), has_error(false) {
}

CHTLJSParser::~CHTLJSParser() {
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parse() {
    auto program = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Program);
    
    while (!isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            program->addChild(statement);
        }
        
        if (hasError()) {
            break;
        }
    }
    
    return program;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseExpression() {
    return parseBinary();
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parsePrimary() {
    if (check(CHTLJSTokenType::String) || check(CHTLJSTokenType::Number) || 
        check(CHTLJSTokenType::Boolean) || check(CHTLJSTokenType::Null) || 
        check(CHTLJSTokenType::Undefined)) {
        return parseLiteral();
    }
    
    if (check(CHTLJSTokenType::Identifier)) {
        return parseIdentifier();
    }
    
    if (check(CHTLJSTokenType::LeftParen)) {
        advance(); // Consume '('
        auto expr = parseExpression();
        consume(CHTLJSTokenType::RightParen, "Expected ')' after expression");
        return expr;
    }
    
    if (check(CHTLJSTokenType::LeftBracket)) {
        return parseArray();
    }
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        return parseObject();
    }
    
    if (check(CHTLJSTokenType::Function)) {
        return parseFunction();
    }
    
    // CHTL JS specific
    if (isCHTLJSKeyword(currentToken().type)) {
        return parseCHTLJSFunction();
    }
    
    error("Expected expression");
    return nullptr;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseBinary() {
    auto left = parseUnary();
    
    while (isBinaryOperator(currentToken().type)) {
        auto operator_token = advance();
        auto right = parseUnary();
        
        auto binary = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Binary, operator_token.value, operator_token.line, operator_token.column);
        binary->addChild(left);
        binary->addChild(right);
        left = binary;
    }
    
    return left;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseUnary() {
    if (isUnaryOperator(currentToken().type)) {
        auto operator_token = advance();
        auto operand = parseUnary();
        
        auto unary = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Unary, operator_token.value, operator_token.line, operator_token.column);
        unary->addChild(operand);
        return unary;
    }
    
    return parseCall();
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseCall() {
    auto expr = parseMember();
    
    while (check(CHTLJSTokenType::LeftParen)) {
        advance(); // Consume '('
        auto args = parseArguments();
        consume(CHTLJSTokenType::RightParen, "Expected ')' after arguments");
        
        auto call = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Call, "call", currentToken().line, currentToken().column);
        call->addChild(expr);
        call->addChild(args);
        expr = call;
    }
    
    return expr;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseMember() {
    auto expr = parsePrimary();
    
    while (check(CHTLJSTokenType::Dot) || check(CHTLJSTokenType::LeftBracket)) {
        if (check(CHTLJSTokenType::Dot)) {
            advance(); // Consume '.'
            auto property = parseIdentifier();
            
            auto member = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Member, ".", currentToken().line, currentToken().column);
            member->addChild(expr);
            member->addChild(property);
            expr = member;
        } else {
            advance(); // Consume '['
            auto property = parseExpression();
            consume(CHTLJSTokenType::RightBracket, "Expected ']' after property");
            
            auto member = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Member, "[]", currentToken().line, currentToken().column);
            member->addChild(expr);
            member->addChild(property);
            expr = member;
        }
    }
    
    return expr;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseArray() {
    advance(); // Consume '['
    auto array = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Array, "[]", currentToken().line, currentToken().column);
    
    if (!check(CHTLJSTokenType::RightBracket)) {
        do {
            auto element = parseExpression();
            array->addChild(element);
        } while (match(CHTLJSTokenType::Comma));
    }
    
    consume(CHTLJSTokenType::RightBracket, "Expected ']' after array");
    return array;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseObject() {
    advance(); // Consume '{'
    auto object = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Object, "{}", currentToken().line, currentToken().column);
    
    if (!check(CHTLJSTokenType::RightBrace)) {
        do {
            auto property = parseProperty();
            object->addChild(property);
        } while (match(CHTLJSTokenType::Comma));
    }
    
    consume(CHTLJSTokenType::RightBrace, "Expected '}' after object");
    return object;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseProperty() {
    auto key = parseExpression();
    consume(CHTLJSTokenType::Colon, "Expected ':' after property key");
    auto value = parseExpression();
    
    auto property = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Property, "property", currentToken().line, currentToken().column);
    property->addChild(key);
    property->addChild(value);
    return property;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseStatement() {
    if (check(CHTLJSTokenType::If)) {
        return parseIf();
    }
    
    if (check(CHTLJSTokenType::For)) {
        return parseFor();
    }
    
    if (check(CHTLJSTokenType::While)) {
        return parseWhile();
    }
    
    if (check(CHTLJSTokenType::Return)) {
        return parseReturn();
    }
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        return parseBlock();
    }
    
    if (check(CHTLJSTokenType::Var) || check(CHTLJSTokenType::Let) || check(CHTLJSTokenType::Const)) {
        return parseDeclaration();
    }
    
    if (check(CHTLJSTokenType::Function)) {
        return parseFunction();
    }
    
    // CHTL JS specific
    if (isCHTLJSKeyword(currentToken().type)) {
        return parseCHTLJSFunction();
    }
    
    // Expression statement
    auto expr = parseExpression();
    if (check(CHTLJSTokenType::Semicolon)) {
        advance(); // Consume ';'
    }
    return expr;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseDeclaration() {
    auto decl_type = advance();
    auto name = consume(CHTLJSTokenType::Identifier, "Expected variable name");
    
    auto declaration = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Declaration, decl_type.value, decl_type.line, decl_type.column);
    declaration->addChild(std::make_shared<CHTLJSNode>(CHTLJSNodeType::Identifier, name.value, name.line, name.column));
    
    if (match(CHTLJSTokenType::Assign)) {
        auto initializer = parseExpression();
        declaration->addChild(initializer);
    }
    
    if (check(CHTLJSTokenType::Semicolon)) {
        advance(); // Consume ';'
    }
    
    return declaration;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseBlock() {
    advance(); // Consume '{'
    auto block = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Block, "{}", currentToken().line, currentToken().column);
    
    while (!check(CHTLJSTokenType::RightBrace) && !isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            block->addChild(statement);
        }
    }
    
    consume(CHTLJSTokenType::RightBrace, "Expected '}' after block");
    return block;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseIf() {
    advance(); // Consume 'if'
    consume(CHTLJSTokenType::LeftParen, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(CHTLJSTokenType::RightParen, "Expected ')' after condition");
    
    auto if_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::If, "if", currentToken().line, currentToken().column);
    if_node->addChild(condition);
    if_node->addChild(parseStatement());
    
    if (match(CHTLJSTokenType::Else)) {
        if_node->addChild(parseStatement());
    }
    
    return if_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseFor() {
    advance(); // Consume 'for'
    consume(CHTLJSTokenType::LeftParen, "Expected '(' after 'for'");
    
    auto for_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::For, "for", currentToken().line, currentToken().column);
    
    // Initializer
    if (!check(CHTLJSTokenType::Semicolon)) {
        for_node->addChild(parseExpression());
    } else {
        for_node->addChild(nullptr);
    }
    consume(CHTLJSTokenType::Semicolon, "Expected ';' after initializer");
    
    // Condition
    if (!check(CHTLJSTokenType::Semicolon)) {
        for_node->addChild(parseExpression());
    } else {
        for_node->addChild(nullptr);
    }
    consume(CHTLJSTokenType::Semicolon, "Expected ';' after condition");
    
    // Update
    if (!check(CHTLJSTokenType::RightParen)) {
        for_node->addChild(parseExpression());
    } else {
        for_node->addChild(nullptr);
    }
    consume(CHTLJSTokenType::RightParen, "Expected ')' after for clause");
    
    // Body
    for_node->addChild(parseStatement());
    
    return for_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseWhile() {
    advance(); // Consume 'while'
    consume(CHTLJSTokenType::LeftParen, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(CHTLJSTokenType::RightParen, "Expected ')' after condition");
    
    auto while_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::While, "while", currentToken().line, currentToken().column);
    while_node->addChild(condition);
    while_node->addChild(parseStatement());
    
    return while_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseReturn() {
    advance(); // Consume 'return'
    
    auto return_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Return, "return", currentToken().line, currentToken().column);
    
    if (!check(CHTLJSTokenType::Semicolon)) {
        return_node->addChild(parseExpression());
    } else {
        return_node->addChild(nullptr);
    }
    
    if (check(CHTLJSTokenType::Semicolon)) {
        advance(); // Consume ';'
    }
    
    return return_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseAssignment() {
    auto left = parseExpression();
    
    if (isAssignmentOperator(currentToken().type)) {
        auto operator_token = advance();
        auto right = parseExpression();
        
        auto assignment = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Assignment, operator_token.value, operator_token.line, operator_token.column);
        assignment->addChild(left);
        assignment->addChild(right);
        return assignment;
    }
    
    return left;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseCHTLJSFunction() {
    auto token = currentToken();
    advance(); // Consume CHTL JS keyword
    
    auto chtl_js_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::CHTLJSFunction, token.value, token.line, token.column);
    
    // Parse CHTL JS specific syntax
    if (check(CHTLJSTokenType::LeftBrace)) {
        chtl_js_node->addChild(parseCHTLJSBlock());
    } else if (check(CHTLJSTokenType::LeftParen)) {
        advance(); // Consume '('
        chtl_js_node->addChild(parseCHTLJSParameters());
        consume(CHTLJSTokenType::RightParen, "Expected ')' after parameters");
    }
    
    return chtl_js_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseVirtualObject() {
    auto token = currentToken();
    advance(); // Consume 'vir'
    
    auto vir_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::VirtualObject, "vir", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        vir_node->addChild(parseCHTLJSBlock());
    }
    
    return vir_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseRouter() {
    auto token = currentToken();
    advance(); // Consume 'router'
    
    auto router_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Router, "router", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        router_node->addChild(parseCHTLJSBlock());
    }
    
    return router_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseAnimation() {
    auto token = currentToken();
    advance(); // Consume 'animate'
    
    auto animate_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Animation, "animate", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        animate_node->addChild(parseCHTLJSBlock());
    }
    
    return animate_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseEventDelegation() {
    auto token = currentToken();
    advance(); // Consume 'delegate'
    
    auto delegate_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::EventDelegation, "delegate", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        delegate_node->addChild(parseCHTLJSBlock());
    }
    
    return delegate_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseEnhancedSelector() {
    auto token = currentToken();
    advance(); // Consume 'selector'
    
    auto selector_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::EnhancedSelector, "selector", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        selector_node->addChild(parseCHTLJSBlock());
    }
    
    return selector_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseEnhancedListener() {
    auto token = currentToken();
    advance(); // Consume 'listener'
    
    auto listener_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::EnhancedListener, "listener", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        listener_node->addChild(parseCHTLJSBlock());
    }
    
    return listener_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseFileLoader() {
    auto token = currentToken();
    advance(); // Consume 'fileloader'
    
    auto fileloader_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::FileLoader, "fileloader", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        fileloader_node->addChild(parseCHTLJSBlock());
    }
    
    return fileloader_node;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseLocalScript() {
    auto token = currentToken();
    advance(); // Consume 'script'
    
    auto script_node = std::make_shared<CHTLJSNode>(CHTLJSNodeType::LocalScript, "script", token.line, token.column);
    
    if (check(CHTLJSTokenType::LeftBrace)) {
        script_node->addChild(parseCHTLJSBlock());
    }
    
    return script_node;
}

bool CHTLJSParser::isAtEnd() const {
    return current >= tokens.size() || currentToken().type == CHTLJSTokenType::EndOfFile;
}

CHTLJSToken CHTLJSParser::currentToken() const {
    if (current >= tokens.size()) {
        return CHTLJSToken(CHTLJSTokenType::EndOfFile, "", 0, 0);
    }
    return tokens[current];
}

CHTLJSToken CHTLJSParser::peekToken() const {
    if (current + 1 >= tokens.size()) {
        return CHTLJSToken(CHTLJSTokenType::EndOfFile, "", 0, 0);
    }
    return tokens[current + 1];
}

CHTLJSToken CHTLJSParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLJSParser::match(CHTLJSTokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLJSParser::check(CHTLJSTokenType type) const {
    if (isAtEnd()) {
        return false;
    }
    return currentToken().type == type;
}

CHTLJSToken CHTLJSParser::consume(CHTLJSTokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    error(message);
    return CHTLJSToken(CHTLJSTokenType::Error, message, 0, 0);
}

void CHTLJSParser::error(const std::string& message) {
    error_message = message;
    has_error = true;
}

bool CHTLJSParser::hasError() const {
    return has_error;
}

std::string CHTLJSParser::getError() const {
    return error_message;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseLiteral() {
    auto token = advance();
    return std::make_shared<CHTLJSNode>(CHTLJSNodeType::Literal, token.value, token.line, token.column);
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseIdentifier() {
    auto token = advance();
    return std::make_shared<CHTLJSNode>(CHTLJSNodeType::Identifier, token.value, token.line, token.column);
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseFunction() {
    advance(); // Consume 'function'
    auto name = consume(CHTLJSTokenType::Identifier, "Expected function name");
    
    auto function = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Function, name.value, name.line, name.column);
    
    consume(CHTLJSTokenType::LeftParen, "Expected '(' after function name");
    function->addChild(parseParameters());
    consume(CHTLJSTokenType::RightParen, "Expected ')' after parameters");
    
    function->addChild(parseBody());
    
    return function;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseVariable() {
    auto token = advance();
    return std::make_shared<CHTLJSNode>(CHTLJSNodeType::Variable, token.value, token.line, token.column);
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseParameters() {
    auto params = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Expression, "parameters", currentToken().line, currentToken().column);
    
    if (!check(CHTLJSTokenType::RightParen)) {
        do {
            auto param = consume(CHTLJSTokenType::Identifier, "Expected parameter name");
            params->addChild(std::make_shared<CHTLJSNode>(CHTLJSNodeType::Identifier, param.value, param.line, param.column));
        } while (match(CHTLJSTokenType::Comma));
    }
    
    return params;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseArguments() {
    auto args = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Expression, "arguments", currentToken().line, currentToken().column);
    
    if (!check(CHTLJSTokenType::RightParen)) {
        do {
            auto arg = parseExpression();
            args->addChild(arg);
        } while (match(CHTLJSTokenType::Comma));
    }
    
    return args;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseCondition() {
    return parseExpression();
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseBody() {
    if (check(CHTLJSTokenType::LeftBrace)) {
        return parseBlock();
    } else {
        return parseStatement();
    }
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseInitializer() {
    return parseExpression();
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseUpdate() {
    return parseExpression();
}

int CHTLJSParser::getPrecedence(CHTLJSTokenType type) const {
    switch (type) {
        case CHTLJSTokenType::Or: return 1;
        case CHTLJSTokenType::And: return 2;
        case CHTLJSTokenType::Equal:
        case CHTLJSTokenType::NotEqual: return 3;
        case CHTLJSTokenType::LessThan:
        case CHTLJSTokenType::GreaterThan:
        case CHTLJSTokenType::LessEqual:
        case CHTLJSTokenType::GreaterEqual: return 4;
        case CHTLJSTokenType::Plus:
        case CHTLJSTokenType::Minus: return 5;
        case CHTLJSTokenType::Multiply:
        case CHTLJSTokenType::Divide:
        case CHTLJSTokenType::Modulo: return 6;
        default: return 0;
    }
}

bool CHTLJSParser::isBinaryOperator(CHTLJSTokenType type) const {
    return type == CHTLJSTokenType::Plus || type == CHTLJSTokenType::Minus ||
           type == CHTLJSTokenType::Multiply || type == CHTLJSTokenType::Divide ||
           type == CHTLJSTokenType::Modulo || type == CHTLJSTokenType::Equal ||
           type == CHTLJSTokenType::NotEqual || type == CHTLJSTokenType::LessThan ||
           type == CHTLJSTokenType::GreaterThan || type == CHTLJSTokenType::LessEqual ||
           type == CHTLJSTokenType::GreaterEqual || type == CHTLJSTokenType::And ||
           type == CHTLJSTokenType::Or;
}

bool CHTLJSParser::isUnaryOperator(CHTLJSTokenType type) const {
    return type == CHTLJSTokenType::Not || type == CHTLJSTokenType::Minus ||
           type == CHTLJSTokenType::Plus;
}

bool CHTLJSParser::isAssignmentOperator(CHTLJSTokenType type) const {
    return type == CHTLJSTokenType::Assign || type == CHTLJSTokenType::PlusAssign ||
           type == CHTLJSTokenType::MinusAssign || type == CHTLJSTokenType::MultiplyAssign ||
           type == CHTLJSTokenType::DivideAssign || type == CHTLJSTokenType::ModuloAssign;
}

bool CHTLJSParser::isCHTLJSKeyword(CHTLJSTokenType type) const {
    return type == CHTLJSTokenType::Listen || type == CHTLJSTokenType::Delegate ||
           type == CHTLJSTokenType::Animate || type == CHTLJSTokenType::Vir ||
           type == CHTLJSTokenType::Router || type == CHTLJSTokenType::FileLoader ||
           type == CHTLJSTokenType::Script || type == CHTLJSTokenType::Selector ||
           type == CHTLJSTokenType::Listener;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseCHTLJSBlock() {
    advance(); // Consume '{'
    auto block = std::make_shared<CHTLJSNode>(CHTLJSNodeType::Block, "{}", currentToken().line, currentToken().column);
    
    while (!check(CHTLJSTokenType::RightBrace) && !isAtEnd()) {
        auto statement = parseCHTLJSExpression();
        if (statement) {
            block->addChild(statement);
        }
    }
    
    consume(CHTLJSTokenType::RightBrace, "Expected '}' after CHTL JS block");
    return block;
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseCHTLJSExpression() {
    return parseExpression();
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseCHTLJSParameters() {
    return parseParameters();
}

std::shared_ptr<CHTLJSNode> CHTLJSParser::parseCHTLJSBody() {
    return parseBody();
}

} // namespace CHTL