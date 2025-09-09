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
    if (check(TokenType::LeftBracket)) {
        if (tokens.size() > current + 2 && tokens[current + 1].lexeme == "Template" && tokens[current + 2].type == TokenType::RightBracket) {
            parseTemplateDefinition();
            return;
        }
    }

    if (check(TokenType::Identifier)) {
        if (root) throw std::runtime_error("Multiple root elements defined in the document.");
        root = parseElement().release();
    } else {
        if (!isAtEnd()) advance();
    }
}

void CHTLParser::parseTemplateDefinition() {
    consume(TokenType::LeftBracket, "Expect '['.");
    consume(TokenType::Identifier, "Expect 'Template'.");
    consume(TokenType::RightBracket, "Expect ']'.");
    consume(TokenType::At, "Expect '@'.");
    const Token& type = consume(TokenType::Identifier, "Expect template type.");
    const Token& name = consume(TokenType::Identifier, "Expect template name.");
    consume(TokenType::LeftBrace, "Expect '{'.");

    if (type.lexeme == "Style") {
        CHTLContext::StyleProperties properties;
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            const Token& key = consume(TokenType::Identifier, "Expect style property name.");
            consume(TokenType::Colon, "Expect ':'.");
            std::string value;
            while(!check(TokenType::Semicolon) && !isAtEnd()) { if (!value.empty()) value += " "; value += parseValue(); }
            properties[key.lexeme] = value;
            consume(TokenType::Semicolon, "Expect ';'.");
        }
        context->addStyleTemplate(name.lexeme, properties);
    } else if (type.lexeme == "Element") {
        auto tempHolder = std::make_unique<ElementNode>("__template__");
        parseBlock(tempHolder.get());
        context->addElementTemplate(name.lexeme, tempHolder->releaseChildren());
    } else if (type.lexeme == "Var") {
        CHTLContext::VarGroup vars;
        while (!check(TokenType::RightBrace) && !isAtEnd()) {
            const Token& key = consume(TokenType::Identifier, "Expect variable name.");
            consume(TokenType::Colon, "Expect ':'.");
            vars[key.lexeme] = parseValue();
            consume(TokenType::Semicolon, "Expect ';'.");
        }
        context->addVarTemplate(name.lexeme, vars);
    }
    else { throw std::runtime_error("Unsupported template type: @" + type.lexeme); }
    consume(TokenType::RightBrace, "Expect '}'.");
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
            element->addChild(parseOrigin());
        } else if (check(TokenType::At)) {
            parseElementTemplateUsage(element);
        } else {
            throw std::runtime_error("Invalid token inside block at line " + std::to_string(peek().line));
        }
    }
    consume(TokenType::RightBrace, "Expect '}' after block.");
}

void CHTLParser::parseElementTemplateUsage(ElementNode* parent) {
    consume(TokenType::At, "Expect '@'.");
    const Token& type = consume(TokenType::Identifier, "Expect 'Element'.");
    if (type.lexeme != "Element") throw std::runtime_error("Expected '@Element' usage.");
    const Token& name = consume(TokenType::Identifier, "Expect template name.");
    consume(TokenType::Semicolon, "Expect ';' after template usage.");
    if (context->hasElementTemplate(name.lexeme)) {
        const auto& templateNodes = context->getElementTemplate(name.lexeme);
        for (const auto& node : templateNodes) parent->addChild(node->clone());
    } else { throw std::runtime_error("Used undefined element template: " + name.lexeme); }
}

void CHTLParser::parseAttribute(ElementNode* element) {
    const Token& key = consume(TokenType::Identifier, "Expect attribute name.");
    consume(TokenType::Colon, "Expect ':' after attribute name.");
    std::string value = parseValue();
    consume(TokenType::Semicolon, "Expect ';' after attribute value.");
    element->addAttribute({key.lexeme, value});
}

void CHTLParser::parseStyleBlock(ElementNode* element) {
    consume(TokenType::Style, "Expect 'style' keyword.");
    consume(TokenType::LeftBrace, "Expect '{' after 'style'.");
    std::string styleString;
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (check(TokenType::At)) {
            // ... template usage ...
        } else if (check(TokenType::Dot) || check(TokenType::Hash)) {
            parseCSSRule(element);
        } else {
            const Token& key = consume(TokenType::Identifier, "Expect style property name.");
            consume(TokenType::Colon, "Expect ':' after style property name.");
            std::string value;
            while(!check(TokenType::Semicolon) && !isAtEnd()) { if (!value.empty()) value += " "; value += parseValue(); }
            consume(TokenType::Semicolon, "Expect ';' after style value.");
            styleString += key.lexeme + ":" + value + ";";
        }
    }
    consume(TokenType::RightBrace, "Expect '}' after style block.");
    if (!styleString.empty()) {
        element->addAttribute({"style", styleString});
    }
}

void CHTLParser::parseCSSRule(ElementNode* parent) {
    std::string selector;
    if (check(TokenType::Dot)) {
        selector += ".";
        consume(TokenType::Dot, "Expect '.'");
        const Token& className = consume(TokenType::Identifier, "Expect class name.");
        selector += className.lexeme;
        parent->addAttribute({"class", className.lexeme});
    } else if (check(TokenType::Hash)) {
        selector += "#";
        consume(TokenType::Hash, "Expect '#'.");
        const Token& idName = consume(TokenType::Identifier, "Expect id name.");
        selector += idName.lexeme;
        parent->addAttribute({"id", idName.lexeme});
    } else { return; }

    consume(TokenType::LeftBrace, "Expect '{' after selector.");
    std::string properties;
    while(!check(TokenType::RightBrace) && !isAtEnd()) {
        const Token& key = consume(TokenType::Identifier, "Expect property name.");
        consume(TokenType::Colon, "Expect ':'.");
        std::string value;
        while(!check(TokenType::Semicolon) && !isAtEnd()) {
            if (!value.empty()) value += " ";
            value += parseValue();
        }
        consume(TokenType::Semicolon, "Expect ';'.");
        properties += "  " + key.lexeme + ": " + value + ";\n";
    }
    consume(TokenType::RightBrace, "Expect '}'.");
    context->addGlobalCSS(selector + " {\n" + properties + "}\n");
}

std::string CHTLParser::parseValue() {
    if (check(TokenType::Identifier) && checkNext(TokenType::LeftParen)) {
        const Token& groupName = consume(TokenType::Identifier, "Expect template group name.");
        consume(TokenType::LeftParen, "Expect '('.");
        const Token& varName = consume(TokenType::Identifier, "Expect variable name.");
        consume(TokenType::RightParen, "Expect ')'.");
        return context->getVarTemplateValue(groupName.lexeme, varName.lexeme);
    }
    if (match({TokenType::Identifier, TokenType::UnquotedLiteral, TokenType::StringLiteral})) {
        return previous().lexeme;
    }
    throw std::runtime_error("Expected a value (literal or variable template).");
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
    consume(TokenType::Identifier, "Expect 'Origin'.");
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
    std::string content = source.substr(contentStartOffset, contentEndOffset - contentEndOffset);
    consume(TokenType::RightBrace, "Expect '}' after origin content.");
    return std::make_unique<OriginNode>(typeToken.lexeme, content);
}
