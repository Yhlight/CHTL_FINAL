#include "Parser.hpp"
#include <sstream>

namespace CHTL {

Parser::Parser(const std::string& source) : lexer_(source), hasError_(false) {
    current_ = lexer_.nextToken();
}

Parser::Parser(Lexer& lexer) : lexer_(lexer), hasError_(false) {
    current_ = lexer_.nextToken();
}

std::shared_ptr<BaseNode> Parser::parse() {
    // 创建根节点
    auto root = std::make_shared<ElementNode>("html");
    
    while (current_.getType() != TokenType::END_OF_FILE && !hasError_) {
        if (isElementStart(current_.getType())) {
            auto element = parseElement();
            if (element) {
                root->addChild(element);
            }
        } else if (isTemplateStart(current_.getType())) {
            auto template_node = parseTemplate();
            if (template_node) {
                root->addChild(template_node);
            }
        } else if (isCustomStart(current_.getType())) {
            auto custom = parseCustom();
            if (custom) {
                root->addChild(custom);
            }
        } else if (isOriginStart(current_.getType())) {
            auto origin = parseOrigin();
            if (origin) {
                root->addChild(origin);
            }
        } else if (isImportStart(current_.getType())) {
            auto import = parseImport();
            if (import) {
                root->addChild(import);
            }
        } else if (isConfigStart(current_.getType())) {
            auto config = parseConfig();
            if (config) {
                root->addChild(config);
            }
        } else if (isNamespaceStart(current_.getType())) {
            auto namespace_node = parseNamespace();
            if (namespace_node) {
                root->addChild(namespace_node);
            }
        } else {
            // 跳过未知词法单元
            current_ = lexer_.nextToken();
        }
    }
    
    return root;
}

std::shared_ptr<ElementNode> Parser::parseElement() {
    if (current_.getType() != TokenType::IDENTIFIER) {
        reportError("Expected element name");
        return nullptr;
    }
    
    std::string tagName = current_.getValue();
    auto element = std::make_shared<ElementNode>(tagName, current_.getLine(), current_.getColumn());
    current_ = lexer_.nextToken();
    
    // 解析属性
    parseAttributes(element.get());
    
    // 解析内容
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == TokenType::TEXT) {
                auto text = parseText();
                if (text) {
                    element->addChild(text);
                }
            } else if (current_.getType() == TokenType::STYLE) {
                auto style = parseStyle();
                if (style) {
                    element->addChild(style);
                }
            } else if (current_.getType() == TokenType::IDENTIFIER) {
                auto childElement = parseElement();
                if (childElement) {
                    element->addChild(childElement);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return element;
}

std::shared_ptr<TextNode> Parser::parseText() {
    if (current_.getType() != TokenType::TEXT) {
        reportError("Expected text");
        return nullptr;
    }
    
    std::string content = current_.getValue();
    auto text = std::make_shared<TextNode>(content, current_.getLine(), current_.getColumn());
    current_ = lexer_.nextToken();
    
    return text;
}

std::shared_ptr<StyleNode> Parser::parseStyle() {
    if (current_.getType() != TokenType::STYLE) {
        reportError("Expected style");
        return nullptr;
    }
    
    auto style = std::make_shared<StyleNode>(StyleNode::StyleType::LOCAL, 
                                            current_.getLine(), current_.getColumn());
    current_ = lexer_.nextToken();
    
    if (consume(TokenType::LEFT_BRACE)) {
        parseStyleProperties(style.get());
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return style;
}

std::shared_ptr<BaseNode> Parser::parseTemplate() {
    if (!consume(TokenType::TEMPLATE)) {
        reportError("Expected [Template]");
        return nullptr;
    }
    
    std::string templateType = parseTemplateType();
    if (templateType.empty()) {
        reportError("Expected template type");
        return nullptr;
    }
    
    // 创建模板节点
    auto template_node = std::make_shared<TemplateNode>(templateType,
                                                       current_.getLine(), current_.getColumn());
    
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (isElementStart(current_.getType())) {
                auto element = parseElement();
                if (element) {
                    template_node->addChild(element);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return template_node;
}

std::shared_ptr<BaseNode> Parser::parseCustom() {
    if (!consume(TokenType::CUSTOM)) {
        reportError("Expected [Custom]");
        return nullptr;
    }
    
    std::string customType = parseCustomType();
    if (customType.empty()) {
        reportError("Expected custom type");
        return nullptr;
    }
    
    // 创建自定义节点
    auto custom = std::make_shared<CustomNode>(customType,
                                              current_.getLine(), current_.getColumn());
    
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (isElementStart(current_.getType())) {
                auto element = parseElement();
                if (element) {
                    custom->addChild(element);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return custom;
}

std::shared_ptr<BaseNode> Parser::parseOrigin() {
    if (!consume(TokenType::ORIGIN)) {
        reportError("Expected [Origin]");
        return nullptr;
    }
    
    std::string originType = parseOriginType();
    if (originType.empty()) {
        reportError("Expected origin type");
        return nullptr;
    }
    
    // 创建原始嵌入节点
    auto origin = std::make_shared<OriginNode>(originType,
                                              current_.getLine(), current_.getColumn());
    
    if (consume(TokenType::LEFT_BRACE)) {
        std::string content;
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            content += current_.getValue();
            current_ = lexer_.nextToken();
        }
        origin->setValue(content);
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return origin;
}

std::shared_ptr<BaseNode> Parser::parseImport() {
    if (!consume(TokenType::IMPORT)) {
        reportError("Expected [Import]");
        return nullptr;
    }
    
    std::string importType = parseImportType();
    if (importType.empty()) {
        reportError("Expected import type");
        return nullptr;
    }
    
    // 创建导入节点
    auto import = std::make_shared<ImportNode>(importType,
                                             current_.getLine(), current_.getColumn());
    
    return import;
}

std::shared_ptr<BaseNode> Parser::parseConfig() {
    if (!consume(TokenType::CONFIGURATION)) {
        reportError("Expected [Configuration]");
        return nullptr;
    }
    
    // 创建配置节点
    auto config = std::make_shared<ConfigNode>(current_.getLine(), current_.getColumn());
    
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string key = current_.getValue();
                current_ = lexer_.nextToken();
                
                if (consume(TokenType::EQUAL)) {
                    std::string value = parseAttributeValue();
                    config->setAttribute(key, value);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
    return config;
}

std::shared_ptr<BaseNode> Parser::parseNamespace() {
    if (!consume(TokenType::NAMESPACE)) {
        reportError("Expected [Namespace]");
        return nullptr;
    }
    
    // 创建命名空间节点
    std::string namespaceName;
    if (current_.getType() == TokenType::IDENTIFIER) {
        namespaceName = current_.getValue();
        current_ = lexer_.nextToken();
    }
    
    auto namespace_node = std::make_shared<NamespaceNode>(namespaceName,
                                                        current_.getLine(), current_.getColumn());
    
    return namespace_node;
}

Token Parser::currentToken() const {
    return current_;
}

Token Parser::nextToken() {
    current_ = lexer_.nextToken();
    return current_;
}

Token Parser::peekToken() {
    return lexer_.peekToken();
}

bool Parser::match(TokenType type) {
    return current_.getType() == type;
}

bool Parser::consume(TokenType type) {
    if (match(type)) {
        current_ = lexer_.nextToken();
        return true;
    }
    return false;
}

bool Parser::matchValue(const std::string& value) {
    return current_.getType() != TokenType::END_OF_FILE && current_.getValue() == value;
}

bool Parser::consumeValue(const std::string& value) {
    if (matchValue(value)) {
        current_ = lexer_.nextToken();
        return true;
    }
    return false;
}

void Parser::parseAttributes(ElementNode* element) {
    while (current_.getType() == TokenType::IDENTIFIER) {
        std::string attrName = current_.getValue();
        current_ = lexer_.nextToken();
        
        if (consume(TokenType::COLON) || consume(TokenType::EQUAL)) {
            std::string attrValue = parseAttributeValue();
            element->setAttribute(attrName, attrValue);
        }
        
        if (consume(TokenType::SEMICOLON)) {
            // 属性结束
        }
    }
}

std::string Parser::parseAttributeValue() {
    if (current_.getType() == TokenType::STRING_LITERAL) {
        std::string value = current_.getValue();
        current_ = lexer_.nextToken();
        return value;
    } else if (current_.getType() == TokenType::UNQUOTED_LITERAL) {
        std::string value = current_.getValue();
        current_ = lexer_.nextToken();
        return value;
    } else if (current_.getType() == TokenType::NUMBER) {
        std::string value = current_.getValue();
        current_ = lexer_.nextToken();
        return value;
    }
    
    return "";
}

void Parser::parseStyleProperties(StyleNode* style) {
    while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string property = current_.getValue();
            current_ = lexer_.nextToken();
            
            if (consume(TokenType::COLON)) {
                std::string value = parseAttributeValue();
                style->addCSSProperty(property, value);
            }
            
            if (consume(TokenType::SEMICOLON)) {
                // 属性结束
            }
        } else {
            current_ = lexer_.nextToken();
        }
    }
}

std::string Parser::parseSelector() {
    if (current_.getType() == TokenType::DOT) {
        current_ = lexer_.nextToken();
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string selector = "." + current_.getValue();
            current_ = lexer_.nextToken();
            return selector;
        }
    } else if (current_.getType() == TokenType::HASH) {
        current_ = lexer_.nextToken();
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string selector = "#" + current_.getValue();
            current_ = lexer_.nextToken();
            return selector;
        }
    } else if (current_.getType() == TokenType::IDENTIFIER) {
        std::string selector = current_.getValue();
        current_ = lexer_.nextToken();
        return selector;
    }
    
    return "";
}

std::string Parser::parseTemplateType() {
    if (current_.getType() == TokenType::AT) {
        current_ = lexer_.nextToken();
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string type = "@" + current_.getValue();
            current_ = lexer_.nextToken();
            return type;
        }
    }
    return "";
}

std::string Parser::parseCustomType() {
    return parseTemplateType();
}

std::string Parser::parseOriginType() {
    return parseTemplateType();
}

std::string Parser::parseImportType() {
    return parseTemplateType();
}

std::string Parser::parseConfigType() {
    return parseTemplateType();
}

std::string Parser::parseNamespaceType() {
    return "";
}

void Parser::reportError(const std::string& message) {
    hasError_ = true;
    std::ostringstream oss;
    oss << "Error at line " << current_.getLine() 
        << ", column " << current_.getColumn() << ": " << message;
    errorMessage_ = oss.str();
}

void Parser::synchronize() {
    while (current_.getType() != TokenType::END_OF_FILE && !isStatementStart(current_.getType())) {
        current_ = lexer_.nextToken();
    }
}

bool Parser::isStatementStart(TokenType type) {
    return isElementStart(type) || isTemplateStart(type) || isCustomStart(type) ||
           isOriginStart(type) || isImportStart(type) || isConfigStart(type) ||
           isNamespaceStart(type);
}

bool Parser::isElementStart(TokenType type) {
    return type == TokenType::IDENTIFIER;
}

bool Parser::isTemplateStart(TokenType type) {
    return type == TokenType::TEMPLATE;
}

bool Parser::isCustomStart(TokenType type) {
    return type == TokenType::CUSTOM;
}

bool Parser::isOriginStart(TokenType type) {
    return type == TokenType::ORIGIN;
}

bool Parser::isImportStart(TokenType type) {
    return type == TokenType::IMPORT;
}

bool Parser::isConfigStart(TokenType type) {
    return type == TokenType::CONFIGURATION;
}

bool Parser::isNamespaceStart(TokenType type) {
    return type == TokenType::NAMESPACE;
}

} // namespace CHTL