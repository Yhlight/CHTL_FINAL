#include "CHTLParser.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/ElementTemplateUsageNode.h"
#include "CHTLNode/NumberLiteralNode.h"
#include "CHTLNode/PropertyReferenceNode.h"
#include "CHTLNode/StringLiteralNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/ConditionalExprNode.h"
#include "CHTLNode/SelfPropertyReferenceNode.h"
#include "CHTLNode/VariableUsageNode.h"
#include <stdexcept>
#include <map>

namespace CHTL {

// --- Pratt Parser Implementation ---

static std::map<TokenType, Precedence> precedences = {
    {TokenType::Equals, EQUALS},
    {TokenType::NotEquals, EQUALS},
    {TokenType::LessThan, LESS_GREATER},
    {TokenType::LessThanOrEquals, LESS_GREATER},
    {TokenType::GreaterThan, LESS_GREATER},
    {TokenType::GreaterThanOrEquals, LESS_GREATER},
    {TokenType::Plus, SUM},
    {TokenType::Minus, SUM},
    {TokenType::Star, PRODUCT},
    {TokenType::Slash, PRODUCT},
    {TokenType::Percent, PRODUCT},
    {TokenType::DoubleStar, POWER},
    {TokenType::QuestionMark, TERNARY},
    {TokenType::LogicalAnd, LOGICAL_AND},
    {TokenType::LogicalOr, LOGICAL_OR}
};

Precedence CHTLParser::GetPrecedence() {
    if (precedences.count(Peek().type)) {
        return precedences[Peek().type];
    }
    return LOWEST;
}

ExpressionNodePtr CHTLParser::ParsePrefixExpression() {
    Token token = Peek();
    if (token.type == TokenType::Number) {
        Consume();
        std::string unit;
        if (Peek().type == TokenType::Identifier) {
            unit = Consume().value;
        }
        return std::make_shared<NumberLiteralNode>(std::stod(token.value), unit);
    }
    if (token.type == TokenType::Identifier) {
        Consume();
        return std::make_shared<StringLiteralNode>(token.value);
    }
    if (token.type == TokenType::StringLiteral) {
        Consume();
        return std::make_shared<StringLiteralNode>(token.value);
    }
     if (token.type == TokenType::OpenParen) {
        Consume();
        ExpressionNodePtr expr = ParseExpression(LOWEST);
        Expect(TokenType::CloseParen);
        return expr;
    }

    throw std::runtime_error("Could not parse prefix expression for token: " + token.value);
}


ExpressionNodePtr CHTLParser::ParseInfixExpression(ExpressionNodePtr left) {
    Token op = Consume();
    Precedence precedence = precedences[op.type];
    ExpressionNodePtr right = ParseExpression(precedence);
    return std::make_shared<BinaryOpNode>(left, op, right);
}

ExpressionNodePtr CHTLParser::ParseTernaryExpression(ExpressionNodePtr condition) {
    Consume();
    ExpressionNodePtr trueBranch = ParseExpression(LOWEST);
    ExpressionNodePtr falseBranch = nullptr;
    if (Peek().type == TokenType::Colon) {
        Consume();
        falseBranch = ParseExpression(LOWEST);
    }
    return std::make_shared<ConditionalExprNode>(condition, trueBranch, falseBranch);
}


ExpressionNodePtr CHTLParser::ParseExpression(int precedence) {
    ExpressionNodePtr left = ParsePrefixExpression();

    while (precedence < GetPrecedence()) {
        TokenType next_op_type = Peek().type;
        if (next_op_type == TokenType::QuestionMark) {
            left = ParseTernaryExpression(left);
        } else {
            left = ParseInfixExpression(left);
        }
    }
    return left;
}


// --- CHTL Structure Parsing ---

CHTLParser::CHTLParser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

Token CHTLParser::Peek(size_t offset) {
    if (m_cursor + offset >= m_tokens.size()) { return m_tokens.back(); }
    return m_tokens[m_cursor + offset];
}
Token CHTLParser::Consume() {
    if (m_cursor >= m_tokens.size()) { return m_tokens.back(); }
    return m_tokens[m_cursor++];
}
Token CHTLParser::Expect(TokenType type) {
    Token token = Peek();
    if (token.type != type) {
        throw std::runtime_error("Unexpected token: '" + token.value + "' on line " + std::to_string(token.line) +
                                 ", column " + std::to_string(token.column) + ". Expected type: " + std::to_string(static_cast<int>(type)));
    }
    return Consume();
}

void CHTLParser::SkipComments() {
     while (Peek().type == TokenType::LineComment || Peek().type == TokenType::BlockComment || Peek().type == TokenType::GeneratorComment) {
        Consume();
    }
}

NodeList CHTLParser::Parse() {
    NodeList nodes;
    while (Peek().type != TokenType::EndOfFile) {
        SkipComments();
        if(Peek().type == TokenType::EndOfFile) break;
        NodePtr node = ParseNode();
        if (node) nodes.push_back(node);
    }
    return nodes;
}

NodePtr CHTLParser::ParseNode() {
    SkipComments();
    Token current = Peek();

    if (current.type == TokenType::Identifier) {
        return ParseElement();
    }
    if (current.type == TokenType::Text) {
        return ParseTextBlock();
    }
    if (current.type == TokenType::Style) {
        // Global style block
        return ParseStyleBlock();
    }

    if (current.type == TokenType::EndOfFile || current.type == TokenType::CloseBrace) {
        return nullptr;
    }

    throw std::runtime_error("Unexpected token in node structure: " + current.value + " on line " + std::to_string(current.line));
}

NodePtr CHTLParser::ParseElement() {
    Token tagNameToken = Expect(TokenType::Identifier);
    auto element = std::make_shared<ElementNode>(tagNameToken.value);

    Expect(TokenType::OpenBrace);

    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        SkipComments();
        if (Peek().type == TokenType::CloseBrace) break;

        if (Peek().type == TokenType::Identifier) {
            if (Peek(1).type == TokenType::Colon || Peek(1).type == TokenType::Equals) {
                // This is a property
                Token propName = Consume();
                Consume(); // The ':' or '='

                ExpressionNodePtr propValue = ParseExpression(LOWEST);
                element->AddProperty({propName.value, propValue});

                Expect(TokenType::Semicolon);
            } else {
                // This is a nested element
                NodePtr child = ParseNode();
                if (child) element->AddChild(child);
            }
        } else if (Peek().type == TokenType::Text) {
            NodePtr child = ParseTextBlock();
            if (child) element->AddChild(child);
        } else if (Peek().type == TokenType::Style) {
            NodePtr child = ParseStyleBlock();
            if (child) element->AddChild(child);
        }
        else {
             throw std::runtime_error("Unexpected token inside element " + tagNameToken.value + ": " + Peek().value);
        }
    }

