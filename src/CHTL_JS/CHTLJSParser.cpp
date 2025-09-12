#include "CHTLJSParser.h"
#include <iostream>
#include <sstream>

namespace CHTL {

// CHTLJSNode方法实现
void CHTLJSNode::addChild(std::unique_ptr<CHTLJSNode> child) {
    children.push_back(std::move(child));
}

std::string CHTLJSNode::toString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 2, ' ');
    
    ss << indentStr << "NodeType: " << static_cast<int>(type) << "\n";
    ss << indentStr << "Value: " << value << "\n";
    ss << indentStr << "Line: " << line << ", Column: " << column << "\n";
    
    if (!properties.empty()) {
        ss << indentStr << "Properties:\n";
        for (const auto& prop : properties) {
            ss << indentStr << "  " << prop.first << " = " << prop.second << "\n";
        }
    }
    
    if (!children.empty()) {
        ss << indentStr << "Children:\n";
        for (const auto& child : children) {
            ss << child->toString(indent + 1);
        }
    }
    
    return ss.str();
}

// CHTLJSParser方法实现
CHTLJSParser::CHTLJSParser() : currentTokenIndex(0) {}

CHTLJSParser::~CHTLJSParser() {}

void CHTLJSParser::setLexer(std::unique_ptr<CHTLJSLexer> lex) {
    lexer = std::move(lex);
    tokens = lexer->tokenize(lexer->getSource());
    currentTokenIndex = 0;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parse() {
    if (!lexer) {
        reportError("No lexer set");
        return nullptr;
    }
    
    auto root = std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::ROOT);
    
    while (!isEOF()) {
        auto node = parseExpression();
        if (node) {
            root->addChild(std::move(node));
        } else {
            break;
        }
    }
    
    return root;
}

std::unique_ptr<VirtualObjectNode> CHTLJSParser::parseVirtualObject() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::VIR) {
        return nullptr;
    }
    
    consume(); // 消费 vir
    
    // 解析虚对象名称
    std::string name = parseIdentifier();
    
    auto virtualObject = std::make_unique<VirtualObjectNode>(name, token->line, token->column);
    
    // 解析虚对象属性
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            if (match(CHTLJSTokenType::IDENTIFIER)) {
                std::string propName = parseIdentifier();
                consume(); // 消费 :
                
                if (match(CHTLJSTokenType::LEFT_BRACE)) {
                    consume(); // 消费 {
                    
                    // 解析属性值
                    std::string propValue = parseValue();
                    
                    if (match(CHTLJSTokenType::RIGHT_BRACE)) {
                        consume(); // 消费 }
                    }
                    
                    virtualObject->properties[propName] = propValue;
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return virtualObject;
}

std::unique_ptr<ListenNode> CHTLJSParser::parseListen() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::LISTEN) {
        return nullptr;
    }
    
    consume(); // 消费 listen
    
    // 解析选择器
    std::string selector = parseValue();
    
    auto listenNode = std::make_unique<ListenNode>(selector, token->line, token->column);
    
    // 解析事件
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            if (match(CHTLJSTokenType::IDENTIFIER)) {
                std::string eventName = parseIdentifier();
                consume(); // 消费 :
                
                std::string eventHandler = parseValue();
                listenNode->events[eventName] = eventHandler;
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return listenNode;
}

