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
        } else if (current_.getType() == TokenType::USE) {
            auto use = parseUse();
            if (use) {
                root->addChild(use);
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
    
    // 解析约束（except 语句）
    if (current_.getType() == TokenType::EXCEPT) {
        auto constraint = parseConstraint();
        if (constraint) {
            element->addChild(constraint);
        }
    }
    
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
    
    // 获取模板名称
    std::string templateName;
    if (current_.getType() == TokenType::IDENTIFIER) {
        templateName = current_.getValue();
        current_ = lexer_.nextToken();
    } else {
        reportError("Expected template name");
        return nullptr;
    }
    
    // 根据模板类型创建相应的模板节点
    std::shared_ptr<BaseNode> template_node;
    
    if (templateType == "@Style") {
        auto styleTemplate = std::make_shared<TemplateStyleNode>(templateName,
                                                                current_.getLine(), current_.getColumn());
        
        if (consume(TokenType::LEFT_BRACE)) {
            parseStyleTemplateContent(styleTemplate.get());
            if (!consume(TokenType::RIGHT_BRACE)) {
                reportError("Expected '}'");
            }
        }
        
        // 注册到模板管理器
        TemplateManager::getInstance().addStyleTemplate(templateName, styleTemplate);
        template_node = styleTemplate;
        
    } else if (templateType == "@Element") {
        auto elementTemplate = std::make_shared<TemplateElementNode>(templateName,
                                                                    current_.getLine(), current_.getColumn());
        
        if (consume(TokenType::LEFT_BRACE)) {
            while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
                if (isElementStart(current_.getType())) {
                    auto element = parseElement();
                    if (element) {
                        elementTemplate->addChild(element);
                    }
                } else {
                    current_ = lexer_.nextToken();
                }
            }
            
            if (!consume(TokenType::RIGHT_BRACE)) {
                reportError("Expected '}'");
            }
        }
        
        // 注册到模板管理器
        TemplateManager::getInstance().addElementTemplate(templateName, elementTemplate);
        template_node = elementTemplate;
        
    } else if (templateType == "@Var") {
        auto varTemplate = std::make_shared<TemplateVarNode>(templateName,
                                                            current_.getLine(), current_.getColumn());
        
        if (consume(TokenType::LEFT_BRACE)) {
            parseVarTemplateContent(varTemplate.get());
            if (!consume(TokenType::RIGHT_BRACE)) {
                reportError("Expected '}'");
            }
        }
        
        // 注册到模板管理器
        TemplateManager::getInstance().addVarTemplate(templateName, varTemplate);
        template_node = varTemplate;
        
    } else {
        reportError("Unknown template type: " + templateType);
        return nullptr;
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
    
    // 获取自定义名称
    std::string customName;
    if (current_.getType() == TokenType::IDENTIFIER) {
        customName = current_.getValue();
        current_ = lexer_.nextToken();
    } else {
        reportError("Expected custom name");
        return nullptr;
    }
    
    // 根据自定义类型创建相应的自定义节点
    std::shared_ptr<BaseNode> custom;
    
    if (customType == "@Style") {
        auto customStyle = std::make_shared<CustomStyleNode>(customName,
                                                            current_.getLine(), current_.getColumn());
        
        if (consume(TokenType::LEFT_BRACE)) {
            parseCustomStyleContent(customStyle.get());
            if (!consume(TokenType::RIGHT_BRACE)) {
                reportError("Expected '}'");
            }
        }
        
        custom = customStyle;
        
    } else if (customType == "@Element") {
        auto customElement = std::make_shared<CustomElementNode>(customName,
                                                                current_.getLine(), current_.getColumn());
        
        if (consume(TokenType::LEFT_BRACE)) {
            parseCustomElementContent(customElement.get());
            if (!consume(TokenType::RIGHT_BRACE)) {
                reportError("Expected '}'");
            }
        }
        
        custom = customElement;
        
    } else if (customType == "@Var") {
        auto customVar = std::make_shared<CustomVarNode>(customName,
                                                        current_.getLine(), current_.getColumn());
        
        if (consume(TokenType::LEFT_BRACE)) {
            parseCustomVarContent(customVar.get());
            if (!consume(TokenType::RIGHT_BRACE)) {
                reportError("Expected '}'");
            }
        }
        
        custom = customVar;
        
    } else {
        reportError("Unknown custom type: " + customType);
        return nullptr;
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
    
    // 获取原始嵌入名称（可选）
    std::string originName;
    if (current_.getType() == TokenType::IDENTIFIER) {
        originName = current_.getValue();
        current_ = lexer_.nextToken();
    }
    
    // 创建原始嵌入节点
    auto origin = std::make_shared<OriginNode>(originType,
                                              current_.getLine(), current_.getColumn());
    
    if (!originName.empty()) {
        origin->setAttribute("name", originName);
    }
    
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
    
    // 获取导入路径
    std::string importPath;
    if (current_.getType() == TokenType::STRING_LITERAL) {
        importPath = current_.getValue();
        current_ = lexer_.nextToken();
    } else if (current_.getType() == TokenType::UNQUOTED_LITERAL) {
        importPath = current_.getValue();
        current_ = lexer_.nextToken();
    } else {
        reportError("Expected import path");
        return nullptr;
    }
    
    // 获取导入名称（可选）
    std::string importName;
    if (current_.getType() == TokenType::AS) {
        current_ = lexer_.nextToken();
        if (current_.getType() == TokenType::IDENTIFIER) {
            importName = current_.getValue();
            current_ = lexer_.nextToken();
        }
    }
    
    // 创建导入节点
    auto import = std::make_shared<ImportNode>(importType,
                                             current_.getLine(), current_.getColumn());
    
    import->setAttribute("path", importPath);
    if (!importName.empty()) {
        import->setAttribute("name", importName);
    }
    
    return import;
}

std::shared_ptr<BaseNode> Parser::parseConfig() {
    if (!consume(TokenType::CONFIGURATION)) {
        reportError("Expected [Configuration]");
        return nullptr;
    }
    
    // 获取配置组名称（可选）
    std::string configName;
    if (current_.getType() == TokenType::IDENTIFIER) {
        configName = current_.getValue();
        current_ = lexer_.nextToken();
    }
    
    // 创建配置节点
    auto config = std::make_shared<ConfigNode>(current_.getLine(), current_.getColumn());
    
    if (!configName.empty()) {
        config->setAttribute("name", configName);
    }
    
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string key = current_.getValue();
                current_ = lexer_.nextToken();
                
                if (consume(TokenType::EQUAL)) {
                    std::string value = parseAttributeValue();
                    config->setAttribute(key, value);
                }
            } else if (current_.getType() == TokenType::LEFT_BRACKET) {
                // 处理 [Name] 块
                current_ = lexer_.nextToken();
                if (current_.getType() == TokenType::IDENTIFIER && current_.getValue() == "Name") {
                    current_ = lexer_.nextToken();
                    if (consume(TokenType::RIGHT_BRACKET)) {
                        parseNameBlock(config.get());
                    }
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
    
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (isElementStart(current_.getType())) {
                auto element = parseElement();
                if (element) {
                    namespace_node->addChild(element);
                }
            } else if (current_.getType() == TokenType::EXCEPT) {
                // 处理全局约束
                auto constraint = parseConstraint();
                if (constraint) {
                    namespace_node->addChild(constraint);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
    
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

void Parser::parseStyleTemplateContent(TemplateStyleNode* styleTemplate) {
    while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string property = current_.getValue();
            current_ = lexer_.nextToken();
            
            if (consume(TokenType::COLON)) {
                std::string value = parseAttributeValue();
                styleTemplate->addCSSProperty(property, value);
            }
            
            if (consume(TokenType::SEMICOLON)) {
                // 属性结束
            }
        } else if (current_.getType() == TokenType::TEMPLATE_STYLE) {
            // 处理模板继承
            current_ = lexer_.nextToken();
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string inheritedTemplate = current_.getValue();
                styleTemplate->addInheritedTemplate(inheritedTemplate);
                current_ = lexer_.nextToken();
            }
        } else {
            current_ = lexer_.nextToken();
        }
    }
}

void Parser::parseVarTemplateContent(TemplateVarNode* varTemplate) {
    while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string varName = current_.getValue();
            current_ = lexer_.nextToken();
            
            if (consume(TokenType::COLON)) {
                std::string varValue = parseAttributeValue();
                varTemplate->addVariable(varName, varValue);
            }
            
            if (consume(TokenType::SEMICOLON)) {
                // 变量结束
            }
        } else if (current_.getType() == TokenType::TEMPLATE_VAR) {
            // 处理模板继承
            current_ = lexer_.nextToken();
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string inheritedTemplate = current_.getValue();
                varTemplate->addInheritedTemplate(inheritedTemplate);
                current_ = lexer_.nextToken();
            }
        } else {
            current_ = lexer_.nextToken();
        }
    }
}

