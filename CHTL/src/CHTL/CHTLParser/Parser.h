#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLContext/CHTLContext.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/BaseNode.h"
// All other specific nodes are included by Parser.cpp as needed
#include "../ExpressionNode/Expr.h"

#include <vector>
#include <memory>
#include <string>
#include <initializer_list>

namespace CHTL {

class ElementNode; // Forward-declare instead of include

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens, CHTLContext& context);
    std::unique_ptr<DocumentNode> parse();

private:
    // Main parsing functions
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<BaseNode> parseTextElement(); // Can return different types
    void parseAttributes(ElementNode* element);
    std::string parseIdentifierSequence();

    // Template & Custom parsing
    void parseTopLevelDefinition();
    void parseTemplateDefinition();
    std::unique_ptr<BaseNode> parseUsage();
    void parseCustomDefinition();
    std::unique_ptr<BaseNode> parseCustomUsage();
    std::unique_ptr<BaseNode> parseSpecializationRule();


    // Style-parsing functions
    std::unique_ptr<BaseNode> parseStyleBlock(); // Can return different types
    std::unique_ptr<BaseNode> parseStyleContent();
    std::unique_ptr<BaseNode> parseStyleProperty(); // Can return different types
    std::unique_ptr<BaseNode> parseStyleSelector(); // Can return different types

    // Expression-parsing functions
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseTernary();
    std::unique_ptr<Expr> parseLogicalOr();
    std::unique_ptr<Expr> parseLogicalAnd();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parsePrimary();

    // Token manipulation helpers
    const Token& peek() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(std::initializer_list<TokenType> types);
    const Token& consume(TokenType type, const std::string& message);

    const std::vector<Token>& m_tokens;
    CHTLContext& m_context;
    size_t m_current = 0;
};

} // namespace CHTL
