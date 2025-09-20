#include "CHTLJSParser.h"
#include <stdexcept>
#include <iostream>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const std::vector<Token>& tokens)
    : tokens(tokens) {}

std::vector<std::unique_ptr<CHTLJS_BaseNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJS_BaseNode>> nodes;
    std::string raw_js_buffer;

    while (!isAtEnd()) {
        bool is_chtl_js_construct = false;
        if (check(TokenType::LBRACE_BRACE)) {
            is_chtl_js_construct = true;
        } else if (check(TokenType::IDENTIFIER) && tokens.size() > current + 2 && tokens[current + 1].type == TokenType::ARROW && tokens[current + 2].type == TokenType::LISTEN) {
            is_chtl_js_construct = true;
        }

        if (is_chtl_js_construct) {
            if (!raw_js_buffer.empty()) {
                nodes.push_back(std::make_unique<CHTLJS_RawJSNode>(raw_js_buffer));
                raw_js_buffer.clear();
            }
            nodes.push_back(parseExpression());
        } else {
            raw_js_buffer += advance().lexeme + " ";
        }
    }

    if (!raw_js_buffer.empty()) {
        nodes.push_back(std::make_unique<CHTLJS_RawJSNode>(raw_js_buffer));
    }

    return nodes;
}

std::unique_ptr<CHTLJS_BaseNode> CHTLJSParser::parseExpression() {
    auto expr = parsePrimary();

    if (match({TokenType::ARROW})) {
        if (match({TokenType::LISTEN})) {
            return parseListenExpression(std::move(expr));
        } else {
            error(peek(), "Unsupported operator after '->'.");
        }
    }

    return expr;
}

std::unique_ptr<CHTLJS_ListenNode> CHTLJSParser::parseListenExpression(std::unique_ptr<CHTLJS_BaseNode> target) {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'Listen'.");
    auto listenNode = std::make_unique<CHTLJS_ListenNode>(std::move(target));

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token event_name = consume(TokenType::IDENTIFIER, "Expect event name (e.g. 'click').");
        consume(TokenType::COLON, "Expect ':' after event name.");

        // This is a HACK. We are just consuming tokens until we find a comma or a closing brace.
        // This does not handle nested blocks correctly.
        // The correct solution is a proper JS parser or the placeholder strategy.
        std::string handler_code;
        while(!isAtEnd() && !check(TokenType::COMMA) && !check(TokenType::RIGHT_BRACE)) {
            handler_code += advance().lexeme + " ";
        }

        listenNode->handlers.push_back({event_name.lexeme, handler_code});

        if (check(TokenType::COMMA)) {
            advance();
        } else if (!check(TokenType::RIGHT_BRACE)) {
            error(peek(), "Expect ',' or '}' after event handler.");
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to close Listen block.");
    return listenNode;
}

std::unique_ptr<CHTLJS_BaseNode> CHTLJSParser::parsePrimary() {
    if (match({TokenType::LBRACE_BRACE})) {
        std::string selector_content;
        while (!check(TokenType::RBRACE_BRACE) && !isAtEnd()) {
            selector_content += advance().lexeme;
        }
        consume(TokenType::RBRACE_BRACE, "Expect '}}' to end a selector.");
        return std::make_unique<CHTLJS_SelectorNode>(selector_content);
    }

    if (match({TokenType::IDENTIFIER})) {
        // This is tricky. In my_div->Listen, the "my_div" is consumed here.
        // It's not a CHTL-JS construct on its own, but part of one.
        // The main parse loop handles the lookahead. Here we just return a temporary node.
        // A better implementation would have a proper expression hierarchy.
        // For now, we'll just return a "raw" node that the generator can handle.
        return std::make_unique<CHTLJS_RawJSNode>(previous().lexeme);
    }

    error(peek(), "Expect primary expression (e.g., selector or identifier).");
    return nullptr;
}


// --- Helper Method Implementations ---

bool CHTLJSParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLJSParser::peek() {
    return tokens[current];
}

Token CHTLJSParser::previous() {
    return tokens[current - 1];
}

Token CHTLJSParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLJSParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token CHTLJSParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    error(peek(), message);
    throw std::runtime_error(message);
}

bool CHTLJSParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void CHTLJSParser::error(const Token& token, const std::string& message) {
    // For now, just print to cerr. A more robust system could be used.
    std::cerr << "[CHTL JS Parse Error] line " << token.line << " at '" << token.lexeme << "': " << message << std::endl;
}

} // namespace CHTLJS
