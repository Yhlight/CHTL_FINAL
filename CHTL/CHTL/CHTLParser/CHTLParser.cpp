#include "CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include <stdexcept>
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser(const TokenStream& tokens) 
    : tokens(tokens), current(0), inTemplate(false), inCustom(false), 
      inOrigin(false), inImport(false), inNamespace(false), inConfiguration(false) {
}

std::shared_ptr<BaseNode> CHTLParser::parse() {
    try {
        return parseElement();
    } catch (const std::exception& e) {
        throwError("Parse error: " + std::string(e.what()));
        return nullptr;
    }
}

std::vector<std::shared_ptr<BaseNode>> CHTLParser::parseAll() {
    std::vector<std::shared_ptr<BaseNode>> nodes;
    
    while (!isAtEnd()) {
        std::shared_ptr<BaseNode> node = parse();
        if (node) {
            nodes.push_back(node);
        }
    }
    
    return nodes;
}

Token CHTLParser::peek(size_t offset) const {
    return tokens.peek(offset);
}

Token CHTLParser::consume() {
    return tokens.consume();
}

bool CHTLParser::match(TokenType type) const {
    return tokens.match(type);
}

bool CHTLParser::match(const std::string& value) const {
    return tokens.match(value);
}

bool CHTLParser::match(TokenType type, const std::string& value) const {
    return tokens.match(type, value);
}

bool CHTLParser::check(TokenType type) const {
    return peek().type == type;
}

bool CHTLParser::check(const std::string& value) const {
    return peek().value == value;
}

void CHTLParser::throwError(const std::string& message) const {
    std::ostringstream oss;
    oss << "Parse Error at line " << peek().line 
        << ", column " << peek().column << ": " << message;
    
    // 添加上下文信息
    if (!nodeStack.empty()) {
        auto currentNode = nodeStack.back();
        oss << " (in " << currentNode->getName() << " context)";
    }
    
    // 添加建议
    if (message.find("Expected") != std::string::npos) {
        oss << "\nSuggestion: Check syntax and ensure proper nesting";
    }
    
    throw std::runtime_error(oss.str());
}

