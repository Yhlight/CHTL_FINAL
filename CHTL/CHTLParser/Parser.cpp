#include "Parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        try {
            program->children.push_back(declaration());
        } catch (const ParseError& e) {
            fprintf(stderr, "%s\n", e.what());
            synchronize();
        }
    }
    return program;
}

// --- Main Grammar Rule Implementations ---
std::unique_ptr<BaseNode> Parser::declaration() {
    if (peek().type == TokenType::LEFT_BRACKET) return templateDeclaration();
    if (peek().type == TokenType::COMMENT) return commentDeclaration();
    if (peek().type == TokenType::TEXT) return textDeclaration();
    if (peek().type == TokenType::IDENTIFIER || peek().type == TokenType::STYLE) return elementDeclaration();
    throw error(peek(), "Expect a declaration (element, text, comment, etc.).");
}

std::unique_ptr<ElementNode> Parser::elementDeclaration() {
    Token tagName = advance();
    auto element = std::make_unique<ElementNode>(tagName);
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::STYLE) {
            if (element->styleBlock) throw error(peek(), "An element can only have one style block.");
            element->styleBlock = styleBlockDeclaration();
        } else if (peek().type == TokenType::IDENTIFIER && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL)) {
            element->attributes.push_back(attributeDeclaration());
        } else if (peek().type == TokenType::AT) {
            element->children.push_back(useElementDeclaration());
        } else {
            element->children.push_back(declaration());
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return element;
}

std::unique_ptr<AttributeNode> Parser::attributeDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    if (!match({TokenType::COLON, TokenType::EQUAL})) throw error(peek(), "Expect ':' or '=' after attribute name.");

    // This part is now problematic as it uses a ValueNode.
    // For now, we'll just parse an unquoted literal.
    std::vector<Token> valueTokens;
    while (!check(TokenType::SEMICOLON) && !isAtEnd()) valueTokens.push_back(advance());
    auto val = std::make_unique<UnquotedLiteralNode>(valueTokens);
    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    return std::make_unique<AttributeNode>(name, std::move(val));
}

std::unique_ptr<TextNode> Parser::textDeclaration() {
    consume(TokenType::TEXT, "Expect 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
    std::vector<Token> valueTokens;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) valueTokens.push_back(advance());
    auto val = std::make_unique<UnquotedLiteralNode>(valueTokens);
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return std::make_unique<TextNode>(std::move(val));
}

std::unique_ptr<CommentNode> Parser::commentDeclaration() {
    return std::make_unique<CommentNode>(consume(TokenType::COMMENT, "Expect comment token."));
}

// --- Expression Parser (Pratt Parser) ---

int Parser::getPrecedence(TokenType type) {
    switch (type) {
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 1;
        case TokenType::STAR:
        case TokenType::SLASH:
            return 2;
        default:
            return 0;
    }
}

std::unique_ptr<ExpressionNode> Parser::expression() {
    return parsePrecedence(0);
}

std::unique_ptr<ExpressionNode> Parser::parsePrecedence(int precedence) {
    // Prefix parsing
    Token token = advance();
    std::unique_ptr<ExpressionNode> left;
    if (token.type == TokenType::NUMBER) {
        Token unit = {TokenType::IDENTIFIER, "", token.line, token.column};
        if (peek().type == TokenType::IDENTIFIER) {
            unit = advance(); // Consume unit like 'px'
        }
        left = std::make_unique<DimensionNode>(token, unit);
    } else {
        throw error(token, "Expect an expression.");
    }

    // Infix parsing
    while (precedence < getPrecedence(peek().type)) {
        Token op = advance();
        auto right = parsePrecedence(getPrecedence(op.type));
        left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
    }

    return left;
}


// --- Style-related parsers ---

