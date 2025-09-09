#include "CHTL/CHTLParser.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens)
    : tokens_(tokens), currentToken_(0) {
}

std::shared_ptr<ASTNode> CHTLParser::parse() {
    try {
        return parseRoot();
    } catch (const std::exception& e) {
        reportError(e.what());
        return nullptr;
    }
}

std::shared_ptr<ASTNode> CHTLParser::parseRoot() {
    // 创建一个简单的根节点，不直接实例化抽象类
    auto root = std::make_shared<ElementNode>("root");
    
    while (!isAtEnd()) {
        auto node = parseElement();
        if (node) {
            root->addChild(node);
        }
    }
    
    return root;
}

std::shared_ptr<ASTNode> CHTLParser::parseElement() {
    Token token = current();
    
    // 处理特殊语法结构
    if (token.isType(TokenType::LBRACKET)) {
        return parseSpecialStructure();
    }
    
    // 处理文本节点
    if (token.isType(TokenType::TEXT)) {
        return parseText();
    }
    
    // 处理HTML元素
    if (isElementToken(token)) {
        return parseHTMLElement();
    }
    
    // 跳过未知token
    advance();
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseSpecialStructure() {
    Token token = current();
    
    if (token.isType(TokenType::LBRACKET)) {
        advance(); // 跳过 [
        
        token = current();
        if (token.isType(TokenType::TEMPLATE)) {
            return parseTemplate();
        } else if (token.isType(TokenType::CUSTOM)) {
            return parseCustom();
        } else if (token.isType(TokenType::ORIGIN)) {
            return parseOrigin();
        } else if (token.isType(TokenType::IMPORT)) {
            return parseImport();
        } else if (token.isType(TokenType::NAMESPACE)) {
            return parseNamespace();
        } else if (token.isType(TokenType::CONFIGURATION)) {
            return parseConfiguration();
        }
    }
    
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseHTMLElement() {
    Token token = current();
    std::string tagName = token.getValue();
    advance();
    
    auto element = std::make_shared<ElementNode>(tagName);
    element->setPosition(token.getLine(), token.getColumn());
    
    // 解析属性和子元素
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            Token nextToken = current();
            
            // 如果是属性（key: value格式）
            if (nextToken.isType(TokenType::IDENTIFIER) && 
                peek().isType(TokenType::COLON)) {
                parseAttributes(element);
            } else {
                // 否则是子元素
                auto child = parseElement();
                if (child) {
                    element->addChild(child);
                }
            }
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
    }
    
    return element;
}

std::shared_ptr<ASTNode> CHTLParser::parseText() {
    Token token = current();
    advance(); // 跳过 text
    
    auto textNode = std::make_shared<TextNode>("");
    textNode->setPosition(token.getLine(), token.getColumn());
    
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        std::string content;
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            Token contentToken = current();
            if (contentToken.isType(TokenType::STRING) || 
                contentToken.isType(TokenType::LITERAL) ||
                contentToken.isType(TokenType::IDENTIFIER)) {
                if (!content.empty()) {
                    content += " ";
                }
                content += contentToken.getValue();
            }
            advance();
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
        
        textNode->setText(content);
    }
    
    return textNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseStyle() {
    Token token = current();
    advance(); // 跳过 style
    
    auto styleNode = std::make_shared<StyleNode>();
    styleNode->setPosition(token.getLine(), token.getColumn());
    
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        parseStyleProperties(styleNode);
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
    }
    
    return styleNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseScript() {
    Token token = current();
    advance(); // 跳过 script
    
    auto scriptNode = std::make_shared<ScriptNode>();
    scriptNode->setPosition(token.getLine(), token.getColumn());
    
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        std::string script;
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            Token scriptToken = current();
            script += scriptToken.getValue();
            advance();
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
        
        scriptNode->setScript(script);
    }
    
    return scriptNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseTemplate() {
    Token token = current();
    advance(); // 跳过 Template
    
    if (!check(TokenType::RBRACKET)) {
        reportError("期望 ]");
        return nullptr;
    }
    advance(); // 跳过 ]
    
    // 解析模板类型和名称
    Token typeToken = current();
    if (!typeToken.isType(TokenType::IDENTIFIER)) {
        reportError("期望模板类型");
        return nullptr;
    }
    advance();
    
    Token nameToken = current();
    if (!nameToken.isType(TokenType::IDENTIFIER)) {
        reportError("期望模板名称");
        return nullptr;
    }
    advance();
    
    auto templateNode = std::make_shared<TemplateNode>(
        typeToken.getValue(), nameToken.getValue()
    );
    templateNode->setPosition(token.getLine(), token.getColumn());
    
    // 解析模板内容
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            auto child = parseElement();
            if (child) {
                templateNode->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
    }
    
    return templateNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseCustom() {
    Token token = current();
    advance(); // 跳过 Custom
    
    if (!check(TokenType::RBRACKET)) {
        reportError("期望 ]");
        return nullptr;
    }
    advance(); // 跳过 ]
    
    // 解析自定义类型和名称
    Token typeToken = current();
    if (!typeToken.isType(TokenType::IDENTIFIER)) {
        reportError("期望自定义类型");
        return nullptr;
    }
    advance();
    
    Token nameToken = current();
    if (!nameToken.isType(TokenType::IDENTIFIER)) {
        reportError("期望自定义名称");
        return nullptr;
    }
    advance();
    
    auto customNode = std::make_shared<CustomNode>(
        typeToken.getValue(), nameToken.getValue()
    );
    customNode->setPosition(token.getLine(), token.getColumn());
    
    // 解析自定义内容
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            auto child = parseElement();
            if (child) {
                customNode->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
    }
    
    return customNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseOrigin() {
    Token token = current();
    advance(); // 跳过 Origin
    
    if (!check(TokenType::RBRACKET)) {
        reportError("期望 ]");
        return nullptr;
    }
    advance(); // 跳过 ]
    
    // 解析原始嵌入类型和名称
    Token typeToken = current();
    if (!typeToken.isType(TokenType::IDENTIFIER)) {
        reportError("期望原始嵌入类型");
        return nullptr;
    }
    advance();
    
    std::string name;
    if (check(TokenType::IDENTIFIER)) {
        Token nameToken = current();
        name = nameToken.getValue();
        advance();
    }
    
    auto originNode = std::make_shared<OriginNode>(
        typeToken.getValue(), name
    );
    originNode->setPosition(token.getLine(), token.getColumn());
    
    // 解析原始嵌入内容
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        std::string content;
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            Token contentToken = current();
            content += contentToken.getValue();
            advance();
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
        
        originNode->setText(content);
    }
    
    return originNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseImport() {
    Token token = current();
    advance(); // 跳过 Import
    
    if (!check(TokenType::RBRACKET)) {
        reportError("期望 ]");
        return nullptr;
    }
    advance(); // 跳过 ]
    
    // 解析导入类型和路径
    Token typeToken = current();
    if (!typeToken.isType(TokenType::IDENTIFIER)) {
        reportError("期望导入类型");
        return nullptr;
    }
    advance();
    
    if (!check(TokenType::FROM)) {
        reportError("期望 from");
        return nullptr;
    }
    advance(); // 跳过 from
    
    Token pathToken = current();
    if (!pathToken.isType(TokenType::STRING) && !pathToken.isType(TokenType::LITERAL)) {
        reportError("期望导入路径");
        return nullptr;
    }
    advance();
    
    auto importNode = std::make_shared<ImportNode>(
        typeToken.getValue(), pathToken.getValue()
    );
    importNode->setPosition(token.getLine(), token.getColumn());
    
    // 解析别名
    if (check(TokenType::AS)) {
        advance(); // 跳过 as
        
        Token aliasToken = current();
        if (aliasToken.isType(TokenType::IDENTIFIER)) {
            importNode->setImportAlias(aliasToken.getValue());
            advance();
        }
    }
    
    return importNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseNamespace() {
    Token token = current();
    advance(); // 跳过 Namespace
    
    if (!check(TokenType::RBRACKET)) {
        reportError("期望 ]");
        return nullptr;
    }
    advance(); // 跳过 ]
    
    Token nameToken = current();
    if (!nameToken.isType(TokenType::IDENTIFIER)) {
        reportError("期望命名空间名称");
        return nullptr;
    }
    advance();
    
    auto namespaceNode = std::make_shared<NamespaceNode>(nameToken.getValue());
    namespaceNode->setPosition(token.getLine(), token.getColumn());
    
    // 解析命名空间内容
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            auto child = parseElement();
            if (child) {
                namespaceNode->addChild(child);
            }
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
    }
    
    return namespaceNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseConfiguration() {
    Token token = current();
    advance(); // 跳过 Configuration
    
    if (!check(TokenType::RBRACKET)) {
        reportError("期望 ]");
        return nullptr;
    }
    advance(); // 跳过 ]
    
    std::string configName;
    if (check(TokenType::IDENTIFIER)) {
        Token nameToken = current();
        configName = nameToken.getValue();
        advance();
    }
    
    auto configNode = std::make_shared<ConfigurationNode>(configName);
    configNode->setPosition(token.getLine(), token.getColumn());
    
    // 解析配置内容
    if (check(TokenType::LBRACE)) {
        advance(); // 跳过 {
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            Token keyToken = current();
            if (keyToken.isType(TokenType::IDENTIFIER)) {
                advance();
                
                if (check(TokenType::EQUALS)) {
                    advance(); // 跳过 =
                    
                    Token valueToken = current();
                    if (valueToken.isType(TokenType::STRING) || 
                        valueToken.isType(TokenType::LITERAL) ||
                        valueToken.isType(TokenType::NUMBER)) {
                        configNode->addConfigItem(keyToken.getValue(), valueToken.getValue());
                        advance();
                    }
                }
            } else {
                advance();
            }
        }
        
        if (check(TokenType::RBRACE)) {
            advance(); // 跳过 }
        }
    }
    
    return configNode;
}

std::shared_ptr<ASTNode> CHTLParser::parseConstraint() {
    // 约束解析实现
    return nullptr;
}

std::shared_ptr<ASTNode> CHTLParser::parseUse() {
    Token token = current();
    advance(); // 跳过 use
    
    Token targetToken = current();
    if (!targetToken.isType(TokenType::IDENTIFIER) && 
        !targetToken.isType(TokenType::LITERAL)) {
        reportError("期望使用目标");
        return nullptr;
    }
    advance();
    
    auto useNode = std::make_shared<UseNode>(targetToken.getValue());
    useNode->setPosition(token.getLine(), token.getColumn());
    
    return useNode;
}

void CHTLParser::parseAttributes(std::shared_ptr<ASTNode> node) {
    Token keyToken = current();
    if (!keyToken.isType(TokenType::IDENTIFIER)) {
        advance();
        return;
    }
    advance();
    
    if (check(TokenType::COLON) || check(TokenType::EQUALS)) {
        advance(); // 跳过 : 或 =
        
        Token valueToken = current();
        if (valueToken.isType(TokenType::STRING) || 
            valueToken.isType(TokenType::LITERAL) ||
            valueToken.isType(TokenType::NUMBER)) {
            node->setAttribute(keyToken.getValue(), valueToken.getValue());
            advance();
        }
    }
}

void CHTLParser::parseStyleProperties(std::shared_ptr<StyleNode> styleNode) {
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        Token keyToken = current();
        if (keyToken.isType(TokenType::IDENTIFIER)) {
            advance();
            
            if (check(TokenType::COLON) || check(TokenType::EQUALS)) {
                advance(); // 跳过 : 或 =
                
                Token valueToken = current();
                if (valueToken.isType(TokenType::STRING) || 
                    valueToken.isType(TokenType::LITERAL) ||
                    valueToken.isType(TokenType::NUMBER)) {
                    styleNode->addProperty(keyToken.getValue(), valueToken.getValue());
                    advance();
                }
            }
        } else {
            advance();
        }
    }
}

Token& CHTLParser::current() {
    return tokens_[currentToken_];
}

Token& CHTLParser::peek() {
    if (currentToken_ + 1 < tokens_.size()) {
        return tokens_[currentToken_ + 1];
    }
    return tokens_.back();
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (match(type)) {
            return true;
        }
    }
    return false;
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return current().isType(type);
}

bool CHTLParser::check(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            return true;
        }
    }
    return false;
}