std::unique_ptr<AnimateNode> CHTLJSParser::parseAnimate() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::ANIMATE) {
        return nullptr;
    }
    
    consume(); // 消费 animate
    
    auto animateNode = std::make_unique<AnimateNode>(token->line, token->column);
    
    // 解析动画属性
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            if (match(CHTLJSTokenType::IDENTIFIER)) {
                std::string propName = parseIdentifier();
                consume(); // 消费 :
                
                std::string propValue = parseValue();
                
                if (propName == "target") {
                    animateNode->target = propValue;
                } else if (propName == "duration") {
                    animateNode->duration = propValue;
                } else if (propName == "easing") {
                    animateNode->easing = propValue;
                } else if (propName == "loop") {
                    animateNode->loop = propValue;
                } else if (propName == "direction") {
                    animateNode->direction = propValue;
                } else if (propName == "delay") {
                    animateNode->delay = propValue;
                } else if (propName == "callback") {
                    animateNode->callback = propValue;
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return animateNode;
}

std::unique_ptr<FileLoaderNode> CHTLJSParser::parseFileLoader() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::FILELOADER) {
        return nullptr;
    }
    
    consume(); // 消费 fileloader
    
    auto fileLoaderNode = std::make_unique<FileLoaderNode>(token->line, token->column);
    
    // 解析文件列表
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            if (match(CHTLJSTokenType::STRING)) {
                std::string fileName = parseString();
                fileLoaderNode->files.push_back(fileName);
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return fileLoaderNode;
}

std::unique_ptr<RouterNode> CHTLJSParser::parseRouter() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::ROUTER) {
        return nullptr;
    }
    
    consume(); // 消费 router
    
    auto routerNode = std::make_unique<RouterNode>(token->line, token->column);
    
    // 解析路由配置
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            if (match(CHTLJSTokenType::IDENTIFIER)) {
                std::string propName = parseIdentifier();
                consume(); // 消费 :
                
                std::string propValue = parseValue();
                
                if (propName == "root") {
                    routerNode->root = propValue;
                } else if (propName == "mode") {
                    routerNode->mode = propValue;
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return routerNode;
}

std::unique_ptr<INeverAwayNode> CHTLJSParser::parseINeverAway() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::INEVERAWAY) {
        return nullptr;
    }
    
    consume(); // 消费 iNeverAway
    
    auto iNeverAwayNode = std::make_unique<INeverAwayNode>(token->line, token->column);
    
    // 解析函数列表
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            if (match(CHTLJSTokenType::IDENTIFIER)) {
                std::string funcName = parseIdentifier();
                consume(); // 消费 :
                
                std::string funcBody = parseValue();
                iNeverAwayNode->functions[funcName] = funcBody;
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return iNeverAwayNode;
}

std::unique_ptr<PrintMyloveNode> CHTLJSParser::parsePrintMylove() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::PRINTMYLOVE) {
        return nullptr;
    }
    
    consume(); // 消费 printMylove
    
    auto printMyloveNode = std::make_unique<PrintMyloveNode>(token->line, token->column);
    
    // 解析参数
    if (match(CHTLJSTokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
            if (match(CHTLJSTokenType::IDENTIFIER)) {
                std::string paramName = parseIdentifier();
                consume(); // 消费 :
                
                std::string paramValue = parseValue();
                
                if (paramName == "url") {
                    printMyloveNode->url = paramValue;
                } else if (paramName == "mode") {
                    printMyloveNode->mode = paramValue;
                } else if (paramName == "width") {
                    printMyloveNode->width = paramValue;
                } else if (paramName == "height") {
                    printMyloveNode->height = paramValue;
                } else if (paramName == "scale") {
                    printMyloveNode->scale = paramValue;
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return printMyloveNode;
}

std::unique_ptr<UtilThenNode> CHTLJSParser::parseUtilThen() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::UTIL) {
        return nullptr;
    }
    
    consume(); // 消费 util
    
    // 解析条件
    std::string condition = parseConditionalExpression();
    
    auto utilThenNode = std::make_unique<UtilThenNode>(condition, token->line, token->column);
    
    // 解析 change 和 then 动作
    if (match(CHTLJSTokenType::ARROW)) {
        consume(); // 消费 ->
        
        if (match(CHTLJSTokenType::CHANGE)) {
            consume(); // 消费 change
            utilThenNode->changeAction = parseValue();
        }
        
        if (match(CHTLJSTokenType::ARROW)) {
            consume(); // 消费 ->
            
            if (match(CHTLJSTokenType::THEN)) {
                consume(); // 消费 then
                utilThenNode->thenAction = parseValue();
            }
        }
    }
    
    return utilThenNode;
}

std::unique_ptr<EnhancedSelectorNode> CHTLJSParser::parseEnhancedSelector() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::ENHANCED_SELECTOR) {
        return nullptr;
    }
    
    consume(); // 消费选择器
    
    std::string selector = token->value;
    
    auto enhancedSelectorNode = std::make_unique<EnhancedSelectorNode>(selector, token->line, token->column);
    
    return enhancedSelectorNode;
}

