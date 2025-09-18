#include "CHTL/CHTLParser.h"
#include "CHTL/ElementNode.h"
#include "CHTL/TextNode.h"
#include "CHTL/StyleNode.h"
#include "CHTL/ScriptNode.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
#include "CHTL/NamespaceNode.h"
#include "CHTL/ConfigurationNode.h"
#include "CHTL/OriginNode.h"
#include "CHTL/OperatorNode.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser() 
    : currentPosition_(0) {
    initializeValidators();
}

CHTLParser::~CHTLParser() = default;

void CHTLParser::initializeValidators() {
    // Initialize valid HTML elements
    validElements_ = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "textarea", "select", "option",
        "nav", "header", "footer", "main", "section", "article", "aside",
        "br", "hr", "strong", "em", "b", "i", "u", "s", "code", "pre"
    };
    
    // Initialize valid HTML attributes
    validAttributes_ = {
        "id", "class", "style", "title", "lang", "dir",
        "href", "src", "alt", "width", "height", "type", "value",
        "name", "placeholder", "required", "disabled", "readonly",
        "checked", "selected", "multiple", "autofocus", "autocomplete"
    };
    
    // Initialize valid CSS properties
    validStyleProperties_ = {
        "color", "background-color", "background", "font-size", "font-family",
        "font-weight", "font-style", "text-align", "text-decoration",
        "width", "height", "margin", "padding", "border", "border-radius",
        "display", "position", "top", "left", "right", "bottom",
        "z-index", "opacity", "visibility", "overflow", "float", "clear"
    };
    
    // Initialize valid template names
    validTemplateNames_ = {
        "Style", "Element", "Var", "Html", "JavaScript", "Chtl", "CJmod"
    };
    
    // Initialize valid custom names
    validCustomNames_ = {
        "Style", "Element", "Var", "Html", "JavaScript", "Chtl", "CJmod"
    };
    
    // Initialize valid origin types
    validOriginTypes_ = {
        "Html", "Style", "JavaScript", "Vue", "Markdown", "Chtl", "CJmod"
    };
    
    // Initialize valid operator types
    validOperatorTypes_ = {
        "delete", "insert", "after", "before", "replace", "at", "top", "bottom"
    };
}

CHTLParser::ParseResult CHTLParser::parse(const std::vector<Token>& tokens, const ParseOptions& options) {
    ParseResult result;
    currentOptions_ = options;
    currentTokens_ = tokens;
    currentPosition_ = 0;
    
    try {
        result = parseDocument(tokens, 0);
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
    }
    
    return result;
}

void CHTLParser::setContext(std::shared_ptr<void> context) {
    context_ = context;
}

std::shared_ptr<void> CHTLParser::getContext() const {
    return context_;
}

void CHTLParser::reset() {
    currentPosition_ = 0;
    currentTokens_.clear();
}

