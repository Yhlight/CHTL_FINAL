#include "CHTLJSParser.h"
#include <iostream>
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const TokenList& tokenList) 
    : tokens(tokenList), currentIndex(0) {}

CHTLJSParser::CHTLJSParser(const std::string& source) 
    : currentIndex(0) {
    CHTLJSLexer lexer(source);
    tokens = lexer.tokenize();
}

Token CHTLJSParser::getCurrent() const {
    if (currentIndex < tokens.size()) {
        return tokens[currentIndex];
    }
    return Token(TokenType::EOF_TOKEN);
}

Token CHTLJSParser::peek(size_t offset) const {
    size_t index = currentIndex + offset - 1;
    if (index < tokens.size()) {
        return tokens[index];
    }
    return Token(TokenType::EOF_TOKEN);
}

bool CHTLJSParser::hasNext() const {
    return currentIndex < tokens.size();
}

bool CHTLJSParser::hasNext(size_t offset) const {
    return (currentIndex + offset - 1) < tokens.size();
}

void CHTLJSParser::advance() {
    if (currentIndex < tokens.size()) {
        currentIndex++;
    }
}

bool CHTLJSParser::match(TokenType type) const {
    if (isAtEnd()) return false;
    return getCurrent().type == type;
}

bool CHTLJSParser::match(const std::vector<TokenType>& types) const {
    if (isAtEnd()) return false;
    TokenType currentType = getCurrent().type;
    return std::find(types.begin(), types.end(), currentType) != types.end();
}

Token CHTLJSParser::consume(TokenType type, const std::string& message) {
    if (match(type)) {
        Token token = getCurrent();
        advance();
        return token;
    }
    error(message);
    return Token();
}

bool CHTLJSParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return getCurrent().type == type;
}

bool CHTLJSParser::isAtEnd() const {
    return getCurrent().type == TokenType::EOF_TOKEN;
}

void CHTLJSParser::error(const std::string& message) const {
    Token token = getCurrent();
    std::ostringstream oss;
    oss << "Parser error at line " << token.line 
        << ", column " << token.column << ": " << message;
    throw std::runtime_error(oss.str());
}

void CHTLJSParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (getCurrent().type == TokenType::SEMICOLON) {
            advance();
            break;
        }
        
        switch (getCurrent().type) {
            case TokenType::CLASS:
            case TokenType::FUNCTION:
            case TokenType::VAR:
            case TokenType::LET:
            case TokenType::CONST:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
                return;
            default:
                advance();
                break;
        }
    }
}