void Parser::parseTemplateInheritance(BaseNode* template_node) {
    // 检查是否有继承关键字
    if (current_.getType() == TokenType::INHERIT) {
        current_ = lexer_.nextToken();
        
        if (current_.getType() == TokenType::TEMPLATE_STYLE ||
            current_.getType() == TokenType::TEMPLATE_ELEMENT ||
            current_.getType() == TokenType::TEMPLATE_VAR) {
            
            current_ = lexer_.nextToken();
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string inheritedTemplate = current_.getValue();
                
                // 根据模板类型添加继承
                if (auto styleTemplate = dynamic_cast<TemplateStyleNode*>(template_node)) {
                    styleTemplate->addInheritedTemplate(inheritedTemplate);
                } else if (auto elementTemplate = dynamic_cast<TemplateElementNode*>(template_node)) {
                    elementTemplate->addInheritedTemplate(inheritedTemplate);
                } else if (auto varTemplate = dynamic_cast<TemplateVarNode*>(template_node)) {
                    varTemplate->addInheritedTemplate(inheritedTemplate);
                }
                
                current_ = lexer_.nextToken();
            }
        }
    }
}

void Parser::parseCustomStyleContent(CustomStyleNode* customStyle) {
    while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string property = current_.getValue();
            current_ = lexer_.nextToken();
            
            if (consume(TokenType::COLON)) {
                std::string value = parseAttributeValue();
                customStyle->addCSSProperty(property, value);
            } else if (consume(TokenType::COMMA)) {
                // 无值属性
                customStyle->addUnvaluedProperty(property);
            }
            
            if (consume(TokenType::SEMICOLON)) {
                // 属性结束
            }
        } else if (current_.getType() == TokenType::TEMPLATE_STYLE) {
            // 处理模板继承
            current_ = lexer_.nextToken();
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string inheritedTemplate = current_.getValue();
                customStyle->addInheritedTemplate(inheritedTemplate);
                current_ = lexer_.nextToken();
            }
        } else if (current_.getType() == TokenType::DELETE) {
            // 处理删除操作
            parseDeleteOperation(customStyle);
        } else {
            current_ = lexer_.nextToken();
        }
    }
}

