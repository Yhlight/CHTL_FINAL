#include "Parser.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLNode/DeclarationNode.h"
#include <iostream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens, CHTLContext& context) : tokens(tokens), context(context) {}

// --- Main Parsing Method ---
std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> topLevelNodes;
    while (!isAtEnd()) {
        if (peek().type == TokenType::KEYWORD_TEMPLATE) {
            topLevelNodes.push_back(parseTemplateDefinition());
        } else if (peek().type == TokenType::IDENTIFIER) {
            topLevelNodes.push_back(parseElement());
        } else {
            // Skip unknown top-level tokens for now
            advance();
        }
    }
    return topLevelNodes;
}

// --- Grammar Parsing Methods ---

std::unique_ptr<ElementNode> Parser::parseElement() {
    Token tagToken = advance(); // Consume the tag name identifier
    auto element = std::make_unique<ElementNode>(tagToken.lexeme);

    if (!match({TokenType::LEFT_BRACE})) {
        throw std::runtime_error("Expected '{' after element name '" + tagToken.lexeme + "' on line " + std::to_string(tagToken.line));
    }

    // Parse attributes and children
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::TEXT && peekNext().type == TokenType::LEFT_BRACE) {
            advance(); // consume 'text'
            element->addChild(parseTextNode());
        } else if (peek().type == TokenType::TEXT && (peekNext().type == TokenType::COLON || peekNext().type == TokenType::EQUAL)) {
            advance(); // consume 'text'
            advance(); // consume ':' or '='
            std::string value = parseValue();
            element->addChild(std::make_unique<TextNode>(value));
            if (!match({TokenType::SEMICOLON})) {
                throw std::runtime_error("Expected ';' after text attribute value on line " + std::to_string(previous().line));
            }
        } else if (peek().type == TokenType::IDENTIFIER && peekNext().type == TokenType::LEFT_BRACE) {
            element->addChild(parseElement());
        } else if (peek().type == TokenType::STYLE && peekNext().type == TokenType::LEFT_BRACE) {
            advance(); // consume 'style'
            element->addChild(parseStyleNode(element.get()));
        } else if (peek().type == TokenType::IDENTIFIER && (peekNext().type == TokenType::COLON || peekNext().type == TokenType::EQUAL)) {
            parseAttribute(element.get());
        } else if (peek().type == TokenType::AT) {
            element->addChild(parseTemplateUsage());
        } else if (peek().type == TokenType::GEN_COMMENT) {
            element->addChild(parseCommentNode());
        } else {
            // Should not happen in well-formed CHTL
            advance();
        }
    }

    if (!match({TokenType::RIGHT_BRACE})) {
        throw std::runtime_error("Expected '}' to close element block on line " + std::to_string(peek().line));
    }

    return element;
}

void Parser::parseAttribute(ElementNode* element) {
    Token key = advance(); // Consume attribute name
    if (!match({TokenType::COLON, TokenType::EQUAL})) {
        throw std::runtime_error("Expected ':' or '=' after attribute name '" + key.lexeme + "' on line " + std::to_string(key.line));
    }

    std::string value = parseValue();
    element->addAttribute(key.lexeme, value);

    // Attributes must end with a semicolon
    if (!match({TokenType::SEMICOLON})) {
        throw std::runtime_error("Expected ';' after attribute value for '" + key.lexeme + "' on line " + std::to_string(key.line));
    }
}

std::string Parser::parseValue() {
    if (peek().type == TokenType::STRING) {
        std::string lexeme = advance().lexeme;
        // Strip the surrounding quotes
        if (lexeme.length() >= 2) {
            return lexeme.substr(1, lexeme.length() - 2);
        }
        return ""; // Should not happen
    }

    // It's an unquoted literal. We concatenate identifiers until we hit a semicolon.
    std::string value = "";
    while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
        if (!value.empty()) {
            value += " ";
        }
        value += advance().lexeme;
    }
    return value;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    if (!match({TokenType::LEFT_BRACE})) {
        throw std::runtime_error("Expected '{' for text block on line " + std::to_string(peek().line));
    }

    // Similar to unquoted literals, we gather all tokens until the closing brace.
    std::string textContent = "";
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
         if (!textContent.empty()) {
            textContent += " ";
        }
        textContent += advance().lexeme;
    }

    if (!match({TokenType::RIGHT_BRACE})) {
        throw std::runtime_error("Expected '}' to close text block on line " + std::to_string(peek().line));
    }

    return std::make_unique<TextNode>(textContent);
}

std::unique_ptr<CommentNode> Parser::parseCommentNode() {
    Token startToken = advance(); // consume '--'

    std::string commentContent = "";
    // Consume tokens on the same line as the comment start
    while (peek().line == startToken.line && !isAtEnd()) {
        if (!commentContent.empty()) {
            commentContent += " ";
        }
        commentContent += advance().lexeme;
    }

    return std::make_unique<CommentNode>(commentContent);
}

std::unique_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    advance(); // consume '@'
    Token typeToken = advance(); // e.g., 'Style', 'Element'
    Token nameToken = advance(); // e.g., 'DefaultText'

    TemplateType type;
    if (typeToken.lexeme == "Style") type = TemplateType::STYLE;
    else if (typeToken.lexeme == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.lexeme == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeToken.lexeme);

    auto usageNode = std::make_unique<TemplateUsageNode>(type, nameToken.lexeme);

    // TODO: Parse specializations like { delete ... }

    match({TokenType::SEMICOLON});
    return usageNode;
}