std::shared_ptr<ASTNode> CHTLJSParser::parseProgram() {
    auto program = std::make_shared<ProgramNode>();
    
    while (!isAtEnd()) {
        try {
            auto statement = parseStatement();
            if (statement) {
                program->addChild(statement);
            }
        } catch (const std::exception& e) {
            std::cerr << "Parse error: " << e.what() << std::endl;
            synchronize();
        }
    }
    
    return program;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseStatement() {
    if (match(TokenType::VAR) || match(TokenType::LET) || match(TokenType::CONST)) {
        return parseVariableDeclaration();
    }
    
    if (match(TokenType::FUNCTION)) {
        return parseFunctionDeclaration();
    }
    
    if (match(TokenType::IF)) {
        return parseIfStatement();
    }
    
    if (match(TokenType::WHILE)) {
        return parseWhileStatement();
    }
    
    if (match(TokenType::FOR)) {
        return parseForStatement();
    }
    
    if (match(TokenType::RETURN)) {
        return parseReturnStatement();
    }
    
    if (match(TokenType::BREAK)) {
        return parseBreakStatement();
    }
    
    if (match(TokenType::CONTINUE)) {
        return parseContinueStatement();
    }
    
    if (match(TokenType::TRY)) {
        return parseTryStatement();
    }
    
    if (match(TokenType::THROW)) {
        return parseThrowStatement();
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        return parseBlockStatement();
    }
    
    // CHTL JS 特有语句
    if (match(TokenType::VIR)) {
        return parseVirDeclaration();
    }
    
    return parseExpressionStatement();
}

std::shared_ptr<ASTNode> CHTLJSParser::parseExpression() {
    return parseAssignment();
}

std::shared_ptr<ASTNode> CHTLJSParser::parseExpressionStatement() {
    auto expression = parseExpression();
    if (match(TokenType::SEMICOLON)) {
        advance();
    }
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseBlockStatement() {
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto block = std::make_shared<ASTNode>(ASTNodeType::BLOCK_STATEMENT);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto statement = parseStatement();
        if (statement) {
            block->addChild(statement);
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return block;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseVariableDeclaration() {
    TokenType varType = getCurrent().type;
    advance(); // 跳过 var/let/const
    
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    auto declaration = std::make_shared<VariableDeclarationNode>(
        varType == TokenType::VAR ? "var" : 
        varType == TokenType::LET ? "let" : "const", 
        name.value
    );
    
    if (match(TokenType::ASSIGN)) {
        advance(); // 跳过 '='
        declaration->initializer = parseExpression();
    }
    
    return declaration;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseFunctionDeclaration() {
    consume(TokenType::FUNCTION, "Expected 'function'");
    
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    auto function = std::make_shared<FunctionDeclarationNode>(name.value);
    
    consume(TokenType::LEFT_PAREN, "Expected '('");
    function->parameters = parseParameterList();
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    function->body = parseBlockStatement();
    return function;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseIfStatement() {
    consume(TokenType::IF, "Expected 'if'");
    consume(TokenType::LEFT_PAREN, "Expected '('");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    auto ifStatement = std::make_shared<ASTNode>(ASTNodeType::IF_STATEMENT);
    ifStatement->setProperty("condition", condition);
    ifStatement->setProperty("then", parseStatement());
    
    if (match(TokenType::ELSE)) {
        advance();
        ifStatement->setProperty("else", parseStatement());
    }
    
    return ifStatement;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseWhileStatement() {
    consume(TokenType::WHILE, "Expected 'while'");
    consume(TokenType::LEFT_PAREN, "Expected '('");
    auto condition = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    auto whileStatement = std::make_shared<ASTNode>(ASTNodeType::WHILE_STATEMENT);
    whileStatement->setProperty("condition", condition);
    whileStatement->setProperty("body", parseStatement());
    
    return whileStatement;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseForStatement() {
    consume(TokenType::FOR, "Expected 'for'");
    consume(TokenType::LEFT_PAREN, "Expected '('");
    
    auto forStatement = std::make_shared<ASTNode>(ASTNodeType::FOR_STATEMENT);
    
    if (!check(TokenType::SEMICOLON)) {
        forStatement->setProperty("init", parseExpression());
    }
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    if (!check(TokenType::SEMICOLON)) {
        forStatement->setProperty("condition", parseExpression());
    }
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    if (!check(TokenType::RIGHT_PAREN)) {
        forStatement->setProperty("increment", parseExpression());
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    forStatement->setProperty("body", parseStatement());
    return forStatement;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseReturnStatement() {
    consume(TokenType::RETURN, "Expected 'return'");
    
    auto returnStatement = std::make_shared<ASTNode>(ASTNodeType::RETURN_STATEMENT);
    if (!check(TokenType::SEMICOLON)) {
        returnStatement->setProperty("value", parseExpression());
    }
    
    return returnStatement;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseBreakStatement() {
    consume(TokenType::BREAK, "Expected 'break'");
    return std::make_shared<ASTNode>(ASTNodeType::BREAK_STATEMENT);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseContinueStatement() {
    consume(TokenType::CONTINUE, "Expected 'continue'");
    return std::make_shared<ASTNode>(ASTNodeType::CONTINUE_STATEMENT);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseTryStatement() {
    consume(TokenType::TRY, "Expected 'try'");
    
    auto tryStatement = std::make_shared<ASTNode>(ASTNodeType::TRY_STATEMENT);
    tryStatement->setProperty("try", parseBlockStatement());
    
    if (match(TokenType::CATCH)) {
        advance();
        consume(TokenType::LEFT_PAREN, "Expected '('");
        Token error = consume(TokenType::IDENTIFIER, "Expected error parameter");
        consume(TokenType::RIGHT_PAREN, "Expected ')'");
        tryStatement->setProperty("catch", parseBlockStatement());
    }
    
    if (match(TokenType::FINALLY)) {
        advance();
        tryStatement->setProperty("finally", parseBlockStatement());
    }
    
    return tryStatement;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseThrowStatement() {
    consume(TokenType::THROW, "Expected 'throw'");
    
    auto throwStatement = std::make_shared<ASTNode>(ASTNodeType::THROW_STATEMENT);
    throwStatement->setProperty("value", parseExpression());
    return throwStatement;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseAssignment() {
    auto expression = parseConditional();
    
    if (isAssignmentOperator(getCurrent().type)) {
        Token operatorToken = getCurrent();
        advance();
        auto value = parseAssignment();
        
        auto assignment = std::make_shared<BinaryExpressionNode>(
            expression, operatorToken.type, value
        );
        return assignment;
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseConditional() {
    auto expression = parseLogicalOr();
    
    if (match(TokenType::QUESTION)) {
        advance();
        auto thenBranch = parseExpression();
        consume(TokenType::COLON, "Expected ':'");
        auto elseBranch = parseExpression();
        
        auto conditional = std::make_shared<ASTNode>(ASTNodeType::CONDITIONAL_EXPRESSION);
        conditional->setProperty("condition", expression);
        conditional->setProperty("then", thenBranch);
        conditional->setProperty("else", elseBranch);
        return conditional;
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseLogicalOr() {
    auto expression = parseLogicalAnd();
    
    while (match(TokenType::OR)) {
        Token operatorToken = getCurrent();
        advance();
        auto right = parseLogicalAnd();
        expression = std::make_shared<BinaryExpressionNode>(
            expression, operatorToken.type, right
        );
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseLogicalAnd() {
    auto expression = parseEquality();
    
    while (match(TokenType::AND)) {
        Token operatorToken = getCurrent();
        advance();
        auto right = parseEquality();
        expression = std::make_shared<BinaryExpressionNode>(
            expression, operatorToken.type, right
        );
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseEquality() {
    auto expression = parseComparison();
    
    while (isEqualityOperator(getCurrent().type)) {
        Token operatorToken = getCurrent();
        advance();
        auto right = parseComparison();
        expression = std::make_shared<BinaryExpressionNode>(
            expression, operatorToken.type, right
        );
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseComparison() {
    auto expression = parseAddition();
    
    while (isComparisonOperator(getCurrent().type)) {
        Token operatorToken = getCurrent();
        advance();
        auto right = parseAddition();
        expression = std::make_shared<BinaryExpressionNode>(
            expression, operatorToken.type, right
        );
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseAddition() {
    auto expression = parseMultiplication();
    
    while (isAdditionOperator(getCurrent().type)) {
        Token operatorToken = getCurrent();
        advance();
        auto right = parseMultiplication();
        expression = std::make_shared<BinaryExpressionNode>(
            expression, operatorToken.type, right
        );
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseMultiplication() {
    auto expression = parseUnary();
    
    while (isMultiplicationOperator(getCurrent().type)) {
        Token operatorToken = getCurrent();
        advance();
        auto right = parseUnary();
        expression = std::make_shared<BinaryExpressionNode>(
            expression, operatorToken.type, right
        );
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseUnary() {
    if (isUnaryOperator(getCurrent().type)) {
        Token operatorToken = getCurrent();
        advance();
        auto right = parseUnary();
        return std::make_shared<BinaryExpressionNode>(
            nullptr, operatorToken.type, right
        );
    }
    
    return parseCall();
}

std::shared_ptr<ASTNode> CHTLJSParser::parsePrimary() {
    if (match(TokenType::FALSE)) {
        advance();
        return std::make_shared<LiteralNode>("false", TokenType::BOOLEAN);
    }
    
    if (match(TokenType::TRUE)) {
        advance();
        return std::make_shared<LiteralNode>("true", TokenType::BOOLEAN);
    }
    
    if (match(TokenType::NUMBER) || match(TokenType::STRING)) {
        Token token = getCurrent();
        advance();
        return std::make_shared<LiteralNode>(token.value, token.type);
    }
    
    if (match(TokenType::IDENTIFIER)) {
        Token token = getCurrent();
        advance();
        return std::make_shared<IdentifierNode>(token.value);
    }
    
    if (match(TokenType::LEFT_PAREN)) {
        advance();
        auto expression = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')'");
        return expression;
    }
    
    if (match(TokenType::LEFT_BRACE)) {
        return parseObjectExpression();
    }
    
    if (match(TokenType::LEFT_BRACKET)) {
        return parseArrayExpression();
    }
    
    // CHTL JS 特有语法
    if (match(TokenType::SELECTOR_START)) {
        return parseSelectorExpression();
    }
    
    if (match(TokenType::REACTIVE_START)) {
        return parseReactiveValueExpression();
    }
    
    if (match(TokenType::LISTEN)) {
        return parseListenExpression();
    }
    
    if (match(TokenType::ANIMATE)) {
        return parseAnimateExpression();
    }
    
    if (match(TokenType::ROUTER)) {
        return parseRouterExpression();
    }
    
    if (match(TokenType::SCRIPT_LOADER)) {
        return parseScriptLoaderExpression();
    }
    
    if (match(TokenType::DELEGATE)) {
        return parseDelegateExpression();
    }
    
    error("Expected expression");
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseCall() {
    auto expression = parseMember();
    
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            advance();
            auto call = std::make_shared<CallExpressionNode>(expression);
            call->arguments = parseArgumentList();
            consume(TokenType::RIGHT_PAREN, "Expected ')'");
            expression = call;
        } else if (match(TokenType::DOT) || match(TokenType::ARROW)) {
            bool isArrow = match(TokenType::ARROW);
            advance();
            Token name = consume(TokenType::IDENTIFIER, "Expected property name");
            auto property = std::make_shared<IdentifierNode>(name.value);
            expression = std::make_shared<MemberExpressionNode>(expression, property, false);
        } else if (match(TokenType::LEFT_BRACKET)) {
            advance();
            auto property = parseExpression();
            consume(TokenType::RIGHT_BRACKET, "Expected ']'");
            expression = std::make_shared<MemberExpressionNode>(expression, property, true);
        } else {
            break;
        }
    }
    
    return expression;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseMember() {
    return parsePrimary();
}

// CHTL JS 特有解析方法
std::shared_ptr<ASTNode> CHTLJSParser::parseVirDeclaration() {
    consume(TokenType::VIR, "Expected 'Vir'");
    
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    auto vir = std::make_shared<VirDeclarationNode>(name.value);
    
    consume(TokenType::ASSIGN, "Expected '='");
    vir->value = parseExpression();
    
    return vir;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseListenExpression() {
    consume(TokenType::LISTEN, "Expected 'Listen'");
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto listen = std::make_shared<ListenExpressionNode>();
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token event = consume(TokenType::IDENTIFIER, "Expected event name");
        consume(TokenType::COLON, "Expected ':'");
        auto handler = parseExpression();
        listen->events[event.value] = handler;
        
        if (match(TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return listen;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseAnimateExpression() {
    consume(TokenType::ANIMATE, "Expected 'Animate'");
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto animate = std::make_shared<AnimateExpressionNode>();
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token property = consume(TokenType::IDENTIFIER, "Expected property name");
        consume(TokenType::COLON, "Expected ':'");
        auto value = parseExpression();
        
        if (property.value == "target") {
            animate->target = value;
        } else if (property.value == "duration") {
            animate->duration = value;
        } else if (property.value == "easing") {
            animate->easing = value;
        } else if (property.value == "begin") {
            animate->begin = value;
        } else if (property.value == "end") {
            animate->end = value;
        } else if (property.value == "loop") {
            animate->loop = value;
        } else if (property.value == "direction") {
            animate->direction = value;
        } else if (property.value == "delay") {
            animate->delay = value;
        } else if (property.value == "callback") {
            animate->callback = value;
        } else if (property.value == "when") {
            consume(TokenType::LEFT_BRACKET, "Expected '['");
            while (!check(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
                auto whenItem = parseExpression();
                animate->when.push_back(whenItem);
                if (match(TokenType::COMMA)) {
                    advance();
                }
            }
            consume(TokenType::RIGHT_BRACKET, "Expected ']'");
        }
        
        if (match(TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return animate;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseRouterExpression() {
    consume(TokenType::ROUTER, "Expected 'Router'");
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto router = std::make_shared<ASTNode>(ASTNodeType::ROUTER_EXPRESSION);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token property = consume(TokenType::IDENTIFIER, "Expected property name");
        consume(TokenType::COLON, "Expected ':'");
        auto value = parseExpression();
        router->setProperty(property.value, value);
        
        if (match(TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return router;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseScriptLoaderExpression() {
    consume(TokenType::SCRIPT_LOADER, "Expected 'ScriptLoader'");
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto loader = std::make_shared<ASTNode>(ASTNodeType::SCRIPT_LOADER_EXPRESSION);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token property = consume(TokenType::IDENTIFIER, "Expected property name");
        consume(TokenType::COLON, "Expected ':'");
        auto value = parseExpression();
        loader->setProperty(property.value, value);
        
        if (match(TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return loader;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseDelegateExpression() {
    consume(TokenType::DELEGATE, "Expected 'Delegate'");
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto delegate = std::make_shared<ASTNode>(ASTNodeType::DELEGATE_EXPRESSION);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token property = consume(TokenType::IDENTIFIER, "Expected property name");
        consume(TokenType::COLON, "Expected ':'");
        auto value = parseExpression();
        delegate->setProperty(property.value, value);
        
        if (match(TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return delegate;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseSelectorExpression() {
    Token token = consume(TokenType::SELECTOR_START, "Expected selector");
    return std::make_shared<SelectorExpressionNode>(token.value);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseReactiveValueExpression() {
    Token token = consume(TokenType::REACTIVE_START, "Expected reactive value");
    return std::make_shared<ReactiveValueExpressionNode>(token.value);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseObjectExpression() {
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto object = std::make_shared<ASTNode>(ASTNodeType::OBJECT_EXPRESSION);
    
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto property = parseProperty();
        if (property) {
            object->addChild(property);
        }
        
        if (match(TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return object;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseArrayExpression() {
    consume(TokenType::LEFT_BRACKET, "Expected '['");
    
    auto array = std::make_shared<ASTNode>(ASTNodeType::ARRAY_EXPRESSION);
    
    while (!check(TokenType::RIGHT_BRACKET) && !isAtEnd()) {
        auto element = parseExpression();
        if (element) {
            array->addChild(element);
        }
        
        if (match(TokenType::COMMA)) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACKET, "Expected ']'");
    return array;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseProperty() {
    Token key = consume(TokenType::IDENTIFIER, "Expected property key");
    consume(TokenType::COLON, "Expected ':'");
    auto value = parseExpression();
    
    auto property = std::make_shared<ASTNode>(ASTNodeType::PROPERTY);
    property->setProperty("key", std::make_shared<IdentifierNode>(key.value));
    property->setProperty("value", value);
    
    return property;
}

std::vector<std::shared_ptr<ASTNode>> CHTLJSParser::parseArgumentList() {
    std::vector<std::shared_ptr<ASTNode>> arguments;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            auto argument = parseExpression();
            if (argument) {
                arguments.push_back(argument);
            }
        } while (match(TokenType::COMMA) && advance());
    }
    
    return arguments;
}

std::vector<std::string> CHTLJSParser::parseParameterList() {
    std::vector<std::string> parameters;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            Token param = consume(TokenType::IDENTIFIER, "Expected parameter name");
            parameters.push_back(param.value);
        } while (match(TokenType::COMMA) && advance());
    }
    
    return parameters;
}

std::string CHTLJSParser::getOperatorString(TokenType type) const {
    switch (type) {
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::MULTIPLY: return "*";
        case TokenType::DIVIDE: return "/";
        case TokenType::MODULO: return "%";
        case TokenType::POWER: return "**";
        case TokenType::EQUAL: return "==";
        case TokenType::NOT_EQUAL: return "!=";
        case TokenType::LESS: return "<";
        case TokenType::GREATER: return ">";
        case TokenType::LESS_EQUAL: return "<=";
        case TokenType::GREATER_EQUAL: return ">=";
        case TokenType::AND: return "&&";
        case TokenType::OR: return "||";
        case TokenType::ASSIGN: return "=";
        default: return "?";
    }
}

bool CHTLJSParser::isAssignmentOperator(TokenType type) const {
    return type == TokenType::ASSIGN;
}

bool CHTLJSParser::isLogicalOperator(TokenType type) const {
    return type == TokenType::AND || type == TokenType::OR;
}

bool CHTLJSParser::isEqualityOperator(TokenType type) const {
    return type == TokenType::EQUAL || type == TokenType::NOT_EQUAL;
}

bool CHTLJSParser::isComparisonOperator(TokenType type) const {
    return type == TokenType::LESS || type == TokenType::GREATER || 
           type == TokenType::LESS_EQUAL || type == TokenType::GREATER_EQUAL;
}

bool CHTLJSParser::isAdditionOperator(TokenType type) const {
    return type == TokenType::PLUS || type == TokenType::MINUS;
}

bool CHTLJSParser::isMultiplicationOperator(TokenType type) const {
    return type == TokenType::MULTIPLY || type == TokenType::DIVIDE || type == TokenType::MODULO;
}

bool CHTLJSParser::isUnaryOperator(TokenType type) const {
    return type == TokenType::MINUS || type == TokenType::NOT || type == TokenType::PLUS;
}

std::shared_ptr<ASTNode> CHTLJSParser::parse() {
    return parseProgram();
}

std::shared_ptr<ASTNode> CHTLJSParser::parse(const std::string& source) {
    CHTLJSLexer lexer(source);
    tokens = lexer.tokenize();
    currentIndex = 0;
    return parseProgram();
}

void CHTLJSParser::reset() {
    currentIndex = 0;
}

void CHTLJSParser::setTokens(const TokenList& tokenList) {
    tokens = tokenList;
    currentIndex = 0;
}

TokenList CHTLJSParser::getTokens() const {
    return tokens;
}

size_t CHTLJSParser::getCurrentIndex() const {
    return currentIndex;
}

bool CHTLJSParser::hasErrors() const {
    return false; // 简化实现
}

} // namespace CHTLJS