std::unique_ptr<StyleBlockNode> Parser::styleBlockDeclaration() {
    consume(TokenType::STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");
    auto styleBlock = std::make_unique<StyleBlockNode>();
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        styleBlock->contents.push_back(styleContentDeclaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return styleBlock;
}

std::unique_ptr<StyleContentNode> Parser::styleContentDeclaration() {
    if (peek().type == TokenType::AT) return useStyleDeclaration();
    size_t lookahead_pos = current;
    while (lookahead_pos < tokens.size() && tokens[lookahead_pos].type != TokenType::SEMICOLON && tokens[lookahead_pos].type != TokenType::LEFT_BRACE) {
        lookahead_pos++;
    }
    if (lookahead_pos < tokens.size() && tokens[lookahead_pos].type == TokenType::LEFT_BRACE) {
        return styleRuleDeclaration();
    }
    return stylePropertyDeclaration();
}

std::unique_ptr<StylePropertyNode> Parser::stylePropertyDeclaration() {
    if (peek().type != TokenType::IDENTIFIER) throw error(peek(), "Expect style property name.");
    Token startToken = peek();
    std::string combinedName = advance().lexeme;
    while (match({TokenType::MINUS})) {
        combinedName += "-";
        combinedName += consume(TokenType::IDENTIFIER, "Expect identifier part after '-'.").lexeme;
    }
    Token nameToken = {TokenType::IDENTIFIER, combinedName, startToken.line, startToken.column};
    consume(TokenType::COLON, "Expect ':' after style property name.");
    auto val = expression(); // Use the new expression parser
    consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
    return std::make_unique<StylePropertyNode>(nameToken, std::move(val));
}

std::unique_ptr<StyleRuleNode> Parser::styleRuleDeclaration() {
    std::vector<Token> selectorTokens;
    while (!check(TokenType::LEFT_BRACE) && !isAtEnd()) selectorTokens.push_back(advance());
    if (selectorTokens.empty()) throw error(peek(), "Expect a selector for style rule.");
    auto rule = std::make_unique<StyleRuleNode>(selectorTokens);
    consume(TokenType::LEFT_BRACE, "Expect '{' after style rule selector.");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        rule->properties.push_back(stylePropertyDeclaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after style rule block.");
    return rule;
}

// --- Template-related parsers ---
std::unique_ptr<BaseNode> Parser::templateDeclaration() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start template declaration.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expect 'Template' or 'Custom' etc.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']' after template keyword.");
    if (keyword.lexeme != "Template") throw error(keyword, "Only [Template] is supported currently.");
    consume(TokenType::AT, "Expect '@' for template type.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type (e.g., 'Style', 'Element').");
    if (type.lexeme == "Style") return templateStyleDeclaration();
    else if (type.lexeme == "Element") return templateElementDeclaration();
    throw error(type, "Unknown or unsupported template type.");
}

std::unique_ptr<TemplateStyleNode> Parser::templateStyleDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    auto templateNode = std::make_unique<TemplateStyleNode>();
    templateNode->name = name;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start template body.");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        templateNode->properties.push_back(stylePropertyDeclaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end template body.");
    return templateNode;
}

std::unique_ptr<TemplateElementNode> Parser::templateElementDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    auto templateNode = std::make_unique<TemplateElementNode>();
    templateNode->name = name;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start template body.");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        templateNode->children.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end template body.");
    return templateNode;
}

std::unique_ptr<UseStyleNode> Parser::useStyleDeclaration() {
    consume(TokenType::AT, "Expect '@' to use a template.");
    consume(TokenType::IDENTIFIER, "Expect 'Style' keyword for @Style usage.");
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
    return std::make_unique<UseStyleNode>(name);
}

std::unique_ptr<UseElementNode> Parser::useElementDeclaration() {
    consume(TokenType::AT, "Expect '@' to use a template.");
    consume(TokenType::IDENTIFIER, "Expect 'Element' keyword for @Element usage.");
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
    return std::make_unique<UseElementNode>(name);
}

// --- Token Manipulation and Error Handling ---
bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
Token Parser::peek() const { return tokens[current]; }
Token Parser::previous() const { return tokens[current - 1]; }
Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::check(TokenType type) const { if (isAtEnd()) return false; return peek().type == type; }
bool Parser::match(const std::vector<TokenType>& types) { for (TokenType type : types) { if (check(type)) { advance(); return true; } } return false; }
Token Parser::consume(TokenType type, const std::string& message) { if (check(type)) return advance(); throw error(peek(), message); }
Parser::ParseError Parser::error(const Token& token, const std::string& message) { return ParseError("Parse Error at token " + token.toString() + ": " + message); }
void Parser::synchronize() { advance(); while (!isAtEnd()) { if (previous().type == TokenType::SEMICOLON) return; switch (peek().type) { case TokenType::IDENTIFIER: case TokenType::TEXT: case TokenType::STYLE: case TokenType::SCRIPT: return; default: break; } advance(); } }
