#include "CHTLParser.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/StyleRuleNode.h"
#include "CHTLNode/TemplateDefinitionNode.h"
#include "CHTLNode/TemplateUsageNode.h"
#include "CHTLNode/ElementTemplateUsageNode.h"
#include "CHTLNode/ElementModificationNode.h"
#include "CHTLNode/ElementDeleteNode.h"
#include "CHTLNode/ElementInsertNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/NumberLiteralNode.h"
#include "CHTLNode/StringLiteralNode.h"
#include "CHTLNode/BinaryOpNode.h"
#include "CHTLNode/ConditionalExprNode.h"
#include "CHTLNode/VariableUsageNode.h"
#include "CHTLNode/PropertyReferenceNode.h"
#include "CHTLNode/SelfPropertyReferenceNode.h"
#include <stdexcept>
#include <map>
#include <algorithm>

#include "CHTLCompiler/CompilationContext.h"

namespace CHTL {

// Helper function to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const std::string& whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// --- CHTL Structure Parsing ---

CHTLParser::CHTLParser(std::vector<Token> tokens, std::string source, CompilationContext& context)
    : m_tokens(std::move(tokens)), m_source(std::move(source)), m_context(context) {}

std::string CHTLParser::GetCurrentNamespace() const {
    if (m_namespace_stack.empty()) {
        return "";
    }
    std::string full_namespace;
    for (size_t i = 0; i < m_namespace_stack.size(); ++i) {
        full_namespace += m_namespace_stack[i];
        if (i < m_namespace_stack.size() - 1) {
            full_namespace += ".";
        }
    }
    return full_namespace;
}

void CHTLParser::PushNamespace(const std::string& name) {
    m_namespace_stack.push_back(name);
}

void CHTLParser::PopNamespace() {
    if (!m_namespace_stack.empty()) {
        m_namespace_stack.pop_back();
    }
}

NodePtr CHTLParser::ParseNode() {
    SkipComments();
    Token current = Peek();
    if (current.type == TokenType::OpenBracket) {
        if (Peek(1).type == TokenType::Identifier) {
            if (Peek(1).value == "Origin") {
                return ParseOriginBlock();
            } else if (Peek(1).value == "Import") {
                return ParseImportStatement();
            } else if (Peek(1).value == "Namespace") {
                return ParseNamespace();
            }
        }
        return ParseTemplateDefinition();
    }
    if (current.type == TokenType::Identifier) {
        return ParseElement();
    }
    if (current.type == TokenType::Text) {
        return ParseTextBlock();
    }
    if (current.type == TokenType::Style) {
        return ParseStyleBlock(true);
    }
    if (current.type == TokenType::At) {
        return ParseElementTemplateUsage();
    }
    if (current.type == TokenType::EndOfFile || current.type == TokenType::CloseBrace) {
        return nullptr;
    }
    throw std::runtime_error("Unexpected token in node structure: " + current.value);
}

NodePtr CHTLParser::ParseElementTemplateUsage() {
    Expect(TokenType::At);
    Token typeToken = Expect(TokenType::Identifier);
    if (typeToken.value != "Element") {
        throw std::runtime_error("Expected 'Element' after @ in element template usage, but got '" + typeToken.value + "'");
    }
    Token name = Expect(TokenType::Identifier);
    std::string from;
    if (Peek().type == TokenType::From) {
        Consume(); // Consume 'from'
        from = Expect(TokenType::Identifier).value; // This will need to handle dot-separated namespaces later
    }

    NodeList instructions;
    if (Peek().type == TokenType::OpenBrace) {
        Expect(TokenType::OpenBrace);
        while(Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
            SkipComments();
            if (Peek().type == TokenType::CloseBrace) break;
            Token t = Peek();
            if (t.type == TokenType::Insert) {
                instructions.push_back(ParseElementInsertion());
            } else if (t.type == TokenType::Delete) {
                instructions.push_back(ParseElementDeletion());
            } else {
                instructions.push_back(ParseElementModification());
            }
        }
        Expect(TokenType::CloseBrace);
    } else {
        Expect(TokenType::Semicolon);
    }
    return std::make_shared<ElementTemplateUsageNode>(name.value, instructions, from);
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
        Token subPos = Consume();
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

NodePtr CHTLParser::ParseImportStatement() {
    Expect(TokenType::OpenBracket);
    Expect(TokenType::Identifier); // "Import"
    Expect(TokenType::CloseBracket);
    Expect(TokenType::At);

    Token typeToken = Expect(TokenType::Identifier);
    ImportType importType;
    if (typeToken.value == "Chtl") {
        importType = ImportType::Chtl;
    } else {
        throw std::runtime_error("Unsupported import type: " + typeToken.value);
    }

    Expect(TokenType::From); // "from"
    Token pathToken = Expect(TokenType::StringLiteral);
    Expect(TokenType::Semicolon);

    auto importNode = std::make_shared<ImportNode>(importType, pathToken.value);
    m_context.AddImport(importNode.get());
    return importNode;
}

NodePtr CHTLParser::ParseNamespace() {
    Expect(TokenType::OpenBracket);
    Expect(TokenType::Identifier); // "Namespace"
    Expect(TokenType::CloseBracket);

    Token nameToken = Expect(TokenType::Identifier);
    m_namespace_stack.push_back(nameToken.value);

    NodeList content;
    if (Peek().type == TokenType::OpenBrace) {
        Expect(TokenType::OpenBrace);
        while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
            content.push_back(ParseNode());
        }
        Expect(TokenType::CloseBrace);
    } else {
        // Handle namespace without braces
        while (Peek().type != TokenType::EndOfFile) {
            // This is tricky. How to know when the namespace ends?
            // For now, I will assume it consumes the rest of the file.
            // This is a simplification and might need to be revisited.
            content.push_back(ParseNode());
        }
    }

    m_namespace_stack.pop_back();
    return std::make_shared<NamespaceNode>(nameToken.value, content);
}


// --- Full implementations are restored here ---
static std::map<TokenType, Precedence> precedences = {
    {TokenType::LogicalOr, LOGICAL_OR}, {TokenType::LogicalAnd, LOGICAL_AND},
    {TokenType::DoubleEquals, EQUALS}, {TokenType::NotEquals, EQUALS},
    {TokenType::LessThan, LESS_GREATER}, {TokenType::LessThanOrEquals, LESS_GREATER},
    {TokenType::GreaterThan, LESS_GREATER}, {TokenType::GreaterThanOrEquals, LESS_GREATER},
    {TokenType::Plus, SUM}, {TokenType::Minus, SUM},
    {TokenType::Star, PRODUCT}, {TokenType::Slash, PRODUCT}, {TokenType::Percent, PRODUCT},
    {TokenType::DoubleStar, POWER}, {TokenType::QuestionMark, TERNARY}
};
Token CHTLParser::Peek(size_t offset) { if (m_cursor + offset >= m_tokens.size()) return m_tokens.back(); return m_tokens[m_cursor + offset]; }
Token CHTLParser::Consume() { if (m_cursor >= m_tokens.size()) return m_tokens.back(); return m_tokens[m_cursor++]; }
void CHTLParser::SkipComments() { while (Peek().type == TokenType::LineComment || Peek().type == TokenType::BlockComment || Peek().type == TokenType::GeneratorComment) Consume(); }
Token CHTLParser::Expect(TokenType type) { Token token = Peek(); if (token.type != type) throw std::runtime_error("Unexpected token"); return Consume(); }
NodeList CHTLParser::Parse() {
    NodeList nodes;
    while (Peek().type != TokenType::EndOfFile) {
        SkipComments(); if(Peek().type == TokenType::EndOfFile) break;
        NodePtr node = ParseNode(); if (node) nodes.push_back(node);
    }
    return nodes;
}
Precedence CHTLParser::GetPrecedence() { if (precedences.count(Peek().type)) return precedences[Peek().type]; return LOWEST; }


ExpressionNodePtr CHTLParser::ParsePrefixExpression() {
    Token token = Peek();
    if (token.type == TokenType::Number) { Consume(); std::string unit; if (Peek().type == TokenType::Identifier) unit = Consume().value; return std::make_shared<NumberLiteralNode>(std::stod(token.value), unit); }
    if (token.type == TokenType::Identifier) {
        if (Peek(1).type == TokenType::OpenParen) return ParseVariableUsage();
        if (Peek(1).type == TokenType::Dot) { std::string selector = Consume().value; Consume(); std::string propName = Expect(TokenType::Identifier).value; return std::make_shared<PropertyReferenceNode>(selector, propName); }
        Consume(); return std::make_shared<SelfPropertyReferenceNode>(token.value);
    }
    if (token.type == TokenType::StringLiteral) { Consume(); return std::make_shared<StringLiteralNode>(token.value); }
    if (token.type == TokenType::OpenParen) { Consume(); ExpressionNodePtr expr = ParseExpression(LOWEST); Expect(TokenType::CloseParen); return expr; }
    throw std::runtime_error("Could not parse prefix expression for token: " + token.value);
}
ExpressionNodePtr CHTLParser::ParseInfixExpression(ExpressionNodePtr left) { Token op = Consume(); Precedence precedence = precedences[op.type]; ExpressionNodePtr right = ParseExpression(precedence); return std::make_shared<BinaryOpNode>(left, op, right); }
ExpressionNodePtr CHTLParser::ParseTernaryExpression(ExpressionNodePtr condition) { Consume(); ExpressionNodePtr trueBranch = ParseExpression(LOWEST); ExpressionNodePtr falseBranch = nullptr; if (Peek().type == TokenType::Colon) { Consume(); falseBranch = ParseExpression(LOWEST); } return std::make_shared<ConditionalExprNode>(condition, trueBranch, falseBranch); }
ExpressionNodePtr CHTLParser::ParseExpression(int precedence) { ExpressionNodePtr left = ParsePrefixExpression(); while (precedence < GetPrecedence()) { if (Peek().type == TokenType::QuestionMark) left = ParseTernaryExpression(left); else left = ParseInfixExpression(left); } return left; }
ExpressionNodePtr CHTLParser::ParseVariableUsage() {
    Token groupName = Expect(TokenType::Identifier);

    std::string from;
    if (Peek().type == TokenType::From) {
        Consume(); // Consume 'from'
        from = Expect(TokenType::Identifier).value;
    }

    Expect(TokenType::OpenParen);
    if (Peek().type == TokenType::CloseParen) throw std::runtime_error("Empty variable group usage is not allowed.");
    if (Peek(1).type == TokenType::Equals) {
        std::map<std::string, ExpressionNodePtr> specializations;
        while(Peek().type != TokenType::CloseParen) {
            Token varName = Expect(TokenType::Identifier); Expect(TokenType::Equals);
            ExpressionNodePtr value = ParseExpression(LOWEST);
            specializations[varName.value] = value;
            if (Peek().type == TokenType::Comma) Consume();
        }
        Expect(TokenType::CloseParen); return std::make_shared<VariableUsageNode>(groupName.value, specializations, from);
    } else {
        Token varName = Expect(TokenType::Identifier); Expect(TokenType::CloseParen);
        return std::make_shared<VariableUsageNode>(groupName.value, varName.value, from);
    }
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
            throw std::runtime_error("Unexpected token inside element: " + Peek().value);
        }
    }
    Expect(TokenType::CloseBrace); return element;
}
NodePtr CHTLParser::ParseTextBlock() {
    Expect(TokenType::Text); Expect(TokenType::OpenBrace);
    std::string textContent;
    while (Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
        textContent += Consume().value;
        if(Peek().type != TokenType::CloseBrace) textContent += " ";
    }
    Expect(TokenType::CloseBrace); return std::make_shared<TextNode>(textContent);
}
void CHTLParser::ParseStyleContent(std::shared_ptr<StyleNode> styleNode) {
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

            std::string from;
            if (Peek().type == TokenType::From) {
                Consume(); // Consume 'from'
                from = Expect(TokenType::Identifier).value; // This will need to handle dot-separated namespaces later
            }

            std::shared_ptr<StyleNode> specialization = nullptr;
            if (Peek().type == TokenType::OpenBrace) {
                Expect(TokenType::OpenBrace);
                specialization = std::make_shared<StyleNode>();
                ParseStyleContent(specialization);
                Expect(TokenType::CloseBrace);
            } else {
                Expect(TokenType::Semicolon);
            }
            styleNode->AddProperty({"__TEMPLATE_USAGE__", std::make_shared<TemplateUsageNode>(templateName.value, specialization, from)});
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
            auto nestedBlock = std::static_pointer_cast<StyleNode>(ParseStyleBlock(true)); // This recursive call is for nested rules like .class { &:hover { ... } }
            for(const auto& prop : nestedBlock->GetProperties()) styleRule->AddProperty(prop);
            styleNode->AddRule(styleRule);
        }
    }
}