CHTLParser::ParseResult CHTLParser::parseDocument(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    // Create root element
    auto rootElement = std::make_shared<ElementNode>("document");
    
    while (!isAtEnd(tokens, currentPosition_)) {
        skipWhitespace(tokens, currentPosition_);
        if (isAtEnd(tokens, currentPosition_)) break;
        
        auto currentToken = getCurrentToken(tokens, currentPosition_);
        
        // Parse different types of nodes
        if (currentToken.type == TokenType::IDENTIFIER) {
            // Regular HTML element
            auto elementResult = parseElement(tokens, currentPosition_);
            if (elementResult.success) {
                rootElement->addChild(elementResult.ast);
                currentPosition_ = elementResult.tokensConsumed;
            } else {
                handleError("Failed to parse element: " + elementResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.type == TokenType::TEXT) {
            // Text node
            auto textResult = parseText(tokens, currentPosition_);
            if (textResult.success) {
                rootElement->addChild(textResult.ast);
                currentPosition_ = textResult.tokensConsumed;
            } else {
                handleError("Failed to parse text: " + textResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "text") {
            // Text block
            auto textResult = parseText(tokens, currentPosition_);
            if (textResult.success) {
                rootElement->addChild(textResult.ast);
                currentPosition_ = textResult.tokensConsumed;
            } else {
                handleError("Failed to parse text block: " + textResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "style") {
            // Style block
            auto styleResult = parseStyle(tokens, currentPosition_);
            if (styleResult.success) {
                rootElement->addChild(styleResult.ast);
                currentPosition_ = styleResult.tokensConsumed;
            } else {
                handleError("Failed to parse style: " + styleResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "script") {
            // Script block
            auto scriptResult = parseScript(tokens, currentPosition_);
            if (scriptResult.success) {
                rootElement->addChild(scriptResult.ast);
                currentPosition_ = scriptResult.tokensConsumed;
            } else {
                handleError("Failed to parse script: " + scriptResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "[Template]") {
            // Template
            auto templateResult = parseTemplate(tokens, currentPosition_);
            if (templateResult.success) {
                rootElement->addChild(templateResult.ast);
                currentPosition_ = templateResult.tokensConsumed;
            } else {
                handleError("Failed to parse template: " + templateResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "[Custom]") {
            // Custom
            auto customResult = parseCustom(tokens, currentPosition_);
            if (customResult.success) {
                rootElement->addChild(customResult.ast);
                currentPosition_ = customResult.tokensConsumed;
            } else {
                handleError("Failed to parse custom: " + customResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "[Import]") {
            // Import
            auto importResult = parseImport(tokens, currentPosition_);
            if (importResult.success) {
                rootElement->addChild(importResult.ast);
                currentPosition_ = importResult.tokensConsumed;
            } else {
                handleError("Failed to parse import: " + importResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "[Namespace]") {
            // Namespace
            auto namespaceResult = parseNamespace(tokens, currentPosition_);
            if (namespaceResult.success) {
                rootElement->addChild(namespaceResult.ast);
                currentPosition_ = namespaceResult.tokensConsumed;
            } else {
                handleError("Failed to parse namespace: " + namespaceResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "[Configuration]") {
            // Configuration
            auto configResult = parseConfiguration(tokens, currentPosition_);
            if (configResult.success) {
                rootElement->addChild(configResult.ast);
                currentPosition_ = configResult.tokensConsumed;
            } else {
                handleError("Failed to parse configuration: " + configResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "[Origin]") {
            // Origin
            auto originResult = parseOrigin(tokens, currentPosition_);
            if (originResult.success) {
                rootElement->addChild(originResult.ast);
                currentPosition_ = originResult.tokensConsumed;
            } else {
                handleError("Failed to parse origin: " + originResult.errorMessage, result);
                return result;
            }
        } else if (currentToken.value == "delete" || currentToken.value == "insert") {
            // Operator
            auto operatorResult = parseOperator(tokens, currentPosition_);
            if (operatorResult.success) {
                rootElement->addChild(operatorResult.ast);
                currentPosition_ = operatorResult.tokensConsumed;
            } else {
                handleError("Failed to parse operator: " + operatorResult.errorMessage, result);
                return result;
            }
        } else {
            // Unknown token
            handleWarning("Unknown token: " + std::string(currentToken.value), result);
            advancePosition(currentPosition_);
        }
    }
    
    result.ast = rootElement;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseElement(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.type != TokenType::IDENTIFIER) {
        handleError("Expected element name", result);
        return result;
    }
    
    std::string elementName = std::string(currentToken.value);
    if (!validateElement(elementName)) {
        handleWarning("Unknown element: " + elementName, result);
    }
    
    advancePosition(currentPosition_);
    
    // Parse attributes
    auto attributes = parseAttributes(tokens, currentPosition_);
    
    // Create element node
    auto element = std::make_shared<ElementNode>(elementName);
    for (const auto& attr : attributes) {
        element->setAttribute(attr.first, attr.second);
    }
    
    // Parse children
    skipWhitespace(tokens, currentPosition_);
    if (matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        advancePosition(currentPosition_);
        
        while (!isAtEnd(tokens, currentPosition_)) {
            skipWhitespace(tokens, currentPosition_);
            if (isAtEnd(tokens, currentPosition_)) break;
            
            auto childToken = getCurrentToken(tokens, currentPosition_);
            if (childToken.type == TokenType::RIGHT_BRACE) {
                advancePosition(currentPosition_);
                break;
            }
            
            // Parse child element
            if (childToken.type == TokenType::IDENTIFIER) {
                auto childResult = parseElement(tokens, currentPosition_);
                if (childResult.success) {
                    element->addChild(childResult.ast);
                    currentPosition_ = childResult.tokensConsumed;
                } else {
                    handleError("Failed to parse child element: " + childResult.errorMessage, result);
                    return result;
                }
            } else if (childToken.type == TokenType::TEXT) {
                auto textResult = parseText(tokens, currentPosition_);
                if (textResult.success) {
                    element->addChild(textResult.ast);
                    currentPosition_ = textResult.tokensConsumed;
                } else {
                    handleError("Failed to parse child text: " + textResult.errorMessage, result);
                    return result;
                }
            } else if (childToken.value == "text") {
                auto textResult = parseText(tokens, currentPosition_);
                if (textResult.success) {
                    element->addChild(textResult.ast);
                    currentPosition_ = textResult.tokensConsumed;
                } else {
                    handleError("Failed to parse child text block: " + textResult.errorMessage, result);
                    return result;
                }
            } else if (childToken.value == "style") {
                auto styleResult = parseStyle(tokens, currentPosition_);
                if (styleResult.success) {
                    element->addChild(styleResult.ast);
                    currentPosition_ = styleResult.tokensConsumed;
                } else {
                    handleError("Failed to parse child style: " + styleResult.errorMessage, result);
                    return result;
                }
            } else if (childToken.value == "script") {
                auto scriptResult = parseScript(tokens, currentPosition_);
                if (scriptResult.success) {
                    element->addChild(scriptResult.ast);
                    currentPosition_ = scriptResult.tokensConsumed;
                } else {
                    handleError("Failed to parse child script: " + scriptResult.errorMessage, result);
                    return result;
                }
            } else {
                handleWarning("Unknown child token: " + std::string(childToken.value), result);
                advancePosition(currentPosition_);
            }
        }
    }
    
    result.ast = element;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseText(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    std::string textContent;
    
    if (currentToken.value == "text") {
        // Text block
        advancePosition(currentPosition_);
        skipWhitespace(tokens, currentPosition_);
        
        if (matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
            advancePosition(currentPosition_);
            
            while (!isAtEnd(tokens, currentPosition_)) {
                skipWhitespace(tokens, currentPosition_);
                if (isAtEnd(tokens, currentPosition_)) break;
                
                auto childToken = getCurrentToken(tokens, currentPosition_);
                if (childToken.type == TokenType::RIGHT_BRACE) {
                    advancePosition(currentPosition_);
                    break;
                }
                
                if (childToken.type == TokenType::TEXT || childToken.type == TokenType::STRING) {
                    textContent += std::string(childToken.value);
                    advancePosition(currentPosition_);
                } else {
                    handleWarning("Unexpected token in text block: " + std::string(childToken.value), result);
                    advancePosition(currentPosition_);
                }
            }
        }
    } else if (currentToken.type == TokenType::TEXT || currentToken.type == TokenType::STRING) {
        // Direct text
        textContent = std::string(currentToken.value);
        advancePosition(currentPosition_);
    } else {
        handleError("Expected text content", result);
        return result;
    }
    
    auto textNode = std::make_shared<TextNode>(textContent);
    result.ast = textNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseStyle(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "style") {
        handleError("Expected 'style' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    if (!matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        handleError("Expected '{' after 'style'", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    
    // Parse style properties
    auto properties = parseStyleProperties(tokens, currentPosition_);
    
    auto styleNode = std::make_shared<StyleNode>();
    for (const auto& prop : properties) {
        styleNode->setProperty("", prop.first, prop.second);
    }
    
    result.ast = styleNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseScript(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "script") {
        handleError("Expected 'script' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    if (!matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        handleError("Expected '{' after 'script'", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    
    // Parse script content
    std::string content = parseScriptContent(tokens, currentPosition_);
    
    auto scriptNode = std::make_shared<ScriptNode>();
    result.ast = scriptNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseTemplate(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "[Template]") {
        handleError("Expected '[Template]' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse template type and name
    auto typeToken = getCurrentToken(tokens, currentPosition_);
    if (typeToken.type != TokenType::IDENTIFIER) {
        handleError("Expected template type", result);
        return result;
    }
    
    std::string templateType = std::string(typeToken.value);
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    auto nameToken = getCurrentToken(tokens, currentPosition_);
    if (nameToken.type != TokenType::IDENTIFIER) {
        handleError("Expected template name", result);
        return result;
    }
    
    std::string templateName = std::string(nameToken.value);
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    if (!matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        handleError("Expected '{' after template declaration", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    
    // Parse template content
    auto templateNode = std::make_shared<TemplateNode>(templateType, templateName);
    
    while (!isAtEnd(tokens, currentPosition_)) {
        skipWhitespace(tokens, currentPosition_);
        if (isAtEnd(tokens, currentPosition_)) break;
        
        auto childToken = getCurrentToken(tokens, currentPosition_);
        if (childToken.type == TokenType::RIGHT_BRACE) {
            advancePosition(currentPosition_);
            break;
        }
        
        // Parse child elements
        if (childToken.type == TokenType::IDENTIFIER) {
            auto childResult = parseElement(tokens, currentPosition_);
            if (childResult.success) {
                templateNode->addChild(childResult.ast);
                currentPosition_ = childResult.tokensConsumed;
            } else {
                handleError("Failed to parse template child: " + childResult.errorMessage, result);
                return result;
            }
        } else {
            handleWarning("Unexpected token in template: " + std::string(childToken.value), result);
            advancePosition(currentPosition_);
        }
    }
    
    result.ast = templateNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseCustom(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "[Custom]") {
        handleError("Expected '[Custom]' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse custom type and name
    auto typeToken = getCurrentToken(tokens, currentPosition_);
    if (typeToken.type != TokenType::IDENTIFIER) {
        handleError("Expected custom type", result);
        return result;
    }
    
    std::string customType = std::string(typeToken.value);
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    auto nameToken = getCurrentToken(tokens, currentPosition_);
    if (nameToken.type != TokenType::IDENTIFIER) {
        handleError("Expected custom name", result);
        return result;
    }
    
    std::string customName = std::string(nameToken.value);
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    if (!matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        handleError("Expected '{' after custom declaration", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    
    // Parse custom content
    auto customNode = std::make_shared<CustomNode>(customType, customName);
    
    while (!isAtEnd(tokens, currentPosition_)) {
        skipWhitespace(tokens, currentPosition_);
        if (isAtEnd(tokens, currentPosition_)) break;
        
        auto childToken = getCurrentToken(tokens, currentPosition_);
        if (childToken.type == TokenType::RIGHT_BRACE) {
            advancePosition(currentPosition_);
            break;
        }
        
        // Parse child elements
        if (childToken.type == TokenType::IDENTIFIER) {
            auto childResult = parseElement(tokens, currentPosition_);
            if (childResult.success) {
                customNode->addChild(childResult.ast);
                currentPosition_ = childResult.tokensConsumed;
            } else {
                handleError("Failed to parse custom child: " + childResult.errorMessage, result);
                return result;
            }
        } else {
            handleWarning("Unexpected token in custom: " + std::string(childToken.value), result);
            advancePosition(currentPosition_);
        }
    }
    
    result.ast = customNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseImport(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "[Import]") {
        handleError("Expected '[Import]' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse import type
    auto typeToken = getCurrentToken(tokens, currentPosition_);
    if (typeToken.type != TokenType::IDENTIFIER) {
        handleError("Expected import type", result);
        return result;
    }
    
    std::string importType = std::string(typeToken.value);
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse import path
    std::string importPath = parseImportPath(tokens, currentPosition_);
    if (importPath.empty()) {
        handleError("Expected import path", result);
        return result;
    }
    
    // Parse optional alias
    std::string alias = parseImportAlias(tokens, currentPosition_);
    
    auto importNode = std::make_shared<ImportNode>(importType, importPath, alias);
    result.ast = importNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseNamespace(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "[Namespace]") {
        handleError("Expected '[Namespace]' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse namespace name
    std::string namespaceName = parseNamespaceName(tokens, currentPosition_);
    if (namespaceName.empty()) {
        handleError("Expected namespace name", result);
        return result;
    }
    
    auto namespaceNode = std::make_shared<NamespaceNode>(namespaceName);
    
    // Parse optional namespace content
    skipWhitespace(tokens, currentPosition_);
    if (matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        advancePosition(currentPosition_);
        
        while (!isAtEnd(tokens, currentPosition_)) {
            skipWhitespace(tokens, currentPosition_);
            if (isAtEnd(tokens, currentPosition_)) break;
            
            auto childToken = getCurrentToken(tokens, currentPosition_);
            if (childToken.type == TokenType::RIGHT_BRACE) {
                advancePosition(currentPosition_);
                break;
            }
            
            // Parse child elements
            if (childToken.type == TokenType::IDENTIFIER) {
                auto childResult = parseElement(tokens, currentPosition_);
                if (childResult.success) {
                    namespaceNode->addChild(childResult.ast);
                    currentPosition_ = childResult.tokensConsumed;
                } else {
                    handleError("Failed to parse namespace child: " + childResult.errorMessage, result);
                    return result;
                }
            } else {
                handleWarning("Unexpected token in namespace: " + std::string(childToken.value), result);
                advancePosition(currentPosition_);
            }
        }
    }
    
    result.ast = namespaceNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseConfiguration(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "[Configuration]") {
        handleError("Expected '[Configuration]' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse optional configuration name
    std::string configName;
    auto nameToken = getCurrentToken(tokens, currentPosition_);
    if (nameToken.type == TokenType::IDENTIFIER) {
        configName = nameToken.value;
        advancePosition(currentPosition_);
        skipWhitespace(tokens, currentPosition_);
    }
    
    if (!matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        handleError("Expected '{' after configuration declaration", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    
    // Parse configuration properties
    auto properties = parseConfigurationProperties(tokens, currentPosition_);
    
    auto configNode = std::make_shared<ConfigurationNode>(configName);
    for (const auto& prop : properties) {
        // configNode->setProperty(prop.first, prop.second); // TODO: Implement setProperty method
    }
    
    result.ast = configNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseOrigin(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "[Origin]") {
        handleError("Expected '[Origin]' keyword", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse origin type
    auto typeToken = getCurrentToken(tokens, currentPosition_);
    if (typeToken.type != TokenType::IDENTIFIER) {
        handleError("Expected origin type", result);
        return result;
    }
    
    std::string originType = std::string(typeToken.value);
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse optional origin name
    std::string originName;
    auto nameToken = getCurrentToken(tokens, currentPosition_);
    if (nameToken.type == TokenType::IDENTIFIER) {
        originName = nameToken.value;
        advancePosition(currentPosition_);
        skipWhitespace(tokens, currentPosition_);
    }
    
    if (!matchToken(tokens, currentPosition_, TokenType::LEFT_BRACE)) {
        handleError("Expected '{' after origin declaration", result);
        return result;
    }
    
    advancePosition(currentPosition_);
    
    // Parse origin content
    std::string content = parseOriginContent(tokens, currentPosition_);
    
    auto originNode = std::make_shared<OriginNode>(originType, originName, content);
    result.ast = originNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

CHTLParser::ParseResult CHTLParser::parseOperator(const std::vector<Token>& tokens, size_t startPos) {
    ParseResult result;
    currentPosition_ = startPos;
    
    auto currentToken = getCurrentToken(tokens, currentPosition_);
    if (currentToken.value != "delete" && currentToken.value != "insert") {
        handleError("Expected operator type", result);
        return result;
    }
    
    std::string operatorType = std::string(currentToken.value);
    advancePosition(currentPosition_);
    skipWhitespace(tokens, currentPosition_);
    
    // Parse operator targets
    std::vector<std::string> targets = parseOperatorTargets(tokens, currentPosition_);
    
    auto operatorNode = std::make_shared<OperatorNode>(operatorType, targets);
    result.ast = operatorNode;
    result.tokensConsumed = currentPosition_ - startPos;
    return result;
}

std::map<std::string, std::string> CHTLParser::parseAttributes(const std::vector<Token>& tokens, size_t& pos) {
    std::map<std::string, std::string> attributes;
    
    while (!isAtEnd(tokens, pos)) {
        skipWhitespace(tokens, pos);
        if (isAtEnd(tokens, pos)) break;
        
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::LEFT_BRACE || 
            currentToken.type == TokenType::RIGHT_BRACE ||
            currentToken.type == TokenType::SEMICOLON) {
            break;
        }
        
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string attrName = std::string(currentToken.value);
            advancePosition(pos);
            skipWhitespace(tokens, pos);
            
            if (matchToken(tokens, pos, TokenType::COLON) || matchToken(tokens, pos, TokenType::EQUAL)) {
                advancePosition(pos);
                skipWhitespace(tokens, pos);
                
                std::string attrValue = parseAttributeValue(tokens, pos);
                attributes[attrName] = attrValue;
            } else {
                // Boolean attribute
                attributes[attrName] = "true";
            }
        } else {
            advancePosition(pos);
        }
    }
    
    return attributes;
}

std::string CHTLParser::parseAttributeValue(const std::vector<Token>& tokens, size_t& pos) {
    std::string value;
    
    while (!isAtEnd(tokens, pos)) {
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::SEMICOLON || 
            currentToken.type == TokenType::LEFT_BRACE ||
            currentToken.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        if (currentToken.type == TokenType::STRING) {
            value += currentToken.value;
        } else if (currentToken.type == TokenType::TEXT) {
            value += currentToken.value;
        } else if (currentToken.type == TokenType::IDENTIFIER) {
            value += currentToken.value;
        } else if (currentToken.type == TokenType::NUMBER_LITERAL) {
            value += currentToken.value;
        } else {
            value += currentToken.value;
        }
        
        advancePosition(pos);
        skipWhitespace(tokens, pos);
    }
    
    return value;
}

std::map<std::string, std::string> CHTLParser::parseStyleProperties(const std::vector<Token>& tokens, size_t& pos) {
    std::map<std::string, std::string> properties;
    
    while (!isAtEnd(tokens, pos)) {
        skipWhitespace(tokens, pos);
        if (isAtEnd(tokens, pos)) break;
        
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string propName = std::string(currentToken.value);
            advancePosition(pos);
            skipWhitespace(tokens, pos);
            
            if (matchToken(tokens, pos, TokenType::COLON)) {
                advancePosition(pos);
                skipWhitespace(tokens, pos);
                
                std::string propValue = parseStyleValue(tokens, pos);
                properties[propName] = propValue;
            }
        } else {
            advancePosition(pos);
        }
    }
    
    return properties;
}

std::string CHTLParser::parseStyleValue(const std::vector<Token>& tokens, size_t& pos) {
    std::string value;
    
    while (!isAtEnd(tokens, pos)) {
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::SEMICOLON || 
            currentToken.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        if (currentToken.type == TokenType::STRING) {
            value += currentToken.value;
        } else if (currentToken.type == TokenType::TEXT) {
            value += currentToken.value;
        } else if (currentToken.type == TokenType::IDENTIFIER) {
            value += currentToken.value;
        } else if (currentToken.type == TokenType::NUMBER_LITERAL) {
            value += currentToken.value;
        } else {
            value += currentToken.value;
        }
        
        advancePosition(pos);
        skipWhitespace(tokens, pos);
    }
    
    return value;
}

std::string CHTLParser::parseScriptContent(const std::vector<Token>& tokens, size_t& pos) {
    std::string content;
    
    while (!isAtEnd(tokens, pos)) {
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        content += currentToken.value;
        advancePosition(pos);
    }
    
    return content;
}

std::map<std::string, std::string> CHTLParser::parseTemplateParameters(const std::vector<Token>& tokens, size_t& pos) {
    std::map<std::string, std::string> parameters;
    
    while (!isAtEnd(tokens, pos)) {
        skipWhitespace(tokens, pos);
        if (isAtEnd(tokens, pos)) break;
        
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string paramName = std::string(currentToken.value);
            advancePosition(pos);
            skipWhitespace(tokens, pos);
            
            if (matchToken(tokens, pos, TokenType::COLON) || matchToken(tokens, pos, TokenType::EQUAL)) {
                advancePosition(pos);
                skipWhitespace(tokens, pos);
                
                std::string paramValue = parseAttributeValue(tokens, pos);
                parameters[paramName] = paramValue;
            }
        } else {
            advancePosition(pos);
        }
    }
    
    return parameters;
}

std::string CHTLParser::parseImportPath(const std::vector<Token>& tokens, size_t& pos) {
    skipWhitespace(tokens, pos);
    
    if (matchToken(tokens, pos, "from")) {
        advancePosition(pos);
        skipWhitespace(tokens, pos);
        
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::STRING || currentToken.type == TokenType::TEXT) {
            std::string path = std::string(currentToken.value);
            advancePosition(pos);
            return path;
        }
    }
    
    return "";
}

std::string CHTLParser::parseImportAlias(const std::vector<Token>& tokens, size_t& pos) {
    skipWhitespace(tokens, pos);
    
    if (matchToken(tokens, pos, "as")) {
        advancePosition(pos);
        skipWhitespace(tokens, pos);
        
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string alias = std::string(currentToken.value);
            advancePosition(pos);
            return alias;
        }
    }
    
    return "";
}

std::string CHTLParser::parseNamespaceName(const std::vector<Token>& tokens, size_t& pos) {
    skipWhitespace(tokens, pos);
    
    auto currentToken = getCurrentToken(tokens, pos);
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string name = std::string(currentToken.value);
        advancePosition(pos);
        return name;
    }
    
    return "";
}

std::map<std::string, std::string> CHTLParser::parseConfigurationProperties(const std::vector<Token>& tokens, size_t& pos) {
    std::map<std::string, std::string> properties;
    
    while (!isAtEnd(tokens, pos)) {
        skipWhitespace(tokens, pos);
        if (isAtEnd(tokens, pos)) break;
        
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        if (currentToken.type == TokenType::IDENTIFIER) {
            std::string propName = std::string(currentToken.value);
            advancePosition(pos);
            skipWhitespace(tokens, pos);
            
            if (matchToken(tokens, pos, TokenType::EQUAL)) {
                advancePosition(pos);
                skipWhitespace(tokens, pos);
                
                std::string propValue = parseAttributeValue(tokens, pos);
                properties[propName] = propValue;
            }
        } else {
            advancePosition(pos);
        }
    }
    
    return properties;
}

std::string CHTLParser::parseOriginContent(const std::vector<Token>& tokens, size_t& pos) {
    std::string content;
    
    while (!isAtEnd(tokens, pos)) {
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        content += currentToken.value;
        advancePosition(pos);
    }
    
    return content;
}

std::string CHTLParser::parseOperatorType(const std::vector<Token>& tokens, size_t& pos) {
    auto currentToken = getCurrentToken(tokens, pos);
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string type = std::string(currentToken.value);
        advancePosition(pos);
        return type;
    }
    
    return "";
}

std::vector<std::string> CHTLParser::parseOperatorTargets(const std::vector<Token>& tokens, size_t& pos) {
    std::vector<std::string> targets;
    
    while (!isAtEnd(tokens, pos)) {
        skipWhitespace(tokens, pos);
        if (isAtEnd(tokens, pos)) break;
        
        auto currentToken = getCurrentToken(tokens, pos);
        if (currentToken.type == TokenType::IDENTIFIER) {
            targets.push_back(std::string(currentToken.value));
            advancePosition(pos);
        } else if (currentToken.type == TokenType::COMMA) {
            advancePosition(pos);
        } else {
            break;
        }
    }
    
    return targets;
}

bool CHTLParser::isAtEnd(const std::vector<Token>& tokens, size_t pos) const {
    return pos >= tokens.size();
}

Token CHTLParser::getCurrentToken(const std::vector<Token>& tokens, size_t pos) const {
    if (pos >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", SourceLocation(Position(0, 0), Position(0, 0)));
    }
    return tokens[pos];
}

Token CHTLParser::peekToken(const std::vector<Token>& tokens, size_t pos, size_t offset) const {
    size_t peekPos = pos + offset;
    if (peekPos >= tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", SourceLocation(Position(0, 0), Position(0, 0)));
    }
    return tokens[peekPos];
}

bool CHTLParser::matchToken(const std::vector<Token>& tokens, size_t& pos, TokenType type) const {
    if (pos >= tokens.size()) return false;
    if (tokens[pos].type == type) {
        advancePosition(pos);
        return true;
    }
    return false;
}

bool CHTLParser::matchToken(const std::vector<Token>& tokens, size_t& pos, TokenType type, const std::string& value) const {
    if (pos >= tokens.size()) return false;
    if (tokens[pos].type == type && tokens[pos].value == value) {
        advancePosition(pos);
        return true;
    }
    return false;
}

bool CHTLParser::matchToken(const std::vector<Token>& tokens, size_t& pos, const std::string& value) const {
    if (pos >= tokens.size()) return false;
    if (tokens[pos].value == value) {
        advancePosition(pos);
        return true;
    }
    return false;
}

void CHTLParser::advancePosition(size_t& pos) const {
    pos++;
}

void CHTLParser::skipWhitespace(const std::vector<Token>& tokens, size_t& pos) const {
    while (pos < tokens.size() && tokens[pos].type == TokenType::WHITESPACE) {
        pos++;
    }
}

void CHTLParser::handleError(const std::string& error, ParseResult& result) {
    result.success = false;
    result.errorMessage = error;
}

void CHTLParser::handleWarning(const std::string& warning, ParseResult& result) {
    result.warnings.push_back(warning);
}

bool CHTLParser::validateElement(const std::string& elementName) const {
    return validElements_.find(elementName) != validElements_.end();
}

bool CHTLParser::validateAttribute(const std::string& attributeName) const {
    return validAttributes_.find(attributeName) != validAttributes_.end();
}

bool CHTLParser::validateStyleProperty(const std::string& propertyName) const {
    return validStyleProperties_.find(propertyName) != validStyleProperties_.end();
}

bool CHTLParser::validateTemplateName(const std::string& templateName) const {
    return validTemplateNames_.find(templateName) != validTemplateNames_.end();
}

bool CHTLParser::validateCustomName(const std::string& customName) const {
    return validCustomNames_.find(customName) != validCustomNames_.end();
}

bool CHTLParser::validateImportPath(const std::string& path) const {
    return !path.empty();
}

bool CHTLParser::validateNamespaceName(const std::string& name) const {
    return !name.empty();
}

bool CHTLParser::validateConfigurationProperty(const std::string& property) const {
    return !property.empty();
}

bool CHTLParser::validateOriginType(const std::string& type) const {
    return validOriginTypes_.find(type) != validOriginTypes_.end();
}

bool CHTLParser::validateOperatorType(const std::string& type) const {
    return validOperatorTypes_.find(type) != validOperatorTypes_.end();
}

} // namespace CHTL