void Parser::parseCustomElementContent(CustomElementNode* customElement) {
    while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
        if (isElementStart(current_.getType())) {
            auto element = parseElement();
            if (element) {
                customElement->addChild(element);
            }
        } else if (current_.getType() == TokenType::TEMPLATE_ELEMENT) {
            // 处理模板继承
            current_ = lexer_.nextToken();
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string inheritedTemplate = current_.getValue();
                customElement->addInheritedTemplate(inheritedTemplate);
                current_ = lexer_.nextToken();
            }
        } else if (current_.getType() == TokenType::DELETE) {
            // 处理删除操作
            parseDeleteOperation(customElement);
        } else if (current_.getType() == TokenType::INSERT) {
            // 处理插入操作
            parseInsertOperation(customElement);
        } else {
            current_ = lexer_.nextToken();
        }
    }
}

void Parser::parseCustomVarContent(CustomVarNode* customVar) {
    while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string varName = current_.getValue();
            current_ = lexer_.nextToken();
            
            if (consume(TokenType::COLON)) {
                std::string varValue = parseAttributeValue();
                customVar->addVariable(varName, varValue);
            }
            
            if (consume(TokenType::SEMICOLON)) {
                // 变量结束
            }
        } else if (current_.getType() == TokenType::TEMPLATE_VAR) {
            // 处理模板继承
            current_ = lexer_.nextToken();
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string inheritedTemplate = current_.getValue();
                customVar->addInheritedTemplate(inheritedTemplate);
                current_ = lexer_.nextToken();
            }
        } else {
            current_ = lexer_.nextToken();
        }
    }
}

