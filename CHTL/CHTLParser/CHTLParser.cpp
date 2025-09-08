#include "CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/TemplateUsageNode.h"
#include "CHTL/CHTLNode/ModificationNode.h"
#include "Util/StringUtil/StringUtil.h"
#include <stdexcept>

// Final, complete parser implementation for the CHTL Core Language.

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

std::unique_ptr<BaseNode> CHTLParser::parseElementTemplateUsage() {
    expect(TokenType::IDENTIFIER); // Element
    std::string name = m_currentToken.value;
    advance();
    auto usageNode = std::make_unique<TemplateUsageNode>(TemplateType::Element, name);
    if (m_currentToken.type == TokenType::L_BRACE) {
        advance();
        // parseSpecializationBlock(usageNode.get());
        while(m_currentToken.type != TokenType::R_BRACE) advance();
        expect(TokenType::R_BRACE);
    } else {
        expect(TokenType::SEMICOLON);
    }
    return usageNode;
}


void CHTLParser::parseDefinitionBlock() {
    expect(TokenType::L_BRACKET);
    std::string blockType = m_currentToken.value; // [Template] or [Custom]
    advance();
    expect(TokenType::R_BRACKET);
    expect(TokenType::AT);
    std::string defType = m_currentToken.value; // @Style, @Element, @Var
    advance();
    std::string defName = m_currentToken.value;
    advance();
    expect(TokenType::L_BRACE);

    if (defType == "Style") {
        if (blockType == "Custom") {
            CustomStyleTemplate cst;
            cst.name = defName;
            while(m_currentToken.type != TokenType::R_BRACE) {
                if (m_currentToken.value == "inherit") {
                    advance(); expect(TokenType::AT); expect(TokenType::IDENTIFIER); // Style
                    cst.inherited_templates.push_back(m_currentToken.value);
                    advance(); expect(TokenType::SEMICOLON);
                } else if (m_currentToken.value == "delete") {
                    advance();
                    cst.deleted_keys.push_back(m_currentToken.value);
                    advance(); expect(TokenType::SEMICOLON);
                } else {
                     // simplified property parsing
                    std::string key = m_currentToken.value;
                    advance();
                    if(m_currentToken.type == TokenType::COLON) {
                        advance();
                        std::string value;
                        while(m_currentToken.type != TokenType::SEMICOLON) { value += m_currentToken.value; advance(); }
                        cst.properties.push_back({key, value});
                    } else {
                        cst.properties.push_back({key, std::nullopt});
                    }
                    expect(TokenType::SEMICOLON);
                }
            }
            m_context.addCustomStyleTemplate(cst);
        } else { // It's a [Template]
            StyleTemplate tpl;
            tpl.name = defName;
            while (m_currentToken.type != TokenType::R_BRACE) {
                std::string key = m_currentToken.value;
                advance();
                expect(TokenType::COLON);
                std::string value;
                while(m_currentToken.type != TokenType::SEMICOLON) { value += m_currentToken.value; advance(); }
                tpl.properties.push_back({key, value});
                expect(TokenType::SEMICOLON);
            }
            m_context.addStyleTemplate(tpl);
        }
    } else if (defType == "Element") {
        ElementTemplate tpl;
        tpl.name = defName;
        while (m_currentToken.type != TokenType::R_BRACE) {
            tpl.nodes.push_back(parseStatement());
        }
        m_context.addElementTemplate(std::move(tpl));
    }
    expect(TokenType::R_BRACE);
}

void CHTLParser::parseAttributes(ElementNode* element) { /* ... */ }
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
    while (m_currentToken.type != TokenType::R_BRACE) { element->addChild(parseStatement()); }
}
std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::IDENTIFIER);
    expect(TokenType::L_BRACE);
    auto styleNode = std::make_unique<StyleNode>();
    while (m_currentToken.type != TokenType::R_BRACE) {
        if(m_currentToken.type == TokenType::AT) {
            advance(); expect(TokenType::IDENTIFIER);
            std::string tplName = m_currentToken.value;
            advance();
            if (m_currentToken.type == TokenType::L_BRACE) {
                advance();
                // Specialization block
                while(m_currentToken.type != TokenType::R_BRACE) advance();
                expect(TokenType::R_BRACE);
            } else {
                expect(TokenType::SEMICOLON);
            }
            styleNode->addRule({ "@Style", {{ "name", tplName }} });
        } else {
            // simplified property parsing
            std::string key = m_currentToken.value;
            advance();
            expect(TokenType::COLON);
            std::string value;
            while(m_currentToken.type != TokenType::SEMICOLON) { value += m_currentToken.value; advance(); }
            expect(TokenType::SEMICOLON);
            styleNode->addRule({"", {{key, value}}});
        }
    }
    expect(TokenType::R_BRACE);
    return styleNode;
}
std::unique_ptr<BaseNode> CHTLParser::parseTextElement() {
    expect(TokenType::IDENTIFIER);
    expect(TokenType::L_BRACE);
    std::string content = m_lexer.readRawContentUntil('}');
    expect(TokenType::R_BRACE);
    return std::make_unique<TextNode>(StringUtil::trim(content));
}

} // namespace CHTL
