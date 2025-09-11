#include "CHTLParser.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/ElementTemplateUsageNode.h"
#include "CHTLNode/ElementModificationNode.h"
#include "CHTLNode/ElementDeleteNode.h"
#include "CHTLNode/ElementInsertNode.h"
#include "CHTLNode/NumberLiteralNode.h"
#include "CHTLNode/StringLiteralNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/ConditionalExprNode.h"
#include "CHTLNode/VariableUsageNode.h"
#include <stdexcept>
#include <map>

namespace CHTL {

// --- Pratt Parser Implementation ---
// ... (omitting unchanged expression parser for brevity)
static std::map<TokenType, Precedence> precedences = {
    {TokenType::Equals, EQUALS}, {TokenType::NotEquals, EQUALS},
    {TokenType::LessThan, LESS_GREATER}, {TokenType::LessThanOrEquals, LESS_GREATER},
    {TokenType::GreaterThan, LESS_GREATER}, {TokenType::GreaterThanOrEquals, LESS_GREATER},
    {TokenType::Plus, SUM}, {TokenType::Minus, SUM},
    {TokenType::Star, PRODUCT}, {TokenType::Slash, PRODUCT}, {TokenType::Percent, PRODUCT},
    {TokenType::DoubleStar, POWER}, {TokenType::QuestionMark, TERNARY},
    {TokenType::LogicalAnd, LOGICAL_AND}, {TokenType::LogicalOr, LOGICAL_OR}
};
Precedence CHTLParser::GetPrecedence() {
    if (precedences.count(Peek().type)) return precedences[Peek().type];
    return LOWEST;
}
ExpressionNodePtr CHTLParser::ParsePrefixExpression() {
    Token token = Peek();
    if (token.type == TokenType::Number) {
        Consume(); std::string unit;
        if (Peek().type == TokenType::Identifier) unit = Consume().value;
        return std::make_shared<NumberLiteralNode>(std::stod(token.value), unit);
    }
    if (token.type == TokenType::Identifier) {
        if (Peek(1).type == TokenType::OpenParen) return ParseVariableUsage();
        Consume(); return std::make_shared<StringLiteralNode>(token.value);
    }
    if (token.type == TokenType::StringLiteral) {
        Consume(); return std::make_shared<StringLiteralNode>(token.value);
    }
    if (token.type == TokenType::OpenParen) {
        Consume(); ExpressionNodePtr expr = ParseExpression(LOWEST);
        Expect(TokenType::CloseParen); return expr;
    }
    throw std::runtime_error("Could not parse prefix expression for token: " + token.value);
}
ExpressionNodePtr CHTLParser::ParseVariableUsage() {
    Token groupName = Expect(TokenType::Identifier); Expect(TokenType::OpenParen);
    if (Peek().type == TokenType::CloseParen) throw std::runtime_error("Empty variable group usage is not allowed.");
    if (Peek(1).type == TokenType::Equals) {
        std::map<std::string, ExpressionNodePtr> specializations;
        while(Peek().type != TokenType::CloseParen) {
            Token varName = Expect(TokenType::Identifier); Expect(TokenType::Equals);
            ExpressionNodePtr value = ParseExpression(LOWEST);
            specializations[varName.value] = value;
            if (Peek().type == TokenType::Comma) Consume();
        }
        Expect(TokenType::CloseParen); return std::make_shared<VariableUsageNode>(groupName.value, specializations);
    } else {
        Token varName = Expect(TokenType::Identifier); Expect(TokenType::CloseParen);
        return std::make_shared<VariableUsageNode>(groupName.value, varName.value);
    }
}
ExpressionNodePtr CHTLParser::ParseInfixExpression(ExpressionNodePtr left) {
    Token op = Consume(); Precedence precedence = precedences[op.type];
    ExpressionNodePtr right = ParseExpression(precedence);
    return std::make_shared<BinaryOpNode>(left, op, right);
}
ExpressionNodePtr CHTLParser::ParseTernaryExpression(ExpressionNodePtr condition) {
    Consume(); ExpressionNodePtr trueBranch = ParseExpression(LOWEST);
    ExpressionNodePtr falseBranch = nullptr;
    if (Peek().type == TokenType::Colon) {
        Consume(); falseBranch = ParseExpression(LOWEST);
    }
    return std::make_shared<ConditionalExprNode>(condition, trueBranch, falseBranch);
}
ExpressionNodePtr CHTLParser::ParseExpression(int precedence) {
    ExpressionNodePtr left = ParsePrefixExpression();
    while (precedence < GetPrecedence()) {
        if (Peek().type == TokenType::QuestionMark) left = ParseTernaryExpression(left);
        else left = ParseInfixExpression(left);
    }
    return left;
}


// --- CHTL Structure Parsing ---

CHTLParser::CHTLParser(std::vector<Token> tokens) : m_tokens(std::move(tokens)) {}
Token CHTLParser::Peek(size_t offset) { if (m_cursor + offset >= m_tokens.size()) return m_tokens.back(); return m_tokens[m_cursor + offset]; }
Token CHTLParser::Consume() { if (m_cursor >= m_tokens.size()) return m_tokens.back(); return m_tokens[m_cursor++]; }
Token CHTLParser::Expect(TokenType type) { Token token = Peek(); if (token.type != type) throw std::runtime_error("Unexpected token"); return Consume(); }
void CHTLParser::SkipComments() { while (Peek().type == TokenType::LineComment || Peek().type == TokenType::BlockComment || Peek().type == TokenType::GeneratorComment) Consume(); }

NodeList CHTLParser::Parse() {
    NodeList nodes;
    while (Peek().type != TokenType::EndOfFile) {
        SkipComments(); if(Peek().type == TokenType::EndOfFile) break;
        NodePtr node = ParseNode(); if (node) nodes.push_back(node);
    }
    return nodes;
}

NodePtr CHTLParser::ParseNode() {
    SkipComments(); Token current = Peek();
    if (current.type == TokenType::OpenBracket) return ParseTemplateDefinition();
    if (current.type == TokenType::Identifier) return ParseElement();
    if (current.type == TokenType::Text) return ParseTextBlock();
    if (current.type == TokenType::Style) return ParseStyleBlock(true);
    if (current.type == TokenType::At) return ParseElementTemplateUsage();
    if (current.type == TokenType::EndOfFile || current.type == TokenType::CloseBrace) return nullptr;
    throw std::runtime_error("Unexpected token in node structure: " + current.value);
}

NodePtr CHTLParser::ParseElement() {
    Token tagNameToken = Expect(TokenType::Identifier);
    auto element = std::make_shared<ElementNode>(tagNameToken.value);
    Expect(TokenType::OpenBrace);
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        SkipComments(); if (Peek().type == TokenType::CloseBrace) break;
        Token token = Peek();
        if (token.type == TokenType::Identifier) {
            if (Peek(1).type == TokenType::Colon || Peek(1).type == TokenType::Equals) {
                Token propName = Consume(); Consume();
                ExpressionNodePtr propValue = ParseExpression(LOWEST);
                element->AddProperty({propName.value, propValue}); Expect(TokenType::Semicolon);
            } else {
                NodePtr child = ParseNode(); if (child) element->AddChild(child);
            }
        } else if (token.type == TokenType::Text) {
            NodePtr child = ParseTextBlock(); if (child) element->AddChild(child);
        } else if (token.type == TokenType::Style) {
            NodePtr child = ParseStyleBlock(false); if (child) element->AddChild(child);
        } else if (token.type == TokenType::At) {
            NodePtr child = ParseElementTemplateUsage(); if (child) element->AddChild(child);
        } else {
             throw std::runtime_error("Unexpected token inside element '" + tagNameToken.value + "': " + token.value);
        }
    }
    Expect(TokenType::CloseBrace); return element;
}

