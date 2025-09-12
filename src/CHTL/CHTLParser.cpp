#include "CHTLParser.h"
#include <iostream>
#include <sstream>

namespace CHTL {

// ASTNode方法实现
void ASTNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

std::string ASTNode::toString(int indent) const {
    std::stringstream ss;
    std::string indentStr(indent * 2, ' ');
    
    ss << indentStr << "NodeType: " << static_cast<int>(type) << "\n";
    ss << indentStr << "Value: " << value << "\n";
    ss << indentStr << "Line: " << line << ", Column: " << column << "\n";
    
    if (!attributes.empty()) {
        ss << indentStr << "Attributes:\n";
        for (const auto& attr : attributes) {
            ss << indentStr << "  " << attr.first << " = " << attr.second << "\n";
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

// CHTLParser方法实现
CHTLParser::CHTLParser() : currentTokenIndex(0) {}

CHTLParser::~CHTLParser() {}

void CHTLParser::setLexer(std::unique_ptr<CHTLLexer> lex) {
    lexer = std::move(lex);
    tokens = lexer->tokenize();
    currentTokenIndex = 0;
}

std::unique_ptr<ASTNode> CHTLParser::parse() {
    if (!lexer) {
        reportError("No lexer set");
        return nullptr;
    }
    
    auto root = std::make_unique<ASTNode>(ASTNode::NodeType::ROOT);
    
    while (!isEOF()) {
        auto node = parseElement();
        if (node) {
            // 直接添加ElementNode到root
            root->addChild(std::move(node));
        } else {
            break;
        }
    }
    
    return root;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    
    auto element = std::make_unique<ElementNode>(token->value);
    consume();
    
    // 解析属性
    element->getAttributes() = parseAttributes();
    
    // 检查是否有子内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(TokenType::RIGHT_BRACE)) {
            // 解析子节点
            if (match(TokenType::IDENTIFIER)) {
                auto childElement = parseElement();
                if (childElement) {
                    element->getChildren().push_back(std::move(childElement));
                }
            } else if (match(TokenType::STRING)) {
                auto textNode = parseText();
                if (textNode) {
                    element->getChildren().push_back(std::move(textNode));
                }
            } else if (match(TokenType::STYLE)) {
                auto styleNode = parseStyle();
                if (styleNode) {
                    element->getChildren().push_back(std::move(styleNode));
                }
            } else if (match(TokenType::SCRIPT)) {
                auto scriptNode = parseScript();
                if (scriptNode) {
                    element->getChildren().push_back(std::move(scriptNode));
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return element;
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::STRING) {
        return nullptr;
    }
    
    auto textNode = std::make_unique<TextNode>(token->value);
    consume();
    
    return textNode;
}

std::unique_ptr<StyleNode> CHTLParser::parseStyle() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::STYLE) {
        return nullptr;
    }
    
    auto styleNode = std::make_unique<StyleNode>();
    consume();
    
    // 解析样式属性
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(TokenType::RIGHT_BRACE)) {
            if (match(TokenType::IDENTIFIER)) {
                std::string property = parseIdentifier();
                consume(); // 消费 :
                
                if (match(TokenType::STRING) || match(TokenType::NUMBER)) {
                    std::string value = parseValue();
                    styleNode->addProperty(property, value);
                }
                
                if (match(TokenType::SEMICOLON)) {
                    consume(); // 消费 ;
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return styleNode;
}

std::unique_ptr<ScriptNode> CHTLParser::parseScript() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::SCRIPT) {
        return nullptr;
    }
    
    auto scriptNode = std::make_unique<ScriptNode>();
    consume();
    
    // 解析脚本内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        std::stringstream scriptContent;
        while (!isEOF() && !match(TokenType::RIGHT_BRACE)) {
            scriptContent << currentToken()->value << " ";
            consume();
        }
        
        scriptNode->setScript(scriptContent.str());
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return scriptNode;
}

std::unique_ptr<TemplateNode> CHTLParser::parseTemplate() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::TEMPLATE) {
        return nullptr;
    }
    
    consume(); // 消费 [Template]
    
    // 解析模板类型
    std::string templateType = "STYLE";
    if (match(TokenType::IDENTIFIER)) {
        std::string typeStr = parseIdentifier();
        if (typeStr == "Element") {
            templateType = "ELEMENT";
        } else if (typeStr == "Var") {
            templateType = "VAR";
        }
    }
    
    // 解析模板名称
    std::string name = parseIdentifier();
    
    auto templateNode = std::make_unique<TemplateNode>(name);
    
    // 解析模板内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(TokenType::RIGHT_BRACE)) {
            // 解析模板属性
            if (match(TokenType::IDENTIFIER)) {
                std::string property = parseIdentifier();
                consume(); // 消费 :
                
                if (match(TokenType::STRING) || match(TokenType::NUMBER)) {
                    std::string value = parseValue();
                    templateNode->getProperties()[property] = value;
                }
                
                if (match(TokenType::SEMICOLON)) {
                    consume(); // 消费 ;
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return templateNode;
}

std::unique_ptr<CustomNode> CHTLParser::parseCustom() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::CUSTOM) {
        return nullptr;
    }
    
    consume(); // 消费 [Custom]
    
    // 解析自定义类型
    std::string customType = "STYLE";
    if (match(TokenType::IDENTIFIER)) {
        std::string typeStr = parseIdentifier();
        if (typeStr == "Element") {
            customType = "ELEMENT";
        } else if (typeStr == "Var") {
            customType = "VAR";
        }
    }
    
    // 解析自定义名称
    std::string name = parseIdentifier();
    
    auto customNode = std::make_unique<CustomNode>(name);
    
    // 解析自定义内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 消费 {
        
        while (!isEOF() && !match(TokenType::RIGHT_BRACE)) {
            // 解析自定义属性
            if (match(TokenType::IDENTIFIER)) {
                std::string property = parseIdentifier();
                consume(); // 消费 :
                
                if (match(TokenType::STRING) || match(TokenType::NUMBER)) {
                    std::string value = parseValue();
                    customNode->getProperties()[property] = value;
                }
                
                if (match(TokenType::SEMICOLON)) {
                    consume(); // 消费 ;
                }
            } else {
                consume(); // 跳过未知token
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 消费 }
        }
    }
    
