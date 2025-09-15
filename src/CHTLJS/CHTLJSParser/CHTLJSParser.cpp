#include "CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "CHTLJS/CHTLJSNode/ListenNode.h"
#include "CHTLJS/CHTLJSNode/DelegateNode.h"
#include "CHTLJS/CHTLJSNode/AnimateNode.h"
#include "CHTLJS/CHTLJSNode/PlaceholderNode.h"
#include "CHTLJS/CHTLJSNode/ValueNode.h"
#include "CHTLJS/CHTLJSNode/VirNode.h"
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(const std::string& source, std::vector<CHTLJSToken>& tokens, std::shared_ptr<CHTLJSContext> context)
    : source_(source), tokens_(tokens), context_(context) {}

std::unique_ptr<SequenceNode> CHTLJSParser::parse() {
    auto sequence = std::make_unique<SequenceNode>();
    while (!isAtEnd()) {
        sequence->statements_.push_back(parseStatement());
    }
    return sequence;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseStatement() {
    if (peek().type == CHTLJSTokenType::Vir) {
        return parseVirDeclaration();
    }
    return parseExpression();
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseVirDeclaration() {
    consume(CHTLJSTokenType::Vir, "Expected 'vir' keyword.");
    CHTLJSToken name = consume(CHTLJSTokenType::Identifier, "Expected identifier after 'vir'.");
    consume(CHTLJSTokenType::Equals, "Expected '=' after vir identifier.");

    auto value_node = parseExpression();
    if (!value_node) {
        throw std::runtime_error("Expected expression as value for vir declaration.");
    }

    // The node is stored in the context for lookups, and also returned as part of the AST
    // so the generator can decide what to do with it (e.g., declare a const).
    context_->virtual_objects[name.lexeme] = value_node.get();
    return std::make_unique<VirNode>(name.lexeme, std::move(value_node));
}


std::unique_ptr<CHTLJSNode> CHTLJSParser::parseExpression() {
    auto object = parsePrimaryExpression();
    while (match(CHTLJSTokenType::Arrow) || match(CHTLJSTokenType::Dot)) {
        object = parseMemberAccessExpression(std::move(object));
    }
    return object;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parsePrimaryExpression() {
    if (match(CHTLJSTokenType::OpenDoubleBrace)) {
        // The previous token was '{{'. Its end position is our start.
        size_t start_pos = tokens_[current_ - 1].end_pos;

        // Find the closing '}}' without consuming tokens in between.
        size_t selector_end_token_idx = current_;
        while (tokens_[selector_end_token_idx].type != CHTLJSTokenType::CloseDoubleBrace &&
               tokens_[selector_end_token_idx].type != CHTLJSTokenType::EndOfFile) {
            selector_end_token_idx++;
        }

        if (tokens_[selector_end_token_idx].type == CHTLJSTokenType::EndOfFile) {
            throw std::runtime_error("Unterminated enhanced selector.");
        }

        // The end position is the start of the '}}' token.
        size_t end_pos = tokens_[selector_end_token_idx].start_pos;
        std::string selector_str = source_.substr(start_pos, end_pos - start_pos);

        // Now, advance the parser's cursor past all the selector tokens.
        current_ = selector_end_token_idx;
        consume(CHTLJSTokenType::CloseDoubleBrace, "Expected '}}' to close enhanced selector.");

        return std::make_unique<EnhancedSelectorNode>(selector_str);
    }

    if (match(CHTLJSTokenType::Placeholder)) {
        return std::make_unique<PlaceholderNode>(tokens_[current_ - 1].lexeme);
    }

    if (match(CHTLJSTokenType::Animate)) {
        return parseAnimateBlock();
    }

    if (peek().type == CHTLJSTokenType::Identifier && context_->virtual_objects.count(peek().lexeme)) {
        // This is a vir object access, which is a primary expression.
        return parseExpression();
    }

    // Fallback for simple values
    if (match(CHTLJSTokenType::Identifier) || match(CHTLJSTokenType::String) || match(CHTLJSTokenType::Number)) {
        return std::make_unique<ValueNode>(tokens_[current_ - 1].lexeme);
    }

    throw std::runtime_error("Unexpected token when parsing expression: " + peek().lexeme);
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseMemberAccessExpression(std::unique_ptr<CHTLJSNode> object) {
    if (match(CHTLJSTokenType::Listen)) {
        return parseListenBlock(std::move(object));
    }
    if (match(CHTLJSTokenType::Delegate)) {
        return parseDelegateBlock(std::move(object));
    }

    // Handle simple property access, e.g., myVir->prop
    CHTLJSToken prop_name = consume(CHTLJSTokenType::Identifier, "Expected property name after '->' or '.'");

    // This is where we would perform a vir object lookup at parse time if needed.
    // For now, we just create a value node representing the access.
    // The generator will be responsible for resolving it.
    // Example: myVir->click becomes ValueNode("myVir.click")
    if (object->getType() == CHTLJSNodeType::Value) {
         auto value_node = static_cast<ValueNode*>(object.get());
         return std::make_unique<ValueNode>(value_node->getValue() + "." + prop_name.lexeme);
    }

    // For now, we can't handle member access on other node types yet.
    throw std::runtime_error("Member access on this expression type is not yet supported.");
}


// --- Block Parsers ---

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseAnimateBlock() {
    auto animate_node = std::make_unique<AnimateNode>();
    consume(CHTLJSTokenType::OpenBrace, "Expected '{' to open animate block.");

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        CHTLJSToken key = consume(CHTLJSTokenType::Identifier, "Expected property key identifier in animate block.");
        consume(CHTLJSTokenType::Colon, "Expected ':' after animate property key.");

        if (key.lexeme == "target") {
            // The value can be a single expression or an array of expressions
            if (peek().type == CHTLJSTokenType::OpenBracket) {
                consume(CHTLJSTokenType::OpenBracket, "Expected '[' for target array.");
                while (peek().type != CHTLJSTokenType::CloseBracket && !isAtEnd()) {
                    animate_node->targets_.push_back(parseExpression());
                    match(CHTLJSTokenType::Comma);
                }
                consume(CHTLJSTokenType::CloseBracket, "Expected ']' to close target array.");
            } else {
                animate_node->targets_.push_back(parseExpression());
            }
        } else if (key.lexeme == "when") {
            // when: [ { at: 0.5, ... }, { ... } ]
            consume(CHTLJSTokenType::OpenBracket, "Expected '[' for 'when' keyframes.");
            while (peek().type != CHTLJSTokenType::CloseBracket && !isAtEnd()) {
                consume(CHTLJSTokenType::OpenBrace, "Expected '{' for keyframe block.");
                Keyframe k;
                while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
                    CHTLJSToken prop_key = consume(CHTLJSTokenType::Identifier, "Expected property key in keyframe.");
                    consume(CHTLJSTokenType::Colon, "Expected ':' in keyframe.");
                    auto prop_val_node = parseExpression();
                    if (prop_val_node->getType() != CHTLJSNodeType::Value) {
                         throw std::runtime_error("Keyframe property values must be simple values.");
                    }
                    std::string prop_val = static_cast<ValueNode*>(prop_val_node.get())->getValue();

                    if (prop_key.lexeme == "at") {
                        k.offset = std::stod(prop_val);
                    } else {
                        k.properties[prop_key.lexeme] = prop_val;
                    }
                    match(CHTLJSTokenType::Comma);
                }
                consume(CHTLJSTokenType::CloseBrace, "Expected '}' to close keyframe.");
                animate_node->keyframes_.push_back(k);
                match(CHTLJSTokenType::Comma);
            }
            consume(CHTLJSTokenType::CloseBracket, "Expected ']' to close 'when' keyframes array.");
        } else {
            // Other properties like duration, easing, etc.
            auto value_node = parseExpression();
            if (value_node->getType() != CHTLJSNodeType::Value && value_node->getType() != CHTLJSNodeType::Placeholder) {
                throw std::runtime_error("Animate property values must be simple values or placeholders.");
            }
            std::string value_str = (value_node->getType() == CHTLJSNodeType::Value)
                ? static_cast<ValueNode*>(value_node.get())->getValue()
                : static_cast<PlaceholderNode*>(value_node.get())->getPlaceholderText();

            if (key.lexeme == "duration") animate_node->duration_ = value_str;
            else if (key.lexeme == "easing") animate_node->easing_ = value_str;
            else if (key.lexeme == "loop") animate_node->loop_ = value_str;
            else if (key.lexeme == "direction") animate_node->direction_ = value_str;
            else if (key.lexeme == "delay") animate_node->delay_ = value_str;
            else if (key.lexeme == "callback") animate_node->callback_ = value_str;
            // 'begin' and 'end' can be added here as special cases for keyframes at 0 and 1
        }

        match(CHTLJSTokenType::Comma);
    }

    consume(CHTLJSTokenType::CloseBrace, "Expected '}' to close animate block.");
    return animate_node;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseListenBlock(std::unique_ptr<CHTLJSNode> object) {
    auto listen_node = std::make_unique<ListenNode>(std::move(object));
    consume(CHTLJSTokenType::OpenBrace, "Expected '{' to open listen block.");

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        CHTLJSToken event_name = consume(CHTLJSTokenType::Identifier, "Expected event name identifier in listen block.");
        consume(CHTLJSTokenType::Colon, "Expected ':' after event name.");

        // The callback is an expression, which could be a placeholder.
        auto callback_expr = parseExpression();
        if (!callback_expr || callback_expr->getType() != CHTLJSNodeType::Placeholder) {
             throw std::runtime_error("Expected a placeholder for the callback body.");
        }
        auto placeholder_node = static_cast<PlaceholderNode*>(callback_expr.get());
        listen_node->addEvent(event_name.lexeme, placeholder_node->getPlaceholderText());

        match(CHTLJSTokenType::Comma); // Consume optional comma
    }
    consume(CHTLJSTokenType::CloseBrace, "Expected '}' to close listen block.");
    return listen_node;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseDelegateBlock(std::unique_ptr<CHTLJSNode> object) {
    auto delegate_node = std::make_unique<DelegateNode>(std::move(object));
    consume(CHTLJSTokenType::OpenBrace, "Expected '{' to open delegate block.");

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        CHTLJSToken key = consume(CHTLJSTokenType::Identifier, "Expected property key identifier in delegate block.");
        consume(CHTLJSTokenType::Colon, "Expected ':' after property key.");

        if (key.lexeme == "target") {
            if (peek().type == CHTLJSTokenType::OpenBracket) {
                consume(CHTLJSTokenType::OpenBracket, "Expected '[' for target array.");
                while (peek().type != CHTLJSTokenType::CloseBracket && !isAtEnd()) {
                    delegate_node->addTarget(parseExpression());
                    match(CHTLJSTokenType::Comma);
                }
                consume(CHTLJSTokenType::CloseBracket, "Expected ']' to close target array.");
            } else {
                delegate_node->addTarget(parseExpression());
            }
        } else {
            // It's an event:callback pair
            auto callback_expr = parseExpression();
            if (!callback_expr || callback_expr->getType() != CHTLJSNodeType::Placeholder) {
                 throw std::runtime_error("Expected a placeholder for the callback body.");
            }
            auto placeholder_node = static_cast<PlaceholderNode*>(callback_expr.get());
            delegate_node->addEvent(key.lexeme, placeholder_node->getPlaceholderText());
        }

        match(CHTLJSTokenType::Comma);
    }

    consume(CHTLJSTokenType::CloseBrace, "Expected '}' to close delegate block.");
    return delegate_node;
}


// --- Helpers ---

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

const CHTLJSToken& CHTLJSParser::consume(CHTLJSTokenType type, const std::string& error_message) {
    if (peek().type == type) {
        return advance();
    }
    throw std::runtime_error(error_message + " Got " + peek().lexeme + " instead.");
}

bool CHTLJSParser::match(CHTLJSTokenType type) {
    if (peek().type == type) {
        advance();
        return true;
    }
    return false;
}

} // namespace CHTLJS
