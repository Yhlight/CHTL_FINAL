#include "CHTL/CHTLParser.h"
#include <stdexcept>
#include <algorithm>
#include <iterator>

namespace CHTL {

CHTLParser::CHTLParser(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_current(0) {
}

std::shared_ptr<BaseNode> CHTLParser::parse(const std::vector<Token>& tokens) {
    m_tokens = tokens;
    m_current = 0;
    
    try {
        return parseProgram();
    } catch (const std::exception& e) {
        m_context->addError("Parse error: " + std::string(e.what()));
        return nullptr;
    }
}

void CHTLParser::reset() {
    m_current = 0;
}

const Token& CHTLParser::current() const {
    return m_tokens[m_current];
}

const Token& CHTLParser::peek(size_t offset) const {
    size_t index = m_current + offset;
    if (index >= m_tokens.size()) {
        static Token eofToken(TokenType::EOF_TOKEN, "", 0, 0);
        return eofToken;
    }
    return m_tokens[index];
}

bool CHTLParser::isAtEnd() const {
    return m_current >= m_tokens.size() || current().type == TokenType::EOF_TOKEN;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return current().type == type;
}

bool CHTLParser::check(const std::vector<TokenType>& types) const {
    if (isAtEnd()) return false;
    return std::find(types.begin(), types.end(), current().type) != types.end();
}

const Token& CHTLParser::advance() {
    if (!isAtEnd()) m_current++;
    return m_tokens[m_current - 1];
}

bool CHTLParser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    if (check(types)) {
        advance();
        return true;
    }
    return false;
}

const Token& CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    
    throw std::runtime_error(message + " at line " + std::to_string(current().line) + 
                           ", column " + std::to_string(current().column));
}

void CHTLParser::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (m_tokens[m_current - 1].type == TokenType::SEMICOLON) return;
        
        switch (current().type) {
            case TokenType::TEMPLATE:
            case TokenType::CUSTOM:
            case TokenType::ORIGIN:
            case TokenType::IMPORT:
            case TokenType::CONFIGURATION:
            case TokenType::NAMESPACE:
                return;
            default:
                break;
        }
        
        advance();
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseProgram() {
    auto program = std::make_shared<ElementNode>("program", 0, 0);
    
    while (!isAtEnd()) {
        try {
            auto statement = parseStatement();
            if (statement) {
                program->addChild(statement);
            }
        } catch (const std::exception& e) {
            error(current(), e.what());
            synchronize();
        }
    }
    
    return program;
}

