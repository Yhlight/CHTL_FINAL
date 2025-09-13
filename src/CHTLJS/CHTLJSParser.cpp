#include "CHTLJSParser.h"
#include <stdexcept>

namespace CHTL {
namespace CHTLJS {

CHTLJSParser::CHTLJSParser(std::vector<JSToken> tokens) : tokens_(std::move(tokens)) {}

std::vector<std::unique_ptr<JSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<JSNode>> nodes;
    while (!isAtEnd()) {
        nodes.push_back(parseExpression());
    }
    return nodes;
}

std::unique_ptr<JSNode> CHTLJSParser::parseExpression() {
    const JSToken& token = advance();

    if (token.type == JSTokenType::StandardJS) {
        return std::make_unique<StandardJSNode>(token.content);
    }

    if (token.type == JSTokenType::EnhancedSelector) {
        auto selectorNode = std::make_unique<EnhancedSelectorNode>(token.content);
        if (!isAtEnd() && peek().type == JSTokenType::Arrow) {
            return parseCall(std::move(selectorNode));
        }
        return selectorNode;
    }

    throw std::runtime_error("Unexpected token in CHTL JS content.");
}

std::unique_ptr<JSNode> CHTLJSParser::parseCall(std::unique_ptr<JSNode> target) {
    advance(); // consume '->'
    const JSToken& callToken = advance();
    if (callToken.type == JSTokenType::ListenBlock) {
        auto listenNode = std::make_unique<ListenNode>(std::move(target));
        // Super simple parser for "key: value," format.
        // This is a placeholder and should be replaced with a real parser.
        std::string content = callToken.content;
        size_t cursor = 0;
        while(cursor < content.length()) {
            size_t key_end = content.find(':', cursor);
            if (key_end == std::string::npos) break;
            std::string key = content.substr(cursor, key_end - cursor);
            key.erase(0, key.find_first_not_of(" \t\n\r,"));
            key.erase(key.find_last_not_of(" \t\n\r,") + 1);

            size_t value_start = key_end + 1;
            size_t value_end = content.find(',', value_start);
            if (value_end == std::string::npos) {
                value_end = content.length();
            }
            std::string value = content.substr(value_start, value_end - value_start);
            value.erase(0, value.find_first_not_of(" \t\n\r,"));
            value.erase(value.find_last_not_of(" \t\n\r,") + 1);

            listenNode->events[key] = value;
            cursor = value_end + 1;
        }
        return listenNode;
    }
    throw std::runtime_error("Expected 'listen' after '->'.");
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
