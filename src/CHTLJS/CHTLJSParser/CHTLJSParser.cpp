#include "CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "CHTLJS/CHTLJSNode/ListenNode.h"
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(std::vector<CHTLJSToken>& tokens) : tokens_(tokens) {}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parse() {
    // For now, we parse a single expression, not a full script.
    if (isAtEnd()) return nullptr;

    std::unique_ptr<CHTLJSNode> object_node;

    if (peek().type == CHTLJSTokenType::OpenDoubleBrace) {
        advance(); // consume '{{'
        if (peek().type != CHTLJSTokenType::Identifier) {
            throw std::runtime_error("Expected an identifier inside {{...}}.");
        }
        CHTLJSToken selector_token = advance();
        if (peek().type != CHTLJSTokenType::CloseDoubleBrace) {
            throw std::runtime_error("Expected '}}' to close enhanced selector.");
        }
        advance(); // consume '}}'
        object_node = std::make_unique<EnhancedSelectorNode>(selector_token.lexeme);
    } else {
        // In the future, other object types could be parsed here.
        return nullptr;
    }

    // Check for chained calls
    if (peek().type == CHTLJSTokenType::Arrow || peek().type == CHTLJSTokenType::Dot) {
        advance(); // consume '->' or '.'

        if (peek().type == CHTLJSTokenType::Listen) {
            advance(); // consume 'listen'
            return parseListenBlock(std::move(object_node));
        }
    }

    return object_node;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseListenBlock(std::unique_ptr<CHTLJSNode> object) {
    auto listen_node = std::make_unique<ListenNode>(std::move(object));

    if (peek().type != CHTLJSTokenType::OpenBrace) {
        throw std::runtime_error("Expected '{' to open listen block.");
    }
    advance(); // consume '{'

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        if (peek().type != CHTLJSTokenType::Identifier) {
            throw std::runtime_error("Expected event name identifier in listen block.");
        }
        std::string event_name = advance().lexeme;

        if (peek().type != CHTLJSTokenType::Colon) {
            throw std::runtime_error("Expected ':' after event name.");
        }
        advance(); // consume ':'

        if (peek().type != CHTLJSTokenType::Identifier) {
            throw std::runtime_error("Expected callback body after event name.");
        }
        std::string callback_body = advance().lexeme;

        listen_node->addEvent(event_name, callback_body);

        if (peek().type == CHTLJSTokenType::Comma) {
            advance(); // consume optional comma
        }
    }

    if (peek().type != CHTLJSTokenType::CloseBrace) {
        throw std::runtime_error("Expected '}' to close listen block.");
    }
    advance(); // consume '}'

    return listen_node;
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
