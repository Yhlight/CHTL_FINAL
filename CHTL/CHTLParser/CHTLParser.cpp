#include "CHTLParser.h"
#include <iostream>
#include <map>

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

// --- Pratt Parser Implementation ---

CHTLParser::ParseRule* CHTLParser::getRule(TokenType type) {
    static std::map<TokenType, CHTLParser::ParseRule> rules;
    static bool initialized = false;
    if (!initialized) {
        rules[TokenType::LPAREN]        = {&CHTLParser::parseGrouping, nullptr, PREC_NONE};
        rules[TokenType::MINUS]         = {nullptr, &CHTLParser::parseBinary, PREC_TERM};
        rules[TokenType::PLUS]          = {nullptr, &CHTLParser::parseBinary, PREC_TERM};
        rules[TokenType::SLASH]         = {nullptr, &CHTLParser::parseBinary, PREC_FACTOR};
        rules[TokenType::ASTERISK]      = {nullptr, &CHTLParser::parseBinary, PREC_FACTOR};
        rules[TokenType::PERCENT]       = {nullptr, &CHTLParser::parseBinary, PREC_FACTOR};
        rules[TokenType::DOUBLE_ASTERISK] = {nullptr, &CHTLParser::parseBinary, PREC_POWER};

        rules[TokenType::IDENTIFIER]       = {&CHTLParser::parseLiteral, nullptr, PREC_NONE};
        rules[TokenType::UNQUOTED_LITERAL] = {&CHTLParser::parseLiteral, nullptr, PREC_NONE};
        rules[TokenType::STRING_LITERAL]   = {&CHTLParser::parseLiteral, nullptr, PREC_NONE};
        initialized = true;
    }
    if (rules.find(type) != rules.end()) {
        return &rules.at(type);
    }
    static CHTLParser::ParseRule defaultRule = {nullptr, nullptr, PREC_NONE};
    return &defaultRule;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrecedence(Precedence precedence) {
    advance();
    PrefixParseFn prefixRule = getRule(previous().type)->prefix;
    if (prefixRule == nullptr) {
        std::cerr << "Parse Error: Expected expression." << std::endl;
        return nullptr;
    }
    auto left = (this->*prefixRule)();

    while (precedence <= getRule(peek().type)->precedence) {
        advance();
        InfixParseFn infixRule = getRule(previous().type)->infix;
        left = (this->*infixRule)(std::move(left));
    }
    return left;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseLiteral() {
    return std::make_unique<LiteralNode>(previous());
}

std::unique_ptr<ExpressionNode> CHTLParser::parseGrouping() {
    auto expr = parsePrecedence(PREC_ASSIGNMENT);
    consume(TokenType::RPAREN, "Expect ')' after expression.");
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseUnary() {
    return parsePrecedence(PREC_UNARY);
}

std::unique_ptr<ExpressionNode> CHTLParser::parseBinary(std::unique_ptr<ExpressionNode> left) {
    Token op = previous();
    ParseRule* rule = getRule(op.type);
    auto right = parsePrecedence((Precedence)(rule->precedence + 1));
    return std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
}


// --- Main Parser Logic ---

std::unique_ptr<ProgramNode> CHTLParser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        auto stmt = parseStatement();
        if (stmt) {
            program->children.push_back(std::move(stmt));
        }
    }
    return program;
}

std::unique_ptr<Node> CHTLParser::parseStatement() {
    if (peek().type == TokenType::KEYWORD_TEMPLATE) return parseTemplateDefinition();
    if (peek().type == TokenType::TEXT) return parseText();
    if (peek().type == TokenType::STYLE) return parseStyle();
    if (peek().type == TokenType::IDENTIFIER) return parseElement();

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
        const Token& nextToken = (current + 1 < tokens.size()) ? tokens[current + 1] : currentToken;

        if (currentToken.type == TokenType::AT_STYLE || currentToken.type == TokenType::AT_ELEMENT) {
            element->children.push_back(parseTemplateUsage());
        }
        else if (currentToken.type == TokenType::IDENTIFIER && nextToken.type == TokenType::COLON) {
            element->children.push_back(parseAttribute());
        } else if (currentToken.type == TokenType::STYLE) {
            element->children.push_back(parseStyle());
        } else if (currentToken.type == TokenType::TEXT) {
            element->children.push_back(parseText());
        } else if (currentToken.type == TokenType::IDENTIFIER) {
            element->children.push_back(parseElement());
        } else {
            consume(TokenType::IDENTIFIER, "Expect identifier, style, text, or template usage inside element body.");
            break;
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after element body.");
    return element;
}

std::unique_ptr<AttributeNode> CHTLParser::parseAttribute() {
    const Token& key = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    consume(TokenType::COLON, "Expect ':' after attribute name.");
    auto value = parseExpression();
    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    return std::make_unique<AttributeNode>(key.lexeme, std::move(value));
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    consume(TokenType::TEXT, "Expect 'text' keyword.");
    consume(TokenType::LBRACE, "Expect '{' after 'text' keyword.");
    auto value = parseExpression();
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

        if (currentToken.type == TokenType::AT_STYLE) {
            styleNode->children.push_back(parseTemplateUsage());
        }
        else if (currentToken.type == TokenType::IDENTIFIER && nextToken.type == TokenType::COLON) {
            styleNode->children.push_back(parseAttribute());
        } else {
            styleNode->children.push_back(parseSelector());
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after style block.");
    return styleNode;
}

std::unique_ptr<SelectorNode> CHTLParser::parseSelector() {
    std::string selector_text;
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

std::unique_ptr<TemplateDefinitionNode> CHTLParser::parseTemplateDefinition() {
    consume(TokenType::KEYWORD_TEMPLATE, "Expect '[Template]' keyword.");
    auto node = std::make_unique<TemplateDefinitionNode>();

    // Parse @Type
    if (match({TokenType::AT_STYLE, TokenType::AT_ELEMENT, TokenType::AT_VAR})) {
        node->template_type = previous();
    } else {
        consume(TokenType::AT_STYLE, "Expect template type like @Style, @Element, or @Var.");
    }

    // Parse Name
    node->name = consume(TokenType::IDENTIFIER, "Expect template name.");

    // Parse Body
    consume(TokenType::LBRACE, "Expect '{' after template name.");
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        // The body of a template can contain different things depending on its type.
        if (peek().type == TokenType::AT_STYLE) {
            // Handle nested @Style template usage (inheritance)
            node->children.push_back(parseTemplateUsage());
        }
        else if (node->template_type.type == TokenType::AT_STYLE) {
            // Handle attribute for @Style template
            node->children.push_back(parseAttribute());
        } else { // @Element
            // Handle statement for @Element template
            node->children.push_back(parseStatement());
        }
    }
    consume(TokenType::RBRACE, "Expect '}' after template body.");

    return node;
}

std::unique_ptr<TemplateUsageNode> CHTLParser::parseTemplateUsage() {
    auto node = std::make_unique<TemplateUsageNode>();

    if (match({TokenType::AT_STYLE, TokenType::AT_ELEMENT, TokenType::AT_VAR})) {
        node->template_type = previous();
    } else {
        consume(TokenType::AT_STYLE, "Expect template usage like @Style or @Element.");
    }

    node->name = consume(TokenType::IDENTIFIER, "Expect template name for usage.");
    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

    return node;
}

// Hybrid Expression Parser
std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    auto expr = parsePrecedence(PREC_ASSIGNMENT);
    if (auto* literal = dynamic_cast<LiteralNode*>(expr.get())) {
        if (getRule(peek().type)->precedence == PREC_NONE && peek().type != TokenType::SEMICOLON && peek().type != TokenType::RBRACE) {
            std::string new_lexeme = literal->token.lexeme;
            while (!check(TokenType::SEMICOLON) && !check(TokenType::RBRACE) && !isAtEnd()) {
                new_lexeme += " " + advance().lexeme;
            }
            Token new_token = literal->token;
            new_token.lexeme = new_lexeme;
            return std::make_unique<LiteralNode>(new_token);
        }
    }
    return expr;
}

// --- Helper Methods ---
const Token& CHTLParser::peek() { return tokens[current]; }
const Token& CHTLParser::previous() { return tokens[current - 1]; }
const Token& CHTLParser::advance() { if (!isAtEnd()) current++; return previous(); }
bool CHTLParser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
bool CHTLParser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }
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
    std::cerr << "Parse Error at line " << peek().line << ", column " << peek().column << ": " << message << std::endl;
    std::cerr << "  Got " << peek().lexeme << " (" << (int)peek().type << ") instead." << std::endl;
    exit(1);
}
