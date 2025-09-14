#include "CHTLJSParser.h"
#include <stdexcept>
#include <sstream>

namespace CHTLJS {

// =============================================================================
// Parser Public API
// =============================================================================

CHTLJSParser::CHTLJSParser(std::vector<CHTLJSToken>& tokens, std::shared_ptr<CHTLJSContext> context)
    : tokens_(tokens), context_(context) {}

std::unique_ptr<RootNode> CHTLJSParser::parse() {
    auto root = std::make_unique<RootNode>();
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                root->addStatement(std::move(stmt));
            }
        } catch (const std::runtime_error& e) {
            // In a real compiler, you'd report the error without exiting.
            // For now, we'll just stop. A better implementation would use synchronize().
            // For example:
            // hadError_ = true;
            // std::cerr << e.what() << std::endl;
            // synchronize();
            throw; // Re-throwing for now
        }
    }
    return root;
}


// =============================================================================
// Statement Parsers
// =============================================================================

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseStatement() {
    if (match({CHTLJSTokenType::Vir})) {
        return parseVirDeclaration();
    }
    if (match({CHTLJSTokenType::Router})) {
        return parseRouterStatement();
    }
    return parseExpression();
}

std::unique_ptr<VirNode> CHTLJSParser::parseVirDeclaration() {
    const auto& name_token = consume(CHTLJSTokenType::Identifier, "Expect identifier after 'vir'.");
    consume(CHTLJSTokenType::Equals, "Expect '=' after virtual object name.");
    auto value = parseExpression();
    if (!value) {
        throw std::runtime_error("Expect expression after '=' in vir declaration.");
    }

    auto vir_node = std::make_unique<VirNode>(name_token.lexeme, value->clone());

    // Store the parsed value in the context for later lookups
    context_->virtual_objects[name_token.lexeme] = std::move(value);

    return vir_node;
}

std::unique_ptr<RouterNode> CHTLJSParser::parseRouterStatement() {
    auto router_node = std::make_unique<RouterNode>();
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' after 'router'.");

    while (!check(CHTLJSTokenType::CloseBrace) && !isAtEnd()) {
        const auto& key = consume(CHTLJSTokenType::Identifier, "Expect key in router block.");
        consume(CHTLJSTokenType::Colon, "Expect ':' after key in router block.");

        if (key.lexeme == "mode") {
            const auto& mode_val = consume(CHTLJSTokenType::String, "Router mode must be a string.");
            router_node->setMode(mode_val.lexeme);
        } else if (key.lexeme == "root") {
            // Can be string for path or selector for container
            auto value = parseValue();
            if (value->is_string) {
                router_node->setRootPath(value->value);
            } else {
                router_node->setRootContainer(std::move(value));
            }
        } else if (key.lexeme == "page") {
             consume(CHTLJSTokenType::OpenBrace, "Expect '{' for router pages.");
             while (!check(CHTLJSTokenType::CloseBrace) && !isAtEnd()) {
                consume(CHTLJSTokenType::OpenBrace, "Expect '{' for a route entry.");
                const auto& url = consume(CHTLJSTokenType::String, "Expect URL string for route.");
                consume(CHTLJSTokenType::Comma, "Expect ',' separating URL and page selector.");
                auto page_selector = parseValue();
                router_node->addRoute(url.lexeme, std::move(page_selector));
                consume(CHTLJSTokenType::CloseBrace, "Expect '}' to close route entry.");
                if (match({CHTLJSTokenType::Comma})) {
                    // consume optional comma
                }
             }
             consume(CHTLJSTokenType::CloseBrace, "Expect '}' to close router pages.");
        }

        if (match({CHTLJSTokenType::Comma})) {
            // Trailing comma
        }
    }

    consume(CHTLJSTokenType::CloseBrace, "Expect '}' to close 'router' block.");
    return router_node;
}