    return customNode;
}

std::unique_ptr<ImportNode> CHTLParser::parseImport() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::IMPORT) {
        return nullptr;
    }
    
    consume(); // 消费 [Import]
    
    // 解析导入类型
    std::string importType = "CHTL";
    if (match(TokenType::IDENTIFIER)) {
        std::string typeStr = parseIdentifier();
        if (typeStr == "HTML") {
            importType = "HTML";
        } else if (typeStr == "Style") {
            importType = "STYLE";
        } else if (typeStr == "JavaScript") {
            importType = "JAVASCRIPT";
        } else if (typeStr == "CJMOD") {
            importType = "CJMOD";
        }
    }
    
    // 解析导入路径
    std::string path = parsePath();
    
    auto importNode = std::make_unique<ImportNode>(path);
    
    return importNode;
}

std::unique_ptr<NamespaceNode> CHTLParser::parseNamespace() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::NAMESPACE) {
        return nullptr;
    }
    
    consume(); // 消费 [Namespace]
    
    // 解析命名空间名称
    std::string name = parseIdentifier();
    
    auto namespaceNode = std::make_unique<NamespaceNode>(name);
    
    return namespaceNode;
}

std::unique_ptr<OriginNode> CHTLParser::parseOrigin() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::ORIGIN) {
        return nullptr;
    }
    
    consume(); // 消费 [Origin]
    
    // 解析原始类型
    std::string originType = "HTML";
    if (match(TokenType::IDENTIFIER)) {
        std::string typeStr = parseIdentifier();
        if (typeStr == "Style") {
            originType = "STYLE";
        } else if (typeStr == "JavaScript") {
            originType = "JAVASCRIPT";
        }
    }
    
    // 解析原始内容
    std::string content = parseValue();
    
    auto originNode = std::make_unique<OriginNode>(originType);
    
    return originNode;
}

std::unique_ptr<ConfigurationNode> CHTLParser::parseConfiguration() {
    if (isEOF()) return nullptr;
    
    const Token* token = currentToken();
    if (token->type != TokenType::CONFIGURATION) {
        return nullptr;
    }
    
    consume(); // 消费 [Configuration]
    
    auto configNode = std::make_unique<ConfigurationNode>("", token->line, token->column);
    
    return configNode;
}

std::map<std::string, std::string> CHTLParser::parseAttributes() {
    std::map<std::string, std::string> attributes;
    
    while (!isEOF() && !match(TokenType::LEFT_BRACE) && !match(TokenType::RIGHT_BRACE)) {
        if (match(TokenType::IDENTIFIER)) {
            std::string key = parseIdentifier();
            consume(); // 消费 :
            
            if (match(TokenType::STRING) || match(TokenType::NUMBER)) {
                std::string value = parseValue();
                attributes[key] = value;
            }
            
            if (match(TokenType::SEMICOLON)) {
                consume(); // 消费 ;
            }
        } else {
            consume(); // 跳过未知token
        }
    }
    
    return attributes;
}

