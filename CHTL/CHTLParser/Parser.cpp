#include "Parser.h"
#include "../CHTLNode/LiteralNode.h"
#include "../CHTLNode/BinaryOpNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/CSSPropertyNode.h"
#include "../CHTLNode/CSSRuleNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CallExpressionNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLLoader/CHTLLoader.h"
#include <sstream>
#include <iostream>

Parser::Parser(std::vector<Token> tokens, TemplateStore& templateStore, CHTLLoader& loader, const std::string& initialNamespace)
    : m_tokens(std::move(tokens)), m_position(0), m_templateStore(templateStore), m_loader(loader), m_currentNamespace(initialNamespace) {
    // Initialize current and peek tokens
    nextToken();
    nextToken();

    // --- Register Pratt parser functions ---
    registerPrefix(TokenType::IDENTIFIER, &Parser::parseIdentifierExpression);
    registerPrefix(TokenType::NUMBER_LITERAL, &Parser::parseNumberLiteralExpression);
    registerPrefix(TokenType::STRING_LITERAL, &Parser::parseStringLiteralExpression);
    registerPrefix(TokenType::KEYWORD_TEXT, &Parser::parseIdentifierExpression);

    // Register infix parsers for arithmetic
    registerInfix(TokenType::PLUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::MINUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::SLASH, &Parser::parseInfixExpression);
    registerInfix(TokenType::ASTERISK, &Parser::parseInfixExpression);
    registerInfix(TokenType::LEFT_PAREN, &Parser::parseCallExpression);

    // --- Setup Precedence Table ---
    m_precedences = {
        {TokenType::PLUS,       Precedence::SUM},
        {TokenType::MINUS,      Precedence::SUM},
        {TokenType::SLASH,      Precedence::PRODUCT},
        {TokenType::ASTERISK,   Precedence::PRODUCT},
        {TokenType::LEFT_PAREN, Precedence::CALL},
    };
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    if (m_position < m_tokens.size()) {
        m_peekToken = m_tokens[m_position];
    } else {
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
       << m_peekToken.typeToString() << " ('" << m_peekToken.lexeme << "') instead.";
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
    if (currentTokenIs(TokenType::KEYWORD_TEMPLATE)) {
        auto templateDef = parseTemplateDefinition();
        if (templateDef) {
            m_templateStore.add(m_currentNamespace, templateDef);
        }
        return nullptr; // Definitions don't go into the AST directly
    } else if (currentTokenIs(TokenType::KEYWORD_IMPORT)) {
        auto importNode = parseImportStatement();
        if (importNode) {
            m_loader.load(importNode.get());
        }
        return nullptr; // Imports are processed immediately, not added to AST
    } else if (currentTokenIs(TokenType::KEYWORD_NAMESPACE)) {
        return parseNamespaceStatement();
    } else if (currentTokenIs(TokenType::AT)) {
        return parseTemplateUsage();
    } else if (currentTokenIs(TokenType::KEYWORD_TEXT)) {
        return parseTextElement();
    } else if (currentTokenIs(TokenType::KEYWORD_STYLE)) {
        return parseStyleBlock();
    } else if (currentTokenIs(TokenType::IDENTIFIER)) {
        return parseElement();
    } else if (currentTokenIs(TokenType::SINGLE_LINE_COMMENT) ||
               currentTokenIs(TokenType::MULTI_LINE_COMMENT) ||
               currentTokenIs(TokenType::GENERATOR_COMMENT)) {
        return parseComment();
    }
    return nullptr;
}

std::unique_ptr<AttributeNode> Parser::parseAttribute() {
    Token identToken = m_currentToken;
    if (!expectPeek(TokenType::COLON) && !expectPeek(TokenType::EQUALS)) {
        return nullptr;
    }
    nextToken();
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
    nextToken();
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
            auto attr = parseAttribute();
            if (attr) {
                element->addAttribute(std::move(attr));
            }
        } else {
            auto child = parseStatement();
            if (child) {
                element->addChild(std::move(child));
            }
        }
        nextToken();
    }
    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }
    return element;
}

std::unique_ptr<TextNode> Parser::parseTextElement() {
    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken();
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

std::unique_ptr<ExpressionNode> Parser::parseExpression(Precedence precedence) {
    if (!m_prefixParseFns.count(m_currentToken.type)) {
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

std::unique_ptr<ExpressionNode> Parser::parseStringLiteralExpression() {
    return std::make_unique<LiteralNode>(m_currentToken);
}

std::unique_ptr<ExpressionNode> Parser::parseInfixExpression(std::unique_ptr<ExpressionNode> left) {
    Token op = m_currentToken;
    Precedence precedence = currentPrecedence();
    nextToken();
    auto right = parseExpression(precedence);
    return std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
}

std::unique_ptr<ExpressionNode> Parser::parseCallExpression(std::unique_ptr<ExpressionNode> function) {
    auto callNode = std::make_unique<CallExpressionNode>(m_currentToken, std::move(function));
    if (peekTokenIs(TokenType::RIGHT_PAREN)) {
        nextToken();
        return callNode;
    }
    nextToken();
    // In CHTL, var arguments are single identifiers, possibly hyphenated, treated as one literal.
    // The lexer now handles this, so we expect a single IDENTIFIER token.
    callNode->arguments.push_back(std::make_unique<LiteralNode>(m_currentToken));

    if (!expectPeek(TokenType::RIGHT_PAREN)) {
        return nullptr;
    }
    return callNode;
}

void Parser::parseStyleBlockBody(StyleBlockNode* styleNode) {
     while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        std::unique_ptr<BaseNode> stmt = nullptr;
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            stmt = parseCSSProperty();
        } else if (currentTokenIs(TokenType::DOT) || currentTokenIs(TokenType::HASH) || currentTokenIs(TokenType::AMPERSAND)) {
            stmt = parseCSSRule();
        } else if (currentTokenIs(TokenType::AT)) {
            stmt = parseTemplateUsage();
        }
        if (stmt) {
            styleNode->addNode(std::move(stmt));
        }
        nextToken();
    }
}

std::unique_ptr<BaseNode> Parser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>(m_currentToken);
    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken();
    parseStyleBlockBody(styleNode.get());
    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }
    return styleNode;
}

