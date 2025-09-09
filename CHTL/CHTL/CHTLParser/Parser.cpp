#include "Parser.h"
#include <memory>
#include <sstream>
#include <iostream>

namespace CHTL {

Parser::Parser(Lexer& lexer, Context& context) : m_lexer(lexer), m_context(context) {
    nextToken();
    nextToken();

    // Register prefix functions
    registerPrefix(TokenType::IDENTIFIER, &Parser::parseIdentifier);
    registerPrefix(TokenType::NUMBER_LITERAL, &Parser::parseNumberLiteral);

    // Register infix functions
    registerInfix(TokenType::PLUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::MINUS, &Parser::parseInfixExpression);
    registerInfix(TokenType::SLASH, &Parser::parseInfixExpression);
    registerInfix(TokenType::ASTERISK, &Parser::parseInfixExpression);
    registerInfix(TokenType::PERCENT, &Parser::parseInfixExpression);
    registerInfix(TokenType::POWER, &Parser::parseInfixExpression);

    registerInfix(TokenType::QUESTION, &Parser::parseConditionalExpression);
    registerInfix(TokenType::DOT, &Parser::parsePropertyReferenceExpression);
    registerInfix(TokenType::LPAREN, &Parser::parseCallLikeExpression);
    registerInfix(TokenType::LBRACKET, &Parser::parseIndexExpression);

    // Set precedences
    m_precedences = {
        {TokenType::LPAREN, CALL},
        {TokenType::LBRACKET, CALL},
        {TokenType::QUESTION, TERNARY},
        {TokenType::DOT, CALL},
        {TokenType::PLUS, SUM},
        {TokenType::MINUS, SUM},
        {TokenType::SLASH, PRODUCT},
        {TokenType::ASTERISK, PRODUCT},
        {TokenType::PERCENT, PRODUCT},
        {TokenType::POWER, PRODUCT} // Power has higher precedence, but handled by right-associativity
    };
}

void Parser::registerPrefix(TokenType type, prefixParseFn fn) {
    m_prefixParseFns[type] = fn;
}

void Parser::registerInfix(TokenType type, infixParseFn fn) {
    m_infixParseFns[type] = fn;
}

Parser::Precedence Parser::peekPrecedence() {
    if (m_precedences.count(m_peekToken.type)) {
        return m_precedences[m_peekToken.type];
    }
    return LOWEST;
}

Parser::Precedence Parser::curPrecedence() {
    if (m_precedences.count(m_currentToken.type)) {
        return m_precedences[m_currentToken.type];
    }
    return LOWEST;
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

const std::vector<std::string>& Parser::getErrors() const {
    return m_errors;
}

void Parser::peekError(TokenType t) {
    std::string msg = "expected next token to be " + std::to_string(static_cast<int>(t)) +
                      ", got " + std::to_string(static_cast<int>(m_peekToken.type)) + " instead";
    m_errors.push_back(msg);
}

bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (m_currentToken.type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement(true);
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement(bool is_global_scope) {
    if (m_currentToken.type == TokenType::LBRACKET) {
        if (m_peekToken.literal == "Template") return parseTemplateNode();
        if (m_peekToken.literal == "Namespace") return parseNamespaceNode();
        if (m_peekToken.literal == "Import") return parseImportNode();
    }

    switch (m_currentToken.type) {
        case TokenType::TYPE_STYLE:
        case TokenType::TYPE_ELEMENT:
        case TokenType::TYPE_VAR:
            return parseTemplateInstantiationNode();
        default:
            break;
    }

    if (m_currentToken.type == TokenType::IDENTIFIER) {
        if (m_currentToken.literal == "text") return parseTextNode();
        if (m_currentToken.literal == "style") {
            return is_global_scope ? std::unique_ptr<Statement>(parseRawBlockNode(RawBlockType::CSS)) : std::unique_ptr<Statement>(parseStyleNode());
        }
        if (m_currentToken.literal == "script") {
             if (is_global_scope) return parseRawBlockNode(RawBlockType::JS);
             else return parseLocalScriptNode();
        }
        return parseElementNode();
    }
    return nullptr;
}

std::unique_ptr<RawBlockNode> Parser::parseRawBlockNode(RawBlockType type) {
    auto node = std::make_unique<RawBlockNode>();
    node->token = m_currentToken;
    node->type = type;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    std::stringstream content_stream;
    int brace_count = 1;
    nextToken();

    while (brace_count > 0 && m_currentToken.type != TokenType::EOF_TOKEN) {
        if (m_currentToken.type == TokenType::LBRACE) brace_count++;
        else if (m_currentToken.type == TokenType::RBRACE) brace_count--;

        if (brace_count > 0) {
            content_stream << m_currentToken.literal << " ";
        }
        nextToken();
    }

    node->content = content_stream.str();
    return node;
}


std::unique_ptr<TemplateNode> Parser::parseTemplateNode() {
    auto node = std::make_unique<TemplateNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    if (!expectPeek(TokenType::RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->type = m_currentToken;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken;
    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement(false);
        if (stmt) {
            node->body.push_back(std::move(stmt));
        }
        nextToken();
    }

    auto raw_node_ptr = node.get();
    m_context.addTemplate(raw_node_ptr->name.literal, std::move(node));

    return nullptr;
}

std::unique_ptr<TemplateInstantiationNode> Parser::parseTemplateInstantiationNode() {
    auto node = std::make_unique<TemplateInstantiationNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken;

    if (m_peekToken.type != TokenType::LBRACE) {
        if (m_peekToken.type == TokenType::SEMICOLON) {
            nextToken();
        }
        return node; // No body
    }

    nextToken(); // consume '{'
    nextToken(); // move to first token in body

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        std::unique_ptr<Statement> stmt = nullptr;
        if (m_currentToken.literal == "delete") {
            stmt = parseDeleteNode();
        } else {
            // In the future, parse other specialization statements here
            // For now, assume it's a style property override
            stmt = parseStylePropertyNode();
        }

        if (stmt) {
            node->body.push_back(std::move(stmt));
        }
        nextToken();
    }

    return node;
}

std::unique_ptr<DeleteNode> Parser::parseDeleteNode() {
    auto deleteNode = std::make_unique<DeleteNode>();
    deleteNode->token = m_currentToken; // the 'delete' token

    nextToken(); // consume 'delete'

    // Parse comma-separated list of identifiers
    while (m_currentToken.type != TokenType::SEMICOLON && m_currentToken.type != TokenType::EOF_TOKEN) {
        if (m_currentToken.type == TokenType::IDENTIFIER) {
            deleteNode->targets.push_back(m_currentToken);
        }
        if (m_peekToken.type == TokenType::COMMA) {
            nextToken(); // consume identifier
            nextToken(); // consume comma
        } else {
            break; // end of list
        }
    }
     if (m_currentToken.type == TokenType::IDENTIFIER) {
        deleteNode->targets.push_back(m_currentToken);
    }

    if (m_peekToken.type == TokenType::SEMICOLON) {
        nextToken();
    }
    return deleteNode;
}

std::unique_ptr<NamespaceNode> Parser::parseNamespaceNode() {
    auto node = std::make_unique<NamespaceNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    if (!expectPeek(TokenType::RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken;

    if (m_peekToken.type == TokenType::LBRACE) {
        nextToken();
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
            auto stmt = parseStatement(false);
            if (stmt) {
                node->body.push_back(std::move(stmt));
            }
            nextToken();
        }
    }
    return node;
}

std::unique_ptr<ImportNode> Parser::parseImportNode() {
    auto node = std::make_unique<ImportNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    if (!expectPeek(TokenType::RBRACKET)) return nullptr;

    nextToken();
    node->type = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER) || m_currentToken.literal != "from") return nullptr;
    if (!expectPeek(TokenType::STRING_LITERAL)) return nullptr;
    node->path = m_currentToken.literal;

    return node;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto element = std::make_unique<ElementNode>();
    element->token = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        if (m_currentToken.type == TokenType::IDENTIFIER) {
            if (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::EQUALS) {
                auto attr = parseAttributeNode();
                if (attr) element->attributes.push_back(std::move(attr));
            } else {
                 auto child = parseStatement(false);
                 if (child) element->children.push_back(std::move(child));
                 continue;
            }
        }
        nextToken();
    }

    return element;
}

std::unique_ptr<StyleNode> Parser::parseStyleNode() {
    auto styleNode = std::make_unique<StyleNode>();
    styleNode->token = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        std::unique_ptr<Statement> stmt = nullptr;

        switch (m_currentToken.type) {
            case TokenType::TYPE_STYLE:
                stmt = parseTemplateInstantiationNode();
                break;
            case TokenType::IDENTIFIER:
                if (m_peekToken.type == TokenType::LBRACE) stmt = parseStyleRuleNode();
                else if (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::EQUALS) {
                    stmt = parseStylePropertyNode();
                }
                break;
            default:
                break;
        }

        if (stmt) styleNode->children.push_back(std::move(stmt));
        nextToken();
    }
    return styleNode;
}

