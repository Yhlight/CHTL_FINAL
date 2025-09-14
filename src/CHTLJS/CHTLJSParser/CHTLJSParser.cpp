#include "CHTLJS/CHTLJSParser/CHTLJSParser.h"
#include "CHTLJS/CHTLJSNode/RootNode.h"
#include "CHTLJS/CHTLJSNode/PlaceholderNode.h"
#include "CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "CHTLJS/CHTLJSNode/ListenNode.h"
#include "CHTLJS/CHTLJSNode/DelegateNode.h"
#include "CHTLJS/CHTLJSNode/AnimateNode.h"
#include "CHTLJS/CHTLJSNode/ValueNode.h"
#include <stdexcept>

namespace CHTLJS {

CHTLJSParser::CHTLJSParser(std::vector<CHTLJSToken>& tokens, std::shared_ptr<CHTLJSContext> context)
    : tokens_(tokens), context_(context) {}

// The main entry point, which now returns a RootNode containing a sequence of nodes.
std::unique_ptr<RootNode> CHTLJSParser::parse() {
    auto root = std::make_unique<RootNode>();
    while (!isAtEnd()) {
        root->addChild(parseDeclarationOrExpression());
    }
    return root;
}

// Parses a single declaration (like 'vir') or an expression.
std::unique_ptr<CHTLJSNode> CHTLJSParser::parseDeclarationOrExpression() {
    if (peek().type == CHTLJSTokenType::Placeholder) {
        return std::make_unique<PlaceholderNode>(advance().lexeme);
    }

    if (peek().type == CHTLJSTokenType::Vir) {
        return parseVirDeclaration();
    }

    return parseExpression();
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseVirDeclaration() {
    advance(); // consume 'vir'
    CHTLJSToken name = advance();
    if (name.type != CHTLJSTokenType::Identifier) {
        throw std::runtime_error("Expected identifier after 'vir'.");
    }
    if (advance().type != CHTLJSTokenType::Equals) {
         throw std::runtime_error("Expected '=' after vir identifier.");
    }

    auto value_node = parseExpression();
    if (value_node) {
        context_->virtual_objects[name.lexeme] = std::move(value_node);
    }
    // A declaration does not become a node in the final generated code,
    // it only populates the context. We return a nullptr which the main loop should handle.
    // A better approach might be a specific 'DeclarationNode' that generates no output.
    // For now, let's make the main parse loop smart enough.
    // Let's create a dummy node that generates nothing.
    return std::make_unique<PlaceholderNode>("");
}


// Parses a single expression, which can be chained.
std::unique_ptr<CHTLJSNode> CHTLJSParser::parseExpression() {
    if (peek().type == CHTLJSTokenType::Placeholder) {
        return std::make_unique<PlaceholderNode>(advance().lexeme);
    }

    std::unique_ptr<CHTLJSNode> left;

    if (peek().type == CHTLJSTokenType::Animate) {
        advance(); // consume 'animate'
        left = parseAnimateBlock();
    } else if (peek().type == CHTLJSTokenType::OpenDoubleBrace) {
        advance(); // consume '{{'
        if (peek().type != CHTLJSTokenType::Identifier) {
            throw std::runtime_error("Expected an identifier inside {{...}}.");
        }
        CHTLJSToken selector_token = advance();
        if (peek().type != CHTLJSTokenType::CloseDoubleBrace) {
            throw std::runtime_error("Expected '}}' to close enhanced selector.");
        }
        advance(); // consume '}}'
        left = std::make_unique<EnhancedSelectorNode>(selector_token.lexeme);
    } else if (peek().type == CHTLJSTokenType::Identifier && context_->virtual_objects.count(peek().lexeme)) {
        // This is a vir object access, e.g., myListener->click
        return parseVirAccess();
    } else {
        // Not a recognized start of an expression.
        return nullptr;
    }

    // Now check for chained calls like ->listen
    if (peek().type == CHTLJSTokenType::Arrow || peek().type == CHTLJSTokenType::Dot) {
        advance(); // consume '->' or '.'

        if (peek().type == CHTLJSTokenType::Listen) {
            advance(); // consume 'listen'
            return parseListenBlock(std::move(left));
        } else if (peek().type == CHTLJSTokenType::Delegate) {
            advance(); // consume 'delegate'
            return parseDelegateBlock(std::move(left));
        }
    }

    return left;
}

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseVirAccess() {
    CHTLJSToken vir_name = advance();
    if (peek().type != CHTLJSTokenType::Arrow && peek().type != CHTLJSTokenType::Dot) {
        throw std::runtime_error("Expected '->' or '.' after virtual object name.");
    }
    advance(); // consume '->' or '.'
    CHTLJSToken prop_name = advance();
    if (prop_name.type != CHTLJSTokenType::Identifier) {
        throw std::runtime_error("Expected property name after '->'.");
    }

    auto stored_node = context_->virtual_objects.at(vir_name.lexeme);
    if (stored_node->getType() == CHTLJSNodeType::Listen) {
        auto listen_node = static_cast<ListenNode*>(stored_node.get());
        if (listen_node->getEvents().count(prop_name.lexeme)) {
            return std::make_unique<ValueNode>(listen_node->getEvents().at(prop_name.lexeme));
        }
    }
    // TODO: Add cases for other vir-compatible nodes like AnimateNode

    throw std::runtime_error("Property '" + prop_name.lexeme + "' not found on virtual object '" + vir_name.lexeme + "'.");
}


std::unique_ptr<CHTLJSNode> CHTLJSParser::parseAnimateBlock() {
    auto animate_node = std::make_unique<AnimateNode>();

    if (peek().type != CHTLJSTokenType::OpenBrace) {
        throw std::runtime_error("Expected '{' to open animate block.");
    }
    advance(); // consume '{'

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        if (peek().type != CHTLJSTokenType::Identifier) {
            throw std::runtime_error("Expected property key identifier in animate block.");
        }
        CHTLJSToken key = advance();
        if (peek().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after animate property key.");
        advance(); // consume ':'

        if (key.lexeme == "target") {
            if (peek().type == CHTLJSTokenType::OpenBracket) {
                advance(); // consume '['
                while (peek().type != CHTLJSTokenType::CloseBracket && !isAtEnd()) {
                    animate_node->targets_.push_back(parseExpression());
                    if (peek().type == CHTLJSTokenType::Comma) advance();
                }
                if (peek().type != CHTLJSTokenType::CloseBracket) throw std::runtime_error("Expected ']' to close target array.");
                advance(); // consume ']'
            } else {
                animate_node->targets_.push_back(parseExpression());
            }
        } else if (key.lexeme == "duration") {
            animate_node->duration_ = advance().lexeme;
        } else if (key.lexeme == "easing") {
            animate_node->easing_ = advance().lexeme;
        } else if (key.lexeme == "loop") {
            animate_node->loop_ = advance().lexeme;
        } else if (key.lexeme == "direction") {
            animate_node->direction_ = advance().lexeme;
        } else if (key.lexeme == "delay") {
            animate_node->delay_ = advance().lexeme;
        } else if (key.lexeme == "callback") {
            animate_node->callback_ = advance().lexeme;
        }
        // Simplified keyframe parsing for brevity

        if (peek().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }

    if (peek().type != CHTLJSTokenType::CloseBrace) {
        throw std::runtime_error("Expected '}' to close animate block.");
    }
    advance(); // consume '}'

    return animate_node;
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

std::unique_ptr<CHTLJSNode> CHTLJSParser::parseDelegateBlock(std::unique_ptr<CHTLJSNode> object) {
    auto delegate_node = std::make_unique<DelegateNode>(std::move(object));

    if (peek().type != CHTLJSTokenType::OpenBrace) {
        throw std::runtime_error("Expected '{' to open delegate block.");
    }
    advance(); // consume '{'

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        if (peek().type == CHTLJSTokenType::Identifier && peek().lexeme == "target") {
            advance(); // consume 'target'
            if (peek().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after 'target'.");
            advance(); // consume ':'

            if (peek().type == CHTLJSTokenType::OpenBracket) {
                advance(); // consume '['
                while(peek().type != CHTLJSTokenType::CloseBracket && !isAtEnd()) {
                    delegate_node->addTarget(parseExpression());
                    if (peek().type == CHTLJSTokenType::Comma) advance();
                }
                if (peek().type != CHTLJSTokenType::CloseBracket) throw std::runtime_error("Expected ']' to close target array.");
                advance(); // consume ']'
            } else {
                delegate_node->addTarget(parseExpression());
            }
        } else {
            if (peek().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected event name identifier in delegate block.");
            std::string event_name = advance().lexeme;
            if (peek().type != CHTLJSTokenType::Colon) throw std::runtime_error("Expected ':' after event name.");
            advance(); // consume ':'
            if (peek().type != CHTLJSTokenType::Identifier) throw std::runtime_error("Expected callback body after event name.");
            std::string callback_body = advance().lexeme;
            delegate_node->addEvent(event_name, callback_body);
        }

        if (peek().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }

    if (peek().type != CHTLJSTokenType::CloseBrace) {
        throw std::runtime_error("Expected '}' to close delegate block.");
    }
    advance(); // consume '}'

    return delegate_node;
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
