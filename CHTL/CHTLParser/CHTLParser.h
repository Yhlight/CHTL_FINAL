#pragma once

#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ProgramNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ScriptNode.h"
#include <vector>
#include <map>
#include <functional>

namespace CHTL {

enum Precedence {
    LOWEST,
    TERNARY,
    LOGICAL,
    SUM,
    PRODUCT,
    PREFIX
};

class CHTLParser {
public:
    CHTLParser(CHTLLexer& lexer, bool debug_mode = false);

    std::shared_ptr<ProgramNode> ParseProgram();
    const std::vector<std::string>& Errors() const { return m_errors; }

private:
    // General parsing helpers
    void pushError(const std::string& msg);
    void nextToken();
    bool currentTokenIs(TokenType type) const;
    bool peekTokenIs(TokenType type) const;
    bool expectPeek(TokenType type);
    Precedence currentPrecedence();
    Precedence peekPrecedence();

    // Statement-level parsing
    NodePtr parseStatement();
    std::shared_ptr<TemplateDefinitionNode> parseTemplateDefinition();
    NodePtr parseTemplateUsage();
    std::shared_ptr<ImportNode> parseImportStatement();

    // Node-specific parsing functions
    std::shared_ptr<ElementNode> parseElementNode();
    std::shared_ptr<TextNode> parseTextNodeShorthand();
    std::shared_ptr<StyleNode> parseStyleNode();
    std::shared_ptr<ScriptNode> parseScriptNode();

    // Expression parsing (Pratt parser)
    ExpressionPtr parseExpression(Precedence precedence);
    ExpressionPtr parseIdentifierAsExpression();
    ExpressionPtr parseInfixExpression(ExpressionPtr left);
    ExpressionPtr parseConditionalExpression(ExpressionPtr condition);

    // Member variables
    CHTLLexer& m_lexer;
    bool m_debug_mode;
    std::vector<std::string> m_errors;

    Token m_currentToken;
    Token m_peekToken;

    static const std::map<TokenType, Precedence> m_precedences;
};

} // namespace CHTL