NodePtr CHTLParser::ParseTextBlock() {
    Expect(TokenType::Text); Expect(TokenType::OpenBrace);
    std::string textContent;
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        Token current = Consume(); textContent += current.value;
        if (Peek().type != TokenType::CloseBrace) {
            if (m_tokens[m_cursor-1].column + m_tokens[m_cursor-1].value.length() < m_tokens[m_cursor].column) textContent += " ";
        }
    }
    size_t first = textContent.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) textContent = "";
    else { size_t last = textContent.find_last_not_of(" \t\n\r"); textContent = textContent.substr(first, (last - first + 1)); }
    Expect(TokenType::CloseBrace); return std::make_shared<TextNode>(textContent);
}

NodePtr CHTLParser::ParseStyleBlock(bool isGlobal) {
    Expect(TokenType::Style); Expect(TokenType::OpenBrace);
    auto styleNode = std::make_shared<StyleNode>();
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        SkipComments(); if (Peek().type == TokenType::CloseBrace) break;
        Token token1 = Peek();
        if (token1.type == TokenType::Delete) {
            Consume(); std::string deletedItems;
            while(Peek().type != TokenType::Semicolon) {
                deletedItems += Consume().value;
                if (Peek().type == TokenType::Comma) deletedItems += Consume().value;
            }
            Expect(TokenType::Semicolon); styleNode->AddProperty({"__DELETE__", std::make_shared<StringLiteralNode>(deletedItems)});
        } else if (token1.type == TokenType::At) {
            Consume(); Expect(TokenType::Style); Token templateName = Expect(TokenType::Identifier);
            std::shared_ptr<StyleNode> specialization = nullptr;
            if (Peek().type == TokenType::OpenBrace) specialization = std::static_pointer_cast<StyleNode>(ParseStyleBlock(isGlobal));
            else Expect(TokenType::Semicolon);
            styleNode->AddProperty({"__TEMPLATE_USAGE__", std::make_shared<TemplateUsageNode>(templateName.value, specialization)});
        } else if (token1.type == TokenType::Identifier) {
            if (Peek(1).type == TokenType::Colon) {
                Token propName = Consume(); Consume();
                ExpressionNodePtr propValue = ParseExpression(LOWEST);
                styleNode->AddProperty({propName.value, propValue}); Expect(TokenType::Semicolon);
            } else {
                while (Peek().type != TokenType::Semicolon && Peek().type != TokenType::EndOfFile) {
                    Token propName = Expect(TokenType::Identifier);
                    styleNode->AddProperty({propName.value, nullptr});
                    if (Peek().type == TokenType::Comma) Consume(); else break;
                }
                Expect(TokenType::Semicolon);
            }
        } else {
            std::string selector;
            if (token1.type == TokenType::Dot || token1.type == TokenType::Hash) {
                selector += Consume().value; selector += Expect(TokenType::Identifier).value;
            } else if (token1.type == TokenType::Ampersand) {
                selector += Consume().value;
                if(Peek().type == TokenType::Colon) {
                    selector += Consume().value;
                    if(Peek().type == TokenType::Colon) selector += Consume().value;
                    selector += Expect(TokenType::Identifier).value;
                }
            } else { throw std::runtime_error("Invalid start of style rule: " + token1.value); }
            auto styleRule = std::make_shared<StyleRuleNode>(selector);
            auto nestedBlock = std::static_pointer_cast<StyleNode>(ParseStyleBlock(isGlobal));
            for(const auto& prop : nestedBlock->GetProperties()) styleRule->AddProperty(prop);
            styleNode->AddRule(styleRule);
        }
    }
    Expect(TokenType::CloseBrace); return styleNode;
}

