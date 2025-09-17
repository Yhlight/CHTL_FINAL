#include "CHTLJSParser.h"
#include <algorithm>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser() : position(0) {
}

CHTLJSParser::~CHTLJSParser() {
}

std::shared_ptr<ProgramNode> CHTLJSParser::parse(const std::vector<Token>& tokens) {
    setTokens(tokens);
    auto program = std::make_shared<ProgramNode>();
    
    while (!isAtEnd()) {
        try {
            auto statement = parseStatement();
            if (statement) {
                program->addStatement(statement);
            }
        } catch (const std::exception& e) {
            reportError(e.what());
            synchronize();
        }
    }
    
    return program;
}

void CHTLJSParser::setTokens(const std::vector<Token>& tokens) {
    this->tokens = tokens;
    reset();
}

void CHTLJSParser::reset() {
    position = 0;
    errors.clear();
}

bool CHTLJSParser::hasMoreTokens() const {
    return position < tokens.size();
}

Token CHTLJSParser::currentToken() const {
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0);
    }
    return tokens[position];
}

Token CHTLJSParser::peekToken() const {
    if (position + 1 >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0);
    }
    return tokens[position + 1];
}

void CHTLJSParser::advance() {
    if (!isAtEnd()) {
        position++;
    }
}

std::vector<std::string> CHTLJSParser::getErrors() const {
    return errors;
}

bool CHTLJSParser::hasErrors() const {
    return !errors.empty();
}

std::shared_ptr<ASTNode> CHTLJSParser::parseStatement() {
    if (isAtEnd()) {
        return nullptr;
    }
    
    Token token = currentToken();
    
    // 虚对象声明
    if (token.getType() == TokenType::VIR) {
        return parseVirtualObject();
    }
    
    // 函数调用
    if (token.getType() == TokenType::IDENTIFIER) {
        return parseFunctionCall();
    }
    
    // 监听器
    if (token.getType() == TokenType::LISTEN) {
        return parseListen();
    }
    
    // 动画
    if (token.getType() == TokenType::ANIMATE) {
        return parseAnimate();
    }
    
    // 路由
    if (token.getType() == TokenType::ROUTER) {
        return parseRouter();
    }
    
    // 脚本加载器
    if (token.getType() == TokenType::SCRIPT_LOADER) {
        return parseScriptLoader();
    }
    
    // 事件委托
    if (token.getType() == TokenType::DELEGATE) {
        return parseDelegate();
    }
    
    // iNeverAway
    if (token.getType() == TokenType::INEVERAWAY) {
        return parseINeverAway();
    }
    
    // util表达式
    if (token.getType() == TokenType::UTIL) {
        return parseUtilExpression();
    }
    
    // 事件绑定
    if (token.getType() == TokenType::AMPERSAND) {
        return parseEventBinding();
    }
    
    // 选择器
    if (token.getType() == TokenType::HASH || token.getType() == TokenType::PERCENT) {
        return parseSelector();
    }
    
    // 表达式
    return parseExpression();
}

std::shared_ptr<ASTNode> CHTLJSParser::parseExpression() {
    return parsePrimary();
}

std::shared_ptr<ASTNode> CHTLJSParser::parsePrimary() {
    Token token = currentToken();
    
    switch (token.getType()) {
        case TokenType::IDENTIFIER:
            advance();
            return std::make_shared<IdentifierNode>(token.getValue());
            
        case TokenType::STRING:
        case TokenType::NUMBER:
        case TokenType::BOOLEAN:
            advance();
            return std::make_shared<LiteralNode>(token.getValue(), 
                token.getType() == TokenType::STRING ? "string" : 
                token.getType() == TokenType::NUMBER ? "number" : "boolean");
                
        case TokenType::LEFT_BRACE:
            return parseObjectLiteral();
            
        case TokenType::HASH:
        case TokenType::PERCENT:
            return parseSelector();
            
        default:
            reportError("Unexpected token: " + token.getValue());
            return nullptr;
    }
}