std::shared_ptr<BaseNode> CHTLParser::parseStatement() {
    if (check(TokenType::TEMPLATE)) {
        return parseTemplate();
    } else if (check(TokenType::CUSTOM)) {
        return parseCustom();
    } else if (check(TokenType::ORIGIN)) {
        return parseOrigin();
    } else if (check(TokenType::IMPORT)) {
        return parseImport();
    } else if (check(TokenType::CONFIGURATION)) {
        return parseConfiguration();
    } else if (check(TokenType::NAMESPACE)) {
        return parseNamespace();
    } else if (check(TokenType::TEXT)) {
        return parseText();
    } else if (check(TokenType::LINE_COMMENT) || check(TokenType::BLOCK_COMMENT) || check(TokenType::GENERATOR_COMMENT)) {
        return parseComment();
    } else if (check(TokenType::IDENTIFIER) || check(TokenType::LITERAL)) {
        return parseElement();
    } else {
        error("Unexpected token: " + current().value);
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    const Token& nameToken = advance();
    std::string tagName = nameToken.value;
    
    auto element = std::make_shared<ElementNode>(tagName, nameToken.line, nameToken.column);
    
    // 解析属性
    parseAttributes(element);
    
    // 解析内容块
    if (match(TokenType::LEFT_BRACE)) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseStatement();
            if (child) {
                element->addChild(child);
            }
        }
        
        consume(TokenType::RIGHT_BRACE, "Expected '}' after element body");
    }
    
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseText() {
    const Token& textToken = advance();
    
    if (match(TokenType::LEFT_BRACE)) {
        // text { content }
        std::string content;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            const Token& token = advance();
            if (token.type == TokenType::STRING || token.type == TokenType::LITERAL) {
                content += token.value;
            } else if (token.type == TokenType::IDENTIFIER) {
                content += token.value;
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after text content");
        
        return std::make_shared<TextNode>(content, textToken.line, textToken.column);
    } else {
        // text: "content"
        if (match(TokenType::COLON)) {
            const Token& contentToken = advance();
            if (contentToken.type == TokenType::STRING || contentToken.type == TokenType::LITERAL) {
                return std::make_shared<TextNode>(contentToken.value, textToken.line, textToken.column);
            }
        }
        error("Expected text content after 'text'");
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    const Token& commentToken = advance();
    bool isGenerator = (commentToken.type == TokenType::GENERATOR_COMMENT);
    
    return std::make_shared<CommentNode>(commentToken.value, isGenerator, 
                                        commentToken.line, commentToken.column);
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    advance(); // 跳过 [Template]
    
    const Token& typeToken = advance();
    if (typeToken.type != TokenType::AT) {
        error("Expected '@' after [Template]");
        return nullptr;
    }
    
    const Token& nameToken = advance();
    if (nameToken.type != TokenType::IDENTIFIER && nameToken.type != TokenType::LITERAL) {
        error("Expected template name after '@'");
        return nullptr;
    }
    
    std::string templateType = typeToken.value + nameToken.value;
    std::string templateName = nameToken.value;
    
    auto templateNode = std::make_shared<TemplateNode>(
        getTemplateNodeType(templateType), templateName, 
        typeToken.line, typeToken.column);
    
    if (match(TokenType::LEFT_BRACE)) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseStatement();
            if (child) {
                templateNode->addChild(child);
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after template body");
    }
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    advance(); // 跳过 [Custom]
    
    const Token& typeToken = advance();
    if (typeToken.type != TokenType::AT) {
        error("Expected '@' after [Custom]");
        return nullptr;
    }
    
    const Token& nameToken = advance();
    if (nameToken.type != TokenType::IDENTIFIER && nameToken.type != TokenType::LITERAL) {
        error("Expected custom name after '@'");
        return nullptr;
    }
    
    std::string customType = typeToken.value + nameToken.value;
    std::string customName = nameToken.value;
    
    auto customNode = std::make_shared<CustomNode>(
        getCustomNodeType(customType), customName, 
        typeToken.line, typeToken.column);
    
    if (match(TokenType::LEFT_BRACE)) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseStatement();
            if (child) {
                customNode->addChild(child);
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after custom body");
    }
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    advance(); // 跳过 [Origin]
    
    const Token& typeToken = advance();
    if (typeToken.type != TokenType::AT) {
        error("Expected '@' after [Origin]");
        return nullptr;
    }
    
    const Token& nameToken = advance();
    std::string originType = typeToken.value + nameToken.value;
    std::string originName = nameToken.value;
    
    auto originNode = std::make_shared<OriginNode>(originType, originName, 
                                                  typeToken.line, typeToken.column);
    
    if (match(TokenType::LEFT_BRACE)) {
        std::string content;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            const Token& token = advance();
            content += token.value;
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after origin content");
        originNode->setAttribute("content", content);
    }
    
    return originNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    advance(); // 跳过 [Import]
    
    const Token& typeToken = advance();
    if (typeToken.type != TokenType::AT) {
        error("Expected '@' after [Import]");
        return nullptr;
    }
    
    const Token& nameToken = advance();
    std::string importType = typeToken.value + nameToken.value;
    
    if (!match(TokenType::FROM)) {
        error("Expected 'from' after import type");
        return nullptr;
    }
    
    const Token& pathToken = advance();
    std::string path = pathToken.value;
    
    std::string alias;
    if (match(TokenType::AS)) {
        const Token& aliasToken = advance();
        alias = aliasToken.value;
    }
    
    return std::make_shared<ImportNode>(importType, path, alias, 
                                       typeToken.line, typeToken.column);
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    advance(); // 跳过 [Configuration]
    
    const Token& nameToken = advance();
    std::string configName = nameToken.value;
    
    auto configNode = std::make_shared<ConfigNode>(configName, 
                                                  nameToken.line, nameToken.column);
    
    if (match(TokenType::LEFT_BRACE)) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            const Token& keyToken = advance();
            if (keyToken.type == TokenType::IDENTIFIER || keyToken.type == TokenType::LITERAL) {
                if (match(TokenType::EQUAL)) {
                    const Token& valueToken = advance();
                    configNode->setAttribute(keyToken.value, valueToken.value);
                }
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after configuration body");
    }
    
    return configNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    advance(); // 跳过 [Namespace]
    
    const Token& nameToken = advance();
    std::string namespaceName = nameToken.value;
    
    auto namespaceNode = std::make_shared<NamespaceNode>(namespaceName, 
                                                        nameToken.line, nameToken.column);
    
    if (match(TokenType::LEFT_BRACE)) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto child = parseStatement();
            if (child) {
                namespaceNode->addChild(child);
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' after namespace body");
    }
    
    return namespaceNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOperator() {
    const Token& opToken = advance();
    
    switch (opToken.type) {
        case TokenType::DELETE:
            return parseDeleteOperator();
        case TokenType::INSERT:
            return parseInsertOperator();
        case TokenType::USE:
            return parseUseOperator();
        case TokenType::INHERIT:
            return parseInheritOperator();
        default:
            error("Unknown operator: " + opToken.value);
            return nullptr;
    }
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> node) {
    while (check(TokenType::IDENTIFIER) || check(TokenType::LITERAL)) {
        const Token& keyToken = advance();
        std::string key = keyToken.value;
        
        if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
            const Token& valueToken = advance();
            std::string value = valueToken.value;
            node->setAttribute(key, value);
        }
    }
}

void CHTLParser::parseStyleBlock(std::shared_ptr<BaseNode> node) {
    if (match(TokenType::STYLE)) {
        auto styleNode = std::make_shared<StyleNode>(current().line, current().column);
        
        if (match(TokenType::LEFT_BRACE)) {
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                const Token& keyToken = advance();
                if (keyToken.type == TokenType::IDENTIFIER || keyToken.type == TokenType::LITERAL) {
                    if (match(TokenType::COLON)) {
                        const Token& valueToken = advance();
                        styleNode->setAttribute(keyToken.value, valueToken.value);
                    }
                }
            }
            consume(TokenType::RIGHT_BRACE, "Expected '}' after style block");
        }
        
        node->addChild(styleNode);
    }
}

void CHTLParser::parseScriptBlock(std::shared_ptr<BaseNode> node) {
    if (match(TokenType::SCRIPT)) {
        auto scriptNode = std::make_shared<ScriptNode>(current().line, current().column);
        
        if (match(TokenType::LEFT_BRACE)) {
            std::string content;
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                const Token& token = advance();
                content += token.value;
            }
            consume(TokenType::RIGHT_BRACE, "Expected '}' after script block");
            scriptNode->setAttribute("content", content);
        }
        
        node->addChild(scriptNode);
    }
}

