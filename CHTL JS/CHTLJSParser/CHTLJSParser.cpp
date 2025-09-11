#include "CHTLJSParser.h"
#include "../CHTLJSNode/JSCodeNode.h"
#include "../CHTLJSNode/EnhancedSelectorNode.h"

namespace CHTL_JS {

CHTLJSParser::CHTLJSParser(CHTLJSLexer& lexer) : m_lexer(lexer) {
    nextToken();
}

void CHTLJSParser::nextToken() {
    m_currentToken = m_lexer.NextToken();
}

NodeList CHTLJSParser::Parse() {
    NodeList nodes;
    while (m_currentToken.type != TokenType::TOKEN_EOF) {
        switch (m_currentToken.type) {
            case TokenType::TOKEN_JS_CODE:
                nodes.push_back(std::make_shared<JSCodeNode>(m_currentToken.literal));
                break;
            case TokenType::TOKEN_LDBRACE:
                {
                    nextToken(); // Consume {{
                    // Everything until }} is part of the selector
                    std::string selector_content;
                    if (m_currentToken.type == TokenType::TOKEN_JS_CODE) {
                        selector_content = m_currentToken.literal;
                    }
                    nodes.push_back(std::make_shared<EnhancedSelectorNode>(selector_content));
                    nextToken(); // Consume selector content
                    // The next token should be RDBRACE, which we consume in the main loop
                }
                break;
            case TokenType::TOKEN_RDBRACE:
                // This is consumed by the main loop's nextToken call
                break;
            default:
                break;
        }
        nextToken();
    }
    return nodes;
}

} // namespace CHTL_JS
