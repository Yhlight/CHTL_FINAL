#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/TemplateNode.h"
#include "CHTL/CHTLNode/CustomNode.h"
#include "CHTL/CHTLNode/ImportNode.h"
#include "CHTL/CHTLNode/NamespaceNode.h"
#include "CHTL/CHTLNode/ConfigurationNode.h"
#include "CHTL/CHTLNode/ModuleNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_currentIndex(0)
    , m_debugMode(false)
{
}

CHTLParser::~CHTLParser() {
}

std::shared_ptr<BaseNode> CHTLParser::parse(const TokenList& tokens) {
    m_tokens = tokens;
    m_currentIndex = 0;
    
    if (m_debugMode) {
        std::cout << "[CHTLParser] Starting parse with " << tokens.size() << " tokens" << std::endl;
    }
    
    try {
        return parseDocument();
    } catch (const std::exception& e) {
        addError("Parse error: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parse(const std::string& sourceCode) {
    // 简单的扫描器实现
    TokenList tokens;
    size_t pos = 0;
    
    while (pos < sourceCode.length()) {
        // 跳过空白字符
        while (pos < sourceCode.length() && std::isspace(sourceCode[pos])) {
            pos++;
        }
        
        if (pos >= sourceCode.length()) break;
        
        // 识别标识符和关键字
        if (std::isalpha(sourceCode[pos])) {
            size_t start = pos;
            while (pos < sourceCode.length() && (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '-' || sourceCode[pos] == '_')) {
                pos++;
            }
            std::string value = sourceCode.substr(start, pos - start);
            
            // 检查是否为关键字
            if (value == "text") {
                tokens.addToken(TokenType::TEXT, value, 1, 1);
            } else if (value == "style") {
                tokens.addToken(TokenType::STYLE, value, 1, 1);
            } else if (value == "script") {
                tokens.addToken(TokenType::SCRIPT, value, 1, 1);
            } else {
                tokens.addToken(TokenType::IDENTIFIER, value, 1, 1);
            }
        }
        // 识别@符号
        else if (sourceCode[pos] == '@') {
            tokens.addToken(TokenType::AT, "@", 1, 1);
            pos++;
        }
        // 识别左大括号
        else if (sourceCode[pos] == '{') {
            tokens.addToken(TokenType::LEFT_BRACE, "{", 1, 1);
            pos++;
        }
        // 识别右大括号
        else if (sourceCode[pos] == '}') {
            tokens.addToken(TokenType::RIGHT_BRACE, "}", 1, 1);
            pos++;
        }
        // 识别冒号
        else if (sourceCode[pos] == ':') {
            tokens.addToken(TokenType::COLON, ":", 1, 1);
            pos++;
        }
        // 识别分号
        else if (sourceCode[pos] == ';') {
            tokens.addToken(TokenType::SEMICOLON, ";", 1, 1);
            pos++;
        }
        // 识别字符串
        else if (sourceCode[pos] == '"') {
            size_t start = pos;
            pos++;
            while (pos < sourceCode.length() && sourceCode[pos] != '"') {
                pos++;
            }
            if (pos < sourceCode.length()) {
                pos++;
                std::string value = sourceCode.substr(start + 1, pos - start - 2);
                tokens.addToken(TokenType::STRING, value, 1, 1);
            }
        }
        // 其他字符
        else {
            pos++;
        }
    }
    
    return parse(tokens);
}

void CHTLParser::reset() {
    m_currentIndex = 0;
}

void CHTLParser::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CHTLParser::isDebugMode() const {
    return m_debugMode;
}

std::shared_ptr<BaseNode> CHTLParser::parseDocument() {
    auto document = std::make_shared<BaseNode>(NodeType::ELEMENT);
    document->setName("document");
    
    skipWhitespace();
    
    while (!isAtEnd()) {
        auto node = parseElement();
        if (node) {
            document->addChild(node);
        } else {
            // 尝试解析其他类型的节点
            if (checkToken(TokenType::TEXT)) {
                node = parseText();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::COMMENT)) {
                node = parseComment();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::TEMPLATE)) {
                node = parseTemplate();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::CUSTOM)) {
                node = parseCustom();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::ORIGIN)) {
                node = parseOrigin();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::IMPORT)) {
                node = parseImport();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::CONFIGURATION)) {
                node = parseConfiguration();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::NAMESPACE)) {
                node = parseNamespace();
                if (node) {
                    document->addChild(node);
                }
            } else {
                addError("Unexpected token: " + currentToken().getValue());
                nextToken();
            }
        }
        
        skipWhitespace();
    }
    
    return document;
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    if (isAtEnd()) return nullptr;
    
    // 检查是否为自定义语法
    if (checkToken(TokenType::AT)) {
        return parseCustom();
    }
    
    // 检查是否为HTML元素
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        auto element = std::make_shared<ElementNode>(currentToken().getValue());
        element->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        if (m_debugMode) {
            std::cout << "[CHTLParser] Parsing element: " << currentToken().getValue() << std::endl;
        }
        
        nextToken(); // 消费元素名
        
        // 解析属性
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            // 解析属性列表
            parseAttributes(element);
            
            // 解析子节点
            skipWhitespace();
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto child = parseElement();
                if (child) {
                    element->addChild(child);
                } else {
                    // 尝试解析文本节点
                    auto textNode = parseText();
                    if (textNode) {
                        element->addChild(textNode);
                    } else {
                        break;
                    }
                }
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after element");
            }
        }
        
        return element;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseText() {
    if (isAtEnd()) return nullptr;
    
    // 检查是否为text关键字
    if (checkToken(TokenType::TEXT)) {
        auto textNode = std::make_shared<TextNode>();
        textNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        nextToken(); // 消费text关键字
        
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 收集文本内容
            std::string textContent;
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::STRING) {
                    textContent += currentToken().getValue();
                } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                    textContent += currentToken().getValue();
                } else if (currentToken().getType() == TokenType::NUMBER) {
                    textContent += currentToken().getValue();
                } else if (!std::isspace(currentToken().getValue()[0])) {
                    textContent += currentToken().getValue();
                }
                nextToken();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after text content");
            }
            
            textNode->setText(textContent);
            return textNode;
        } else if (checkToken(TokenType::COLON)) {
            // text: "value" 形式
            nextToken(); // 消费 :
            skipWhitespace();
            
            std::string value;
            if (currentToken().getType() == TokenType::STRING) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                value = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected string or identifier after text:");
            }
            
            textNode->setText(value);
            return textNode;
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    if (isAtEnd()) return nullptr;
    
    if (currentToken().getType() == TokenType::COMMENT) {
        auto commentNode = std::make_shared<TextNode>();
        commentNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        commentNode->setText(currentToken().getValue());
        commentNode->setGeneratorComment(true);
        
        nextToken();
        return commentNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::TEMPLATE)) {
        nextToken(); // 消费 [Template]
        
        // 解析模板类型和名称
        if (checkToken(TokenType::AT)) {
            nextToken(); // 消费 @
            
            std::string templateType;
            std::string templateName;
            
            // 解析模板类型
            if (currentToken().getType() == TokenType::IDENTIFIER) {
                templateType = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected template type after @");
                return nullptr;
            }
            
            // 解析模板名称
            if (currentToken().getType() == TokenType::IDENTIFIER) {
                templateName = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected template name");
                return nullptr;
            }
            
            // 创建模板节点
            TemplateType type = TemplateType::UNKNOWN;
            if (templateType == "Style") {
                type = TemplateType::STYLE;
            } else if (templateType == "Element") {
                type = TemplateType::ELEMENT;
            } else if (templateType == "Var") {
                type = TemplateType::VAR;
            }
            
            auto templateNode = std::make_shared<TemplateNode>(type, templateName);
            templateNode->setPosition(currentToken().getLine(), currentToken().getColumn());
            
            // 解析模板内容
            if (checkToken(TokenType::LEFT_BRACE)) {
                nextToken(); // 消费 {
                
                skipWhitespace();
                
                // 解析模板内容
                while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                    auto child = parseElement();
                    if (child) {
                        templateNode->addChild(child);
                    } else {
                        // 尝试解析其他类型的节点
                        if (checkToken(TokenType::TEXT)) {
                            child = parseText();
                            if (child) {
                                templateNode->addChild(child);
                            }
                        } else if (checkToken(TokenType::STYLE)) {
                            child = parseStyle();
                            if (child) {
                                templateNode->addChild(child);
                            }
                        } else {
                            break;
                        }
                    }
                    skipWhitespace();
                }
                
                if (checkToken(TokenType::RIGHT_BRACE)) {
                    nextToken(); // 消费 }
                } else {
                    addError("Expected '}' after template content");
                }
            }
            
            return templateNode;
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    if (isAtEnd()) return nullptr;
    
    // 检查是否是自定义语法：@Style, @Element, @Var
    if (checkToken(TokenType::AT)) {
        nextToken(); // 消费 @
        
        std::string customType;
        std::string customName;
        
        // 解析自定义类型
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            customType = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected custom type after @");
            return nullptr;
        }
        
        // 解析自定义名称
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            customName = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected custom name");
            return nullptr;
        }
        
        // 创建自定义节点
        CustomType type = CustomType::UNKNOWN;
        if (customType == "Style") {
            type = CustomType::STYLE;
        } else if (customType == "Element") {
            type = CustomType::ELEMENT;
        } else if (customType == "Var") {
            type = CustomType::VAR;
        }
        
        auto customNode = std::make_shared<CustomNode>(type, customName);
        customNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析自定义内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析自定义内容
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto child = parseElement();
                if (child) {
                    customNode->addChild(child);
                } else {
                    // 尝试解析其他类型的节点
                    if (checkToken(TokenType::TEXT)) {
                        child = parseText();
                        if (child) {
                            customNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::STYLE)) {
                        child = parseStyle();
                        if (child) {
                            customNode->addChild(child);
                        }
                    } else {
                        break;
                    }
                }
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after custom content");
            }
        }
        
        return customNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    // TODO: 实现原始嵌入解析
    addWarning("Origin parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::IMPORT)) {
        nextToken(); // 消费 [Import]
        
        // 解析导入类型
        ImportType importType = ImportType::UNKNOWN;
        if (checkToken(TokenType::IDENTIFIER)) {
            std::string typeStr = currentToken().getValue();
            nextToken();
            
            if (typeStr == "CHTL") {
                importType = ImportType::CHTL;
            } else if (typeStr == "HTML") {
                importType = ImportType::HTML;
            } else if (typeStr == "CSS") {
                importType = ImportType::CSS;
            } else if (typeStr == "JS") {
                importType = ImportType::JS;
            } else if (typeStr == "CMOD") {
                importType = ImportType::CMOD;
            } else if (typeStr == "CJMOD") {
                importType = ImportType::CJMOD;
            }
        }
        
        // 解析文件路径
        std::string filePath;
        if (currentToken().getType() == TokenType::STRING) {
            filePath = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected file path after import type");
            return nullptr;
        }
        
        // 创建导入节点
        auto importNode = std::make_shared<ImportNode>(importType, filePath);
        importNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析导入模式和项目
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析导入项目
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::IDENTIFIER) {
                    std::string item = currentToken().getValue();
                    importNode->addImportItem(item);
                    nextToken();
                    
                    // 检查是否有别名
                    if (checkToken(TokenType::AS)) {
                        nextToken(); // 消费 as
                        if (currentToken().getType() == TokenType::IDENTIFIER) {
                            std::string alias = currentToken().getValue();
                            importNode->setAlias(alias);
                            nextToken();
                        } else {
                            addError("Expected alias after 'as'");
                        }
                    }
                } else if (currentToken().getType() == TokenType::ASTERISK) {
                    // 通配符导入
                    importNode->setImportMode(ImportMode::WILDCARD);
                    nextToken();
                } else {
                    break;
                }
                
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after import items");
            }
        } else {
            // 精确导入
            importNode->setImportMode(ImportMode::PRECISE);
        }
        
        return importNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::CONFIGURATION)) {
        nextToken(); // 消费 [Configuration]
        
        // 解析配置类型
        ConfigurationType configurationType = ConfigurationType::UNKNOWN;
        if (checkToken(TokenType::IDENTIFIER)) {
            std::string typeStr = currentToken().getValue();
            nextToken();
            
            if (typeStr == "Keyword") {
                configurationType = ConfigurationType::KEYWORD;
            } else if (typeStr == "Module") {
                configurationType = ConfigurationType::MODULE;
            } else if (typeStr == "Compiler") {
                configurationType = ConfigurationType::COMPILER;
            } else if (typeStr == "Output") {
                configurationType = ConfigurationType::OUTPUT;
            } else if (typeStr == "Debug") {
                configurationType = ConfigurationType::DEBUG;
            }
        }
        
        // 解析配置名称
        std::string configurationName;
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            configurationName = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected configuration name after configuration type");
            return nullptr;
        }
        
        // 创建配置节点
        auto configurationNode = std::make_shared<ConfigurationNode>(configurationType, configurationName);
        configurationNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析配置内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析配置项
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::IDENTIFIER) {
                    std::string key = currentToken().getValue();
                    nextToken();
                    
                    // 检查是否有值
                    if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
                        nextToken(); // 消费 : 或 =
                        skipWhitespace();
                        
                        // 解析配置值
                        std::string value;
                        if (currentToken().getType() == TokenType::STRING) {
                            value = currentToken().getValue();
                            nextToken();
                        } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                            value = currentToken().getValue();
                            nextToken();
                        } else if (currentToken().getType() == TokenType::NUMBER) {
                            value = currentToken().getValue();
                            nextToken();
                        } else {
                            addError("Expected value after configuration key");
                            break;
                        }
                        
                        configurationNode->addConfigurationItem(key, value);
                    } else {
                        addError("Expected ':' or '=' after configuration key");
                        break;
                    }
                } else {
                    break;
                }
                
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after configuration content");
            }
        }
        
        return configurationNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::NAMESPACE)) {
        nextToken(); // 消费 [Namespace]
        
        // 解析命名空间名称
        std::string namespaceName;
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            namespaceName = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected namespace name after [Namespace]");
            return nullptr;
        }
        
        // 创建命名空间节点
        auto namespaceNode = std::make_shared<NamespaceNode>(namespaceName);
        namespaceNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析命名空间内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析命名空间内容
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto child = parseElement();
                if (child) {
                    namespaceNode->addChild(child);
                } else {
                    // 尝试解析其他类型的节点
                    if (checkToken(TokenType::TEXT)) {
                        child = parseText();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::STYLE)) {
                        child = parseStyle();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::TEMPLATE)) {
                        child = parseTemplate();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::CUSTOM)) {
                        child = parseCustom();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::IMPORT)) {
                        child = parseImport();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else {
                        break;
                    }
                }
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after namespace content");
            }
        }
        
        return namespaceNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseModule() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::MODULE)) {
        nextToken(); // 消费 [Module]
        
        // 解析模块类型
        ModuleType moduleType = ModuleType::UNKNOWN;
        if (checkToken(TokenType::IDENTIFIER)) {
            std::string typeStr = currentToken().getValue();
            nextToken();
            
            if (typeStr == "CMOD") {
                moduleType = ModuleType::CMOD;
            } else if (typeStr == "CJMOD") {
                moduleType = ModuleType::CJMOD;
            } else if (typeStr == "HTML") {
                moduleType = ModuleType::HTML;
            } else if (typeStr == "CSS") {
                moduleType = ModuleType::CSS;
            } else if (typeStr == "JS") {
                moduleType = ModuleType::JS;
            }
        }
        
        // 解析模块名称
        std::string moduleName;
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            moduleName = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected module name after module type");
            return nullptr;
        }
        
        // 创建模块节点
        auto moduleNode = std::make_shared<ModuleNode>(moduleType, moduleName);
        moduleNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析模块内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析模块属性
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::IDENTIFIER) {
                    std::string key = currentToken().getValue();
                    nextToken();
                    
                    // 检查是否有值
                    if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
                        nextToken(); // 消费 : 或 =
                        skipWhitespace();
                        
                        // 解析模块属性值
                        std::string value;
                        if (currentToken().getType() == TokenType::STRING) {
                            value = currentToken().getValue();
                            nextToken();
                        } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                            value = currentToken().getValue();
                            nextToken();
                        } else if (currentToken().getType() == TokenType::NUMBER) {
                            value = currentToken().getValue();
                            nextToken();
                        } else {
                            addError("Expected value after module attribute");
                            break;
                        }
                        
                        // 设置模块属性
                        if (key == "version") {
                            moduleNode->setModuleVersion(value);
                        } else if (key == "path") {
                            moduleNode->setModulePath(value);
                        } else if (key == "description") {
                            moduleNode->setModuleDescription(value);
                        } else if (key == "dependency") {
                            // 解析依赖
                            moduleNode->addModuleDependency(value);
                        } else if (key == "export") {
                            // 解析导出
                            moduleNode->setModuleExport(value, "");
                        } else {
                            addWarning("Unknown module attribute: " + key);
                        }
                    } else {
                        addError("Expected ':' or '=' after module attribute");
                        break;
                    }
                } else {
                    break;
                }
                
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after module content");
            }
        }
        
        return moduleNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyle() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::STYLE)) {
        auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE);
        styleNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        nextToken(); // 消费style关键字
        
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析样式属性
            parseStyleProperties(styleNode);
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after style block");
            }
        }
        
        return styleNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseScript() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::SCRIPT)) {
        auto scriptNode = std::make_shared<BaseNode>(NodeType::SCRIPT);
        scriptNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        nextToken(); // 消费script关键字
        
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 收集脚本内容
            std::string scriptContent;
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                scriptContent += currentToken().getValue();
                nextToken();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after script block");
            }
            
            scriptNode->setValue(scriptContent);
        }
        
        return scriptNode;
    }
    
    return nullptr;
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> element) {
    skipWhitespace();
    
    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto attr = parseAttribute();
        if (!attr.first.empty()) {
            element->setAttribute(attr.first, attr.second);
        } else {
            break;
        }
        skipWhitespace();
    }
}