NodeType CHTLParser::getTemplateNodeType(const std::string& type) const {
    if (type == "@Style") return NodeType::TEMPLATE_STYLE;
    if (type == "@Element") return NodeType::TEMPLATE_ELEMENT;
    if (type == "@Var") return NodeType::TEMPLATE_VAR;
    return NodeType::TEMPLATE_STYLE; // 默认
}

NodeType CHTLParser::getCustomNodeType(const std::string& type) const {
    if (type == "@Style") return NodeType::CUSTOM_STYLE;
    if (type == "@Element") return NodeType::CUSTOM_ELEMENT;
    if (type == "@Var") return NodeType::CUSTOM_VAR;
    return NodeType::CUSTOM_STYLE; // 默认
}

std::shared_ptr<OperatorNode> CHTLParser::parseDeleteOperator() {
    auto opNode = std::make_shared<OperatorNode>(OperatorNode::OperatorType::DELETE, 
                                                current().line, current().column);
    return opNode;
}

std::shared_ptr<OperatorNode> CHTLParser::parseInsertOperator() {
    auto opNode = std::make_shared<OperatorNode>(OperatorNode::OperatorType::INSERT, 
                                                current().line, current().column);
    return opNode;
}

std::shared_ptr<OperatorNode> CHTLParser::parseUseOperator() {
    auto opNode = std::make_shared<OperatorNode>(OperatorNode::OperatorType::USE, 
                                                current().line, current().column);
    return opNode;
}

std::shared_ptr<OperatorNode> CHTLParser::parseInheritOperator() {
    auto opNode = std::make_shared<OperatorNode>(OperatorNode::OperatorType::INHERIT, 
                                                current().line, current().column);
    return opNode;
}

void CHTLParser::error(const Token& token, const std::string& message) {
    m_context->addError(message, token.line, token.column);
}

void CHTLParser::error(const std::string& message) {
    m_context->addError(message, current().line, current().column);
}

} // namespace CHTL