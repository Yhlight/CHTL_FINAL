#include "CHTLJSParser.h"
#include "CHTL/CHTLNode/ScriptNode.h"
#include <iostream>

namespace CHTL {

CHTLJSParser::CHTLJSParser(const std::vector<CHTLJSToken>& tokens)
    : tokens(tokens), currentTokenIndex(0) {}

CHTLJSToken CHTLJSParser::peek(int offset) const {
    if (currentTokenIndex + offset >= tokens.size()) {
        return CHTLJSToken(CHTLJSTokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return tokens[currentTokenIndex + offset];
}

CHTLJSToken CHTLJSParser::consume(CHTLJSTokenType type) {
    CHTLJSToken token = peek();
    if (token.type == type) {
        currentTokenIndex++;
        return token;
    }
    throwError("Expected token type " + CHTLJSToken::tokenTypeToString(type) + 
               ", but got " + CHTLJSToken::tokenTypeToString(token.type));
}

CHTLJSToken CHTLJSParser::consumeIdentifier() {
    CHTLJSToken token = peek();
    if (token.type == CHTLJSTokenType::IDENTIFIER) {
        currentTokenIndex++;
        return token;
    }
    throwError("Expected an identifier, but got " + CHTLJSToken::tokenTypeToString(token.type));
}

bool CHTLJSParser::match(CHTLJSTokenType type) {
    return peek().type == type;
}

void CHTLJSParser::throwError(const std::string& message) const {
    const CHTLJSToken& token = peek();
    throw std::runtime_error("CHTL JS Parsing Error at Line " + std::to_string(token.line) + 
                            ", Column " + std::to_string(token.column) + ": " + message);
}

std::shared_ptr<BaseNode> CHTLJSParser::parse() {
    return parseProgram();
}

std::shared_ptr<BaseNode> CHTLJSParser::parseProgram() {
    auto programNode = std::make_shared<BaseNode>(NodeType::Program);
    
    while (peek().type != CHTLJSTokenType::EOF_TOKEN) {
        programNode->children.push_back(parseStatement());
    }
    
    return programNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseStatement() {
    CHTLJSToken token = peek();
    
    switch (token.type) {
        case CHTLJSTokenType::FILELOADER:
            return parseFileLoader();
        case CHTLJSTokenType::LISTEN:
            return parseListen();
        case CHTLJSTokenType::DELEGATE:
            return parseDelegate();
        case CHTLJSTokenType::ANIMATE:
            return parseAnimate();
        case CHTLJSTokenType::VIR:
            return parseVir();
        case CHTLJSTokenType::ROUTER:
            return parseRouter();
        case CHTLJSTokenType::UTIL:
            return parseUtil();
        case CHTLJSTokenType::SELECTOR:
            return parseSelector();
        default:
            return parseExpression();
    }
}

std::shared_ptr<BaseNode> CHTLJSParser::parseExpression() {
    return parseConditionalExpression();
}

std::shared_ptr<BaseNode> CHTLJSParser::parseFileLoader() {
    consume(CHTLJSTokenType::FILELOADER);
    consume(CHTLJSTokenType::LEFT_BRACE);
    
    auto fileLoaderNode = std::make_shared<ScriptNode>(ScriptType::FILELOADER);
    std::vector<std::string> paths;
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
        if (peek().type == CHTLJSTokenType::STRING) {
            CHTLJSToken pathToken = consume(CHTLJSTokenType::STRING);
            paths.push_back(pathToken.value);
        } else if (peek().type == CHTLJSTokenType::COMMA) {
            consume(CHTLJSTokenType::COMMA);
        } else {
            throwError("Expected file path or comma in fileloader");
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACE);
    
    for (const auto& path : paths) {
        fileLoaderNode->addFileLoaderPath(path);
    }
    
    return fileLoaderNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseListen() {
    consume(CHTLJSTokenType::LISTEN);
    consume(CHTLJSTokenType::LEFT_BRACE);
    
    auto listenNode = std::make_shared<ScriptNode>(ScriptType::LISTEN);
    std::map<std::string, std::string> eventHandlers;
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
        if (peek().type == CHTLJSTokenType::IDENTIFIER) {
            CHTLJSToken eventToken = consumeIdentifier();
            consume(CHTLJSTokenType::COLON);
            
            // 解析事件处理函数
            std::string handler;
            if (peek().type == CHTLJSTokenType::LEFT_PAREN) {
                // 箭头函数
                consume(CHTLJSTokenType::LEFT_PAREN);
                handler += "(";
                while (peek().type != CHTLJSTokenType::RIGHT_PAREN) {
                    handler += peek().value;
                    currentTokenIndex++;
                }
                consume(CHTLJSTokenType::RIGHT_PAREN);
                handler += ")";
                consume(CHTLJSTokenType::ARROW);
                consume(CHTLJSTokenType::LEFT_BRACE);
                handler += " => {";
                while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
                    handler += peek().value;
                    currentTokenIndex++;
                }
                consume(CHTLJSTokenType::RIGHT_BRACE);
                handler += "}";
            } else {
                // 函数名
                handler = consumeIdentifier().value;
            }
            
            eventHandlers[eventToken.value] = handler;
            
            if (peek().type == CHTLJSTokenType::COMMA) {
                consume(CHTLJSTokenType::COMMA);
            }
        } else {
            throwError("Expected event name in listen block");
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACE);
    
    for (const auto& handler : eventHandlers) {
        listenNode->addEventListener(handler.first, handler.second);
    }
    
    return listenNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseDelegate() {
    consume(CHTLJSTokenType::DELEGATE);
    consume(CHTLJSTokenType::LEFT_BRACE);
    
    auto delegateNode = std::make_shared<ScriptNode>(ScriptType::DELEGATE);
    std::map<std::string, std::string> delegates;
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
        if (peek().type == CHTLJSTokenType::IDENTIFIER) {
            CHTLJSToken targetToken = consumeIdentifier();
            consume(CHTLJSTokenType::COLON);
            
            // 解析委托配置
            std::string config;
            if (peek().type == CHTLJSTokenType::STRING) {
                config = consume(CHTLJSTokenType::STRING).value;
            } else {
                config = consumeIdentifier().value;
            }
            
            delegates[targetToken.value] = config;
            
            if (peek().type == CHTLJSTokenType::COMMA) {
                consume(CHTLJSTokenType::COMMA);
            }
        } else {
            throwError("Expected target in delegate block");
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACE);
    
    for (const auto& delegate : delegates) {
        delegateNode->addDelegate(delegate.first, delegate.second);
    }
    
    return delegateNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseAnimate() {
    consume(CHTLJSTokenType::ANIMATE);
    consume(CHTLJSTokenType::LEFT_BRACE);
    
    auto animateNode = std::make_shared<ScriptNode>(ScriptType::ANIMATE);
    std::map<std::string, std::string> animations;
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
        if (peek().type == CHTLJSTokenType::IDENTIFIER) {
            CHTLJSToken propToken = consumeIdentifier();
            consume(CHTLJSTokenType::COLON);
            
            // 解析动画配置
            std::string config;
            if (peek().type == CHTLJSTokenType::STRING) {
                config = consume(CHTLJSTokenType::STRING).value;
            } else if (peek().type == CHTLJSTokenType::NUMBER) {
                config = consume(CHTLJSTokenType::NUMBER).value;
            } else {
                config = consumeIdentifier().value;
            }
            
            animations[propToken.value] = config;
            
            if (peek().type == CHTLJSTokenType::COMMA) {
                consume(CHTLJSTokenType::COMMA);
            }
        } else {
            throwError("Expected property in animate block");
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACE);
    
    for (const auto& anim : animations) {
        animateNode->addAnimation(anim.first, anim.second);
    }
    
    return animateNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseVir() {
    consume(CHTLJSTokenType::VIR);
    CHTLJSToken nameToken = consumeIdentifier();
    consume(CHTLJSTokenType::EQUAL);
    consume(CHTLJSTokenType::LEFT_BRACE);
    
    auto virNode = std::make_shared<ScriptNode>(ScriptType::VIR);
    std::map<std::string, std::string> virtualObjects;
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
        if (peek().type == CHTLJSTokenType::IDENTIFIER) {
            CHTLJSToken propToken = consumeIdentifier();
            consume(CHTLJSTokenType::COLON);
            
            // 解析虚对象配置
            std::string config;
            if (peek().type == CHTLJSTokenType::LEFT_PAREN) {
                consume(CHTLJSTokenType::LEFT_PAREN);
                config += "(";
                while (peek().type != CHTLJSTokenType::RIGHT_PAREN) {
                    config += peek().value;
                    currentTokenIndex++;
                }
                consume(CHTLJSTokenType::RIGHT_PAREN);
                config += ")";
            } else {
                config = consumeIdentifier().value;
            }
            
            virtualObjects[propToken.value] = config;
            
            if (peek().type == CHTLJSTokenType::COMMA) {
                consume(CHTLJSTokenType::COMMA);
            }
        } else {
            throwError("Expected property in vir block");
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACE);
    
    for (const auto& vir : virtualObjects) {
        virNode->addVirtualObject(vir.first, vir.second);
    }
    
    return virNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseRouter() {
    consume(CHTLJSTokenType::ROUTER);
    consume(CHTLJSTokenType::LEFT_BRACE);
    
    auto routerNode = std::make_shared<ScriptNode>(ScriptType::ROUTER);
    std::map<std::string, std::string> routes;
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
        if (peek().type == CHTLJSTokenType::IDENTIFIER) {
            CHTLJSToken urlToken = consumeIdentifier();
            consume(CHTLJSTokenType::COLON);
            
            // 解析路由配置
            std::string config;
            if (peek().type == CHTLJSTokenType::STRING) {
                config = consume(CHTLJSTokenType::STRING).value;
            } else {
                config = consumeIdentifier().value;
            }
            
            routes[urlToken.value] = config;
            
            if (peek().type == CHTLJSTokenType::COMMA) {
                consume(CHTLJSTokenType::COMMA);
            }
        } else {
            throwError("Expected URL in router block");
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACE);
    
    for (const auto& route : routes) {
        routerNode->addRoute(route.first, route.second);
    }
    
    return routerNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseUtil() {
    consume(CHTLJSTokenType::UTIL);
    
    // 解析util表达式
    auto utilNode = std::make_shared<ScriptNode>(ScriptType::UTIL);
    std::string expression;
    
    while (peek().type != CHTLJSTokenType::SEMICOLON && peek().type != CHTLJSTokenType::EOF_TOKEN) {
        expression += peek().value;
        currentTokenIndex++;
    }
    
    if (peek().type == CHTLJSTokenType::SEMICOLON) {
        consume(CHTLJSTokenType::SEMICOLON);
    }
    
    utilNode->setScriptContent(expression);
    return utilNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseSelector() {
    CHTLJSToken token = consume(CHTLJSTokenType::SELECTOR);
    
    auto selectorNode = std::make_shared<ScriptNode>(ScriptType::INLINE);
    selectorNode->addSelector(token.value);
    selectorNode->setScriptContent("{{" + token.value + "}}");
    
    return selectorNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseFunctionCall() {
    CHTLJSToken nameToken = consumeIdentifier();
    consume(CHTLJSTokenType::LEFT_PAREN);
    
    auto funcNode = std::make_shared<BaseNode>(NodeType::FunctionCall);
    funcNode->setValue(nameToken.value);
    
    while (peek().type != CHTLJSTokenType::RIGHT_PAREN) {
        funcNode->children.push_back(parseExpression());
        if (peek().type == CHTLJSTokenType::COMMA) {
            consume(CHTLJSTokenType::COMMA);
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_PAREN);
    return funcNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseObjectLiteral() {
    consume(CHTLJSTokenType::LEFT_BRACE);
    
    auto objNode = std::make_shared<BaseNode>(NodeType::ObjectLiteral);
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACE) {
        objNode->children.push_back(parseProperty());
        if (peek().type == CHTLJSTokenType::COMMA) {
            consume(CHTLJSTokenType::COMMA);
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACE);
    return objNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseArrayLiteral() {
    consume(CHTLJSTokenType::LEFT_BRACKET);
    
    auto arrNode = std::make_shared<BaseNode>(NodeType::ArrayLiteral);
    
    while (peek().type != CHTLJSTokenType::RIGHT_BRACKET) {
        arrNode->children.push_back(parseExpression());
        if (peek().type == CHTLJSTokenType::COMMA) {
            consume(CHTLJSTokenType::COMMA);
        }
    }
    
    consume(CHTLJSTokenType::RIGHT_BRACKET);
    return arrNode;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseConditionalExpression() {
    auto left = parseLogicalExpression();
    
    if (match(CHTLJSTokenType::QUESTION)) {
        consume(CHTLJSTokenType::QUESTION);
        auto trueExpr = parseExpression();
        consume(CHTLJSTokenType::COLON);
        auto falseExpr = parseExpression();
        
        auto condNode = std::make_shared<BaseNode>(NodeType::ConditionalExpression);
        condNode->children.push_back(left);
        condNode->children.push_back(trueExpr);
        condNode->children.push_back(falseExpr);
        return condNode;
    }
    
    return left;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseLogicalExpression() {
    auto left = parseEqualityExpression();
    
    while (match(CHTLJSTokenType::AND) || match(CHTLJSTokenType::OR)) {
        CHTLJSToken op = peek();
        currentTokenIndex++;
        auto right = parseEqualityExpression();
        
        auto logNode = std::make_shared<BaseNode>(NodeType::BinaryExpression);
        logNode->setValue(op.value);
        logNode->children.push_back(left);
        logNode->children.push_back(right);
        left = logNode;
    }
    
    return left;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseEqualityExpression() {
    auto left = parseRelationalExpression();
    
    while (match(CHTLJSTokenType::EQUAL_EQUAL) || match(CHTLJSTokenType::NOT_EQUAL)) {
        CHTLJSToken op = peek();
        currentTokenIndex++;
        auto right = parseRelationalExpression();
        
        auto eqNode = std::make_shared<BaseNode>(NodeType::BinaryExpression);
        eqNode->setValue(op.value);
        eqNode->children.push_back(left);
        eqNode->children.push_back(right);
        left = eqNode;
    }
    
    return left;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseRelationalExpression() {
    auto left = parseAdditiveExpression();
    
    while (match(CHTLJSTokenType::GREATER) || match(CHTLJSTokenType::LESS) ||
           match(CHTLJSTokenType::GREATER_EQUAL) || match(CHTLJSTokenType::LESS_EQUAL)) {
        CHTLJSToken op = peek();
        currentTokenIndex++;
        auto right = parseAdditiveExpression();
        
        auto relNode = std::make_shared<BaseNode>(NodeType::BinaryExpression);
        relNode->setValue(op.value);
        relNode->children.push_back(left);
        relNode->children.push_back(right);
        left = relNode;
    }
    
    return left;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseAdditiveExpression() {
    auto left = parseMultiplicativeExpression();
    
    while (match(CHTLJSTokenType::PLUS) || match(CHTLJSTokenType::MINUS)) {
        CHTLJSToken op = peek();
        currentTokenIndex++;
        auto right = parseMultiplicativeExpression();
        
        auto addNode = std::make_shared<BaseNode>(NodeType::BinaryExpression);
        addNode->setValue(op.value);
        addNode->children.push_back(left);
        addNode->children.push_back(right);
        left = addNode;
    }
    
    return left;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseMultiplicativeExpression() {
    auto left = parseUnaryExpression();
    
    while (match(CHTLJSTokenType::MULTIPLY) || match(CHTLJSTokenType::DIVIDE) || match(CHTLJSTokenType::MODULO)) {
        CHTLJSToken op = peek();
        currentTokenIndex++;
        auto right = parseUnaryExpression();
        
        auto mulNode = std::make_shared<BaseNode>(NodeType::BinaryExpression);
        mulNode->setValue(op.value);
        mulNode->children.push_back(left);
        mulNode->children.push_back(right);
        left = mulNode;
    }
    
    return left;
}

std::shared_ptr<BaseNode> CHTLJSParser::parseUnaryExpression() {
    if (match(CHTLJSTokenType::NOT) || match(CHTLJSTokenType::MINUS) || match(CHTLJSTokenType::PLUS)) {
        CHTLJSToken op = peek();
        currentTokenIndex++;
        auto operand = parseUnaryExpression();
        
        auto unaryNode = std::make_shared<BaseNode>(NodeType::UnaryExpression);
        unaryNode->setValue(op.value);
        unaryNode->children.push_back(operand);
        return unaryNode;
    }
    
    return parsePrimaryExpression();
}

std::shared_ptr<BaseNode> CHTLJSParser::parsePrimaryExpression() {
    CHTLJSToken token = peek();
    
    switch (token.type) {
        case CHTLJSTokenType::IDENTIFIER:
            if (peek(1).type == CHTLJSTokenType::LEFT_PAREN) {
                return parseFunctionCall();
            }
            currentTokenIndex++;
            return std::make_shared<BaseNode>(NodeType::Identifier, token.value);
            
        case CHTLJSTokenType::STRING:
        case CHTLJSTokenType::NUMBER:
        case CHTLJSTokenType::BOOLEAN:
            currentTokenIndex++;
            return std::make_shared<BaseNode>(NodeType::Literal, token.value);
            
        case CHTLJSTokenType::SELECTOR:
            return parseSelector();
            
        case CHTLJSTokenType::LEFT_PAREN:
            consume(CHTLJSTokenType::LEFT_PAREN);
            auto expr = parseExpression();
            consume(CHTLJSTokenType::RIGHT_PAREN);
            return expr;
            
        case CHTLJSTokenType::LEFT_BRACE:
            return parseObjectLiteral();
            
        case CHTLJSTokenType::LEFT_BRACKET:
            return parseArrayLiteral();
            
        default:
            throwError("Unexpected token in expression: " + token.value);
    }
}

std::shared_ptr<BaseNode> CHTLJSParser::parseProperty() {
    CHTLJSToken keyToken = consumeIdentifier();
    consume(CHTLJSTokenType::COLON);
    auto valueNode = parseExpression();
    
    auto propNode = std::make_shared<BaseNode>(NodeType::Property);
    propNode->setValue(keyToken.value);
    propNode->children.push_back(valueNode);
    
    return propNode;
}

} // namespace CHTL