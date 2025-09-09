#include "CHTLParser.h"
#include <stdexcept>

CHTLParser::CHTLParser(const std::vector<Token>& tokens, const std::string& source, CHTLContext* context)
    : tokens(tokens), source(source), context(context) {}

// --- Token management methods ---
const Token& CHTLParser::peek() const { return tokens[current]; }
const Token& CHTLParser::previous() const { return tokens[current - 1]; }
bool CHTLParser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
const Token& CHTLParser::advance() { if (!isAtEnd()) current++; return previous(); }
bool CHTLParser::check(TokenType type) const { if (isAtEnd()) return false; return peek().type == type; }
bool CHTLParser::checkNext(TokenType type) const { if (isAtEnd() || tokens.size() <= current + 1) return false; return tokens[current + 1].type == type; }
bool CHTLParser::match(const std::vector<TokenType>& types) { for (TokenType type : types) { if (check(type)) { advance(); return true; } } return false; }
const Token& CHTLParser::consume(TokenType type, const std::string& message) { if (check(type)) return advance(); throw std::runtime_error(message + " at line " + std::to_string(peek().line)); }

// --- Main Parsing Logic ---
void CHTLParser::parse() {
    while (!isAtEnd()) {
        parseTopLevelStatement();
    }
}

void CHTLParser::parseTopLevelStatement() {
    if (check(TokenType::TemplateKeyword)) {
        parseTemplateDefinition();
    } else if (check(TokenType::Identifier)) {
        if (root) throw std::runtime_error("Multiple root elements defined in the document.");
        root = parseElement().release(); // The main document tree
    } else {
        throw std::runtime_error("Invalid top-level statement at line " + std::to_string(peek().line));
    }
}

void CHTLParser::parseTemplateDefinition() {
    consume(TokenType::TemplateKeyword, "Expect '[Template]'.");
    consume(TokenType::At, "Expect '@'.");
    const Token& type = consume(TokenType::Identifier, "Expect template type (e.g., 'Style').");

    if (type.lexeme != "Style") {
        throw std::runtime_error("Only '@Style' templates are supported in this version.");
    }

    const Token& name = consume(TokenType::Identifier, "Expect template name.");
    consume(TokenType::LeftBrace, "Expect '{'.");

    CHTLContext::StyleProperties properties;
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        const Token& key = consume(TokenType::Identifier, "Expect style property name.");
        consume(TokenType::Colon, "Expect ':'.");
        std::string value;
        while(!check(TokenType::Semicolon) && !isAtEnd()) {
            if (!value.empty()) value += " ";
            value += advance().lexeme;
        }
        consume(TokenType::Semicolon, "Expect ';'.");
        properties[key.lexeme] = value;
    }
    consume(TokenType::RightBrace, "Expect '}'.");

    context->addStyleTemplate(name.lexeme, properties);
}

void CHTLParser::parseBlock(ElementNode* element) {
    consume(TokenType::LeftBrace, "Expect '{' after element name.");
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (check(TokenType::Identifier) && checkNext(TokenType::Colon)) {
            parseAttribute(element);
        } else if (check(TokenType::Style)) {
            parseStyleBlock(element);
        } else if (check(TokenType::Identifier)) {
            element->addChild(parseElement());
        } else if (check(TokenType::Text)) {
            element->addChild(parseText());
        } else if (check(TokenType::StringLiteral)) {
            element->addChild(std::make_unique<TextNode>(advance().lexeme));
        } else if (check(TokenType::LeftBracket)) {
             // This is simplified, assuming any LeftBracket starts an Origin block
            element->addChild(parseOrigin());
        } else {
            throw std::runtime_error("Invalid token inside block at line " + std::to_string(peek().line));
        }
    }
    consume(TokenType::RightBrace, "Expect '}' after block.");
}