std::unique_ptr<ReactiveValueNode> CHTLJSParser::parseReactiveValue() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::REACTIVE_VALUE) {
        return nullptr;
    }
    
    consume(); // 消费响应式值
    
    std::string variableName = token->value;
    
    auto reactiveValueNode = std::make_unique<ReactiveValueNode>(variableName, token->line, token->column);
    
    return reactiveValueNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseExpression() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    
    switch (token->type) {
        case CHTLJSTokenType::VIR:
            return parseVirtualObject();
        case CHTLJSTokenType::LISTEN:
            return parseListen();
        case CHTLJSTokenType::ANIMATE:
            return parseAnimate();
        case CHTLJSTokenType::FILELOADER:
            return parseFileLoader();
        case CHTLJSTokenType::ROUTER:
            return parseRouter();
        case CHTLJSTokenType::INEVERAWAY:
            return parseINeverAway();
        case CHTLJSTokenType::PRINTMYLOVE:
            return parsePrintMylove();
        case CHTLJSTokenType::UTIL:
            return parseUtilThen();
        case CHTLJSTokenType::ENHANCED_SELECTOR:
            return parseEnhancedSelector();
        case CHTLJSTokenType::REACTIVE_VALUE:
            return parseReactiveValue();
        default:
            return parsePrimaryExpression();
    }
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseObjectLiteral() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::LEFT_BRACE) {
        return nullptr;
    }
    
    consume(); // 消费 {
    
    auto objectNode = std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::OBJECT_LITERAL, "", token->line, token->column);
    
    while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        if (match(CHTLJSTokenType::IDENTIFIER)) {
            std::string key = parseIdentifier();
            consume(); // 消费 :
            
            std::string value = parseValue();
            objectNode->properties[key] = value;
        } else {
            consume(); // 跳过未知token
        }
    }
    
    if (match(CHTLJSTokenType::RIGHT_BRACE)) {
        consume(); // 消费 }
    }
    
    return objectNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseArrayLiteral() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::LEFT_BRACKET) {
        return nullptr;
    }
    
    consume(); // 消费 [
    
    auto arrayNode = std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::ARRAY_LITERAL, "", token->line, token->column);
    
    while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACKET)) {
        if (match(CHTLJSTokenType::STRING) || match(CHTLJSTokenType::NUMBER)) {
            std::string value = parseValue();
            auto valueNode = std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::EXPRESSION, value, token->line, token->column);
            arrayNode->addChild(std::move(valueNode));
        } else {
            consume(); // 跳过未知token
        }
    }
    
    if (match(CHTLJSTokenType::RIGHT_BRACKET)) {
        consume(); // 消费 ]
    }
    
    return arrayNode;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseFunctionCall() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    if (token->type != CHTLJSTokenType::IDENTIFIER) {
        return nullptr;
    }
    
    std::string functionName = parseIdentifier();
    
    auto functionCallNode = std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::FUNCTION_CALL, functionName, token->line, token->column);
    
    if (match(CHTLJSTokenType::LEFT_PAREN)) {
        consume(); // 消费 (
        
        while (!isEOF() && !match(CHTLJSTokenType::RIGHT_PAREN)) {
            if (match(CHTLJSTokenType::STRING) || match(CHTLJSTokenType::NUMBER)) {
                std::string arg = parseValue();
                auto argNode = std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::EXPRESSION, arg, token->line, token->column);
                functionCallNode->addChild(std::move(argNode));
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(CHTLJSTokenType::RIGHT_PAREN)) {
            consume(); // 消费 )
        }
    }
    
    return functionCallNode;
}