void Parser::parseDeleteOperation(BaseNode* customNode) {
    if (!consume(TokenType::DELETE)) {
        return;
    }
    
    while (current_.getType() != TokenType::SEMICOLON && !hasError_) {
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string item = current_.getValue();
            current_ = lexer_.nextToken();
            
            // 根据节点类型添加删除项
            if (auto customStyle = dynamic_cast<CustomStyleNode*>(customNode)) {
                customStyle->addDeletedProperty(item);
            } else if (auto customElement = dynamic_cast<CustomElementNode*>(customNode)) {
                customElement->addDeletedElement(item);
            }
            
            if (current_.getType() == TokenType::COMMA) {
                current_ = lexer_.nextToken();
            }
        } else {
            current_ = lexer_.nextToken();
        }
    }
    
    if (consume(TokenType::SEMICOLON)) {
        // 删除操作结束
    }
}

void Parser::parseInsertOperation(CustomElementNode* customElement) {
    if (!consume(TokenType::INSERT)) {
        return;
    }
    
    std::string position = "at bottom"; // 默认位置
    
    if (current_.getType() == TokenType::AFTER) {
        position = "after";
        current_ = lexer_.nextToken();
    } else if (current_.getType() == TokenType::BEFORE) {
        position = "before";
        current_ = lexer_.nextToken();
    } else if (current_.getType() == TokenType::REPLACE) {
        position = "replace";
        current_ = lexer_.nextToken();
    } else if (current_.getType() == TokenType::AT_TOP) {
        position = "at top";
        current_ = lexer_.nextToken();
    } else if (current_.getType() == TokenType::AT_BOTTOM) {
        position = "at bottom";
        current_ = lexer_.nextToken();
    }
    
    std::string selector;
    if (current_.getType() == TokenType::IDENTIFIER) {
        selector = current_.getValue();
        current_ = lexer_.nextToken();
    }
    
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (isElementStart(current_.getType())) {
                auto element = parseElement();
                if (element) {
                    customElement->addInsertedElement(position, selector, element);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
}

void Parser::parseNameBlock(ConfigNode* config) {
    if (consume(TokenType::LEFT_BRACE)) {
        while (current_.getType() != TokenType::RIGHT_BRACE && !hasError_) {
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string keyword = current_.getValue();
                current_ = lexer_.nextToken();
                
                if (consume(TokenType::EQUAL)) {
                    std::string value = parseAttributeValue();
                    config->setAttribute("name_" + keyword, value);
                }
            } else {
                current_ = lexer_.nextToken();
            }
        }
        
        if (!consume(TokenType::RIGHT_BRACE)) {
            reportError("Expected '}'");
        }
    }
}

std::shared_ptr<BaseNode> Parser::parseUse() {
    if (!consume(TokenType::USE)) {
        reportError("Expected use");
        return nullptr;
    }
    
    // 创建 Use 节点（使用 ConstraintNode 作为基类）
    auto use = std::make_shared<ConstraintNode>(ConstraintNode::ConstraintType::PRECISE,
                                               current_.getLine(), current_.getColumn());
    
    if (current_.getType() == TokenType::IDENTIFIER) {
        std::string useTarget = current_.getValue();
        current_ = lexer_.nextToken();
        
        if (useTarget == "html5") {
            use->setAttribute("type", "html5");
        } else {
            use->setAttribute("type", "config");
            use->setAttribute("target", useTarget);
        }
    } else if (current_.getType() == TokenType::AT) {
        current_ = lexer_.nextToken();
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string configName = "@" + current_.getValue();
            current_ = lexer_.nextToken();
            use->setAttribute("type", "config");
            use->setAttribute("target", configName);
        }
    }
    
    if (consume(TokenType::SEMICOLON)) {
        // Use 语句结束
    }
    
    return use;
}

