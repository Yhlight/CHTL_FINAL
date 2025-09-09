#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/ModuleNode.h"
#include "../CHTLNode/RawBlockNode.h"
#include "../CHTLNode/OperatorNode.h"
#include "../CHTLContext/Context.h"
#include <vector>
#include <string>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer, Context& context);

    std::unique_ptr<Program> parseProgram();
    const std::vector<std::string>& getErrors() const;

private:
    void nextToken();
    bool expectPeek(TokenType t);
    void peekError(TokenType t);

    // Pratt Parser related declarations
    using prefixParseFn = std::unique_ptr<Expression> (Parser::*)();
    using infixParseFn = std::unique_ptr<Expression> (Parser::*)(std::unique_ptr<Expression>);

    enum Precedence {
        LOWEST,
        TERNARY, // ?
        SUM,     // +
        PRODUCT, // *
        CALL,    // .
        PREFIX   // -X or !X
    };

    void registerPrefix(TokenType type, prefixParseFn fn);
    void registerInfix(TokenType type, infixParseFn fn);
    Precedence peekPrecedence();
    Precedence curPrecedence();

    // Main parsing methods
    std::unique_ptr<Statement> parseStatement(bool is_global_scope);
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<AttributeNode> parseAttributeNode();
    std::unique_ptr<Expression> parseExpression(Precedence precedence);
    std::unique_ptr<StyleNode> parseStyleNode();
    std::unique_ptr<StylePropertyNode> parseStylePropertyNode();
    std::unique_ptr<StyleRuleNode> parseStyleRuleNode();
    std::unique_ptr<TemplateNode> parseTemplateNode();

    // Prefix and Infix parsing functions for Pratt parser
    std::unique_ptr<Expression> parseIdentifier();
    std::unique_ptr<Expression> parseNumberLiteral();
    std::unique_ptr<Expression> parseInfixExpression(std::unique_ptr<Expression> left);
    std::unique_ptr<Expression> parseConditionalExpression(std::unique_ptr<Expression> condition);
    std::unique_ptr<Expression> parsePropertyReferenceExpression(std::unique_ptr<Expression> left);
    std::unique_ptr<Expression> parseCallLikeExpression(std::unique_ptr<Expression> function);
    std::unique_ptr<Expression> parseIndexExpression(std::unique_ptr<Expression> left);
    std::unique_ptr<TemplateInstantiationNode> parseTemplateInstantiationNode();
    std::unique_ptr<LocalScriptNode> parseLocalScriptNode();
    std::unique_ptr<DeleteNode> parseDeleteNode();
    std::unique_ptr<NamespaceNode> parseNamespaceNode();
    std::unique_ptr<ImportNode> parseImportNode();
    std::unique_ptr<RawBlockNode> parseRawBlockNode(RawBlockType type);


    Lexer& m_lexer;
    Context& m_context;
    std::vector<std::string> m_errors;

    Token m_currentToken;
    Token m_peekToken;

    // Pratt parser members
    std::unordered_map<TokenType, prefixParseFn> m_prefixParseFns;
    std::unordered_map<TokenType, infixParseFn> m_infixParseFns;
    std::unordered_map<TokenType, Precedence> m_precedences;
};

} // namespace CHTL
