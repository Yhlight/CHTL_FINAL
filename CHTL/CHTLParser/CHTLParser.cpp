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
    {TokenType::LogicalOr, LOGICAL_OR},
    {TokenType::LogicalAnd, LOGICAL_AND},
    {TokenType::DoubleEquals, EQUALS},
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
    {TokenType::QuestionMark, TERNARY}
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
        double value = std::stod(token.value);
        std::string unit;
        if (Peek().type == TokenType::Identifier || Peek().type == TokenType::Percent) {
            unit = Consume().value;
        }
        return std::make_shared<NumberLiteralNode>(value, unit);
    }
    if (token.type == TokenType::Identifier) {
         // Check for variable usage: Var(arg)
        if (Peek(1).type == TokenType::OpenParen) {
            std::string groupName = Consume().value;
            Consume(); // (
            std::string varName = Expect(TokenType::Identifier).value;
            Expect(TokenType::CloseParen);
            return std::make_shared<VariableUsageNode>(groupName, varName);
        }
        // Check for property reference: selector.prop
        if (Peek(1).type == TokenType::Dot) {
            std::string selector = Consume().value;
            Consume(); // .
            std::string propName = Expect(TokenType::Identifier).value;
            return std::make_shared<PropertyReferenceNode>(selector, propName);
        }
        // It's a self-property reference or a string literal like 'blue'
        Consume();
        return std::make_shared<SelfPropertyReferenceNode>(token.value);
    }
    if (token.type == TokenType::StringLiteral) {
        Consume();
        return std::make_shared<StringLiteralNode>(token.value);
    }
    // Check for #id.prop or .class.prop
    if (token.type == TokenType::Hash || token.type == TokenType::Dot) {
        if (Peek(1).type == TokenType::Identifier && Peek(2).type == TokenType::Dot) {
            Token symbol = Consume(); Token name = Consume();
            std::string selector = symbol.value + name.value;
            Consume();
            std::string propName = Expect(TokenType::Identifier).value;
            return std::make_shared<PropertyReferenceNode>(selector, propName);
        }
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
    Consume(); // consume '?'
    ExpressionNodePtr trueBranch = ParseExpression();
    Expect(TokenType::Colon);
    ExpressionNodePtr falseBranch = ParseExpression();
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
Token CHTLParser::Consume() { return m_tokens[m_cursor++]; }
Token CHTLParser::Expect(TokenType type) {
    Token token = Peek();
    if (token.type != type) {
        throw std::runtime_error("Unexpected token: '" + token.value + "', expected type: " + std::to_string(static_cast<int>(type)));
    }
    return Consume();
}

NodeList CHTLParser::Parse() {
    NodeList nodes;
    while (Peek().type != TokenType::EndOfFile) {
        NodePtr node = ParseNode();
        if (node) nodes.push_back(node);
    }
    return nodes;
}

NodePtr CHTLParser::ParseNode() {
    while (Peek().type == TokenType::LineComment || Peek().type == TokenType::BlockComment || Peek().type == TokenType::GeneratorComment) {
        Consume();
    }
    Token current = Peek();
    if (current.type == TokenType::OpenBracket) {
        if (Peek(1).type == TokenType::Identifier && Peek(1).value == "Template") return ParseTemplateDefinition();
    }
    if (current.type == TokenType::At) return ParseTemplateUsage();
    if (current.type == TokenType::Identifier && Peek(1).type == TokenType::OpenBrace) return ParseElement();
    if (current.type == TokenType::Style) return ParseStyleBlock();
    if (current.type == TokenType::Text) return ParseTextBlock();
    if (current.type == TokenType::EndOfFile || current.type == TokenType::CloseBrace) return nullptr;
    throw std::runtime_error("Unexpected token in node structure: " + current.value);
}

NodePtr CHTLParser::ParseTemplateDefinition() {
    Expect(TokenType::OpenBracket); Expect(TokenType::Identifier); Expect(TokenType::CloseBracket);
    Expect(TokenType::At);
    Token templateTypeToken = Expect(TokenType::Identifier);
    Token templateName = Expect(TokenType::Identifier);
    if (templateTypeToken.value == "Style" || templateTypeToken.value == "Var") {
        NodePtr content = ParseStyleBlockContent();
        TemplateType type = (templateTypeToken.value == "Style") ? TemplateType::Style : TemplateType::Var;
        return std::make_shared<TemplateDefinitionNode>(templateName.value, type, NodeList{content});
    }
    if (templateTypeToken.value == "Element") {
        NodeList content;
        Expect(TokenType::OpenBrace);
        while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
            NodePtr node = ParseNode();
            if (node) content.push_back(node);
        }
        Expect(TokenType::CloseBrace);
        return std::make_shared<TemplateDefinitionNode>(templateName.value, TemplateType::Element, content);
    }
    throw std::runtime_error("Unsupported template type: " + templateTypeToken.value);
}

NodePtr CHTLParser::ParseTemplateUsage() {
    Expect(TokenType::At);
    Token templateType = Expect(TokenType::Identifier);
    Token templateName = Expect(TokenType::Identifier);
    Expect(TokenType::Semicolon);
    if (templateType.value == "Element") {
        return std::make_shared<ElementTemplateUsageNode>(templateName.value);
    }
    throw std::runtime_error("Unsupported template usage in this context: @" + templateType.value);
}

NodePtr CHTLParser::ParseElement() {
    Token tagNameToken = Expect(TokenType::Identifier);
    auto element = std::make_shared<ElementNode>(tagNameToken.value);
    Expect(TokenType::OpenBrace);
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        if (Peek().type == TokenType::Identifier && (Peek(1).type == TokenType::Colon || Peek(1).type == TokenType::Equals)) {
            Token name = Consume(); Consume(); Token value = Consume();
            element->AddAttribute({name.value, value.value});
            if (Peek().type == TokenType::Semicolon) Consume();
        } else {
            NodePtr child = ParseNode();
            if (child) element->AddChild(child);
        }
    }
    Expect(TokenType::CloseBrace);
    return element;
}

NodePtr CHTLParser::ParseTextBlock() {
    Expect(TokenType::Text); Expect(TokenType::OpenBrace);
    Token textToken = Expect(TokenType::StringLiteral);
    Expect(TokenType::CloseBrace);
    return std::make_shared<TextNode>(textToken.value);
}

NodePtr CHTLParser::ParseStyleBlockContent() {
    auto styleNode = std::make_shared<StyleNode>();
    Expect(TokenType::OpenBrace);
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        Token next = Peek();
        if (next.type == TokenType::At) {
            Consume(); Expect(TokenType::Identifier);
            Token templateName = Expect(TokenType::Identifier);
            styleNode->AddProperty({"__TEMPLATE_USAGE__", std::make_shared<TemplateUsageNode>(templateName.value)});
            Expect(TokenType::Semicolon);
        }
        else if (next.type == TokenType::Identifier && Peek(1).type == TokenType::Colon) {
            Token name = Consume(); Consume();
            styleNode->AddProperty({name.value, ParseExpression(LOWEST)});
            if (Peek().type == TokenType::Semicolon) Consume();
        } else { Consume(); }
    }
    Expect(TokenType::CloseBrace);
    return styleNode;
}

NodePtr CHTLParser::ParseStyleBlock() {
    Expect(TokenType::Style);
    return ParseStyleBlockContent();
}

} // namespace CHTL