std::pair<std::string, std::string> CHTLParser::parseAttribute() {
    if (isAtEnd()) return {"", ""};
    
    std::string key;
    std::string value;
    
    // 解析属性名
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        key = currentToken().getValue();
        nextToken();
        
        // 检查是否有值
        if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
            nextToken(); // 消费 : 或 =
            skipWhitespace();
            
            // 解析属性值
            if (currentToken().getType() == TokenType::STRING) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::NUMBER) {
                value = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected value after attribute name");
            }
        }
    }
    
    return {key, value};
}

void CHTLParser::parseStyleProperties(std::shared_ptr<BaseNode> styleNode) {
    skipWhitespace();
    
    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto prop = parseStyleProperty();
        if (!prop.first.empty()) {
            styleNode->setAttribute(prop.first, prop.second);
        } else {
            break;
        }
        skipWhitespace();
    }
}

std::pair<std::string, std::string> CHTLParser::parseStyleProperty() {
    if (isAtEnd()) return {"", ""};
    
    std::string key;
    std::string value;
    
    // 解析属性名
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        key = currentToken().getValue();
        nextToken();
        
        // 检查是否有值
        if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
            nextToken(); // 消费 : 或 =
            skipWhitespace();
            
            // 解析属性值
            if (currentToken().getType() == TokenType::STRING) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::NUMBER) {
                value = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected value after style property name");
            }
        }
    }
    
    return {key, value};
}

