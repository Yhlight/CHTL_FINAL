#include "CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(std::vector<CHTLJSToken>& tokens) : tokens_(tokens) {}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parse() {
    // This is a stub parser that only looks for a single enhanced selector.
    // A full implementation would parse a complete CHTL JS AST.
    while (!isAtEnd()) {
        if (peek().type == CHTLJSTokenType::OpenDoubleBrace) {
            advance(); // consume '{{'

            if (peek().type != CHTLJSTokenType::Identifier) {
                throw std::runtime_error("Expected an identifier inside {{...}}.");
            }
            CHTLJSToken selector_token = advance(); // consume identifier

            if (peek().type != CHTLJSTokenType::CloseDoubleBrace) {
                throw std::runtime_error("Expected '}}' to close enhanced selector.");
            }
            advance(); // consume '}}'

            return std::make_unique<EnhancedSelectorNode>(selector_token.lexeme);
        }
        advance();
    }
    return nullptr; // Return null if no selector is found
}

const CHTLJSToken& CHTLJSParser::peek() const {
    return tokens_[current_];
}

const CHTLJSToken& CHTLJSParser::advance() {
    if (!isAtEnd()) current_++;
    return tokens_[current_ - 1];
}

bool CHTLJSParser::isAtEnd() const {
    return peek().type == CHTLJSTokenType::EndOfFile;
}

} // namespace CHTLJS
