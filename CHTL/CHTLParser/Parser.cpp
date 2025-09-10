#include "Parser.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/CSSPropertyNode.h"
#include "../CHTLNode/CSSRuleNode.h"
#include <sstream>

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_position(0) {
    // Initialize current and peek tokens
    nextToken();
    nextToken();

    // --- Register Pratt parser functions ---

    // Register prefix parsers
    registerPrefix(TokenType::IDENTIFIER, &Parser::parseIdentifierExpression);
    registerPrefix(TokenType::NUMBER_LITERAL, &Parser::parseNumberLiteralExpression);

    // Register infix parsers for arithmetic
    registerInfix(TokenType::PLUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::MINUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::SLASH, &Parser::parseInfixExpression);
    registerInfix(TokenType::ASTERISK, &Parser::parseInfixExpression);

    // --- Setup Precedence Table ---
    m_precedences = {
        {TokenType::PLUS,     Precedence::SUM},
        {TokenType::MINUS,    Precedence::SUM},
        {TokenType::SLASH,    Precedence::PRODUCT},
        {TokenType::ASTERISK, Precedence::PRODUCT},
    };
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    if (m_position < m_tokens.size()) {
        m_peekToken = m_tokens[m_position];
    } else {
        // When we run out of tokens, peek token becomes EOF
        m_peekToken = Token{TokenType::END_OF_FILE, "", m_currentToken.line};
    }
    m_position++;
}

bool Parser::currentTokenIs(TokenType t) const {
    return m_currentToken.type == t;
}

bool Parser::peekTokenIs(TokenType t) const {
    return m_peekToken.type == t;
}

void Parser::peekError(TokenType t) {
    std::stringstream ss;
    ss << "Line " << m_peekToken.line << ": Expected next token to be "
       << Token{t, "", 0}.typeToString() << ", got "
       << m_peekToken.typeToString() << " instead.";
    m_errors.push_back(ss.str());
}

Precedence Parser::peekPrecedence() {
    if (m_precedences.count(m_peekToken.type)) {
        return m_precedences[m_peekToken.type];
    }
    return Precedence::LOWEST;
}

Precedence Parser::currentPrecedence() {
    if (m_precedences.count(m_currentToken.type)) {
        return m_precedences[m_currentToken.type];
    }
    return Precedence::LOWEST;
}

bool Parser::expectPeek(TokenType t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

const std::vector<std::string>& Parser::getErrors() const {
    return m_errors;
}

void Parser::registerPrefix(TokenType tokenType, PrefixParseFn fn) {
    m_prefixParseFns[tokenType] = fn;
}

void Parser::registerInfix(TokenType tokenType, InfixParseFn fn) {
    m_infixParseFns[tokenType] = fn;
}

std::vector<std::unique_ptr<BaseNode>> Parser::parseProgram() {
    std::vector<std::unique_ptr<BaseNode>> program;
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            program.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (currentTokenIs(TokenType::KEYWORD_TEXT)) {
        return parseTextElement();
    } else if (currentTokenIs(TokenType::KEYWORD_STYLE)) {
        return parseStyleBlock();
    } else if (currentTokenIs(TokenType::IDENTIFIER)) {
        // Could be an element or an unquoted text literal inside a text block.
        // For now, we assume it's an element.
        return parseElement();
    } else if (currentTokenIs(TokenType::SINGLE_LINE_COMMENT) ||
               currentTokenIs(TokenType::MULTI_LINE_COMMENT) ||
               currentTokenIs(TokenType::GENERATOR_COMMENT)) {
        return parseComment();
    }
    // Skip illegal tokens
    return nullptr;
}

std::unique_ptr<AttributeNode> Parser::parseAttribute() {
    Token identToken = m_currentToken; // The attribute name

    if (!expectPeek(TokenType::COLON) && !expectPeek(TokenType::EQUALS)) {
        // CHTL allows both ':' and '=' for attributes.
        // If neither is found, it's a syntax error. We might have already logged a peekError.
        return nullptr;
    }
    nextToken(); // Consume the colon/equals, move to the value

    Token valueToken = m_currentToken;
    auto attribute = std::make_unique<AttributeNode>(identToken, identToken.lexeme, valueToken.lexeme);

    if (!expectPeek(TokenType::SEMICOLON)) {
        return nullptr;
    }

    return attribute;
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>(m_currentToken, m_currentToken.lexeme);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
            // This is an attribute
            auto attr = parseAttribute();
            if (attr) {
                element->addAttribute(std::move(attr));
            }
        } else {
            // This is a child element/statement
            auto child = parseStatement();
            if (child) {
                element->addChild(std::move(child));
            }
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
         // Missing closing brace, error should be logged by caller if needed
        return nullptr;
    }

    return element;
}