std::string CHTLParser::parseSelector() {
    // TODO: 实现选择器解析
    return "";
}

std::string CHTLParser::parseExpression() {
    // TODO: 实现表达式解析
    return "";
}

std::string CHTLParser::parseString() {
    if (currentToken().getType() == TokenType::STRING) {
        std::string value = currentToken().getValue();
        nextToken();
        return value;
    }
    return "";
}

std::string CHTLParser::parseLiteral() {
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        std::string value = currentToken().getValue();
        nextToken();
        return value;
    }
    return "";
}

void CHTLParser::skipWhitespace() {
    while (!isAtEnd() && 
           (currentToken().getType() == TokenType::IDENTIFIER && 
            std::all_of(currentToken().getValue().begin(), currentToken().getValue().end(), ::isspace))) {
        nextToken();
    }
}

bool CHTLParser::checkToken(TokenType type) const {
    return !isAtEnd() && currentToken().getType() == type;
}

bool CHTLParser::checkToken(const std::string& value) const {
    return !isAtEnd() && currentToken().getValue() == value;
}

bool CHTLParser::matchToken(TokenType type) {
    if (checkToken(type)) {
        nextToken();
        return true;
    }
    return false;
}

bool CHTLParser::matchToken(const std::string& value) {
    if (checkToken(value)) {
        nextToken();
        return true;
    }
    return false;
}

