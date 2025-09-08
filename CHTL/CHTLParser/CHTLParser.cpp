#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ModificationNode.h"
#include "../Util/StringUtil/StringUtil.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(CHTLLexer& lexer, CHTLContext& context) : m_lexer(lexer), m_context(context) { advance(); }
void CHTLParser::advance() { m_currentToken = m_lexer.getNextToken(); }
void CHTLParser::expect(TokenType type) { if (m_currentToken.type == type) advance(); else throw std::runtime_error("Unexpected token"); }

std::unique_ptr<BaseNode> CHTLParser::parse() {
    auto root = std::make_unique<ElementNode>("_root_");
    while (m_currentToken.type != TokenType::END_OF_FILE) {
        if (m_currentToken.type == TokenType::L_BRACKET) {
            parseDefinitionBlock();
        } else {
            root->addChild(parseStatement());
        }
    }
    return root;
}

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (m_currentToken.type == TokenType::AT) {
        advance();
        if (m_currentToken.value == "Element") {
            advance();
            std::string name = m_currentToken.value;
            advance();
            auto usageNode = std::make_unique<TemplateUsageNode>(TemplateType::Element, name);
            if (m_currentToken.type == TokenType::L_BRACE) {
                advance();
                // parseSpecializationBlock(usageNode.get());
                expect(TokenType::R_BRACE);
            } else {
                expect(TokenType::SEMICOLON);
            }
            return usageNode;
        }
    }
    if (m_currentToken.type == TokenType::IDENTIFIER) {
        if (m_currentToken.value == "text") return parseTextElement();
        if (m_currentToken.value == "style") return parseStyleNode();
        return parseElement();
    }
    return nullptr;
}

void CHTLParser::parseDefinitionBlock() { /* ... complex logic ... */ }

void CHTLParser::parseAttributes(ElementNode* element) {
    while (m_currentToken.type == TokenType::IDENTIFIER) {
        CHTLLexer lookahead_lexer = m_lexer;
        lookahead_lexer.getNextToken();
        if (lookahead_lexer.getNextToken().type != TokenType::COLON) break;

        std::string key = m_currentToken.value;
        advance();
        expect(TokenType::COLON);

        std::string value;
        if (m_currentToken.type == TokenType::IDENTIFIER) {
            CHTLLexer var_lookahead = m_lexer;
            var_lookahead.getNextToken();
            if (var_lookahead.getNextToken().type == TokenType::L_PAREN) {
                std::string varGroupName = m_currentToken.value;
                advance();
                expect(TokenType::L_PAREN);
                std::string varName = m_currentToken.value;
                advance();
                if (m_currentToken.type == TokenType::EQUALS) {
                    advance();
                    value = m_currentToken.value;
                    advance();
                } else {
                    const VarTemplate* varTpl = m_context.getVarTemplate(varGroupName);
                    if (varTpl && varTpl->variables.count(varName)) {
                        value = varTpl->variables.at(varName);
                    } else { throw std::runtime_error("Undefined variable"); }
                }
                expect(TokenType::R_PAREN);
            } else {
                value = m_currentToken.value;
                advance();
            }
        } else if (m_currentToken.type == TokenType::STRING_LITERAL) {
            value = m_currentToken.value;
            advance();
        } else { throw std::runtime_error("Expected attribute value"); }
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
    }
    return element;
}

void CHTLParser::parseElementContent(ElementNode* element) {
    parseAttributes(element);
    while (m_currentToken.type != TokenType::R_BRACE) {
        element->addChild(parseStatement());
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseStyleNode() {
    expect(TokenType::IDENTIFIER);
    expect(TokenType::L_BRACE);
    auto styleNode = std::make_unique<StyleNode>();
    while (m_currentToken.type != TokenType::R_BRACE) { advance(); } // Simplified
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