void CHTLParser::parseAttribute(ElementNode* element) {
    const Token& key = consume(TokenType::Identifier, "Expect attribute name.");
    consume(TokenType::Colon, "Expect ':' after attribute name.");
    std::string value;
    if (match({TokenType::Identifier, TokenType::UnquotedLiteral, TokenType::StringLiteral})) {
        value = previous().lexeme;
    } else { throw std::runtime_error("Unsupported attribute value type."); }
    consume(TokenType::Semicolon, "Expect ';' after attribute value.");
    element->addAttribute({key.lexeme, value});
}

void CHTLParser::parseStyleBlock(ElementNode* element) {
    consume(TokenType::Style, "Expect 'style' keyword.");
    consume(TokenType::LeftBrace, "Expect '{' after 'style'.");

    std::string styleString;
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (check(TokenType::At)) { // Template usage
            consume(TokenType::At, "Expect '@'.");
            consume(TokenType::Identifier, "Expect 'Style'."); // Consume "Style"
            const Token& templateName = consume(TokenType::Identifier, "Expect template name.");
            consume(TokenType::Semicolon, "Expect ';'.");

            if (context->hasStyleTemplate(templateName.lexeme)) {
                const auto& properties = context->getStyleTemplate(templateName.lexeme);
                for (const auto& prop : properties) {
                    styleString += prop.first + ":" + prop.second + ";";
                }
            } else {
                throw std::runtime_error("Used undefined style template: " + templateName.lexeme);
            }
        } else { // Regular style property
            const Token& key = consume(TokenType::Identifier, "Expect style property name.");
            consume(TokenType::Colon, "Expect ':' after style property name.");
            std::string value;
            while(!check(TokenType::Semicolon) && !isAtEnd()) {
                if (!value.empty()) value += " ";
                value += advance().lexeme;
            }
            consume(TokenType::Semicolon, "Expect ';' after style value.");
            styleString += key.lexeme + ":" + value + ";";
        }
    }

    consume(TokenType::RightBrace, "Expect '}' after style block.");
    element->addAttribute({"style", styleString});
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    const Token& nameToken = consume(TokenType::Identifier, "Expect element name.");
    auto element = std::make_unique<ElementNode>(nameToken.lexeme);
    parseBlock(element.get());
    return element;
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    consume(TokenType::Text, "Expect 'text' keyword.");
    consume(TokenType::LeftBrace, "Expect '{' after 'text'.");
    std::string textContent;
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (match({TokenType::Identifier, TokenType::UnquotedLiteral, TokenType::StringLiteral})) {
            if (!textContent.empty()) textContent += " ";
            textContent += previous().lexeme;
        } else { break; }
    }
    consume(TokenType::RightBrace, "Expect '}' after text block.");
    return std::make_unique<TextNode>(textContent);
}

std::unique_ptr<OriginNode> CHTLParser::parseOrigin() {
    consume(TokenType::LeftBracket, "Expect '['.");
    // This is a simplified check. A real implementation would check for "Origin" identifier.
    advance(); // Consume "Origin"
    consume(TokenType::RightBracket, "Expect ']'.");
    consume(TokenType::At, "Expect '@'.");
    const Token& typeToken = consume(TokenType::Identifier, "Expect origin type.");
    consume(TokenType::LeftBrace, "Expect '{'.");
    if (isAtEnd()) throw std::runtime_error("Unterminated [Origin] block.");
    size_t contentStartOffset = peek().offset;
    int braceDepth = 1;
    while (braceDepth > 0 && !isAtEnd()) {
        if (peek().type == TokenType::LeftBrace) braceDepth++;
        else if (peek().type == TokenType::RightBrace) braceDepth--;
        if (braceDepth > 0) advance();
    }
    if (braceDepth > 0) throw std::runtime_error("Unterminated [Origin] block.");
    size_t contentEndOffset = peek().offset;
    std::string content = source.substr(contentStartOffset, contentEndOffset - contentStartOffset);
    consume(TokenType::RightBrace, "Expect '}' after origin content.");
    return std::make_unique<OriginNode>(typeToken.lexeme, content);
}