const Token& CHTLParser::currentToken() const {
    if (m_currentIndex < m_tokens.size()) {
        return m_tokens[m_currentIndex];
    }
    static Token endToken(TokenType::END_OF_FILE, "", 0, 0);
    return endToken;
}

const Token& CHTLParser::peekToken() const {
    if (m_currentIndex + 1 < m_tokens.size()) {
        return m_tokens[m_currentIndex + 1];
    }
    static Token endToken(TokenType::END_OF_FILE, "", 0, 0);
    return endToken;
}

bool CHTLParser::nextToken() {
    if (m_currentIndex < m_tokens.size()) {
        m_currentIndex++;
        return true;
    }
    return false;
}

bool CHTLParser::isAtEnd() const {
    return m_currentIndex >= m_tokens.size() || 
           currentToken().getType() == TokenType::END_OF_FILE;
}

void CHTLParser::addError(const std::string& message) {
    if (m_context) {
        std::ostringstream oss;
        oss << "Parser Error at " << currentToken().getLine() << ":" 
            << currentToken().getColumn() << ": " << message;
        m_context->addError(oss.str());
    }
}

void CHTLParser::addWarning(const std::string& message) {
    if (m_context) {
        std::ostringstream oss;
        oss << "Parser Warning at " << currentToken().getLine() << ":" 
            << currentToken().getColumn() << ": " << message;
        m_context->addWarning(oss.str());
    }
}

} // namespace CHTL