NodePtr CHTLParser::ParseStyleBlock(bool isGlobal) {
    Expect(TokenType::Style);
    Expect(TokenType::OpenBrace);
    auto styleNode = std::make_shared<StyleNode>();
    ParseStyleContent(styleNode);
    Expect(TokenType::CloseBrace);
    return styleNode;
}

NodePtr CHTLParser::ParseTemplateDefinition() {
    Expect(TokenType::OpenBracket);
    Token definitionType = Expect(TokenType::Identifier); bool isCustom = (definitionType.value == "Custom");
    if (!isCustom && definitionType.value != "Template") throw std::runtime_error("Expected '[Template]' or '[Custom]'");
    Expect(TokenType::CloseBracket); Expect(TokenType::At);
    Token typeToken = Expect(TokenType::Identifier);
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::Style;
    else if (typeToken.value == "Element") type = TemplateType::Element;
    else if (typeToken.value == "Var") type = TemplateType::Var;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);
    Token nameToken = Expect(TokenType::Identifier);

    NodeList content;
    Expect(TokenType::OpenBrace);
    if (type == TemplateType::Element) {
        while(Peek().type != TokenType::CloseBrace && Peek().type != TokenType::EndOfFile) {
            NodePtr node = ParseNode(); if (node) content.push_back(node);
        }
    } else { // Style or Var
        auto styleNode = std::make_shared<StyleNode>();
        ParseStyleContent(styleNode);
        content.push_back(styleNode);
    }
    Expect(TokenType::CloseBrace);
    auto templateNode = std::make_shared<TemplateDefinitionNode>(nameToken.value, type, content, isCustom);

    std::string full_name = GetCurrentNamespace();
    if (!full_name.empty()) {
        full_name += ".";
    }
    full_name += templateNode->GetName();

    m_context.AddTemplate(full_name, templateNode.get());
    return templateNode;
}