std::unique_ptr<StylePropertyNode> Parser::parseStylePropertyNode() {
    auto prop = std::make_unique<StylePropertyNode>();
    prop->key = m_currentToken;

    if (!expectPeek(TokenType::COLON) && m_currentToken.type != TokenType::EQUALS) return nullptr;
    nextToken();

    prop->value = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::SEMICOLON) nextToken();
    return prop;
}

std::unique_ptr<StyleRuleNode> Parser::parseStyleRuleNode() {
    auto rule = std::make_unique<StyleRuleNode>();
    rule->selector = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        auto prop = parseStylePropertyNode();
        if (prop) rule->properties.push_back(std::move(prop));
        nextToken();
    }
    return rule;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    auto textNode = std::make_unique<TextNode>();
    textNode->token = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();

    if (m_currentToken.type == TokenType::STRING_LITERAL || m_currentToken.type == TokenType::IDENTIFIER) {
        textNode->value = m_currentToken.literal;
    }

    if (!expectPeek(TokenType::RBRACE)) return nullptr;

    return textNode;
}

std::unique_ptr<AttributeNode> Parser::parseAttributeNode() {
    auto attr = std::make_unique<AttributeNode>();
    attr->key = m_currentToken;

    if (!expectPeek(TokenType::COLON) && m_currentToken.type != TokenType::EQUALS) return nullptr;
    nextToken();

    attr->value = parseExpression(LOWEST);

    if (m_peekToken.type == TokenType::SEMICOLON) nextToken();

    return attr;
}

