#include "CHTLJSParser.h"
#include <stdexcept>
#include <iostream> // For debugging

namespace CHTL {
namespace CHTLJS {

CHTLJSParser::CHTLJSParser(std::vector<JSToken> tokens) : tokens_(std::move(tokens)) {
    // Debug print
    // for(const auto& token : tokens_) {
    //     std::cout << "TOKEN: " << (int)token.type << " | " << token.content << std::endl;
    // }
}

std::vector<std::unique_ptr<JSNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<JSNode>> nodes;
    while (!isAtEnd()) {
        nodes.push_back(parseExpression());
    }
    return nodes;
}

std::unique_ptr<JSNode> CHTLJSParser::parseExpression() {
    const JSToken& token = peek();

    if (token.type == JSTokenType::StandardJS) {
        advance();
        return std::make_unique<StandardJSNode>(token.content);
    }

    if (token.type == JSTokenType::EnhancedSelector) {
        advance();
        auto selectorNode = std::make_unique<EnhancedSelectorNode>(token.content);
        if (!isAtEnd() && peek().type == JSTokenType::Arrow) {
            return parseCall(std::move(selectorNode));
        }
        return selectorNode;
    }

    if (token.type == JSTokenType::ScriptLoaderBlock) {
        advance();
        return parseScriptLoader(token);
    }

    throw std::runtime_error("Unexpected token in CHTL JS content.");
}

std::unique_ptr<JSNode> CHTLJSParser::parseScriptLoader(const JSToken& loader_token) {
    auto loaderNode = std::make_unique<ScriptLoaderNode>();
    // Super simple parser for "load: path," format.
    std::string content = loader_token.content;
    size_t cursor = 0;
    while(cursor < content.length()) {
        size_t key_end = content.find(':', cursor);
        if (key_end == std::string::npos) break;

        size_t value_start = key_end + 1;
        size_t value_end = content.find(',', value_start);
        if (value_end == std::string::npos) {
            value_end = content.length();
        }

        std::string value = content.substr(value_start, value_end - value_start);
        // Trim whitespace and quotes
        value.erase(0, value.find_first_not_of(" \t\n\r,\""));
        value.erase(value.find_last_not_of(" \t\n\r,\"") + 1);

        loaderNode->paths.push_back(value);
        cursor = value_end + 1;
    }
    return loaderNode;
}

std::unique_ptr<JSNode> CHTLJSParser::parseCall(std::unique_ptr<JSNode> target) {
    advance(); // consume '->'
    const JSToken& callToken = advance();
    if (callToken.type == JSTokenType::ListenBlock) {
        auto listenNode = std::make_unique<ListenNode>(std::move(target));
        // Super simple parser for "key: value," format.
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
