#include "CHTLJSParser.h"

namespace CHTL {
namespace CHTLJS {

CHTLJSParser::CHTLJSParser(std::vector<JSToken> tokens) : tokens_(std::move(tokens)) {}

std::vector<std::unique_ptr<JSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<JSNode>> nodes;
    while (!isAtEnd()) {
        const JSToken& token = advance();
        if (token.type == JSTokenType::StandardJS) {
            nodes.push_back(std::make_unique<StandardJSNode>(token.content));
        } else if (token.type == JSTokenType::EnhancedSelector) {
            nodes.push_back(std::make_unique<EnhancedSelectorNode>(token.content));
        }
    }
    return nodes;
}

const JSToken& CHTLJSParser::peek() const {
    return tokens_[current_];
}

const JSToken& CHTLJSParser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return tokens_[current_ - 1];
}

bool CHTLJSParser::isAtEnd() const {
    return current_ >= tokens_.size() || tokens_[current_].type == JSTokenType::EndOfFile;
}

} // namespace CHTLJS
} // namespace CHTL
