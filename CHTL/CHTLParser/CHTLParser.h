#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/Node.h"
#include <vector>
#include <memory>

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    std::unique_ptr<ProgramNode> parse();

private:
    const std::vector<Token>& tokens;
    size_t current = 0;

    // Parsing methods
    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    std::unique_ptr<StyleNode> parseStyle();
    std::unique_ptr<SelectorNode> parseSelector();
    std::unique_ptr<AttributeNode> parseAttribute();
    std::unique_ptr<ExpressionNode> parseExpression();
    std::unique_ptr<TemplateDefinitionNode> parseTemplateDefinition();
    std::unique_ptr<TemplateUsageNode> parseTemplateUsage();
    std::unique_ptr<DeleteNode> parseDelete();

    // Pratt parser helpers
    enum Precedence {
        PREC_NONE,
        PREC_ASSIGNMENT,
        PREC_TERM,       // + -
        PREC_FACTOR,     // * / %
        PREC_POWER,      // **
        PREC_UNARY,      // -
        PREC_PRIMARY
    };
    struct ParseRule;
    typedef std::unique_ptr<ExpressionNode> (CHTLParser::*PrefixParseFn)();
    typedef std::unique_ptr<ExpressionNode> (CHTLParser::*InfixParseFn)(std::unique_ptr<ExpressionNode>);
    struct ParseRule {
        PrefixParseFn prefix;
        InfixParseFn infix;
        Precedence precedence;
    };

    ParseRule* getRule(TokenType type);
    std::unique_ptr<ExpressionNode> parsePrecedence(Precedence precedence);
    std::unique_ptr<ExpressionNode> parseLiteral();
    std::unique_ptr<ExpressionNode> parseGrouping();
    std::unique_ptr<ExpressionNode> parseUnary();
    std::unique_ptr<ExpressionNode> parseBinary(std::unique_ptr<ExpressionNode> left);

    // Token helpers
    const Token& peek();
    const Token& previous();
    const Token& advance();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& message);

    // Error handling
    void synchronize();
    // In a real parser, we'd have a more robust error reporting system.
};
