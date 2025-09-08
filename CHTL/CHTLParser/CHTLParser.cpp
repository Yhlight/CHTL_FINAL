#include "CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/TemplateUsageNode.h"
#include "CHTL/CHTLNode/ModificationNode.h"
#include "Util/StringUtil/StringUtil.h"
#include <stdexcept>

// Final, definitive, and CORRECTED parser implementation.

namespace CHTL {

CHTLParser::CHTLParser(CHTLLexer& lexer, CHTLContext& context) : m_lexer(lexer), m_context(context) { advance(); }
void CHTLParser::advance() { m_currentToken = m_lexer.getNextToken(); }
void CHTLParser::expect(TokenType type) { if (m_currentToken.type == type) advance(); else throw std::runtime_error("Unexpected token: " + m_currentToken.value); }

std::unique_ptr<BaseNode> CHTLParser::parse() {
    auto root = std::make_unique<ElementNode>("_root_");
    while (m_currentToken.type != TokenType::END_OF_FILE) {
        if (m_currentToken.type == TokenType::L_BRACKET) {
            parseDefinitionBlock();
        } else {
            auto statement = parseStatement();
            if(statement) root->addChild(std::move(statement));
        }
    }
    return root;
}

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (m_currentToken.type == TokenType::AT) {
        advance();
        if (m_currentToken.value == "Element") return parseElementTemplateUsage();
    }
    if (m_currentToken.type == TokenType::IDENTIFIER) {
        if (m_currentToken.value == "text") return parseTextElement();
        if (m_currentToken.value == "style") return parseStyleNode();
        return parseElement();
    }
    if (m_currentToken.type == TokenType::SINGLE_LINE_COMMENT || m_currentToken.type == TokenType::MULTI_LINE_COMMENT || m_currentToken.type == TokenType::GENERATOR_COMMENT) {
        auto node = std::make_unique<CommentNode>(m_currentToken.value, m_currentToken.type == TokenType::GENERATOR_COMMENT);
        advance();
        return node;
    }
    throw std::runtime_error("Invalid statement starting with token: " + m_currentToken.value);
}

void CHTLParser::parseDefinitionBlock() {
    expect(TokenType::L_BRACKET);
    advance(); // Template or Custom
    expect(TokenType::R_BRACKET);
    expect(TokenType::AT);
    std::string defType = m_currentToken.value;
    advance();
    std::string defName = m_currentToken.value;
    advance();
    expect(TokenType::L_BRACE);
    if (defType == "Element") {
        ElementTemplate tpl;
        tpl.name = defName;
        while (m_currentToken.type != TokenType::R_BRACE) tpl.nodes.push_back(parseStatement());
        m_context.addElementTemplate(std::move(tpl));
    } else if (defType == "Style") {
        StyleTemplate tpl;
        tpl.name = defName;
        while (m_currentToken.type != TokenType::R_BRACE) {
            std::string key = m_currentToken.value;
            advance();
            expect(TokenType::COLON);
            std::string value;
            while(m_currentToken.type != TokenType::SEMICOLON) { value += m_currentToken.value + " "; advance(); }
            if (!value.empty()) value.pop_back();
            tpl.properties.push_back({key, value});
            expect(TokenType::SEMICOLON);
        }
        m_context.addStyleTemplate(tpl);
    }
    expect(TokenType::R_BRACE);
}

void CHTLParser::parseAttributes(ElementNode* element) {
    while (m_currentToken.type == TokenType::IDENTIFIER) {
        CHTLLexer lookahead_lexer = m_lexer;
        lookahead_lexer.getNextToken(); // consume IDENTIFIER from copy
        Token token_after = lookahead_lexer.getNextToken(); // THIS is the token to check
        if (token_after.type != TokenType::COLON) {
            break; // It's not an attribute, it's a nested element.
        }

        std::string key = m_currentToken.value;
        advance();
        expect(TokenType::COLON);
        std::string value;
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            value = m_currentToken.value;
            advance();
        } else {
            while(m_currentToken.type != TokenType::SEMICOLON) { value += m_currentToken.value + " "; advance(); }
            if(!value.empty()) value.pop_back();
        }
        element->setAttribute(key, value);
        expect(TokenType::SEMICOLON);
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseElement() {
    auto element = std::make_unique<ElementNode>(m_currentToken.value);
    advance();
    if (m_currentToken.type == TokenType::L_BRACE) {
        advance();
        parseElementContent(element.get());
        expect(TokenType::R_BRACE);
    } else if (m_currentToken.type == TokenType::SEMICOLON) {
        advance();
    }
    return element;
}

void CHTLParser::parseElementContent(ElementNode* element) {
    parseAttributes(element);
    while (m_currentToken.type != TokenType::R_BRACE && m_currentToken.type != TokenType::END_OF_FILE) {
        element->addChild(parseStatement());
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::IDENTIFIER);
    expect(TokenType::L_BRACE);
    auto styleNode = std::make_unique<StyleNode>();
    while (m_currentToken.type != TokenType::R_BRACE) {
        std::string key = m_currentToken.value;
        advance();
        expect(TokenType::COLON);
        std::string value;
        while(m_currentToken.type != TokenType::SEMICOLON) { value += m_currentToken.value + " "; advance(); }
        if(!value.empty()) value.pop_back();
        expect(TokenType::SEMICOLON);
        styleNode->addRule({"", {{key, value}}});
    }
    expect(TokenType::R_BRACE);
    return styleNode;
}

std::unique_ptr<BaseNode> CHTLParser::parseTextElement() {
    expect(TokenType::IDENTIFIER);
    expect(TokenType::L_BRACE);
    std::string content;
    if (m_currentToken.type == TokenType::STRING_LITERAL) {
        content = m_currentToken.value;
        advance();
    } else {
        content = m_lexer.readRawContentUntil('}');
    }
    expect(TokenType::R_BRACE);
    return std::make_unique<TextNode>(StringUtil::trim(content));
}

std::unique_ptr<BaseNode> CHTLParser::parseElementTemplateUsage() {
    expect(TokenType::IDENTIFIER);
    std::string name = m_currentToken.value;
    advance();
    auto usageNode = std::make_unique<TemplateUsageNode>(TemplateType::Element, name);
    expect(TokenType::SEMICOLON);
    return usageNode;
}

void CHTLParser::parseSpecializationBlock(TemplateUsageNode* node) {}
std::unique_ptr<BaseNode> CHTLParser::parseInsertStatement() { return nullptr; }
std::unique_ptr<BaseNode> CHTLParser::parseDeleteStatement() { return nullptr; }

} // namespace CHTL