std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence) {
    if (m_prefixParseFns.find(m_currentToken.type) == m_prefixParseFns.end()) {
        // No prefix parse function found, for now we treat it as a simple identifier/string literal
        if (m_currentToken.type == TokenType::IDENTIFIER) return parseIdentifier();
        if (m_currentToken.type == TokenType::STRING_LITERAL) {
            auto str = std::make_unique<StringLiteral>();
            str->token = m_currentToken;
            str->value = m_currentToken.literal;
            return str;
        }
        return nullptr;
    }

    prefixParseFn prefix = m_prefixParseFns[m_currentToken.type];
    std::unique_ptr<Expression> leftExp = (this->*prefix)();

    while (m_peekToken.type != TokenType::SEMICOLON && precedence < peekPrecedence()) {
        if (m_infixParseFns.find(m_peekToken.type) == m_infixParseFns.end()) {
            return leftExp;
        }
        infixParseFn infix = m_infixParseFns[m_peekToken.type];
        nextToken();
        leftExp = (this->*infix)(std::move(leftExp));
    }

    return leftExp;
}

std::unique_ptr<Expression> Parser::parseIdentifier() {
    auto ident = std::make_unique<Identifier>();
    ident->token = m_currentToken;
    ident->value = m_currentToken.literal;
    return ident;
}

std::unique_ptr<Expression> Parser::parseNumberLiteral() {
    auto lit = std::make_unique<NumberLiteral>();
    lit->token = m_currentToken;
    lit->value = m_currentToken.literal;
    return lit;
}

std::unique_ptr<Expression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
    auto expr = std::make_unique<InfixExpression>();
    expr->token = m_currentToken;
    expr->op = m_currentToken.literal;
    expr->left = std::move(left);

    Precedence precedence = curPrecedence();
    nextToken();
    expr->right = parseExpression(precedence);

    return expr;
}

std::unique_ptr<Expression> Parser::parseIndexExpression(std::unique_ptr<Expression> left) {
    auto expr = std::make_unique<IndexedExpressionNode>();
    expr->token = m_currentToken; // The '[' token
    expr->target = std::move(left);

    nextToken(); // consume '['
    expr->index = parseExpression(LOWEST);

    if (!expectPeek(TokenType::RBRACKET)) {
        return nullptr;
    }

    return expr;
}

std::unique_ptr<Expression> Parser::parseCallLikeExpression(std::unique_ptr<Expression> function) {
    auto node = std::make_unique<VariableSubstitutionNode>();
    node->token = m_currentToken; // The '(' token
    node->var_group = std::move(function);

    // This is a simplified implementation that only expects one argument.
    if (m_peekToken.type == TokenType::RPAREN) {
        nextToken(); // consume ')'
        return node;
    }

    nextToken(); // consume '('

    // For now, assume the argument is a single identifier
    if (m_currentToken.type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    node->variable = Identifier{m_currentToken, m_currentToken.literal};

    if (!expectPeek(TokenType::RPAREN)) {
        return nullptr;
    }

    return node;
}

std::unique_ptr<Expression> Parser::parseConditionalExpression(std::unique_ptr<Expression> condition) {
    auto expr = std::make_unique<ConditionalExpression>();
    expr->token = m_currentToken; // The '?' token
    expr->condition = std::move(condition);

    nextToken(); // consume '?'
    expr->consequence = parseExpression(LOWEST);

    if (!expectPeek(TokenType::COLON)) {
        return nullptr; // Missing colon
    }

    nextToken(); // consume ':'
    expr->alternative = parseExpression(LOWEST);

    return expr;
}

std::unique_ptr<Expression> Parser::parsePropertyReferenceExpression(std::unique_ptr<Expression> left) {
    auto expr = std::make_unique<PropertyReferenceExpression>();
    expr->token = m_currentToken; // The '.' token
    expr->target = std::move(left);

    if (!expectPeek(TokenType::IDENTIFIER)) {
        return nullptr; // Expected a property name after '.'
    }

    expr->property = Identifier{m_currentToken, m_currentToken.literal};

    return expr;
}

std::unique_ptr<LocalScriptNode> Parser::parseLocalScriptNode() {
    auto node = std::make_unique<LocalScriptNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    int brace_count = 1;
    size_t start_pos = m_readPosition;

    while (brace_count > 0 && m_peekToken.type != TokenType::EOF_TOKEN) {
        if (m_peekToken.type == TokenType::LBRACE) brace_count++;
        if (m_peekToken.type == TokenType::RBRACE) brace_count--;
        nextToken();
    }

    node->raw_content = m_input.substr(start_pos, m_position - start_pos);

    if (!expectPeek(TokenType::RBRACE)) return nullptr;

    return node;
}

}