void CHTLParser::synchronize() {
    // 改进的同步机制，提供更好的错误恢复
    int braceLevel = 0;
    int parenLevel = 0;
    int bracketLevel = 0;
    
    while (!isAtEnd()) {
        Token current = peek();
        
        // 跟踪括号层级
        switch (current.type) {
            case TokenType::LEFT_BRACE:
                braceLevel++;
                break;
            case TokenType::RIGHT_BRACE:
                braceLevel--;
                if (braceLevel <= 0) {
                    return; // 找到匹配的右括号
                }
                break;
            case TokenType::LEFT_PAREN:
                parenLevel++;
                break;
            case TokenType::RIGHT_PAREN:
                parenLevel--;
                break;
            case TokenType::LEFT_BRACKET:
                bracketLevel++;
                break;
            case TokenType::RIGHT_BRACKET:
                bracketLevel--;
                break;
            case TokenType::SEMICOLON:
                if (braceLevel == 0 && parenLevel == 0 && bracketLevel == 0) {
                    consume();
                    return;
                }
                break;
            default:
                break;
        }
        
        consume();
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    if (isAtEnd()) {
        return nullptr;
    }
    
    Token currentToken = peek();
    
    // 检查是否为模板、自定义等特殊结构
    if (currentToken.type == TokenType::TEMPLATE) {
        return parseTemplate();
    } else if (currentToken.type == TokenType::CUSTOM) {
        return parseCustom();
    } else if (currentToken.type == TokenType::ORIGIN) {
        return parseOrigin();
    } else if (currentToken.type == TokenType::IMPORT) {
        return parseImport();
    } else if (currentToken.type == TokenType::NAMESPACE) {
        return parseNamespace();
    } else if (currentToken.type == TokenType::CONFIGURATION) {
        return parseConfiguration();
    } else if (currentToken.type == TokenType::TEXT) {
        return parseText();
    } else if (currentToken.type == TokenType::COMMENT) {
        return parseComment();
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        // 普通HTML元素
        return parseHTMLElement();
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseHTMLElement() {
    Token elementToken = consume();
    std::string elementName = elementToken.value;
    
    // 验证元素名
    if (!isValidElementName(elementName)) {
        throwError("Invalid element name: " + elementName);
    }
    
    auto element = std::make_shared<ElementNode>(elementName);
    element->setPosition(elementToken.line, elementToken.column, elementToken.position);
    
    // 解析属性
    parseAttributes(element);
    
    // 检查是否有子内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        pushNode(element); // 推入上下文栈
        
        // 解析子节点
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::shared_ptr<BaseNode> child = parseElement();
            if (child) {
                element->addChild(child);
                child->setParent(element);
            } else {
                // 尝试同步到下一个安全位置
                synchronize();
                break;
            }
        }
        
        popNode(); // 弹出上下文栈
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 跳过 }
        } else {
            throwError("Expected '}' to close element '" + elementName + "'");
        }
    }
    
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseText() {
    Token textToken = consume();
    
    auto textNode = std::make_shared<TextNode>();
    textNode->setPosition(textToken.line, textToken.column, textToken.position);
    
    // 检查是否有文本内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        std::string content;
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            Token token = consume();
            if (token.type == TokenType::STRING || token.type == TokenType::IDENTIFIER) {
                content += token.value;
            } else if (token.type == TokenType::WHITESPACE) {
                content += " ";
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 跳过 }
        }
        
        textNode->setContent(content);
    }
    
    return textNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    Token commentToken = consume();
    
    auto commentNode = std::make_shared<BaseNode>(NodeType::COMMENT, "comment");
    commentNode->setPosition(commentToken.line, commentToken.column, commentToken.position);
    commentNode->setAttribute("content", commentToken.value);
    
    return commentNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    Token templateToken = consume();
    
    auto templateNode = std::make_shared<BaseNode>(NodeType::TEMPLATE_STYLE, "template");
    templateNode->setPosition(templateToken.line, templateToken.column, templateToken.position);
    
    // 解析模板类型和名称
    if (match(TokenType::AT)) {
        consume(); // 跳过 @
        
        if (match(TokenType::IDENTIFIER)) {
            Token typeToken = consume();
            templateNode->setAttribute("type", typeToken.value);
            
            if (match(TokenType::IDENTIFIER)) {
                Token nameToken = consume();
                templateNode->setAttribute("name", nameToken.value);
            }
        }
    }
    
    // 解析模板内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::shared_ptr<BaseNode> child = parseElement();
            if (child) {
                templateNode->addChild(child);
            } else {
                break;
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 跳过 }
        }
    }
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    Token customToken = consume();
    
    auto customNode = std::make_shared<BaseNode>(NodeType::CUSTOM_STYLE, "custom");
    customNode->setPosition(customToken.line, customToken.column, customToken.position);
    
    // 解析自定义类型和名称
    if (match(TokenType::AT)) {
        consume(); // 跳过 @
        
        if (match(TokenType::IDENTIFIER)) {
            Token typeToken = consume();
            customNode->setAttribute("type", typeToken.value);
            
            if (match(TokenType::IDENTIFIER)) {
                Token nameToken = consume();
                customNode->setAttribute("name", nameToken.value);
            }
        }
    }
    
    // 解析自定义内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::shared_ptr<BaseNode> child = parseElement();
            if (child) {
                customNode->addChild(child);
            } else {
                break;
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 跳过 }
        }
    }
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    Token originToken = consume();
    
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN, "origin");
    originNode->setPosition(originToken.line, originToken.column, originToken.position);
    
    // 解析原始嵌入类型
    if (match(TokenType::AT)) {
        consume(); // 跳过 @
        
        if (match(TokenType::IDENTIFIER)) {
            Token typeToken = consume();
            originNode->setAttribute("type", typeToken.value);
        }
    }
    
    // 解析原始内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        std::string content;
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            Token token = consume();
            content += token.value;
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 跳过 }
        }
        
        originNode->setAttribute("content", content);
    }
    
    return originNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    Token importToken = consume();
    
    auto importNode = std::make_shared<BaseNode>(NodeType::IMPORT, "import");
    importNode->setPosition(importToken.line, importToken.column, importToken.position);
    
    // 解析导入类型和路径
    if (match(TokenType::AT)) {
        consume(); // 跳过 @
        
        if (match(TokenType::IDENTIFIER)) {
            Token typeToken = consume();
            importNode->setAttribute("type", typeToken.value);
        }
    }
    
    if (match(TokenType::FROM)) {
        consume(); // 跳过 from
        
        if (match(TokenType::STRING) || match(TokenType::IDENTIFIER)) {
            Token pathToken = consume();
            importNode->setAttribute("path", pathToken.value);
        }
    }
    
    return importNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    Token namespaceToken = consume();
    
    auto namespaceNode = std::make_shared<BaseNode>(NodeType::NAMESPACE, "namespace");
    namespaceNode->setPosition(namespaceToken.line, namespaceToken.column, namespaceToken.position);
    
    if (match(TokenType::IDENTIFIER)) {
        Token nameToken = consume();
        namespaceNode->setAttribute("name", nameToken.value);
    }
    
    return namespaceNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    Token configToken = consume();
    
    auto configNode = std::make_shared<BaseNode>(NodeType::CONFIGURATION, "configuration");
    configNode->setPosition(configToken.line, configToken.column, configToken.position);
    
    // 解析配置内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::shared_ptr<BaseNode> child = parseElement();
            if (child) {
                configNode->addChild(child);
            } else {
                break;
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 跳过 }
        }
    }
    
    return configNode;
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> node) {
    while (!isAtEnd() && !match(TokenType::LEFT_BRACE) && !match(TokenType::RIGHT_BRACE)) {
        if (match(TokenType::IDENTIFIER)) {
            auto attr = parseAttribute();
            if (!attr.first.empty()) {
                node->setAttribute(attr.first, attr.second);
            }
        } else {
            break;
        }
    }
}