std::unique_ptr<TemplateNode> Parser::parseTemplateDefinition() {
    advance(); // consume '[Template]'
    if (!match({TokenType::AT})) throw std::runtime_error("Expected '@' after [Template]");

    Token typeToken = advance();
    Token nameToken = advance();

    TemplateType type;
    if (typeToken.lexeme == "Style") type = TemplateType::STYLE;
    else if (typeToken.lexeme == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.lexeme == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeToken.lexeme);

    auto templateNode = std::make_unique<TemplateNode>(type, nameToken.lexeme);

    if (!match({TokenType::LEFT_BRACE})) throw std::runtime_error("Expected '{' for template definition");

    // The body parsing depends on the template type.
    if (type == TemplateType::ELEMENT) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            templateNode->addChild(parseElement());
        }
    } else if (type == TemplateType::STYLE) {
        // A style template contains declarations and rules, just like a style block.
        // We can't call parseStyleNode because that expects a `{` which we already consumed.
        // So we replicate the inner logic of parseStyleNode here.
        // This is a simplification. A better refactoring would be to have a
        // `parseStyleBody` method that both can call.
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            // Simplified: for now, only parsing declarations in style templates
            auto declNode = std::make_unique<DeclarationNode>(advance().lexeme); // property
            match({TokenType::COLON});
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                declNode->valueTokens.push_back(advance());
            }
            match({TokenType::SEMICOLON});
            templateNode->addChild(std::move(declNode));
        }
    } else if (type == TemplateType::VAR) {
        // Var templates contain key-value pairs, which are like declarations.
         while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto declNode = std::make_unique<DeclarationNode>(advance().lexeme); // var name
            match({TokenType::COLON});
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                declNode->valueTokens.push_back(advance());
            }
            match({TokenType::SEMICOLON});
            templateNode->addChild(std::move(declNode));
        }
    }

    if (!match({TokenType::RIGHT_BRACE})) throw std::runtime_error("Expected '}' to close template definition");

    // Add to context
    if (type == TemplateType::STYLE) context.styleTemplates[templateNode->name] = templateNode.get();
    else if (type == TemplateType::ELEMENT) context.elementTemplates[templateNode->name] = templateNode.get();
    else if (type == TemplateType::VAR) context.varTemplates[templateNode->name] = templateNode.get();

    return templateNode;
}

std::unique_ptr<StyleNode> Parser::parseStyleNode(ElementNode* parent) {
    auto styleNode = std::make_unique<StyleNode>();
    if (!match({TokenType::LEFT_BRACE})) {
        throw std::runtime_error("Expected '{' for style block on line " + std::to_string(peek().line));
    }

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Look ahead to see if it's a rule or a declaration.
        int bracePos = -1;
        int semiPos = -1;
        for (int i = current; !check(TokenType::RIGHT_BRACE) && i < tokens.size(); ++i) {
            if (tokens[i].type == TokenType::LEFT_BRACE) { bracePos = i; break; }
            if (tokens[i].type == TokenType::SEMICOLON) { semiPos = i; break; }
        }

        if (bracePos != -1 && (semiPos == -1 || bracePos < semiPos)) {
            // It's a RuleNode
            auto ruleNode = std::make_unique<RuleNode>();

            // --- Auto-attribute logic ---
            if (peek().type == TokenType::DOT) {
                std::string className = tokens[current + 1].lexeme;
                if (parent->attributes.find("class") == parent->attributes.end()) {
                    parent->addAttribute("class", className);
                } else {
                    parent->attributes["class"] += " " + className;
                }
            } else if (peek().type == TokenType::HASH) {
                std::string idName = tokens[current + 1].lexeme;
                parent->addAttribute("id", idName); // Overwrites if exists
            }

            while (!check(TokenType::LEFT_BRACE) && !isAtEnd()) {
                ruleNode->selectorTokens.push_back(advance());
            }
            match({TokenType::LEFT_BRACE});
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto declNode = std::make_unique<DeclarationNode>(advance().lexeme); // property
                match({TokenType::COLON});
                while (!check(TokenType::SEMICOLON) && !check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                    declNode->valueTokens.push_back(advance());
                }
                match({TokenType::SEMICOLON});
                ruleNode->addDeclaration(std::move(declNode));
            }
            match({TokenType::RIGHT_BRACE});
            styleNode->addChild(std::move(ruleNode));
        } else if (peek().type == TokenType::AT) {
            styleNode->addChild(parseTemplateUsage());
        } else {
            // It's a DeclarationNode
            auto declNode = std::make_unique<DeclarationNode>(advance().lexeme); // property
            match({TokenType::COLON});
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                declNode->valueTokens.push_back(advance());
            }
            match({TokenType::SEMICOLON});
            styleNode->addChild(std::move(declNode));
        }
    }

    if (!match({TokenType::RIGHT_BRACE})) {
        throw std::runtime_error("Expected '}' to close style block on line " + std::to_string(peek().line));
    }

    return styleNode;
}


// --- Parser Helper Methods ---

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::peekNext() {
    if (current + 1 >= tokens.size()) {
        return tokens.back(); // Return EOF token
    }
    return tokens[current + 1];
}

Token Parser::previous() {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}