std::shared_ptr<BaseNode> Parser::parseConstraint() {
    if (!consume(TokenType::EXCEPT)) {
        reportError("Expected except");
        return nullptr;
    }
    
    // 创建约束节点
    auto constraint = std::make_shared<ConstraintNode>(ConstraintNode::ConstraintType::PRECISE,
                                                      current_.getLine(), current_.getColumn());
    
    parseExceptConstraint(constraint.get());
    
    return constraint;
}

void Parser::parseExceptConstraint(ConstraintNode* constraint) {
    while (current_.getType() != TokenType::SEMICOLON && 
           current_.getType() != TokenType::RIGHT_BRACE && 
           !hasError_) {
        
        if (current_.getType() == TokenType::IDENTIFIER) {
            std::string element = current_.getValue();
            current_ = lexer_.nextToken();
            
            // 检查是否为类型约束
            if (element == "@Html") {
                constraint->addTypeConstraint("@Html");
            } else if (element == "[Custom]") {
                constraint->addTypeConstraint("[Custom]");
            } else if (element == "[Template]") {
                constraint->addTypeConstraint("[Template]");
                // 检查是否有具体的模板类型
                if (current_.getType() == TokenType::AT) {
                    current_ = lexer_.nextToken();
                    if (current_.getType() == TokenType::IDENTIFIER) {
                        std::string templateType = "@" + current_.getValue();
                        constraint->addTypeConstraint("[Template] " + templateType);
                        current_ = lexer_.nextToken();
                    }
                }
            } else {
                // 精确约束
                constraint->addPreciseConstraint(element);
            }
        } else if (current_.getType() == TokenType::LEFT_BRACKET) {
            // 处理 [Custom] 或 [Template] 约束
            current_ = lexer_.nextToken();
            if (current_.getType() == TokenType::IDENTIFIER) {
                std::string type = "[" + current_.getValue() + "]";
                current_ = lexer_.nextToken();
                
                if (consume(TokenType::RIGHT_BRACKET)) {
                    if (type == "[Custom]") {
                        constraint->addTypeConstraint("[Custom]");
                    } else if (type == "[Template]") {
                        constraint->addTypeConstraint("[Template]");
                        // 检查是否有具体的模板类型
                        if (current_.getType() == TokenType::AT) {
                            current_ = lexer_.nextToken();
                            if (current_.getType() == TokenType::IDENTIFIER) {
                                std::string templateType = "@" + current_.getValue();
                                constraint->addTypeConstraint("[Template] " + templateType);
                                current_ = lexer_.nextToken();
                            }
                        }
                    }
                }
            }
        } else {
            current_ = lexer_.nextToken();
        }
        
        if (current_.getType() == TokenType::COMMA) {
            current_ = lexer_.nextToken();
        }
    }
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