#include "CHTL/CHTLParser.h"
#include <iostream>
#include <fstream>

namespace CHTL {

CHTLParser::CHTLParser() 
    : m_current(0)
    , m_currentNamespace("")
{
}

std::shared_ptr<CHTLNode> CHTLParser::parse(const std::vector<Token>& tokens) {
    m_tokens = tokens;
    m_current = 0;
    m_errors.clear();
    
    try {
        return parseRoot();
    } catch (const std::exception& e) {
        addError("Parse error: " + std::string(e.what()));
        return nullptr;
    }
}

std::shared_ptr<CHTLNode> CHTLParser::parseFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filePath);
        return nullptr;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    CHTLLexer lexer;
    auto tokens = lexer.tokenize(content);
    return parse(tokens);
}

const std::vector<std::string>& CHTLParser::getErrors() const {
    return m_errors;
}

bool CHTLParser::hasErrors() const {
    return !m_errors.empty();
}

bool CHTLParser::isAtEnd() const {
    return m_current >= m_tokens.size() || current().type == TokenType::EOF_TOKEN;
}

Token CHTLParser::current() const {
    if (m_current >= m_tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return m_tokens[m_current];
}

Token CHTLParser::peek() const {
    if (m_current + 1 >= m_tokens.size()) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return m_tokens[m_current + 1];
}

Token CHTLParser::previous() const {
    if (m_current == 0) {
        return Token(TokenType::EOF_TOKEN, "", 0, 0, 0);
    }
    return m_tokens[m_current - 1];
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return current().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        m_current++;
    }
    return previous();
}

void CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    errorAtCurrent(message);
}

void CHTLParser::error(const std::string& message) {
    addError(message);
}

void CHTLParser::errorAtCurrent(const std::string& message) {
    errorAt(current(), message);
}

void CHTLParser::errorAt(const Token& token, const std::string& message) {
    std::string errorMsg = "Error at line " + std::to_string(token.line) + 
                          ", column " + std::to_string(token.column) + 
                          ": " + message;
    addError(errorMsg);
}

std::shared_ptr<CHTLNode> CHTLParser::parseRoot() {
    auto root = std::make_shared<CHTLNode>(NodeType::ROOT, "root");
    
    while (!isAtEnd()) {
        auto node = parseElement();
        if (node) {
            root->addChild(node);
        }
    }
    
    return root;
}

std::shared_ptr<CHTLNode> CHTLParser::parseElement() {
    if (isAtEnd()) return nullptr;
    
    Token token = current();
    
    // 检查是否是模板
    if (token.type == TokenType::TEMPLATE) {
        return parseTemplate();
    }
    
    // 检查是否是自定义
    if (token.type == TokenType::CUSTOM) {
        return parseCustom();
    }
    
    // 检查是否是导入
    if (token.type == TokenType::IMPORT) {
        return parseImport();
    }
    
    // 检查是否是原始嵌入
    if (token.type == TokenType::ORIGIN) {
        return parseOrigin();
    }
    
    // 检查是否是配置
    if (token.type == TokenType::CONFIGURATION) {
        return parseConfiguration();
    }
    
    // 检查是否是命名空间
    if (token.type == TokenType::NAMESPACE) {
        return parseNamespace();
    }
    
    // 检查是否是操作符
    if (token.type == TokenType::DELETE || token.type == TokenType::INSERT) {
        return parseOperator();
    }
    
    // 检查是否是use语句
    if (token.type == TokenType::USE) {
        return parseUse();
    }
    
    // 普通HTML元素
    if (token.type == TokenType::IDENTIFIER || token.type == TokenType::LITERAL) {
        return parseHTMLElement();
    }
    
    // 文本节点
    if (token.type == TokenType::TEXT) {
        return parseText();
    }
    
    // 样式节点
    if (token.type == TokenType::STYLE) {
        return parseStyle();
    }
    
    // 脚本节点
    if (token.type == TokenType::SCRIPT) {
        return parseScript();
    }
    
    advance(); // 跳过未知token
    return nullptr;
}

std::shared_ptr<CHTLNode> CHTLParser::parseHTMLElement() {
    Token token = advance();
    auto element = std::make_shared<ElementNode>(token.value);
    element->setPosition(token.line, token.column);
    
    // 解析属性
    parseAttributes(element);
    
    // 解析子内容
    if (match({TokenType::LBRACE})) {
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                element->addChild(child);
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after element body");
    }
    
    return element;
}

