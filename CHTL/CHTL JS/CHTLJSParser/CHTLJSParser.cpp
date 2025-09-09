#include "CHTLJSParser.h"

namespace CHTL {
namespace CHTL_JS {

CHTLJSParser::CHTLJSParser(CHTLJSLexer& lexer) : m_lexer(lexer) {
    nextToken();
    nextToken();
}

void CHTLJSParser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

const std::vector<std::string>& CHTLJSParser::getErrors() const {
    return m_errors;
}

bool CHTLJSParser::expectPeek(CHTLJSTokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    }
    // In a real implementation, add error reporting here
    return false;
}

std::unique_ptr<CHTLJSProgram> CHTLJSParser::parseProgram() {
    auto program = std::make_unique<CHTLJSProgram>();
    while (m_currentToken.type != CHTLJSTokenType::EOF_TOKEN) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<CHTLJSStatement> CHTLJSParser::parseStatement() {
    switch (m_currentToken.type) {
        case CHTLJSTokenType::DOUBLE_LBRACE:
            return parseEnhancedSelector();
        case CHTLJSTokenType::LISTEN:
            // This is not quite right, listen is part of a larger expression
            // e.g., {{selector}}->listen { ... }
            // This simple parser will just parse `listen { ... }` for now.
            return parseListenStatement();
        default:
            // For now, treat other things as raw JS statements
            auto raw_node = std::make_unique<RawJavaScriptNode>();
            raw_node->code = m_currentToken.literal;
            return raw_node;
    }
}

std::unique_ptr<EnhancedSelectorNode> CHTLJSParser::parseEnhancedSelector() {
    auto node = std::make_unique<EnhancedSelectorNode>();
    node->token = m_currentToken; // The '{{' token

    // Simplified: consume everything until '}}'
    std::string selector_text;
    nextToken();
    while(m_currentToken.type != CHTLJSTokenType::DOUBLE_RBRACE && m_currentToken.type != CHTLJSTokenType::EOF_TOKEN) {
        selector_text += m_currentToken.literal;
        nextToken();
    }
    node->selector = selector_text;
    return node;
}

std::unique_ptr<ListenNode> CHTLJSParser::parseListenStatement() {
    auto node = std::make_unique<ListenNode>();
    node->token = m_currentToken; // The 'listen' token

    if (!expectPeek(CHTLJSTokenType::LBRACE)) return nullptr;

    // Simplified: does not parse individual handlers yet.
    // Skips to the matching brace.
    int brace_count = 1;
    nextToken();
    while(brace_count > 0 && m_currentToken.type != CHTLJSTokenType::EOF_TOKEN) {
        if (m_currentToken.type == CHTLJSTokenType::LBRACE) brace_count++;
        if (m_currentToken.type == CHTLJSTokenType::RBRACE) brace_count--;
        nextToken();
    }

    return node;
}

} // namespace CHTL_JS
} // namespace CHTL