std::shared_ptr<ASTNode> CHTLJSParser::parseObjectLiteral() {
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    auto object = std::make_shared<ObjectLiteralNode>();
    
    while (!isAtEnd() && currentToken().getType() != TokenType::RIGHT_BRACE) {
        // 解析键
        Token keyToken = currentToken();
        if (keyToken.getType() != TokenType::IDENTIFIER && 
            keyToken.getType() != TokenType::STRING) {
            reportError("Expected property key");
            break;
        }
        advance();
        
        std::string key = keyToken.getValue();
        if (keyToken.getType() == TokenType::STRING) {
            key = key.substr(1, key.length() - 2); // 移除引号
        }
        
        // 解析冒号
        if (currentToken().getType() == TokenType::COLON) {
            advance();
        }
        
        // 解析值
        auto value = parseExpression();
        if (value) {
            object->addProperty(key, value);
        }
        
        // 解析逗号
        if (currentToken().getType() == TokenType::COMMA) {
            advance();
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    return object;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseFunctionCall() {
    Token nameToken = currentToken();
    advance();
    
    auto functionCall = std::make_shared<FunctionCallNode>(nameToken.getValue());
    
    if (currentToken().getType() == TokenType::LEFT_PAREN) {
        advance(); // 跳过 '('
        
        while (!isAtEnd() && currentToken().getType() != TokenType::RIGHT_PAREN) {
            auto argument = parseExpression();
            if (argument) {
                functionCall->addArgument(argument);
            }
            
            if (currentToken().getType() == TokenType::COMMA) {
                advance();
            }
        }
        
        consume(TokenType::RIGHT_PAREN, "Expected ')'");
    }
    
    return functionCall;
}

std::shared_ptr<ASTNode> CHTLJSParser::parseVirtualObject() {
    consume(TokenType::VIR, "Expected 'Vir'");
    
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected virtual object name");
    std::string name = nameToken.getValue();
    
    auto definition = std::dynamic_pointer_cast<ObjectLiteralNode>(parseObjectLiteral());
    if (!definition) {
        reportError("Expected object literal for virtual object definition");
        return nullptr;
    }
    
    return std::make_shared<VirtualObjectNode>(name, definition);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseListen() {
    consume(TokenType::LISTEN, "Expected 'Listen'");
    
    auto events = std::dynamic_pointer_cast<ObjectLiteralNode>(parseObjectLiteral());
    if (!events) {
        reportError("Expected object literal for Listen events");
        return nullptr;
    }
    
    return std::make_shared<ListenNode>(events);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseAnimate() {
    consume(TokenType::ANIMATE, "Expected 'Animate'");
    
    auto properties = std::dynamic_pointer_cast<ObjectLiteralNode>(parseObjectLiteral());
    if (!properties) {
        reportError("Expected object literal for Animate properties");
        return nullptr;
    }
    
    return std::make_shared<AnimateNode>(properties);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseRouter() {
    consume(TokenType::ROUTER, "Expected 'Router'");
    
    auto properties = std::dynamic_pointer_cast<ObjectLiteralNode>(parseObjectLiteral());
    if (!properties) {
        reportError("Expected object literal for Router properties");
        return nullptr;
    }
    
    return std::make_shared<RouterNode>(properties);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseScriptLoader() {
    consume(TokenType::SCRIPT_LOADER, "Expected 'ScriptLoader'");
    
    auto properties = std::dynamic_pointer_cast<ObjectLiteralNode>(parseObjectLiteral());
    if (!properties) {
        reportError("Expected object literal for ScriptLoader properties");
        return nullptr;
    }
    
    return std::make_shared<ScriptLoaderNode>(properties);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseDelegate() {
    consume(TokenType::DELEGATE, "Expected 'Delegate'");
    
    auto properties = std::dynamic_pointer_cast<ObjectLiteralNode>(parseObjectLiteral());
    if (!properties) {
        reportError("Expected object literal for Delegate properties");
        return nullptr;
    }
    
    return std::make_shared<DelegateNode>(properties);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseINeverAway() {
    consume(TokenType::INEVERAWAY, "Expected 'iNeverAway'");
    
    auto functions = std::dynamic_pointer_cast<ObjectLiteralNode>(parseObjectLiteral());
    if (!functions) {
        reportError("Expected object literal for iNeverAway functions");
        return nullptr;
    }
    
    return std::make_shared<INeverAwayNode>(functions);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseUtilExpression() {
    consume(TokenType::UTIL, "Expected 'util'");
    
    auto condition = parseExpression();
    if (!condition) {
        reportError("Expected condition expression");
        return nullptr;
    }
    
    consume(TokenType::ARROW, "Expected '->'");
    consume(TokenType::CHANGE, "Expected 'change'");
    
    auto changeBlock = parseExpression();
    if (!changeBlock) {
        reportError("Expected change block");
        return nullptr;
    }
    
    consume(TokenType::ARROW, "Expected '->'");
    consume(TokenType::THEN, "Expected 'then'");
    
    auto thenBlock = parseExpression();
    if (!thenBlock) {
        reportError("Expected then block");
        return nullptr;
    }
    
    return std::make_shared<UtilExpressionNode>(condition, changeBlock, thenBlock);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseSelector() {
    Token token = currentToken();
    advance();
    
    std::string selector = token.getValue();
    
    // 处理选择器内容
    if (currentToken().getType() == TokenType::LEFT_BRACE) {
        advance(); // 跳过 '{'
        
        while (!isAtEnd() && currentToken().getType() != TokenType::RIGHT_BRACE) {
            selector += currentToken().getValue();
            advance();
        }
        
        if (currentToken().getType() == TokenType::RIGHT_BRACE) {
            advance();
        }
    }
    
    return std::make_shared<SelectorNode>(selector);
}

std::shared_ptr<ASTNode> CHTLJSParser::parseEventBinding() {
    consume(TokenType::AMPERSAND, "Expected '&'");
    consume(TokenType::EVENT_ARROW, "Expected '&->'");
    
    auto selector = std::dynamic_pointer_cast<SelectorNode>(parseSelector());
    if (!selector) {
        reportError("Expected selector for event binding");
        return nullptr;
    }
    
    Token eventType = consume(TokenType::IDENTIFIER, "Expected event type");
    consume(TokenType::COLON, "Expected ':'");
    
    auto handler = parseExpression();
    if (!handler) {
        reportError("Expected event handler");
        return nullptr;
    }
    
    return std::make_shared<EventBindingNode>(selector, eventType.getValue(), handler);
}

std::shared_ptr<ASTNode> CHTLJSParser::parsePropertyAccess() {
    auto object = parsePrimary();
    if (!object) {
        return nullptr;
    }
    
    if (currentToken().getType() == TokenType::ARROW) {
        advance(); // 跳过 '->'
        
        Token propertyToken = consume(TokenType::IDENTIFIER, "Expected property name");
        return std::make_shared<PropertyAccessNode>(object, propertyToken.getValue());
    }
    
    return object;
}

bool CHTLJSParser::match(TokenType type) const {
    return currentToken().getType() == type;
}

bool CHTLJSParser::match(const std::vector<TokenType>& types) const {
    TokenType current = currentToken().getType();
    return std::find(types.begin(), types.end(), current) != types.end();
}

bool CHTLJSParser::check(TokenType type) const {
    return currentToken().getType() == type;
}

Token CHTLJSParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        Token token = currentToken();
        advance();
        return token;
    }
    
    reportError(message);
    return Token(TokenType::UNKNOWN, "", 0, 0);
}

Token CHTLJSParser::consume(const std::string& message) {
    if (isAtEnd()) {
        reportError(message);
        return Token(TokenType::UNKNOWN, "", 0, 0);
    }
    
    Token token = currentToken();
    advance();
    return token;
}

bool CHTLJSParser::isAtEnd() const {
    return position >= tokens.size() || currentToken().getType() == TokenType::EOF_TOKEN;
}

void CHTLJSParser::reportError(const std::string& message) {
    Token token = currentToken();
    reportError(token, message);
}

void CHTLJSParser::reportError(const Token& token, const std::string& message) {
    std::string error = "Error at line " + std::to_string(token.getLine()) + 
                       ", column " + std::to_string(token.getColumn()) + ": " + message;
    errors.push_back(error);
}

void CHTLJSParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (currentToken().getType() == TokenType::SEMICOLON) {
            advance();
            break;
        }
        
        TokenType type = currentToken().getType();
        if (type == TokenType::VIR || type == TokenType::LISTEN || 
            type == TokenType::ANIMATE || type == TokenType::ROUTER ||
            type == TokenType::SCRIPT_LOADER || type == TokenType::DELEGATE ||
            type == TokenType::INEVERAWAY || type == TokenType::UTIL) {
            break;
        }
        
        advance();
    }
}

} // namespace CHTLJS