std::unique_ptr<TextNode> Parser::parseTextElement() {
    Token textKeywordToken = m_currentToken;

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    // The content of the text block is the next token
    Token contentToken = m_currentToken;
    auto textNode = std::make_unique<TextNode>(contentToken, contentToken.lexeme);

    if (!expectPeek(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }

    return textNode;
}

std::unique_ptr<CommentNode> Parser::parseComment() {
    return std::make_unique<CommentNode>(m_currentToken, m_currentToken.lexeme);
}

// --- Expression Parsing Implementations ---

std::unique_ptr<ExpressionNode> Parser::parseExpression(Precedence precedence) {
    // This is the core of the Pratt Parser
    if (!m_prefixParseFns.count(m_currentToken.type)) {
        // No prefix parser found for this token
        return nullptr;
    }
    auto prefix = m_prefixParseFns[m_currentToken.type];
    std::unique_ptr<ExpressionNode> leftExp = prefix(this);

    while (!peekTokenIs(TokenType::SEMICOLON) && precedence < peekPrecedence()) {
        if (!m_infixParseFns.count(m_peekToken.type)) {
            return leftExp;
        }
        auto infix = m_infixParseFns[m_peekToken.type];
        nextToken();
        leftExp = infix(this, std::move(leftExp));
    }

    return leftExp;
}

std::unique_ptr<ExpressionNode> Parser::parseIdentifierExpression() {
    return std::make_unique<LiteralNode>(m_currentToken);
}

std::unique_ptr<ExpressionNode> Parser::parseNumberLiteralExpression() {
    return std::make_unique<LiteralNode>(m_currentToken);
}

std::unique_ptr<ExpressionNode> Parser::parseInfixExpression(std::unique_ptr<ExpressionNode> left) {
    Token op = m_currentToken;
    Precedence precedence = currentPrecedence();
    nextToken();
    auto right = parseExpression(precedence);
    return std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
}

std::unique_ptr<BaseNode> Parser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>(m_currentToken);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            // This is an inline property for the parent element
            auto prop = parseCSSProperty();
            if (prop) {
                styleNode->addProperty(std::move(prop));
            }
        } else if (currentTokenIs(TokenType::DOT) || currentTokenIs(TokenType::HASH) || currentTokenIs(TokenType::AMPERSAND)) {
            // This is a CSS rule for the global stylesheet
            auto rule = parseCSSRule();
            if (rule) {
                styleNode->addRule(std::move(rule));
            }
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }

    return styleNode;
}

std::unique_ptr<CSSPropertyNode> Parser::parseCSSProperty() {
    Token propToken = m_currentToken;
    std::string propName = m_currentToken.lexeme;

    // Handle multi-part property names like "background-color"
    while (peekTokenIs(TokenType::MINUS)) {
        nextToken(); // consume the current part, current token is now '-'
        propName += m_currentToken.lexeme; // append '-'

        if (!expectPeek(TokenType::IDENTIFIER)) return nullptr; // expect next part
        propName += m_currentToken.lexeme; // append the next part
    }

    if (!expectPeek(TokenType::COLON)) {
        return nullptr;
    }
    nextToken(); // consume ':'

    auto value = parseExpression(Precedence::LOWEST);

    if (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken(); // consume optional ';'
    }

    return std::make_unique<CSSPropertyNode>(propToken, propName, std::move(value));
}

std::unique_ptr<CSSRuleNode> Parser::parseCSSRule() {
    // For now, a selector is simple: .class, #id, &:hover, etc.
    // We'll just consume tokens until the left brace.
    // A more robust implementation would parse the selector structure.
    std::stringstream selector_ss;
    Token selectorToken = m_currentToken;

    while(!currentTokenIs(TokenType::LEFT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        selector_ss << m_currentToken.lexeme;
        nextToken();
    }

    auto rule = std::make_unique<CSSRuleNode>(selectorToken, selector_ss.str());

    if (!currentTokenIs(TokenType::LEFT_BRACE)) {
        return nullptr; // Expected a '{'
    }
    nextToken(); // consume '{'

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            auto prop = parseCSSProperty();
            if (prop) {
                rule->addProperty(std::move(prop));
            }
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
        return nullptr; // Expected a '}'
    }

    return rule;
}