NodePtr CHTLParser::ParseOriginBlock() {
    Expect(TokenType::OpenBracket);
    Expect(TokenType::Identifier); // "Origin"
    Expect(TokenType::CloseBracket);
    Expect(TokenType::At);
    Token typeToken = Expect(TokenType::Identifier);
    OriginType originType;
    if (typeToken.value == "Html") {
        originType = OriginType::Html;
    } else if (typeToken.value == "Style") {
        originType = OriginType::Style;
    } else if (typeToken.value == "JavaScript") {
        originType = OriginType::JavaScript;
    } else {
        throw std::runtime_error("Unknown origin type: " + typeToken.value);
    }

    Token nameToken = Expect(TokenType::Identifier);
    Token openBrace = Expect(TokenType::OpenBrace);

    size_t startPos = openBrace.pos + 1;
    size_t braceCount = 1;
    size_t currentPos = startPos;
    while (braceCount > 0 && currentPos < m_source.length()) {
        if (m_source[currentPos] == '{') {
            braceCount++;
        } else if (m_source[currentPos] == '}') {
            braceCount--;
        }
        currentPos++;
    }

    if (braceCount != 0) {
        throw std::runtime_error("Unmatched braces in origin block");
    }

    size_t endPos = currentPos - 1;
    std::string content = m_source.substr(startPos, endPos - startPos);
    std::string trimmed_content = trim(content);

    // Manually advance the cursor past the raw content
    m_cursor = 0; // Reset cursor to search from the beginning
    while(m_cursor < m_tokens.size() && m_tokens[m_cursor].pos < endPos) {
        m_cursor++;
    }
    Expect(TokenType::CloseBrace);

    return std::make_shared<OriginNode>(originType, nameToken.value, trimmed_content);
}


} // namespace CHTL