std::pair<std::string, std::string> CHTLParser::parseAttribute() {
    std::string key, value;
    
    if (match(TokenType::IDENTIFIER)) {
        Token keyToken = consume();
        key = keyToken.value;
        
        if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
            consume(); // 跳过 : 或 =
            
            if (match(TokenType::STRING)) {
                Token valueToken = consume();
                value = valueToken.value;
            } else if (match(TokenType::IDENTIFIER)) {
                Token valueToken = consume();
                value = valueToken.value;
            }
        }
    }
    
    return {key, value};
}

bool CHTLParser::isAtEnd() const {
    return current >= tokens.size() || peek().type == TokenType::EOF_TOKEN;
}

size_t CHTLParser::getCurrentPosition() const {
    return current;
}

void CHTLParser::reset() {
    current = 0;
    inTemplate = false;
    inCustom = false;
    inOrigin = false;
    inImport = false;
    inNamespace = false;
    inConfiguration = false;
    nodeStack.clear();
}

void CHTLParser::setPosition(size_t position) {
    current = position;
}

void CHTLParser::pushNode(std::shared_ptr<BaseNode> node) {
    nodeStack.push_back(node);
}

void CHTLParser::popNode() {
    if (!nodeStack.empty()) {
        nodeStack.pop_back();
    }
}

std::shared_ptr<BaseNode> CHTLParser::getCurrentNode() const {
    if (nodeStack.empty()) {
        return nullptr;
    }
    return nodeStack.back();
}

bool CHTLParser::isValidElementName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool CHTLParser::isValidAttributeName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLParser::isValidTemplateName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