    Expect(TokenType::CloseBrace);
    return element;
}

NodePtr CHTLParser::ParseTextBlock() {
    Expect(TokenType::Text);
    Expect(TokenType::OpenBrace);

    std::string textContent;
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        Token current = Consume();
        textContent += current.value;
        if (Peek().type != TokenType::CloseBrace) {
            if (m_tokens[m_cursor-1].column + m_tokens[m_cursor-1].value.length() < m_tokens[m_cursor].column) {
                 textContent += " ";
            }
        }
    }

    size_t first = textContent.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        textContent = "";
    } else {
        size_t last = textContent.find_last_not_of(" \t\n\r");
        textContent = textContent.substr(first, (last - first + 1));
    }

    Expect(TokenType::CloseBrace);
    return std::make_shared<TextNode>(textContent);
}

NodePtr CHTLParser::ParseStyleBlock() {
    Expect(TokenType::Style);
    Expect(TokenType::OpenBrace);

    auto styleNode = std::make_shared<StyleNode>();

    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        SkipComments();
        if (Peek().type == TokenType::CloseBrace) break;

        Token token1 = Peek();
        Token token2 = Peek(1);

        if (token1.type == TokenType::Identifier && token2.type == TokenType::Colon) {
            // Direct property for inline style
            Token propName = Consume();
            Consume(); // Consume ':'
            ExpressionNodePtr propValue = ParseExpression(LOWEST);
            styleNode->AddProperty({propName.value, propValue});
            Expect(TokenType::Semicolon);
        } else {
            // This is a nested style rule (e.g., .class, #id, &:pseudo)
            std::string selector;
            if (token1.type == TokenType::Dot || token1.type == TokenType::Hash) {
                selector += Consume().value; // . or #
                selector += Expect(TokenType::Identifier).value;
            } else if (token1.type == TokenType::Ampersand) {
                selector += Consume().value; // &
                if(Peek().type == TokenType::Colon) {
                    selector += Consume().value; // :
                    if(Peek().type == TokenType::Colon) { // ::pseudo-element
                        selector += Consume().value;
                    }
                    selector += Expect(TokenType::Identifier).value;
                }
            } else {
                throw std::runtime_error("Invalid start of style rule: " + token1.value);
            }

            auto styleRule = std::make_shared<StyleRuleNode>(selector);
            Expect(TokenType::OpenBrace);
            while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
                SkipComments();
                if(Peek().type == TokenType::CloseBrace) break;

                Token propName = Expect(TokenType::Identifier);
                Expect(TokenType::Colon);
                ExpressionNodePtr propValue = ParseExpression(LOWEST);
                styleRule->AddProperty({propName.value, propValue});
                Expect(TokenType::Semicolon);
            }
            Expect(TokenType::CloseBrace);
            styleNode->AddRule(styleRule);
        }
    }

    Expect(TokenType::CloseBrace);
    return styleNode;
}


// Keeping these stubs for future implementation
NodePtr CHTLParser::ParseStyleBlockContent() { return nullptr; }
NodePtr CHTLParser::ParseTemplateDefinition() { return nullptr; }
NodePtr CHTLParser::ParseTemplateUsage() { return nullptr; }

} // namespace CHTL
