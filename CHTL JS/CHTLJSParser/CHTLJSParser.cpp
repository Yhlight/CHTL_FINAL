#include "CHTLJSParser.h"
#include "../CHTLJSNode/RawJSNode.h"
#include <stdexcept>
#include <algorithm>

namespace CHTL_JS {

// Helper function to trim whitespace from the start and end of a string
std::string trim(const std::string& s) {
    size_t first = s.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return s;
    }
    size_t last = s.find_last_not_of(" \t\n\r");
    return s.substr(first, (last - first + 1));
}

CHTLJSParser::CHTLJSParser(const std::vector<Token>& tokens, const std::string& source) : tokens(tokens), source(source) {}

std::vector<std::unique_ptr<CHTLJSBaseNode>> CHTLJSParser::parse() {
    std::vector<std::unique_ptr<CHTLJSBaseNode>> nodes;
    while (!isAtEnd()) {
        if(peek().type == TokenType::END_OF_FILE) break;
        nodes.push_back(parseStatement());
    }
    return nodes;
}

std::unique_ptr<CHTLJSBaseNode> CHTLJSParser::parseStatement() {
    if (peek().type == TokenType::LEFT_BRACE_BRACE) {
        auto selector = parseEnhancedSelector();
        if (match({TokenType::ARROW})) {
            consume(TokenType::LISTEN, "Expect 'Listen' keyword.");
            return parseListenExpression(std::move(selector));
        }
        return selector;
    }

    std::string raw_js;
    while (!isAtEnd() && peek().type != TokenType::LEFT_BRACE_BRACE) {
        raw_js += advance().lexeme;
    }
    return std::make_unique<RawJSNode>(raw_js);
}

std::unique_ptr<EnhancedSelectorNode> CHTLJSParser::parseEnhancedSelector() {
    consume(TokenType::LEFT_BRACE_BRACE, "Expect '{{' to start enhanced selector.");
    std::string content;
    while (!check(TokenType::RIGHT_BRACE_BRACE) && !isAtEnd()) {
        content += advance().lexeme;
    }
    consume(TokenType::RIGHT_BRACE_BRACE, "Expect '}}' to end enhanced selector.");

    auto node = std::make_unique<EnhancedSelectorNode>(content);

    // Parse the selector content
    std::string trimmed_content = trim(content);
    size_t bracket_pos = trimmed_content.find('[');
    if (bracket_pos != std::string::npos) {
        node->parsed_selector.type = SelectorType::IndexedQuery;
        node->parsed_selector.selector_string = trim(trimmed_content.substr(0, bracket_pos));
        size_t end_bracket_pos = trimmed_content.find(']', bracket_pos);
        if (end_bracket_pos != std::string::npos) {
            std::string index_str = trimmed_content.substr(bracket_pos + 1, end_bracket_pos - (bracket_pos + 1));
            try {
                node->parsed_selector.index = std::stoi(index_str);
            } catch (const std::invalid_argument& e) {
                // Handle error: invalid index
            }
        }
    } else {
        node->parsed_selector.type = SelectorType::Query;
        node->parsed_selector.selector_string = trimmed_content;
    }

    return node;
}

std::unique_ptr<ListenNode> CHTLJSParser::parseListenExpression(std::unique_ptr<EnhancedSelectorNode> selector) {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'Listen'.");

    auto listenNode = std::make_unique<ListenNode>(selector->parsed_selector);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token eventName = consume(TokenType::IDENTIFIER, "Expect event name.");
        consume(TokenType::COLON, "Expect ':' after event name.");

        Token start_token = peek();
        int brace_level = 0;
        if (check(TokenType::LEFT_BRACE)) brace_level++;

        while ((brace_level > 0 || (!check(TokenType::COMMA) && !check(TokenType::RIGHT_BRACE))) && !isAtEnd()) {
            if(peek().type == TokenType::LEFT_BRACE) brace_level++;
            if(peek().type == TokenType::RIGHT_BRACE) brace_level--;
            advance();
        }
        Token end_token = previous();

        int start_pos = start_token.position;
        int end_pos = end_token.position + end_token.lexeme.length();
        listenNode->events[eventName.lexeme] = source.substr(start_pos, end_pos - start_pos);

        if (match({TokenType::COMMA})) {
            // continue
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after Listen block.");
    return listenNode;
}


// --- Helper Method Implementations ---
bool CHTLJSParser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token CHTLJSParser::peek() { return tokens[current]; }
Token CHTLJSParser::previous() { return tokens[current - 1]; }
Token CHTLJSParser::advance() { if (!isAtEnd()) current++; return previous(); }
bool CHTLJSParser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }
Token CHTLJSParser::consume(TokenType type, const std::string& message) { if (check(type)) return advance(); throw std::runtime_error(message); }
bool CHTLJSParser::match(const std::vector<TokenType>& types) { for (TokenType type : types) { if (check(type)) { advance(); return true; } } return false; }

} // namespace CHTL_JS
