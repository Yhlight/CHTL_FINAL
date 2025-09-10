#include "CHTLParser.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/NumberLiteralNode.h"
#include "CHTLNode/PropertyReferenceNode.h"
#include "CHTLNode/StringLiteralNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}

Token CHTLParser::Peek(size_t offset) {
    if (m_cursor + offset >= m_tokens.size()) {
        return m_tokens.back();
    }
    return m_tokens[m_cursor + offset];
}

Token CHTLParser::Consume() {
    return m_tokens[m_cursor++];
}

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

NodePtr CHTLParser::ParseNode() {
    while (Peek().type == TokenType::LineComment || Peek().type == TokenType::BlockComment || Peek().type == TokenType::GeneratorComment) {
        Consume();
    }

    Token current = Peek();

    if (current.type == TokenType::OpenBracket) {
        if (Peek(1).type == TokenType::Identifier && Peek(1).value == "Template") {
            Consume(); // [
            Consume(); // Template
            Consume(); // ]

            Expect(TokenType::At);
            Token templateType = Expect(TokenType::Identifier); // Style, Element, Var
            Token templateName = Expect(TokenType::Identifier);

            NodePtr content = nullptr;
            if (templateType.value == "Style") {
                content = ParseStyleBlockContent();
            } else {
                throw std::runtime_error("Unsupported template type: " + templateType.value);
            }

            if (content) {
                return std::make_shared<TemplateDefinitionNode>(templateName.value, TemplateType::Style, content);
            }
        }
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
            Token name = Consume();
            Consume();
            Token value = Consume();
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
    Expect(TokenType::Text);
    Expect(TokenType::OpenBrace);
    Token textToken = Expect(TokenType::StringLiteral);
    Expect(TokenType::CloseBrace);
    return std::make_shared<TextNode>(textToken.value);
}

ExpressionNodePtr CHTLParser::ParsePrimaryExpression() {
    Token t0 = Peek(0);
    Token t1 = Peek(1);
    Token t2 = Peek(2);

    if ((t0.type == TokenType::Hash || t0.type == TokenType::Dot) && t1.type == TokenType::Identifier && t2.type == TokenType::Dot) {
        Token symbol = Consume(); Token name = Consume();
        std::string selector = symbol.value + name.value;
        Consume();
        std::string propName = Expect(TokenType::Identifier).value;
        return std::make_shared<PropertyReferenceNode>(selector, propName);
    }
    if (t0.type == TokenType::Identifier && t1.type == TokenType::Dot) {
        std::string selector = Consume().value; Consume();
        std::string propName = Expect(TokenType::Identifier).value;
        return std::make_shared<PropertyReferenceNode>(selector, propName);
    }
    if (t0.type == TokenType::Number) {
        Consume();
        double value = std::stod(t0.value);
        std::string unit;
        if (Peek().type == TokenType::Identifier || Peek().type == TokenType::Percent) {
            unit = Consume().value;
        }
        return std::make_shared<NumberLiteralNode>(value, unit);
    }

    // Case 4: String or Unadorned Literal
    if (t0.type == TokenType::Identifier || t0.type == TokenType::StringLiteral) {
        Consume();
        return std::make_shared<StringLiteralNode>(t0.value);
    }

    throw std::runtime_error("Unexpected token in expression: " + t0.value);
}


ExpressionNodePtr CHTLParser::ParseExpression() {
    ExpressionNodePtr left = ParsePrimaryExpression();
    while (Peek().type >= TokenType::Plus && Peek().type <= TokenType::DoubleStar) {
        Token op = Consume();
        ExpressionNodePtr right = ParsePrimaryExpression();
        left = std::make_shared<BinaryOpNode>(left, op, right);
    }
    return left;
}

// This function parses the content of a style block, whether it's standalone or in a template
NodePtr CHTLParser::ParseStyleBlockContent() {
    auto styleNode = std::make_shared<StyleNode>();
    Expect(TokenType::OpenBrace);

    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        Token next = Peek();
        // Template Usage: @Style MyTemplate;
        if (next.type == TokenType::At) {
            Consume(); // @
            Expect(TokenType::Identifier); // Style
            Token templateName = Expect(TokenType::Identifier);
            styleNode->AddProperty({
                "__TEMPLATE_USAGE__", // Special name
                std::make_shared<TemplateUsageNode>(templateName.value)
            });
            Expect(TokenType::Semicolon);
        }
        // Selector rule
        else if (next.type == TokenType::Dot || next.type == TokenType::Hash || next.type == TokenType::Ampersand) {
            std::string selector_text;
            selector_text += Consume().value;
            if (next.type == TokenType::Dot || next.type == TokenType::Hash) {
                selector_text += Expect(TokenType::Identifier).value;
            }
            if (Peek().type == TokenType::Colon) {
                selector_text += Consume().value;
                selector_text += Expect(TokenType::Identifier).value;
            }
            auto ruleNode = std::make_shared<StyleRuleNode>(selector_text);
            Expect(TokenType::OpenBrace);
            while(Peek().type != TokenType::CloseBrace) {
                Token prop_name = Expect(TokenType::Identifier);
                Expect(TokenType::Colon);

                // Check if value is an arithmetic expression
                bool is_expression = false;
                for (int i = 1; i < 5; ++i) { // Check a few tokens ahead
                    TokenType type = Peek(i).type;
                    if (type >= TokenType::Plus && type <= TokenType::DoubleStar) {
                        is_expression = true;
                        break;
                    }
                    if (type == TokenType::Semicolon) break;
                }

                if (is_expression) {
                    ruleNode->AddProperty({prop_name.value, ParseExpression()});
                } else {
                    // Treat as a simple list of values
                    std::string value_str;
                    while(Peek().type != TokenType::Semicolon && Peek().type != TokenType::CloseBrace) {
                        value_str += Consume().value + " ";
                    }
                    if (!value_str.empty()) value_str.pop_back();
                    ruleNode->AddProperty({prop_name.value, std::make_shared<StringLiteralNode>(value_str)});
                }

                if (Peek().type == TokenType::Semicolon) Consume();
            }
            Expect(TokenType::CloseBrace);
            styleNode->AddRule(ruleNode);
        }
        // Flat property
        else if (next.type == TokenType::Identifier && Peek(1).type == TokenType::Colon) {
            Token name = Consume();
            Consume();

            // Check if value is an arithmetic expression
            bool is_expression = false;
            for (int i = 1; i < 5; ++i) { // Check a few tokens ahead
                TokenType type = Peek(i).type;
                if (type >= TokenType::Plus && type <= TokenType::DoubleStar) {
                    is_expression = true;
                    break;
                }
                if (type == TokenType::Semicolon) break;
            }

            if (is_expression) {
                styleNode->AddProperty({name.value, ParseExpression()});
            } else {
                // Treat as a simple list of values
                std::string value_str;
                while(Peek().type != TokenType::Semicolon && Peek().type != TokenType::CloseBrace) {
                    value_str += Consume().value + " ";
                }
                if (!value_str.empty()) value_str.pop_back();
                styleNode->AddProperty({name.value, std::make_shared<StringLiteralNode>(value_str)});
            }

             if (Peek().type == TokenType::Semicolon) Consume();
        } else {
            Consume();
        }
    }

    Expect(TokenType::CloseBrace);
    return styleNode;
}


NodePtr CHTLParser::ParseStyleBlock() {
    Expect(TokenType::Style);
    return ParseStyleBlockContent();
}

} // namespace CHTL