// 实现缺失的解析方法
std::shared_ptr<BaseNode> CHTLParser::parseStyle() {
    Token styleToken = consume();
    
    auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE, "style");
    styleNode->setPosition(styleToken.line, styleToken.column, styleToken.position);
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        std::string content;
        int braceLevel = 1;
        bool inString = false;
        char stringDelimiter = '\0';
        
        while (!isAtEnd() && braceLevel > 0) {
            Token token = consume();
            
            if (inString) {
                if (token.type == TokenType::STRING && token.value[0] == stringDelimiter) {
                    inString = false;
                    stringDelimiter = '\0';
                }
            } else {
                if (token.type == TokenType::STRING) {
                    inString = true;
                    stringDelimiter = token.value[0];
                } else if (token.type == TokenType::LEFT_BRACE) {
                    braceLevel++;
                } else if (token.type == TokenType::RIGHT_BRACE) {
                    braceLevel--;
                }
            }
            
            if (braceLevel > 0) {
                content += token.value;
            }
        }
        
        styleNode->setAttribute("content", content);
    }
    
    return styleNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseScript() {
    Token scriptToken = consume();
    
    auto scriptNode = std::make_shared<BaseNode>(NodeType::SCRIPT, "script");
    scriptNode->setPosition(scriptToken.line, scriptToken.column, scriptToken.position);
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        std::string content;
        int braceLevel = 1;
        bool inString = false;
        char stringDelimiter = '\0';
        
        while (!isAtEnd() && braceLevel > 0) {
            Token token = consume();
            
            if (inString) {
                if (token.type == TokenType::STRING && token.value[0] == stringDelimiter) {
                    inString = false;
                    stringDelimiter = '\0';
                }
            } else {
                if (token.type == TokenType::STRING) {
                    inString = true;
                    stringDelimiter = token.value[0];
                } else if (token.type == TokenType::LEFT_BRACE) {
                    braceLevel++;
                } else if (token.type == TokenType::RIGHT_BRACE) {
                    braceLevel--;
                }
            }
            
            if (braceLevel > 0) {
                content += token.value;
            }
        }
        
        scriptNode->setAttribute("content", content);
    }
    
    return scriptNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOperator() {
    Token opToken = peek();
    
    if (opToken.value == "delete") {
        return parseDeleteOperator();
    } else if (opToken.value == "insert") {
        return parseInsertOperator();
    } else if (opToken.value == "inherit") {
        return parseInheritOperator();
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseDeleteOperator() {
    Token deleteToken = consume();
    
    auto deleteNode = std::make_shared<BaseNode>(NodeType::OPERATOR, "delete");
    deleteNode->setPosition(deleteToken.line, deleteToken.column, deleteToken.position);
    
    // 解析要删除的目标
    while (!isAtEnd() && !match(TokenType::SEMICOLON) && !match(TokenType::RIGHT_BRACE)) {
        if (match(TokenType::IDENTIFIER)) {
            Token targetToken = consume();
            deleteNode->setAttribute("target", targetToken.value);
        } else if (match(TokenType::COMMA)) {
            consume(); // 跳过逗号
        } else {
            break;
        }
    }
    
    return deleteNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseInsertOperator() {
    Token insertToken = consume();
    
    auto insertNode = std::make_shared<BaseNode>(NodeType::OPERATOR, "insert");
    insertNode->setPosition(insertToken.line, insertToken.column, insertToken.position);
    
    // 解析插入位置
    if (match(TokenType::IDENTIFIER)) {
        Token positionToken = consume();
        insertNode->setAttribute("position", positionToken.value);
    }
    
    // 解析插入内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(); // 跳过 {
        
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            std::shared_ptr<BaseNode> child = parseElement();
            if (child) {
                insertNode->addChild(child);
            } else {
                break;
            }
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            consume(); // 跳过 }
        }
    }
    
    return insertNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseInheritOperator() {
    Token inheritToken = consume();
    
    auto inheritNode = std::make_shared<BaseNode>(NodeType::OPERATOR, "inherit");
    inheritNode->setPosition(inheritToken.line, inheritToken.column, inheritToken.position);
    
    // 解析继承的目标
    if (match(TokenType::IDENTIFIER)) {
        Token targetToken = consume();
        inheritNode->setAttribute("target", targetToken.value);
    }
    
    return inheritNode;
}

} // namespace CHTL