std::map<std::string, std::string> CHTLParser::parseStyleProperties() {
    std::map<std::string, std::string> properties;
    
    while (!isEOF() && !match(TokenType::RIGHT_BRACE)) {
        if (match(TokenType::IDENTIFIER)) {
            std::string property = parseIdentifier();
            consume(); // 消费 :
            
            if (match(TokenType::STRING) || match(TokenType::NUMBER)) {
                std::string value = parseValue();
                properties[property] = value;
            }
            
            if (match(TokenType::SEMICOLON)) {
                consume(); // 消费 ;
            }
        } else {
            consume(); // 跳过未知token
        }
    }
    
    return properties;
}

std::string CHTLParser::parseConditionalExpression() {
    // 简化的条件表达式解析
    return parseExpression();
}

std::string CHTLParser::parsePropertyReference() {
    // 简化的属性引用解析
    return parseExpression();
}

std::string CHTLParser::parseArithmeticExpression() {
    // 简化的算术表达式解析
    return parseExpression();
}

const Token* CHTLParser::currentToken() const {
    if (currentTokenIndex >= tokens.size()) {
        return nullptr;
    }
    return &tokens[currentTokenIndex];
}

const Token* CHTLParser::nextToken() {
    if (currentTokenIndex + 1 >= tokens.size()) {
        return nullptr;
    }
    return &tokens[++currentTokenIndex];
}

bool CHTLParser::match(TokenType type) {
    const Token* token = currentToken();
    return token && token->type == type;
}

void CHTLParser::consume() {
    if (currentTokenIndex < tokens.size()) {
        currentTokenIndex++;
    }
}

bool CHTLParser::isEOF() const {
    return currentTokenIndex >= tokens.size();
}

const std::string& CHTLParser::getError() const {
    return errorMessage;
}

void CHTLParser::reportError(const std::string& message) {
    errorMessage = message;
    std::cerr << "Parse Error: " << message << std::endl;
}

void CHTLParser::skipToMatchingBrace() {
    int braceCount = 0;
    while (!isEOF()) {
        const Token* token = currentToken();
        if (token->type == TokenType::LEFT_BRACE) {
            braceCount++;
        } else if (token->type == TokenType::RIGHT_BRACE) {
            braceCount--;
            if (braceCount == 0) {
                consume();
                break;
            }
        }
        consume();
    }
}

std::string CHTLParser::parseExpression() {
    std::stringstream expr;
    
    while (!isEOF() && !match(TokenType::SEMICOLON) && !match(TokenType::RIGHT_BRACE)) {
        expr << currentToken()->value << " ";
        consume();
    }
    
    return expr.str();
}

std::string CHTLParser::parseSelector() {
    return parseExpression();
}

std::string CHTLParser::parseValue() {
    const Token* token = currentToken();
    if (token) {
        consume();
        return token->value;
    }
    return "";
}

std::string CHTLParser::parseLiteral() {
    return parseValue();
}

std::string CHTLParser::parseString() {
    return parseValue();
}

std::string CHTLParser::parseNumber() {
    return parseValue();
}

std::string CHTLParser::parseIdentifier() {
    return parseValue();
}

std::string CHTLParser::parsePath() {
    return parseValue();
}

std::string CHTLParser::parseNamespaceReference() {
    return parseValue();
}

std::string CHTLParser::parseTemplateReference() {
    return parseValue();
}

std::string CHTLParser::parseCustomReference() {
    return parseValue();
}

std::string CHTLParser::parseVariableReference() {
    return parseValue();
}

std::string CHTLParser::parseReactiveValue() {
    return parseValue();
}

std::string CHTLParser::parseCHTLJSSyntax() {
    return parseValue();
}

std::string CHTLParser::parseEnhancedSelector() {
    return parseValue();
}

std::string CHTLParser::parseVirtualObject() {
    return parseValue();
}

std::string CHTLParser::parseRouter() {
    return parseValue();
}

std::string CHTLParser::parseAnimation() {
    return parseValue();
}

std::string CHTLParser::parseListener() {
    return parseValue();
}

std::string CHTLParser::parseFileLoader() {
    return parseValue();
}

std::string CHTLParser::parseINeverAway() {
    return parseValue();
}

std::string CHTLParser::parsePrintMylove() {
    return parseValue();
}

std::string CHTLParser::parseUtilThenExpression() {
    return parseValue();
}

} // namespace CHTL