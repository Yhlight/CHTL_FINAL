#include "CHTLParser.h"
#include <iostream>
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::shared_ptr<ElementNode> CHTLParser::parse() {
    auto root = std::make_shared<ElementNode>("root");
    while (!isAtEnd()) {
        root->addChild(declaration(root));
    }
    return root;
}

std::shared_ptr<BaseNode> CHTLParser::declaration(std::shared_ptr<ElementNode> rootNode) {
    if (check(TokenType::KEYWORD_TEXT)) {
        // Lookahead to see if it's text: "..." or text { ... }
        if (tokens[current + 1].type == TokenType::LEFT_BRACE) {
            return textElement();
        }
    }
    // Default to parsing an element
    return element(rootNode);
}

std::shared_ptr<ElementNode> CHTLParser::element(std::shared_ptr<ElementNode> rootNode) {
    Token identifier = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto node = std::make_shared<ElementNode>(identifier.lexeme);
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token currentToken = peek();

        if (currentToken.type == TokenType::KEYWORD_STYLE) {
            parseStyleBlock(node, rootNode);
        } else if (currentToken.type == TokenType::KEYWORD_TEXT) {
             advance(); // consume 'text'
             consume(TokenType::COLON, "Expect ':' after 'text' attribute.");
             Token value = consume(TokenType::STRING, "Expect string value for 'text' attribute.");
             consume(TokenType::SEMICOLON, "Expect ';' after text value.");
             node->addChild(std::make_shared<TextNode>(value.lexeme));

        } else if (currentToken.type == TokenType::IDENTIFIER) {
            if (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL) {
                Token key = advance();
                match({TokenType::COLON, TokenType::EQUAL});
                Token value = consume(TokenType::STRING, "Expect attribute value.");
                consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
                node->addAttribute(std::make_shared<AttributeNode>(key.lexeme, value.lexeme));
            } else {
                node->addChild(element(rootNode));
            }
        } else {
            throw error(peek(), "Unexpected token inside element block.");
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return node;
}

std::shared_ptr<TextNode> CHTLParser::textElement() {
    consume(TokenType::KEYWORD_TEXT, "Expect 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
    Token content = consume(TokenType::STRING, "Expect string literal inside text block.");
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return std::make_shared<TextNode>(content.lexeme);
}

void CHTLParser::parseStyleBlock(std::shared_ptr<ElementNode> currentNode, std::shared_ptr<ElementNode> rootNode) {
    consume(TokenType::KEYWORD_STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::AMPERSAND) {
            advance(); // consume '&'

            // Find parent class or id
            std::string parent_selector;
            for(const auto& attr : currentNode->attributes) {
                if (attr->key == "class") {
                    parent_selector = "." + attr->value;
                    break;
                }
            }
            if (parent_selector.empty()) {
                for(const auto& attr : currentNode->attributes) {
                    if (attr->key == "id") {
                        parent_selector = "#" + attr->value;
                        break;
                    }
                }
            }

            if (parent_selector.empty()) {
                throw error(previous(), "Found '&' but parent element has no class or id.");
            }

            // Consume the rest of the selector (e.g., :hover)
            std::stringstream pseudo_selector;
            while(!check(TokenType::LEFT_BRACE) && !isAtEnd()) {
                pseudo_selector << advance().lexeme;
            }

            std::string full_selector = parent_selector + pseudo_selector.str();
            std::string css_rules = parseCssRuleBlock();
            rootNode->global_styles[full_selector] += css_rules;

        } else if (peek().type == TokenType::CLASS_SELECTOR || peek().type == TokenType::ID_SELECTOR) {
            Token selector = advance();

            if (selector.type == TokenType::CLASS_SELECTOR) {
                currentNode->addAttribute(std::make_shared<AttributeNode>("class", selector.lexeme.substr(1)));
            } else {
                currentNode->addAttribute(std::make_shared<AttributeNode>("id", selector.lexeme.substr(1)));
            }

            std::string css_rules = parseCssRuleBlock();
            rootNode->global_styles[selector.lexeme] += css_rules;

        } else if (peek().type == TokenType::IDENTIFIER) {
            Token key = consume(TokenType::IDENTIFIER, "Expect property name.");
            consume(TokenType::COLON, "Expect ':' after property name.");
            std::stringstream value_ss;
            while(!check(TokenType::SEMICOLON) && !isAtEnd()) {
                if (!value_ss.str().empty()) value_ss << " ";
                value_ss << advance().lexeme;
            }
            consume(TokenType::SEMICOLON, "Expect ';' after property value.");
            currentNode->styles[key.lexeme] = value_ss.str();
        } else {
            throw error(peek(), "Unexpected token in style block.");
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
}

std::string CHTLParser::parseCssRuleBlock() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after selector.");
    std::stringstream css_rules;
    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token key = consume(TokenType::IDENTIFIER, "Expect property name.");
        consume(TokenType::COLON, "Expect ':' after property name.");
        std::stringstream value_ss;
        while(!check(TokenType::SEMICOLON) && !isAtEnd()) {
            if (!value_ss.str().empty()) value_ss << " ";
            value_ss << advance().lexeme;
        }
        consume(TokenType::SEMICOLON, "Expect ';' after property value.");
        css_rules << "  " << key.lexeme << ": " << value_ss.str() << ";\n";
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after rule block.");
    return css_rules.str();
}


// --- Utility and Error Handling Methods ---

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLParser::peek() {
    return tokens[current];
}

Token CHTLParser::previous() {
    return tokens[current - 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

CHTLParser::ParseError CHTLParser::error(const Token& token, const std::string& message) {
    std::cerr << "Parse Error at line " << token.line << " near '" << token.lexeme << "': " << message << std::endl;
    return ParseError(message);
}

} // namespace CHTL