NodePtr CHTLParser::ParseTemplateDefinition() {
    Expect(TokenType::OpenBracket);
    Token definitionType = Expect(TokenType::Identifier); bool isCustom = (definitionType.value == "Custom");
    if (!isCustom && definitionType.value != "Template") throw std::runtime_error("Expected '[Template]' or '[Custom]'");
    Expect(TokenType::CloseBracket); Expect(TokenType::At);
    Token typeToken = Expect(TokenType::Identifier); Token nameToken = Expect(TokenType::Identifier);
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::Style;
    else if (typeToken.value == "Element") type = TemplateType::Element;
    else if (typeToken.value == "Var") type = TemplateType::Var;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);
    NodeList content; Expect(TokenType::OpenBrace);
    if (type == TemplateType::Element) {
        while(Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
            NodePtr node = ParseNode(); if (node) content.push_back(node);
        }
    } else { content.push_back(ParseStyleBlock(true)); }
    Expect(TokenType::CloseBrace);
    return std::make_shared<TemplateDefinitionNode>(nameToken.value, type, content, isCustom);
}

NodePtr CHTLParser::ParseElementTemplateUsage() {
    Expect(TokenType::At);
    Expect(TokenType::Element);
    Token name = Expect(TokenType::Identifier);
    NodeList instructions;
    if (Peek().type == TokenType::OpenBrace) {
        Expect(TokenType::OpenBrace);
        while(Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
            SkipComments();
            if (Peek().type == TokenType::CloseBrace) break;
            Token t = Peek();
            if (t.type == TokenType::Insert) instructions.push_back(ParseElementInsertion());
            else if (t.type == TokenType::Delete) instructions.push_back(ParseElementDeletion());
            else instructions.push_back(ParseElementModification());
        }
        Expect(TokenType::CloseBrace);
    } else {
        Expect(TokenType::Semicolon);
    }
    return std::make_shared<ElementTemplateUsageNode>(name.value, instructions);
}