std::unique_ptr<CSSPropertyNode> Parser::parseCSSProperty() {
    Token propToken = m_currentToken;
    std::string propName = m_currentToken.lexeme;
    if (!expectPeek(TokenType::COLON)) {
        return nullptr;
    }
    nextToken();
    auto value = parseExpression(Precedence::LOWEST);
    if (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken();
    }
    return std::make_unique<CSSPropertyNode>(propToken, propName, std::move(value));
}

std::unique_ptr<CSSRuleNode> Parser::parseCSSRule() {
    std::stringstream selector_ss;
    Token selectorToken = m_currentToken;
    while(!currentTokenIs(TokenType::LEFT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        selector_ss << m_currentToken.lexeme;
        nextToken();
    }
    auto rule = std::make_unique<CSSRuleNode>(selectorToken, selector_ss.str());
    if (!currentTokenIs(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken();
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
        return nullptr;
    }
    return rule;
}

std::shared_ptr<TemplateDefinitionNode> Parser::parseTemplateDefinition() {
    if (!expectPeek(TokenType::AT)) return nullptr;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    Token typeToken = m_currentToken;
    TemplateType type;
    if (typeToken.lexeme == "Style") type = TemplateType::STYLE;
    else if (typeToken.lexeme == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.lexeme == "Var") type = TemplateType::VAR;
    else { return nullptr; }
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    std::string name = m_currentToken.lexeme;
    auto templateNode = std::make_shared<TemplateDefinitionNode>(m_currentToken, type, name);
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken();
    if (type == TemplateType::ELEMENT) {
        while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
            auto stmt = parseStatement();
            if (stmt) {
                templateNode->addNode(std::move(stmt));
            }
            nextToken();
        }
    } else {
        auto tempStyleNode = std::make_unique<StyleBlockNode>(m_currentToken);
        parseStyleBlockBody(tempStyleNode.get());
        templateNode->body = std::move(tempStyleNode->body);
    }
    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }
    return templateNode;
}

std::unique_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    Token atToken = m_currentToken;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    Token typeToken = m_currentToken;
    TemplateType type;
    if (typeToken.lexeme == "Style") type = TemplateType::STYLE;
    else if (typeToken.lexeme == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.lexeme == "Var") type = TemplateType::VAR;
    else { return nullptr; }
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    std::string name = m_currentToken.lexeme;

    std::string from_ns = "";
    if (peekTokenIs(TokenType::KEYWORD_FROM)) {
        nextToken(); // consume 'from'
        if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
        from_ns = m_currentToken.lexeme;
    }

    if (!expectPeek(TokenType::SEMICOLON)) return nullptr;
    return std::make_unique<TemplateUsageNode>(atToken, type, name, from_ns);
}

std::unique_ptr<ImportNode> Parser::parseImportStatement() {
    Token importToken = m_currentToken;
    nextToken(); // consume [Import]

    std::string typeStr;
    if (currentTokenIs(TokenType::AT)) {
        typeStr = m_currentToken.lexeme; // "@"
        nextToken(); // consume "@"
        typeStr += m_currentToken.lexeme; // append "Chtl", typeStr is "@Chtl"
    } else {
        // Handle other import types like [Custom] @Element later
        return nullptr;
    }
    nextToken(); // consume "Chtl"

    if (!currentTokenIs(TokenType::KEYWORD_FROM)) {
        peekError(TokenType::KEYWORD_FROM);
        return nullptr;
    }
    nextToken(); // consume "from"

    std::string path = m_currentToken.lexeme;

    // For now, we don't parse item names or aliases
    return std::make_unique<ImportNode>(importToken, typeStr, path, "", "");
}

std::unique_ptr<NamespaceNode> Parser::parseNamespaceStatement() {
    Token nsToken = m_currentToken;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;

    std::string name = m_currentToken.lexeme;
    auto nsNode = std::make_unique<NamespaceNode>(nsToken, name);

    // Set the context for parsing inside this namespace
    std::string oldNamespace = m_currentNamespace;
    m_currentNamespace = name;

    if (!peekTokenIs(TokenType::LEFT_BRACE)) {
        // Namespace without a body, just sets the context for the rest of the file
        m_currentNamespace = name; // Set state and return
        return nullptr;
    }
    nextToken(); // Consume '{'

    while(!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            nsNode->addNode(std::move(stmt));
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) return nullptr;

    // Restore the old namespace context
    m_currentNamespace = oldNamespace;

    return nsNode;
}
