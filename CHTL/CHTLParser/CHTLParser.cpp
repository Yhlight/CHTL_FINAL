#include "CHTLParser.h"
#include <iostream>

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ProgramNode> CHTLParser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) { // Check for null pointers from failed parses
            program->children.push_back(std::move(stmt));
        }
    }
    return program;
}

std::unique_ptr<Node> CHTLParser::parseStatement() {
    // Dispatch based on the current token type
    if (peek().type == TokenType::TEXT) {
        return parseText();
    }
    if (peek().type == TokenType::STYLE) {
        return parseStyle();
    }
    if (peek().type == TokenType::IDENTIFIER) {
        // Could be an element or something else in the future. For now, assume element.
        return parseElement();
    }

    // If we don't recognize the statement, report an error and advance.
    if (!isAtEnd()) {
        std::cerr << "Parse Error at line " << peek().line << ": Unexpected token '" << peek().lexeme << "' at top level." << std::endl;
        advance();
    }
    return nullptr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    const Token& nameToken = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(nameToken.lexeme);

    consume(TokenType::LBRACE, "Expect '{' after element name.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        const Token& currentToken = peek();
        // Safe check for nextToken to avoid reading past the end of the token vector
        const Token& nextToken = (current + 1 < tokens.size()) ? tokens[current + 1] : currentToken;

        // Attribute: id: "value";
        if (currentToken.type == TokenType::IDENTIFIER && nextToken.type == TokenType::COLON) {
            element->children.push_back(parseAttribute());
        }
        // Style block: style { ... }
        else if (currentToken.type == TokenType::STYLE) {
            element->children.push_back(parseStyle());
        }
        // Text block: text { ... }
        else if (currentToken.type == TokenType::TEXT) {
            element->children.push_back(parseText());
        }
        // Nested element: div { ... }
        else if (currentToken.type == TokenType::IDENTIFIER) {
            element->children.push_back(parseElement());
        }
        else {
            // Error case
            consume(TokenType::IDENTIFIER, "Expect identifier, 'style', or 'text' inside element body.");
            break; // consume() exits, but this is for clarity
        }
    }

    consume(TokenType::RBRACE, "Expect '}' after element body.");
    return element;
}

std::unique_ptr<AttributeNode> CHTLParser::parseAttribute() {
    const Token& key = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    consume(TokenType::COLON, "Expect ':' after attribute name.");
    auto value = parseValue();
    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    return std::make_unique<AttributeNode>(key.lexeme, std::move(value));
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    consume(TokenType::TEXT, "Expect 'text' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'text' keyword.");
    auto value = parseValue();
    consume(TokenType::RBRACE, "Expect '}' after text content.");
    return std::make_unique<TextNode>(std::move(value));
}

std::unique_ptr<StyleNode> CHTLParser::parseStyle() {
    consume(TokenType::STYLE, "Expect 'style' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'style' keyword.");
    auto styleNode = std::make_unique<StyleNode>();
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        const Token& currentToken = peek();
        const Token& nextToken = (current + 1 < tokens.size()) ? tokens[current + 1] : currentToken;

        // Check if it's an inline property (e.g., color: red;)
        if (currentToken.type == TokenType::IDENTIFIER && nextToken.type == TokenType::COLON) {
            styleNode->children.push_back(parseAttribute());
        }
        // Otherwise, assume it's a selector block (e.g., .box { ... })
        else {
            styleNode->children.push_back(parseSelector());
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after style block.");
    return styleNode;
}

std::unique_ptr<SelectorNode> CHTLParser::parseSelector() {
    std::string selector_text;
    // Consume tokens until we hit a left brace, concatenating them to form the selector.
    // For now, we'll do a simple concatenation. This works for simple selectors
    // like `.box` and `&:hover`, but not for descendant selectors like `.box button`.
    // A more advanced implementation would intelligently add spaces.
    while (!check(TokenType::LBRACE) && !isAtEnd()) {
        selector_text += advance().lexeme;
    }

    auto selectorNode = std::make_unique<SelectorNode>(selector_text);

    consume(TokenType::LBRACE, "Expect '{' after selector.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        selectorNode->properties.push_back(parseAttribute());
    }

    consume(TokenType::RBRACE, "Expect '}' after selector property block.");

    return selectorNode;
}


std::unique_ptr<ValueNode> CHTLParser::parseValue() {
    std::string value_text;
    const Token& first_token = peek();

    // Consume tokens until we hit a semicolon or a closing brace.
    // This allows for multi-token values like `1px solid black`.
    if (first_token.type == TokenType::STRING_LITERAL) {
        // If it's a string literal, consume only that one token.
        value_text = advance().lexeme;
    } else {
        while (!check(TokenType::SEMICOLON) && !check(TokenType::RBRACE) && !isAtEnd()) {
            if (!value_text.empty()) {
                value_text += " ";
            }
            value_text += advance().lexeme;
        }
    }

    if (value_text.empty()) {
        std::cerr << "Error: Expected a value at line "
                  << peek().line << ", column " << peek().column << std::endl;
        // Return a dummy value to prevent crashing
        return std::make_unique<ValueNode>(peek());
    }

    // Create a new synthetic token for the whole value
    Token value_token;
    // Preserve the original token type if it was a string
    value_token.type = (first_token.type == TokenType::STRING_LITERAL) ? TokenType::STRING_LITERAL : TokenType::UNQUOTED_LITERAL;
    value_token.lexeme = value_text;
    value_token.line = first_token.line;
    value_token.column = first_token.column;

    return std::make_unique<ValueNode>(value_token);
}


// --- Helper Methods ---

const Token& CHTLParser::peek() {
    return tokens[current];
}

const Token& CHTLParser::previous() {
    return tokens[current - 1];
}

const Token& CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
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

const Token& CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    // In a real parser, we would throw a specific parse error exception here.
    std::cerr << "Parse Error at line " << peek().line << ", column " << peek().column << ": " << message << std::endl;
    std::cerr << "  Got " << peek().lexeme << " (" << (int)peek().type << ") instead." << std::endl;
    // This is a simple way to stop, but a real parser would synchronize.
    exit(1);
}
