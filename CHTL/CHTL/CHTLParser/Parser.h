#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/ModuleNode.h"
#include "../CHTLNode/RawBlockNode.h"
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

    std::unique_ptr<Statement> parseStatement(bool is_global_scope);
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<AttributeNode> parseAttributeNode();
    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<StyleNode> parseStyleNode();
    std::unique_ptr<StylePropertyNode> parseStylePropertyNode();
    std::unique_ptr<StyleRuleNode> parseStyleRuleNode();
    std::unique_ptr<TemplateNode> parseTemplateNode();
    std::unique_ptr<TemplateInstantiationNode> parseTemplateInstantiationNode();
    std::unique_ptr<NamespaceNode> parseNamespaceNode();
    std::unique_ptr<ImportNode> parseImportNode();
    std::unique_ptr<RawBlockNode> parseRawBlockNode(RawBlockType type);


    Lexer& m_lexer;
    Context& m_context;
    std::vector<std::string> m_errors;

    Token m_currentToken;
    Token m_peekToken;
};

} // namespace CHTL