std::shared_ptr<CHTLNode> CHTLParser::parseText() {
    Token token = advance();
    auto textNode = std::make_shared<TextNode>("");
    textNode->setPosition(token.line, token.column);
    
    if (match({TokenType::LBRACE})) {
        // text { content }
        std::string content;
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            Token contentToken = advance();
            if (contentToken.type == TokenType::STRING || 
                contentToken.type == TokenType::LITERAL) {
                content += contentToken.value + " ";
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after text content");
        textNode->setTextContent(content);
    } else if (match({TokenType::COLON})) {
        // text: "content"
        Token valueToken = advance();
        if (valueToken.type == TokenType::STRING || 
            valueToken.type == TokenType::LITERAL) {
            textNode->setTextContent(valueToken.value);
        }
        consume(TokenType::SEMICOLON, "Expected ';' after text value");
    }
    
    return textNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyle() {
    Token token = advance();
    auto styleNode = std::make_shared<StyleNode>();
    styleNode->setPosition(token.line, token.column);
    
    if (match({TokenType::LBRACE})) {
        parseStyleProperties(styleNode);
        consume(TokenType::RBRACE, "Expected '}' after style block");
    }
    
    return styleNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseScript() {
    Token token = advance();
    auto scriptNode = std::make_shared<ScriptNode>();
    scriptNode->setPosition(token.line, token.column);
    
    if (match({TokenType::LBRACE})) {
        std::string script;
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            Token scriptToken = advance();
            script += scriptToken.value + " ";
        }
        consume(TokenType::RBRACE, "Expected '}' after script block");
        scriptNode->addScript(script);
    }
    
    return scriptNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplate() {
    Token token = advance();
    auto templateNode = std::make_shared<TemplateNode>(NodeType::TEMPLATE, "");
    templateNode->setPosition(token.line, token.column);
    
    // 解析模板类型和名称
    if (match({TokenType::AT})) {
        Token typeToken = advance();
        templateNode->setTemplateType(typeToken.value);
        
        Token nameToken = advance();
        templateNode->setName(nameToken.value);
    }
    
    if (match({TokenType::LBRACE})) {
        parseTemplateContent(templateNode);
        consume(TokenType::RBRACE, "Expected '}' after template body");
    }
    
    return templateNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseCustom() {
    Token token = advance();
    auto customNode = std::make_shared<CHTLNode>(NodeType::CUSTOM, "");
    customNode->setPosition(token.line, token.column);
    
    // 解析自定义类型和名称
    if (match({TokenType::AT})) {
        Token typeToken = advance();
        customNode->setName(typeToken.value);
        
        Token nameToken = advance();
        customNode->setName(nameToken.value);
    }
    
    if (match({TokenType::LBRACE})) {
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                customNode->addChild(child);
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after custom body");
    }
    
    return customNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseImport() {
    Token token = advance();
    auto importNode = std::make_shared<ImportNode>(NodeType::IMPORT, "");
    importNode->setPosition(token.line, token.column);
    
    // 解析导入类型
    if (match({TokenType::AT})) {
        Token typeToken = advance();
        importNode->setName(typeToken.value);
    }
    
    // 解析导入路径
    if (match({TokenType::FROM})) {
        Token pathToken = advance();
        importNode->setImportPath(pathToken.value);
    }
    
    // 解析别名
    if (match({TokenType::AS})) {
        Token aliasToken = advance();
        importNode->setAlias(aliasToken.value);
    }
    
    return importNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOrigin() {
    Token token = advance();
    auto originNode = std::make_shared<CHTLNode>(NodeType::ORIGIN, "");
    originNode->setPosition(token.line, token.column);
    
    // 解析原始嵌入类型
    if (match({TokenType::AT})) {
        Token typeToken = advance();
        originNode->setName(typeToken.value);
    }
    
    if (match({TokenType::LBRACE})) {
        std::string content;
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            Token contentToken = advance();
            content += contentToken.value + " ";
        }
        consume(TokenType::RBRACE, "Expected '}' after origin block");
        originNode->setTextContent(content);
    }
    
    return originNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfiguration() {
    Token token = advance();
    auto configNode = std::make_shared<CHTLNode>(NodeType::CONFIGURATION, "");
    configNode->setPosition(token.line, token.column);
    
    if (match({TokenType::LBRACE})) {
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                configNode->addChild(child);
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after configuration block");
    }
    
    return configNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNamespace() {
    Token token = advance();
    auto namespaceNode = std::make_shared<CHTLNode>(NodeType::NAMESPACE, "");
    namespaceNode->setPosition(token.line, token.column);
    
    Token nameToken = advance();
    namespaceNode->setName(nameToken.value);
    m_currentNamespace = nameToken.value;
    
    if (match({TokenType::LBRACE})) {
        while (!isAtEnd() && !check(TokenType::RBRACE)) {
            auto child = parseElement();
            if (child) {
                namespaceNode->addChild(child);
            }
        }
        consume(TokenType::RBRACE, "Expected '}' after namespace block");
    }
    
    return namespaceNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOperator() {
    Token token = advance();
    auto operatorNode = std::make_shared<CHTLNode>(NodeType::OPERATOR, token.value);
    operatorNode->setPosition(token.line, token.column);
    
    // 解析操作参数
    while (!isAtEnd() && !check(TokenType::SEMICOLON)) {
        Token argToken = advance();
        if (argToken.type == TokenType::IDENTIFIER || 
            argToken.type == TokenType::LITERAL) {
            operatorNode->setTextContent(operatorNode->getTextContent() + argToken.value + " ");
        }
    }
    
    if (check(TokenType::SEMICOLON)) {
        advance();
    }
    
    return operatorNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseUse() {
    Token token = advance();
    auto useNode = std::make_shared<CHTLNode>(NodeType::OPERATOR, "use");
    useNode->setPosition(token.line, token.column);
    
    // 解析use参数
    while (!isAtEnd() && !check(TokenType::SEMICOLON)) {
        Token argToken = advance();
        if (argToken.type == TokenType::IDENTIFIER || 
            argToken.type == TokenType::LITERAL) {
            useNode->setTextContent(useNode->getTextContent() + argToken.value + " ");
        }
    }
    
    if (check(TokenType::SEMICOLON)) {
        advance();
    }
    
    return useNode;
}

void CHTLParser::parseAttributes(std::shared_ptr<CHTLNode> node) {
    while (!isAtEnd() && !check(TokenType::LBRACE) && !check(TokenType::RBRACE)) {
        Token attrToken = advance();
        if (attrToken.type == TokenType::IDENTIFIER || attrToken.type == TokenType::LITERAL) {
            std::string attrName = attrToken.value;
            
            if (match({TokenType::COLON, TokenType::EQUAL})) {
                Token valueToken = advance();
                if (valueToken.type == TokenType::STRING || 
                    valueToken.type == TokenType::LITERAL ||
                    valueToken.type == TokenType::NUMBER) {
                    node->setAttribute(attrName, valueToken.value);
                }
                
                if (check(TokenType::SEMICOLON)) {
                    advance();
                }
            }
        } else {
            break;
        }
    }
}

void CHTLParser::parseStyleProperties(std::shared_ptr<StyleNode> styleNode) {
    while (!isAtEnd() && !check(TokenType::RBRACE)) {
        Token propToken = advance();
        if (propToken.type == TokenType::IDENTIFIER || propToken.type == TokenType::LITERAL) {
            std::string property = propToken.value;
            
            if (match({TokenType::COLON})) {
                std::string value;
                while (!isAtEnd() && !check(TokenType::SEMICOLON) && !check(TokenType::RBRACE)) {
                    Token valueToken = advance();
                    if (valueToken.type == TokenType::STRING || 
                        valueToken.type == TokenType::LITERAL ||
                        valueToken.type == TokenType::NUMBER) {
                        value += valueToken.value + " ";
                    }
                }
                
                if (!value.empty()) {
                    styleNode->addStyleProperty(property, value);
                }
                
                if (check(TokenType::SEMICOLON)) {
                    advance();
                }
            }
        } else {
            break;
        }
    }
}

void CHTLParser::parseTemplateContent(std::shared_ptr<TemplateNode> templateNode) {
    while (!isAtEnd() && !check(TokenType::RBRACE)) {
        auto child = parseElement();
        if (child) {
            templateNode->addChild(child);
        }
    }
}

void CHTLParser::addError(const std::string& error) {
    m_errors.push_back(error);
}

} // namespace CHTL