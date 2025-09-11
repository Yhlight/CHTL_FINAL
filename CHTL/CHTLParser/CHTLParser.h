#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/ExpressionNode.h"
#include "CHTLNode/OriginNode.h"
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

class CompilationContext; // Forward declaration

enum Precedence { LOWEST, TERNARY, LOGICAL_OR, LOGICAL_AND, EQUALS, LESS_GREATER, SUM, PRODUCT, POWER, PREFIX };

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens, std::string source, CompilationContext& context);
    NodeList Parse();
    void PushNamespace(const std::string& name);
    void PopNamespace();

private:
    std::vector<Token> m_tokens;
    std::string m_source;
    CompilationContext& m_context;
    std::vector<std::string> m_namespace_stack;
    size_t m_cursor = 0;

    // Token helpers
    std::string GetCurrentNamespace() const;
    Token Peek(size_t offset = 0);
    Token Consume();
    Token Expect(TokenType type);
    void SkipComments();

    // Node Parsers
    NodePtr ParseNode();
    NodePtr ParseNamespace();
    NodePtr ParseElement();
    NodePtr ParseStyleBlock(bool isGlobal);
    void ParseStyleContent(std::shared_ptr<class StyleNode> styleNode);
    NodePtr ParseTextBlock();
    NodePtr ParseTemplateDefinition();
    NodePtr ParseElementTemplateUsage();
    NodePtr ParseOriginBlock();
    NodePtr ParseImportStatement();

    // Expression (Pratt) Parser
    Precedence GetPrecedence();
    ExpressionNodePtr ParseExpression(int precedence = LOWEST);
    ExpressionNodePtr ParsePrefixExpression();
    ExpressionNodePtr ParseInfixExpression(ExpressionNodePtr left);
    ExpressionNodePtr ParseTernaryExpression(ExpressionNodePtr condition);
    ExpressionNodePtr ParseVariableUsage();

    // Element Specialization Parsers
    NodePtr ParseElementModification();
    NodePtr ParseElementDeletion();
    NodePtr ParseElementInsertion();
};

} // namespace CHTL