// =============================================================================
// Expression Parsers
// =============================================================================

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseExpression() {
    auto expr = parsePrimary();

    while (match({CHTLJSTokenType::Arrow, CHTLJSTokenType::Dot})) {
        const auto& op = this->previous();
        if (match({CHTLJSTokenType::Listen})) {
            expr = parseListenExpression(std::move(expr));
        } else if (match({CHTLJSTokenType::Delegate})) {
            expr = parseDelegateExpression(std::move(expr));
        }
        // else if (match({CHTLJSTokenType::Identifier})) {
        //     // Handle property access, e.g., myVir->property
        // }
        else {
            throw std::runtime_error("Unexpected token after '->' or '.'.");
        }
    }

    return expr;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parsePrimary() {
    if (match({CHTLJSTokenType::OpenDoubleBrace})) {
        return parseEnhancedSelector();
    }
    if (match({CHTLJSTokenType::Animate})) {
        return parseAnimateExpression();
    }
    // Handle vir object property access
    if (check(CHTLJSTokenType::Identifier) && context_->virtual_objects.count(peek().lexeme)) {
        const auto& vir_name = advance();
        consume(CHTLJSTokenType::Arrow, "Expect '->' after virtual object access.");
        const auto& prop_name = consume(CHTLJSTokenType::Identifier, "Expect property name after '->'.");

        auto stored_node_it = context_->virtual_objects.find(vir_name.lexeme);
        if(stored_node_it != context_->virtual_objects.end()) {
            auto& stored_node = stored_node_it->second;
            if (stored_node->getType() == CHTLJSNodeType::Listen) {
                auto* listen_node = static_cast<ListenNode*>(stored_node.get());
                const auto& events = listen_node->getEvents();
                auto event_it = events.find(prop_name.lexeme);
                if (event_it != events.end()) {
                    // This is a function reference, return its body as a value
                    return std::make_unique<ValueNode>(event_it->second, false);
                }
            }
        }
        throw std::runtime_error("Property '" + prop_name.lexeme + "' not found on virtual object '" + vir_name.lexeme + "'.");
    }

    throw std::runtime_error("Unexpected token in expression.");
}

// =============================================================================
// CHTL JS Feature Parsers
// =============================================================================

std::unique_ptr<EnhancedSelectorNode> CHTLJSParser::parseEnhancedSelector() {
    auto token = consume(CHTLJSTokenType::Identifier, "Expect selector inside {{...}}.");
    consume(CHTLJSTokenType::CloseDoubleBrace, "Expect '}}' to close enhanced selector.");
    return std::make_unique<EnhancedSelectorNode>(token.lexeme);
}

std::unique_ptr<ListenNode> CHTLJSParser::parseListenExpression(std::unique_ptr<CHTLJSNode> object) {
    auto listen_node = std::make_unique<ListenNode>(std::move(object));
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' to start listen block.");

    while (!check(CHTLJSTokenType::CloseBrace) && !isAtEnd()) {
        const auto& event_name = consume(CHTLJSTokenType::Identifier, "Expect event name.");
        consume(CHTLJSTokenType::Colon, "Expect ':' after event name.");

        // The callback is a raw JS block, which we can't parse with this parser.
        // We'll extract it as a raw string.
        std::string callback_body = parseRawBlock();
        listen_node->addEvent(event_name.lexeme, callback_body);

        if (match({CHTLJSTokenType::Comma})) {
            // optional comma
        }
    }
    consume(CHTLJSTokenType::CloseBrace, "Expect '}' to close listen block.");
    return listen_node;
}

std::unique_ptr<DelegateNode> CHTLJSParser::parseDelegateExpression(std::unique_ptr<CHTLJSNode> object) {
    // Implementation is complex, leaving as a TODO for now, but setting up the structure.
    auto delegate_node = std::make_unique<DelegateNode>(std::move(object));
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' to start delegate block.");
    // TODO: Parse 'target' and event-callback pairs
    while (!check(CHTLJSTokenType::CloseBrace) && !isAtEnd()) {
        advance(); // Just consume tokens for now
    }
    consume(CHTLJSTokenType::CloseBrace, "Expect '}' to close delegate block.");
    return delegate_node;
}

std::unique_ptr<AnimateNode> CHTLJSParser::parseAnimateExpression() {
    // Implementation is complex, leaving as a TODO for now, but setting up the structure.
    auto animate_node = std::make_unique<AnimateNode>();
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' to start animate block.");
    // TODO: Parse all animate properties
    while (!check(CHTLJSTokenType::CloseBrace) && !isAtEnd()) {
        advance(); // Just consume tokens for now
    }
    consume(CHTLJSTokenType::CloseBrace, "Expect '}' to close animate block.");
    return animate_node;
}


std::unique_ptr<ValueNode> CHTLJSParser::parseValue() {
    if (match({CHTLJSTokenType::String})) {
        return std::make_unique<ValueNode>(this->previous().lexeme, true);
    }
    if (match({CHTLJSTokenType::OpenDoubleBrace})) {
        auto selector = parseEnhancedSelector();
        // This is a bit of a hack. A real implementation might need a different node type.
        return std::make_unique<ValueNode>("{{ " + selector->getSelector() + " }}", false);
    }
    throw std::runtime_error("Expected a value (string or enhanced selector).");
}


// =============================================================================
// Helper Methods
// =============================================================================

const CHTLJSToken& CHTLJSParser::peek() const {
    return tokens_[current_];
}

const CHTLJSToken& CHTLJSParser::peekNext() const {
    if (current_ + 1 >= tokens_.size()) {
        return tokens_.back(); // EndOfFile token
    }
    return tokens_[current_ + 1];
}

const CHTLJSToken& CHTLJSParser::previous() const {
    return tokens_[current_ - 1];
}

const CHTLJSToken& CHTLJSParser::advance() {
    if (!isAtEnd()) {
        current_++;
    }
    return this->previous();
}

bool CHTLJSParser::isAtEnd() const {
    return peek().type == CHTLJSTokenType::EndOfFile;
}

bool CHTLJSParser::check(CHTLJSTokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLJSParser::match(const std::vector<CHTLJSTokenType>& types) {
    for (CHTLJSTokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const CHTLJSToken& CHTLJSParser::consume(CHTLJSTokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message);
}

// This function is a major simplification. It does not produce a valid AST for the JS,
// but rather extracts the callback as a raw string. A proper implementation would require
// a full JS parser.
std::string CHTLJSParser::parseRawBlock() {
    std::stringstream raw_content;

    // Case 1: Simple identifier
    if (peek().type == CHTLJSTokenType::Identifier && (peekNext().type == CHTLJSTokenType::Comma || peekNext().type == CHTLJSTokenType::CloseBrace)) {
        raw_content << advance().lexeme;
        return raw_content.str();
    }

    // Case 2: Arrow function or regular function block
    int brace_level = 0;
    int paren_level = 0;

    // Look for the start of the block, which could be an open parenthesis or an open brace
    bool in_block = false;

    while (!isAtEnd()) {
        const auto& token = peek();

        if (token.type == CHTLJSTokenType::OpenParen) paren_level++;
        if (token.type == CHTLJSTokenType::CloseParen) paren_level--;
        if (token.type == CHTLJSTokenType::OpenBrace) brace_level++;
        if (token.type == CHTLJSTokenType::CloseBrace) brace_level--;

        // The block officially starts with the first brace
        if (token.type == CHTLJSTokenType::OpenBrace) {
            in_block = true;
        }

        raw_content << token.lexeme << " ";
        advance();

        // The block ends when all braces are closed, or if not in a block,
        // when the next token is a comma or a closing brace.
        if (in_block) {
            if (brace_level == 0) break;
        } else {
            if (peek().type == CHTLJSTokenType::Comma || peek().type == CHTLJSTokenType::CloseBrace) {
                break;
            }
        }
    }

    if (brace_level != 0 || paren_level != 0) {
        // This indicates a syntax error in the raw block, but we can't fully validate it.
        // We'll let the JS engine catch it.
    }

    return raw_content.str();
}

void CHTLJSParser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == CHTLJSTokenType::CloseBrace) return; // End of a block
        switch (peek().type) {
            case CHTLJSTokenType::Vir:
            case CHTLJSTokenType::Router:
            case CHTLJSTokenType::Listen:
            case CHTLJSTokenType::Animate:
            case CHTLJSTokenType::Delegate:
                return;
            default:
                break;
        }
        advance();
    }
}

} // namespace CHTLJS