// --- Element Specialization Parsers ---

NodePtr CHTLParser::ParseElementModification() {
    // e.g. `div[1] { style { ... } }`
    std::string selector = Expect(TokenType::Identifier).value;
    if (Peek().type == TokenType::OpenBracket) {
        selector += Consume().value; // [
        selector += Expect(TokenType::Number).value;
        selector += Expect(TokenType::CloseBracket).value;
    }
    auto styleNode = std::static_pointer_cast<StyleNode>(ParseStyleBlock(false));
    return std::make_shared<ElementModificationNode>(selector, styleNode);
}

NodePtr CHTLParser::ParseElementDeletion() {
    // e.g. `delete span, div[0];`
    Expect(TokenType::Delete);
    std::vector<std::string> selectors;
    while(Peek().type != TokenType::Semicolon) {
        std::string selector = Expect(TokenType::Identifier).value;
        if (Peek().type == TokenType::OpenBracket) {
             selector += Consume().value; // [
             selector += Expect(TokenType::Number).value;
             selector += Expect(TokenType::CloseBracket).value;
        }
        selectors.push_back(selector);
        if (Peek().type == TokenType::Comma) Consume();
    }
    Expect(TokenType::Semicolon);
    return std::make_shared<ElementDeleteNode>(selectors);
}

NodePtr CHTLParser::ParseElementInsertion() {
    // e.g. `insert after div[0] { ... }`
    Expect(TokenType::Insert);
    Token posToken = Consume();
    InsertPosition position;
    if (posToken.type == TokenType::After) position = InsertPosition::After;
    else if (posToken.type == TokenType::Before) position = InsertPosition::Before;
    else if (posToken.type == TokenType::Replace) position = InsertPosition::Replace;
    else if (posToken.type == TokenType::At) {
        Token subPos = Expect(TokenType::Identifier);
        if (subPos.value == "top") position = InsertPosition::AtTop;
        else if (subPos.value == "bottom") position = InsertPosition::AtBottom;
        else throw std::runtime_error("Invalid 'at' position for insert.");
    } else {
        throw std::runtime_error("Invalid position for insert.");
    }

    std::string targetSelector;
    if (position != InsertPosition::AtTop && position != InsertPosition::AtBottom) {
        targetSelector = Expect(TokenType::Identifier).value;
         if (Peek().type == TokenType::OpenBracket) {
             targetSelector += Consume().value;
             targetSelector += Expect(TokenType::Number).value;
             targetSelector += Expect(TokenType::CloseBracket).value;
        }
    }

    NodeList nodesToInsert;
    Expect(TokenType::OpenBrace);
    while(Peek().type != TokenType::CloseBrace) {
        nodesToInsert.push_back(ParseNode());
    }
    Expect(TokenType::CloseBrace);

    return std::make_shared<ElementInsertNode>(position, targetSelector, nodesToInsert);
}


} // namespace CHTL