const CHTLJSToken* CHTLJSParser::currentToken() const {
    if (currentTokenIndex >= tokens.size()) {
        return nullptr;
    }
    return &tokens[currentTokenIndex];
}

const CHTLJSToken* CHTLJSParser::nextToken() {
    if (currentTokenIndex + 1 >= tokens.size()) {
        return nullptr;
    }
    return &tokens[++currentTokenIndex];
}

bool CHTLJSParser::match(CHTLJSTokenType type) {
    const CHTLJSToken* token = currentToken();
    return token && token->type == type;
}

void CHTLJSParser::consume() {
    if (currentTokenIndex < tokens.size()) {
        currentTokenIndex++;
    }
}

bool CHTLJSParser::isEOF() const {
    return currentTokenIndex >= tokens.size();
}

const std::string& CHTLJSParser::getError() const {
    return errorMessage;
}

void CHTLJSParser::reportError(const std::string& message) {
    errorMessage = message;
    std::cerr << "Parse Error: " << message << std::endl;
}

std::map<std::string, std::string> CHTLJSParser::parseProperties() {
    std::map<std::string, std::string> properties;
    
    while (!isEOF() && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        if (match(CHTLJSTokenType::IDENTIFIER)) {
            std::string key = parseIdentifier();
            consume(); // 消费 :
            
            std::string value = parseValue();
            properties[key] = value;
        } else {
            consume(); // 跳过未知token
        }
    }
    
    return properties;
}

std::string CHTLJSParser::parseValue() {
    const CHTLJSToken* token = currentToken();
    if (token) {
        consume();
        return token->value;
    }
    return "";
}

std::string CHTLJSParser::parseIdentifier() {
    const CHTLJSToken* token = currentToken();
    if (token && token->type == CHTLJSTokenType::IDENTIFIER) {
        consume();
        return token->value;
    }
    return "";
}

std::string CHTLJSParser::parseString() {
    const CHTLJSToken* token = currentToken();
    if (token && token->type == CHTLJSTokenType::STRING) {
        consume();
        return token->value;
    }
    return "";
}

std::string CHTLJSParser::parseNumber() {
    const CHTLJSToken* token = currentToken();
    if (token && token->type == CHTLJSTokenType::NUMBER) {
        consume();
        return token->value;
    }
    return "";
}

std::string CHTLJSParser::parseLiteral() {
    return parseValue();
}

std::string CHTLJSParser::parseConditionalExpression() {
    std::stringstream expr;
    
    while (!isEOF() && !match(CHTLJSTokenType::ARROW) && !match(CHTLJSTokenType::RIGHT_BRACE)) {
        expr << currentToken()->value << " ";
        consume();
    }
    
    return expr.str();
}

std::string CHTLJSParser::parseArithmeticExpression() {
    return parseValue();
}

std::string CHTLJSParser::parseLogicalExpression() {
    return parseValue();
}

std::string CHTLJSParser::parseComparisonExpression() {
    return parseValue();
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parsePrimaryExpression() {
    if (isEOF()) return nullptr;
    
    const CHTLJSToken* token = currentToken();
    
    if (token->type == CHTLJSTokenType::LEFT_BRACE) {
        return parseObjectLiteral();
    } else if (token->type == CHTLJSTokenType::LEFT_BRACKET) {
        return parseArrayLiteral();
    } else if (token->type == CHTLJSTokenType::IDENTIFIER) {
        return parseFunctionCall();
    } else if (token->type == CHTLJSTokenType::STRING || token->type == CHTLJSTokenType::NUMBER) {
        std::string value = parseValue();
        return std::make_unique<CHTLJSNode>(CHTLJSNode::NodeType::EXPRESSION, value, token->line, token->column);
    }
    
    return nullptr;
}

} // namespace CHTL