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
#include "CHTLNode/VariableUsageNode.h"
#include <stdexcept>

namespace CHTL {

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
        if (node) {
            nodes.push_back(node);
        }
    }
    return nodes;
}

NodePtr CHTLParser::ParseTemplateDefinition() {
    Expect(TokenType::OpenBracket);
    Expect(TokenType::Identifier); // Template
    Expect(TokenType::CloseBracket);

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
    Token templateType = Expect(TokenType::Identifier); // Style, Element
    Token templateName = Expect(TokenType::Identifier);
    Expect(TokenType::Semicolon);

    if (templateType.value == "Element") {
        return std::make_shared<ElementTemplateUsageNode>(templateName.value);
    }
    // Style usages are handled inside ParseStyleBlockContent
    throw std::runtime_error("Unsupported template usage in this context: @" + templateType.value);
}


NodePtr CHTLParser::ParseNode() {
    while (Peek().type == TokenType::LineComment || Peek().type == TokenType::BlockComment || Peek().type == TokenType::GeneratorComment) {
        Consume();
    }

    Token current = Peek();

    if (current.type == TokenType::OpenBracket) {
        if (Peek(1).type == TokenType::Identifier && Peek(1).value == "Template") {
            return ParseTemplateDefinition();
        }
    }
    if (current.type == TokenType::At) {
        return ParseTemplateUsage();
    }

    if (current.type == TokenType::Identifier) {
        if (Peek(1).type == TokenType::OpenBrace) {
            return ParseElement();
        }
    } else if (current.type == TokenType::Style) {
        return ParseStyleBlock();
    } else if (current.type == TokenType::Text) {
        return ParseTextBlock();
    }

    if (current.type == TokenType::EndOfFile || current.type == TokenType::CloseBrace) {
        return nullptr;
    }

    throw std::runtime_error("Unexpected token in node structure: " + current.value);
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

ExpressionNodePtr CHTLParser::ParsePrimaryExpression() {
    Token t0 = Peek(0), t1 = Peek(1), t2 = Peek(2);
    if ((t0.type == TokenType::Hash || t0.type == TokenType::Dot) && t1.type == TokenType::Identifier && t2.type == TokenType::Dot) {
        Token symbol = Consume(); Token name = Consume();
        std::string selector = symbol.value + name.value;
        Consume(); std::string propName = Expect(TokenType::Identifier).value;
        return std::make_shared<PropertyReferenceNode>(selector, propName);
    }
    if (t0.type == TokenType::Identifier) {
        if (t1.type == TokenType::Dot) {
            std::string selector = Consume().value; Consume();
            std::string propName = Expect(TokenType::Identifier).value;
            return std::make_shared<PropertyReferenceNode>(selector, propName);
        }
        if (t1.type == TokenType::OpenParen) {
            std::string groupName = Consume().value;
            Consume(); // (
            std::string varName = Expect(TokenType::Identifier).value;
            Expect(TokenType::CloseParen);
            return std::make_shared<VariableUsageNode>(groupName, varName);
        }
    }
    if (t0.type == TokenType::Number) {
        Consume(); double value = std::stod(t0.value); std::string unit;
        if (Peek().type == TokenType::Identifier || Peek().type == TokenType::Percent) { unit = Consume().value; }
        return std::make_shared<NumberLiteralNode>(value, unit);
    }
    if (t0.type == TokenType::Identifier || t0.type == TokenType::StringLiteral) {
        Consume();
        return std::make_shared<StringLiteralNode>(t0.value);
    }
    throw std::runtime_error("Unexpected token in expression: " + t0.value);
}


ExpressionNodePtr CHTLParser::ParseExpression() {
    ExpressionNodePtr left = ParsePrimaryExpression();
    while (Peek().type >= TokenType::Plus && Peek().type <= TokenType::DoubleStar) {
        Token op = Consume(); ExpressionNodePtr right = ParsePrimaryExpression();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

NodePtr CHTLParser::ParseStyleBlockContent() {
    auto styleNode = std::make_shared<StyleNode>();
    Expect(TokenType::OpenBrace);
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        Token next = Peek();
        if (next.type == TokenType::At) {
            Consume(); Expect(TokenType::Identifier); // Style
            Token templateName = Expect(TokenType::Identifier);
            styleNode->AddProperty({"__TEMPLATE_USAGE__", std::make_shared<TemplateUsageNode>(templateName.value)});
            Expect(TokenType::Semicolon);
        }
        else if (next.type == TokenType::Identifier && Peek(1).type == TokenType::Colon) {
            Token name = Consume();
            Consume(); // :
            styleNode->AddProperty({name.value, ParseExpression()});
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
