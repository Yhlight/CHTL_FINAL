#include "CHTL_JS/CHTLJSParser.h"
#include <stdexcept>

namespace CHTL {

CHTLJSParser::CHTLJSParser() 
    : current_token_(0), debug_mode_(false) {
}

std::shared_ptr<CHTLNode> CHTLJSParser::parse(const std::vector<CHTLJSToken>& tokens) {
    tokens_ = tokens;
    current_token_ = 0;
    errors_.clear();
    
    try {
        return parseRoot();
    } catch (const std::exception& e) {
        reportError(e.what());
        return nullptr;
    }
}

std::shared_ptr<CHTLNode> CHTLJSParser::parse(const std::string& source_code) {
    CHTLJSLexer lexer;
    auto tokens = lexer.tokenize(source_code);
    return parse(tokens);
}

void CHTLJSParser::reset() {
    tokens_.clear();
    current_token_ = 0;
    errors_.clear();
}

void CHTLJSParser::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
}

const std::vector<std::string>& CHTLJSParser::getErrors() const {
    return errors_;
}

void CHTLJSParser::clearErrors() {
    errors_.clear();
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseRoot() {
    auto root = std::make_shared<ElementNode>("root");
    root->setName("root");
    
    while (!isAtEnd() && current().type != CHTLJSTokenType::EOF_TOKEN) {
        try {
            auto node = parseStatement();
            if (node) {
                root->addChild(node);
            }
        } catch (const std::exception& e) {
            reportError(e.what());
            synchronize();
        }
    }
    
    return root;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseStatement() {
    if (isAtEnd()) return nullptr;
    
    CHTLJSToken current_token = current();
    
    // 检查CHTL JS特有关键字
    switch (current_token.type) {
        case CHTLJSTokenType::LISTEN:
            return parseListenFunction();
        case CHTLJSTokenType::ANIMATE:
            return parseAnimateFunction();
        case CHTLJSTokenType::DELEGATE:
            return parseDelegateFunction();
        case CHTLJSTokenType::ROUTER:
            return parseRouterFunction();
        case CHTLJSTokenType::FILELOADER:
            return parseFileloaderFunction();
        case CHTLJSTokenType::VIR:
            return parseVirDeclaration();
        case CHTLJSTokenType::INEVERAWAY:
            return parseINeverAwayFunction();
        case CHTLJSTokenType::UTIL:
            return parseUtilExpression();
        case CHTLJSTokenType::FUNCTION:
            return parseFunctionDeclaration();
        case CHTLJSTokenType::CONST:
        case CHTLJSTokenType::LET:
        case CHTLJSTokenType::VAR:
            return parseVariableDeclaration();
        case CHTLJSTokenType::IF:
            return parseIfStatement();
        case CHTLJSTokenType::FOR:
            return parseForStatement();
        case CHTLJSTokenType::WHILE:
            return parseWhileStatement();
        default:
            // 尝试解析表达式语句
            return parseExpression();
    }
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseExpression() {
    if (isAtEnd()) {
        return nullptr;
    }
    return parseAssignmentExpression();
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseAssignmentExpression() {
    auto left = parseConditionalExpression();
    
    if (match(CHTLJSTokenType::EQUAL) || 
        match(CHTLJSTokenType::PLUS_EQ) ||
        match(CHTLJSTokenType::MINUS_EQ) ||
        match(CHTLJSTokenType::MULTIPLY_EQ) ||
        match(CHTLJSTokenType::DIVIDE_EQ) ||
        match(CHTLJSTokenType::MODULO_EQ) ||
        match(CHTLJSTokenType::POWER_EQ)) {
        
        auto op = previous();
        auto right = parseConditionalExpression();
        
        auto assignment = std::make_shared<ElementNode>("assignment");
        assignment->setAttribute("operator", op.value);
        assignment->addChild(left);
        assignment->addChild(right);
        
        return assignment;
    }
    
    return left;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseConditionalExpression() {
    auto expr = parseLogicalOrExpression();
    
    if (match(CHTLJSTokenType::QUESTION)) {
        auto true_expr = parseExpression();
        if (!match(CHTLJSTokenType::COLON)) {
            error("Expected ':' in conditional expression");
        }
        auto false_expr = parseExpression();
        
        auto conditional = std::make_shared<ElementNode>("conditional");
        conditional->addChild(expr);
        conditional->addChild(true_expr);
        conditional->addChild(false_expr);
        
        return conditional;
    }
    
    return expr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseLogicalOrExpression() {
    auto expr = parseLogicalAndExpression();
    
    while (match(CHTLJSTokenType::OR)) {
        auto op = previous();
        auto right = parseLogicalAndExpression();
        
        auto logical_or = std::make_shared<ElementNode>("logical_or");
        logical_or->setAttribute("operator", op.value);
        logical_or->addChild(expr);
        logical_or->addChild(right);
        
        expr = logical_or;
    }
    
    return expr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseLogicalAndExpression() {
    auto expr = parseEqualityExpression();
    
    while (match(CHTLJSTokenType::AND)) {
        auto op = previous();
        auto right = parseEqualityExpression();
        
        auto logical_and = std::make_shared<ElementNode>("logical_and");
        logical_and->setAttribute("operator", op.value);
        logical_and->addChild(expr);
        logical_and->addChild(right);
        
        expr = logical_and;
    }
    
    return expr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseEqualityExpression() {
    auto expr = parseRelationalExpression();
    
    while (match(CHTLJSTokenType::EQ) || 
           match(CHTLJSTokenType::NE) ||
           match(CHTLJSTokenType::STRICT_EQ) ||
           match(CHTLJSTokenType::STRICT_NE)) {
        
        auto op = previous();
        auto right = parseRelationalExpression();
        
        auto equality = std::make_shared<ElementNode>("equality");
        equality->setAttribute("operator", op.value);
        equality->addChild(expr);
        equality->addChild(right);
        
        expr = equality;
    }
    
    return expr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseRelationalExpression() {
    auto expr = parseAdditiveExpression();
    
    while (match(CHTLJSTokenType::GT) ||
           match(CHTLJSTokenType::LT) ||
           match(CHTLJSTokenType::GE) ||
           match(CHTLJSTokenType::LE)) {
        
        auto op = previous();
        auto right = parseAdditiveExpression();
        
        auto relational = std::make_shared<ElementNode>("relational");
        relational->setAttribute("operator", op.value);
        relational->addChild(expr);
        relational->addChild(right);
        
        expr = relational;
    }
    
    return expr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseAdditiveExpression() {
    auto expr = parseMultiplicativeExpression();
    
    while (match(CHTLJSTokenType::PLUS) || match(CHTLJSTokenType::MINUS)) {
        auto op = previous();
        auto right = parseMultiplicativeExpression();
        
        auto additive = std::make_shared<ElementNode>("additive");
        additive->setAttribute("operator", op.value);
        additive->addChild(expr);
        additive->addChild(right);
        
        expr = additive;
    }
    
    return expr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseMultiplicativeExpression() {
    auto expr = parseUnaryExpression();
    
    while (match(CHTLJSTokenType::MULTIPLY) ||
           match(CHTLJSTokenType::DIVIDE) ||
           match(CHTLJSTokenType::MODULO) ||
           match(CHTLJSTokenType::POWER)) {
        
        auto op = previous();
        auto right = parseUnaryExpression();
        
        auto multiplicative = std::make_shared<ElementNode>("multiplicative");
        multiplicative->setAttribute("operator", op.value);
        multiplicative->addChild(expr);
        multiplicative->addChild(right);
        
        expr = multiplicative;
    }
    
    return expr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseUnaryExpression() {
    if (match(CHTLJSTokenType::NOT) ||
        match(CHTLJSTokenType::MINUS) ||
        match(CHTLJSTokenType::PLUS) ||
        match(CHTLJSTokenType::INCREMENT) ||
        match(CHTLJSTokenType::DECREMENT)) {
        
        auto op = previous();
        auto expr = parseUnaryExpression();
        
        auto unary = std::make_shared<ElementNode>("unary");
        unary->setAttribute("operator", op.value);
        unary->addChild(expr);
        
        return unary;
    }
    
    return parsePrimaryExpression();
}

std::shared_ptr<CHTLNode> CHTLJSParser::parsePrimaryExpression() {
    if (isAtEnd()) {
        error("Unexpected end of input");
        return nullptr;
    }
    
    CHTLJSToken current_token = current();
    
    switch (current_token.type) {
        case CHTLJSTokenType::IDENTIFIER:
            return parseMemberAccess();
        case CHTLJSTokenType::STRING:
            consume();
            return std::make_shared<TextNode>(current_token.value);
        case CHTLJSTokenType::NUMBER:
            consume();
            return std::make_shared<TextNode>(current_token.value);
        case CHTLJSTokenType::LBRACE:
            // 在parsePrimaryExpression中不应该处理左大括号
            // 这应该由调用者处理
            error("Unexpected '{' in expression");
            return nullptr;
        case CHTLJSTokenType::LBRACKET:
            return parseArrayLiteral();
        case CHTLJSTokenType::LPAREN: {
            consume(); // 消费 (
            auto expr = parseExpression();
            if (!match(CHTLJSTokenType::RPAREN)) {
                error("Expected ')' after expression");
            }
            return expr;
        }
        default:
            error("Unexpected token: " + current_token.value);
            return nullptr;
    }
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseFunctionCall() {
    auto name = parseIdentifier();
    auto function_call = std::make_shared<ElementNode>("function_call");
    function_call->setName(name);
    
    if (match(CHTLJSTokenType::LPAREN)) {
        while (!check(CHTLJSTokenType::RPAREN) && !isAtEnd()) {
            auto arg = parseExpression();
            if (arg) {
                function_call->addChild(arg);
            }
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RPAREN)) {
            error("Expected ')' after function arguments");
        }
    }
    
    return function_call;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseMemberAccess() {
    auto identifier = parseIdentifier();
    
    if (match(CHTLJSTokenType::DOT) || match(CHTLJSTokenType::ARROW)) {
        auto op = previous();
        auto member = parseIdentifier();
        
        auto member_access = std::make_shared<ElementNode>("member_access");
        member_access->setAttribute("operator", op.value);
        member_access->setName(identifier);
        member_access->setTextContent(member);
        
        return member_access;
    }
    
    if (match(CHTLJSTokenType::LBRACKET)) {
        auto index = parseExpression();
        if (!match(CHTLJSTokenType::RBRACKET)) {
            error("Expected ']' after array index");
        }
        
        auto array_access = std::make_shared<ElementNode>("array_access");
        array_access->setName(identifier);
        array_access->addChild(index);
        
        return array_access;
    }
    
    if (match(CHTLJSTokenType::LPAREN)) {
        return parseFunctionCall();
    }
    
    auto identifier_node = std::make_shared<ElementNode>("identifier");
    identifier_node->setName(identifier);
    return identifier_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseArrayAccess() {
    auto identifier = parseIdentifier();
    
    if (match(CHTLJSTokenType::LBRACKET)) {
        auto index = parseExpression();
        if (!match(CHTLJSTokenType::RBRACKET)) {
            error("Expected ']' after array index");
        }
        
        auto array_access = std::make_shared<ElementNode>("array_access");
        array_access->setName(identifier);
        array_access->addChild(index);
        
        return array_access;
    }
    
    auto identifier_node = std::make_shared<ElementNode>("identifier");
    identifier_node->setName(identifier);
    return identifier_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseCHTLJSFunction() {
    // 通用的CHTL JS函数解析
    return parseObjectLiteral();
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseListenFunction() {
    consume(); // 消费 listen
    
    auto listen_node = std::make_shared<ElementNode>("listen_function");
    
    // 检查是否有左大括号
    if (check(CHTLJSTokenType::LBRACE)) {
        match(CHTLJSTokenType::LBRACE); // 消费左大括号
        
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            if (check(CHTLJSTokenType::IDENTIFIER)) {
                auto event = parseIdentifier();
                if (match(CHTLJSTokenType::COLON)) {
                    // 暂时跳过处理程序，只创建事件处理器节点
                    auto event_handler = std::make_shared<ElementNode>("event_handler");
                    event_handler->setName(event);
                    listen_node->addChild(event_handler);
                }
            }
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after listen function body");
        }
    }
    
    return listen_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseAnimateFunction() {
    consume(); // 消费 animate
    
    auto animate_node = std::make_shared<ElementNode>("animate_function");
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto property = parseIdentifier();
            if (match(CHTLJSTokenType::COLON)) {
                auto value = parseExpression();
                
                auto property_value = std::make_shared<ElementNode>("property_value");
                property_value->setName(property);
                property_value->addChild(value);
                animate_node->addChild(property_value);
            }
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after animate function body");
        }
    }
    
    return animate_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseDelegateFunction() {
    consume(); // 消费 delegate
    
    auto delegate_node = std::make_shared<ElementNode>("delegate_function");
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto property = parseIdentifier();
            if (match(CHTLJSTokenType::COLON)) {
                auto value = parseExpression();
                
                auto property_value = std::make_shared<ElementNode>("property_value");
                property_value->setName(property);
                property_value->addChild(value);
                delegate_node->addChild(property_value);
            }
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after delegate function body");
        }
    }
    
    return delegate_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseRouterFunction() {
    consume(); // 消费 router
    
    auto router_node = std::make_shared<ElementNode>("router_function");
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto property = parseIdentifier();
            if (match(CHTLJSTokenType::COLON)) {
                auto value = parseExpression();
                
                auto property_value = std::make_shared<ElementNode>("property_value");
                property_value->setName(property);
                property_value->addChild(value);
                router_node->addChild(property_value);
            }
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after router function body");
        }
    }
    
    return router_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseFileloaderFunction() {
    consume(); // 消费 fileloader
    
    auto fileloader_node = std::make_shared<ElementNode>("fileloader_function");
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto property = parseIdentifier();
            if (match(CHTLJSTokenType::COLON)) {
                auto value = parseExpression();
                
                auto property_value = std::make_shared<ElementNode>("property_value");
                property_value->setName(property);
                property_value->addChild(value);
                fileloader_node->addChild(property_value);
            }
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after fileloader function body");
        }
    }
    
    return fileloader_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseVirDeclaration() {
    consume(); // 消费 vir
    
    auto name = parseIdentifier();
    auto vir_node = std::make_shared<ElementNode>("vir_declaration");
    vir_node->setName(name);
    
    if (match(CHTLJSTokenType::EQUAL)) {
        auto value = parseExpression();
        vir_node->addChild(value);
    }
    
    return vir_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseINeverAwayFunction() {
    consume(); // 消费 iNeverAway
    
    auto ineveraway_node = std::make_shared<ElementNode>("ineveraway_function");
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto property = parseIdentifier();
            if (match(CHTLJSTokenType::COLON)) {
                auto value = parseExpression();
                
                auto property_value = std::make_shared<ElementNode>("property_value");
                property_value->setName(property);
                property_value->addChild(value);
                ineveraway_node->addChild(property_value);
            }
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after iNeverAway function body");
        }
    }
    
    return ineveraway_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseUtilExpression() {
    consume(); // 消费 util
    
    auto condition = parseExpression();
    
    if (!match(CHTLJSTokenType::ARROW)) {
        error("Expected '->' after util condition");
    }
    
    auto change_expr = parseExpression();
    
    if (!match(CHTLJSTokenType::ARROW)) {
        error("Expected '->' after util change expression");
    }
    
    auto then_expr = parseExpression();
    
    auto util_node = std::make_shared<ElementNode>("util_expression");
    util_node->addChild(condition);
    util_node->addChild(change_expr);
    util_node->addChild(then_expr);
    
    return util_node;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseEnhancedSelector() {
    // 解析增强选择器 {{selector}}
    if (match(CHTLJSTokenType::LBRACE) && match(CHTLJSTokenType::LBRACE)) {
        auto selector = parseIdentifier();
        
        if (!match(CHTLJSTokenType::RBRACE) || !match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}}' after enhanced selector");
        }
        
        auto selector_node = std::make_shared<ElementNode>("enhanced_selector");
        selector_node->setName(selector);
        
        return selector_node;
    }
    
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseObjectLiteral() {
    if (!match(CHTLJSTokenType::LBRACE)) {
        return nullptr;
    }
    
    auto object = std::make_shared<ElementNode>("object_literal");
    
    while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
        auto key = parseIdentifier();
        if (match(CHTLJSTokenType::COLON)) {
            auto value = parseExpression();
            
            auto property = std::make_shared<ElementNode>("property");
            property->setName(key);
            property->addChild(value);
            object->addChild(property);
        }
        
        if (!match(CHTLJSTokenType::COMMA)) {
            break;
        }
    }
    
    if (!match(CHTLJSTokenType::RBRACE)) {
        error("Expected '}' after object literal");
    }
    
    return object;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseArrayLiteral() {
    if (!match(CHTLJSTokenType::LBRACKET)) {
        return nullptr;
    }
    
    auto array = std::make_shared<ElementNode>("array_literal");
    
    while (!check(CHTLJSTokenType::RBRACKET) && !isAtEnd()) {
        auto element = parseExpression();
        if (element) {
            array->addChild(element);
        }
        
        if (!match(CHTLJSTokenType::COMMA)) {
            break;
        }
    }
    
    if (!match(CHTLJSTokenType::RBRACKET)) {
        error("Expected ']' after array literal");
    }
    
    return array;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseFunctionDeclaration() {
    consume(); // 消费 function
    
    auto name = parseIdentifier();
    auto function = std::make_shared<ElementNode>("function_declaration");
    function->setName(name);
    
    if (match(CHTLJSTokenType::LPAREN)) {
        while (!check(CHTLJSTokenType::RPAREN) && !isAtEnd()) {
            auto param = parseIdentifier();
            auto param_node = std::make_shared<ElementNode>("parameter");
            param_node->setName(param);
            function->addChild(param_node);
            
            if (!match(CHTLJSTokenType::COMMA)) {
                break;
            }
        }
        
        if (!match(CHTLJSTokenType::RPAREN)) {
            error("Expected ')' after function parameters");
        }
    }
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            if (check(CHTLJSTokenType::RBRACE)) {
                break;
            }
            auto statement = parseStatement();
            if (statement) {
                function->addChild(statement);
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after function body");
        }
    }
    
    return function;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseVariableDeclaration() {
    auto type = current().type;
    consume(); // 消费 const/let/var
    
    auto name = parseIdentifier();
    auto variable = std::make_shared<ElementNode>("variable_declaration");
    variable->setName(name);
    variable->setAttribute("type", type == CHTLJSTokenType::CONST ? "const" : 
                          type == CHTLJSTokenType::LET ? "let" : "var");
    
    if (match(CHTLJSTokenType::EQUAL)) {
        auto value = parseExpression();
        variable->addChild(value);
    }
    
    return variable;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseIfStatement() {
    consume(); // 消费 if
    
    if (!match(CHTLJSTokenType::LPAREN)) {
        error("Expected '(' after if");
    }
    
    auto condition = parseExpression();
    
    if (!match(CHTLJSTokenType::RPAREN)) {
        error("Expected ')' after if condition");
    }
    
    auto if_stmt = std::make_shared<ElementNode>("if_statement");
    if_stmt->addChild(condition);
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto statement = parseStatement();
            if (statement) {
                if_stmt->addChild(statement);
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after if body");
        }
    }
    
    if (match(CHTLJSTokenType::ELSE)) {
        if (match(CHTLJSTokenType::LBRACE)) {
            while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
                auto statement = parseStatement();
                if (statement) {
                    if_stmt->addChild(statement);
                }
            }
            
            if (!match(CHTLJSTokenType::RBRACE)) {
                error("Expected '}' after else body");
            }
        }
    }
    
    return if_stmt;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseForStatement() {
    consume(); // 消费 for
    
    if (!match(CHTLJSTokenType::LPAREN)) {
        error("Expected '(' after for");
    }
    
    auto for_stmt = std::make_shared<ElementNode>("for_statement");
    
    // 解析初始化
    if (!check(CHTLJSTokenType::SEMICOLON)) {
        auto init = parseStatement();
        if (init) {
            for_stmt->addChild(init);
        }
    }
    
    if (!match(CHTLJSTokenType::SEMICOLON)) {
        error("Expected ';' after for initialization");
    }
    
    // 解析条件
    if (!check(CHTLJSTokenType::SEMICOLON)) {
        auto condition = parseExpression();
        for_stmt->addChild(condition);
    }
    
    if (!match(CHTLJSTokenType::SEMICOLON)) {
        error("Expected ';' after for condition");
    }
    
    // 解析更新
    if (!check(CHTLJSTokenType::RPAREN)) {
        auto update = parseExpression();
        for_stmt->addChild(update);
    }
    
    if (!match(CHTLJSTokenType::RPAREN)) {
        error("Expected ')' after for update");
    }
    
    // 解析循环体
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto statement = parseStatement();
            if (statement) {
                for_stmt->addChild(statement);
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after for body");
        }
    }
    
    return for_stmt;
}

std::shared_ptr<CHTLNode> CHTLJSParser::parseWhileStatement() {
    consume(); // 消费 while
    
    if (!match(CHTLJSTokenType::LPAREN)) {
        error("Expected '(' after while");
    }
    
    auto condition = parseExpression();
    
    if (!match(CHTLJSTokenType::RPAREN)) {
        error("Expected ')' after while condition");
    }
    
    auto while_stmt = std::make_shared<ElementNode>("while_statement");
    while_stmt->addChild(condition);
    
    if (match(CHTLJSTokenType::LBRACE)) {
        while (!check(CHTLJSTokenType::RBRACE) && !isAtEnd()) {
            auto statement = parseStatement();
            if (statement) {
                while_stmt->addChild(statement);
            }
        }
        
        if (!match(CHTLJSTokenType::RBRACE)) {
            error("Expected '}' after while body");
        }
    }
    
    return while_stmt;
}

std::string CHTLJSParser::parseIdentifier() {
    if (check(CHTLJSTokenType::IDENTIFIER)) {
        std::string identifier = current().value;
        consume();
        return identifier;
    }
    
    error("Expected identifier");
    return "";
}

std::string CHTLJSParser::parseString() {
    if (check(CHTLJSTokenType::STRING)) {
        std::string str = current().value;
        consume();
        return str;
    }
    
    error("Expected string");
    return "";
}

double CHTLJSParser::parseNumber() {
    if (check(CHTLJSTokenType::NUMBER)) {
        double num = std::stod(current().value);
        consume();
        return num;
    }
    
    error("Expected number");
    return 0.0;
}

bool CHTLJSParser::check(CHTLJSTokenType type) const {
    if (isAtEnd()) return false;
    return current().type == type;
}

bool CHTLJSParser::match(CHTLJSTokenType type) {
    if (check(type)) {
        consume();
        return true;
    }
    return false;
}

CHTLJSToken CHTLJSParser::consume() {
    if (isAtEnd()) {
        return CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    
    return tokens_[current_token_++];
}

CHTLJSToken CHTLJSParser::consume(CHTLJSTokenType type) {
    if (check(type)) {
        return consume();
    }
    
    error("Expected " + std::to_string(static_cast<int>(type)) + " but got " + std::to_string(static_cast<int>(current().type)));
    return CHTLJSToken();
}

const CHTLJSToken& CHTLJSParser::current() const {
    if (isAtEnd()) {
        static CHTLJSToken eof_token(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0);
        return eof_token;
    }
    return tokens_[current_token_];
}

const CHTLJSToken& CHTLJSParser::previous() const {
    if (current_token_ == 0) {
        static CHTLJSToken empty_token(CHTLJSTokenType::ERROR, "", 0, 0, 0);
        return empty_token;
    }
    return tokens_[current_token_ - 1];
}

bool CHTLJSParser::isAtEnd() const {
    return current_token_ >= tokens_.size() || (current_token_ < tokens_.size() && tokens_[current_token_].type == CHTLJSTokenType::EOF_TOKEN);
}

void CHTLJSParser::synchronize() {
    consume();
    
    while (!isAtEnd()) {
        if (previous().type == CHTLJSTokenType::SEMICOLON) return;
        
        switch (current().type) {
            case CHTLJSTokenType::FUNCTION:
            case CHTLJSTokenType::CONST:
            case CHTLJSTokenType::LET:
            case CHTLJSTokenType::VAR:
            case CHTLJSTokenType::IF:
            case CHTLJSTokenType::FOR:
            case CHTLJSTokenType::WHILE:
            case CHTLJSTokenType::RETURN:
                return;
            default:
                break;
        }
        
        consume();
    }
}

void CHTLJSParser::reportError(const std::string& message) {
    errors_.push_back("Error at line " + std::to_string(current().line) + 
                     ", column " + std::to_string(current().column) + ": " + message);
}

void CHTLJSParser::reportError(const CHTLJSToken& token, const std::string& message) {
    errors_.push_back("Error at line " + std::to_string(token.line) + 
                     ", column " + std::to_string(token.column) + ": " + message);
}

void CHTLJSParser::error(const std::string& message) {
    reportError(message);
    throw std::runtime_error(message);
}

void CHTLJSParser::error(const CHTLJSToken& token, const std::string& message) {
    reportError(token, message);
    throw std::runtime_error(message);
}

} // namespace CHTL