Token& CHTLParser::advance() {
    if (!isAtEnd()) currentToken_++;
    return current();
}

bool CHTLParser::isAtEnd() {
    return currentToken_ >= tokens_.size() || 
           current().isType(TokenType::EOF_TOKEN);
}

void CHTLParser::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

void CHTLParser::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message, current().getLine(), current().getColumn());
    } else {
        throw std::runtime_error("语法分析错误 [" + std::to_string(current().getLine()) + 
                                ":" + std::to_string(current().getColumn()) + "]: " + message);
    }
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (current().isType(TokenType::SEMICOLON)) {
            advance();
            break;
        }
        
        if (current().isType(TokenType::RBRACE)) {
            break;
        }
        
        advance();
    }
}

bool CHTLParser::isElementToken(const Token& token) {
    return token.isType(TokenType::HTML) || token.isType(TokenType::HEAD) ||
           token.isType(TokenType::BODY) || token.isType(TokenType::DIV) ||
           token.isType(TokenType::SPAN) || token.isType(TokenType::P) ||
           token.isType(TokenType::A) || token.isType(TokenType::IMG) ||
           token.isType(TokenType::BR) || token.isType(TokenType::HR) ||
           token.isType(TokenType::H1) || token.isType(TokenType::H2) ||
           token.isType(TokenType::H3) || token.isType(TokenType::H4) ||
           token.isType(TokenType::H5) || token.isType(TokenType::H6) ||
           token.isType(TokenType::UL) || token.isType(TokenType::OL) ||
           token.isType(TokenType::LI) || token.isType(TokenType::TABLE) ||
           token.isType(TokenType::TR) || token.isType(TokenType::TD) ||
           token.isType(TokenType::TH) || token.isType(TokenType::FORM) ||
           token.isType(TokenType::INPUT) || token.isType(TokenType::BUTTON) ||
           token.isType(TokenType::TEXT) || token.isType(TokenType::STYLE) ||
           token.isType(TokenType::SCRIPT) || token.isType(TokenType::IDENTIFIER);
}

bool CHTLParser::isStylePropertyToken(const Token& token) {
    return token.isType(TokenType::WIDTH) || token.isType(TokenType::HEIGHT) ||
           token.isType(TokenType::COLOR) || token.isType(TokenType::BACKGROUND) ||
           token.isType(TokenType::MARGIN) || token.isType(TokenType::PADDING) ||
           token.isType(TokenType::BORDER) || token.isType(TokenType::FONT) ||
           token.isType(TokenType::TEXT_ALIGN) || token.isType(TokenType::DISPLAY) ||
           token.isType(TokenType::POSITION) || token.isType(TokenType::FLOAT) ||
           token.isType(TokenType::CLEAR) || token.isType(TokenType::OVERFLOW) ||
           token.isType(TokenType::Z_INDEX) || token.isType(TokenType::OPACITY) ||
           token.isType(TokenType::TRANSFORM) || token.isType(TokenType::TRANSITION) ||
           token.isType(TokenType::ANIMATION) || token.isType(TokenType::IDENTIFIER);
}

std::string CHTLParser::tokenTypeToString(TokenType type) {
    return std::to_string(static_cast<int>(type));
}

} // namespace CHTL