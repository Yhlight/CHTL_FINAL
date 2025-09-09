#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLContext/CHTLContext.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/VariableDefinitionNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../ExpressionNode/Expr.h"

#include <vector>
#include <memory>
#include <string>
#include <initializer_list>

namespace CHTL {

class ElementNode; // Forward-declare
class CHTLLoader; // Forward-declare
class OriginNode; // Forward-declare

class Parser {
public:
    // Constructor now takes the loader to handle imports, and the current file path for context
    explicit Parser(const std::string& source, const std::vector<Token>& tokens, CHTLContext& context, CHTLLoader& loader, std::string current_path);

    std::unique_ptr<DocumentNode> parse();

private:
    // Main parsing functions
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<BaseNode> parseTextElement();
    std::unique_ptr<BaseNode> parseGeneratorComment();
    void parseAttributes(ElementNode* element);
    void parseTextAttribute(ElementNode* element);
    std::string parseIdentifierSequence();

    // Top Level Directives
    void parseTopLevelDefinition();
    void parseTemplateDefinition();
    void parseVariableGroupDefinition();
    std::unique_ptr<VariableDefinitionNode> parseVariableDefinition();
    void parseCustomDefinition();
    void parseNamespaceDefinition();
    void parseImportDefinition();

    // Usage Directives
    std::unique_ptr<BaseNode> parseUsage();
    std::unique_ptr<BaseNode> parseCustomUsage();
    std::unique_ptr<BaseNode> parseOrigin();
    std::unique_ptr<BaseNode> parseSpecializationRule();
    std::unique_ptr<BaseNode> parseInsertRule();
    std::unique_ptr<BaseNode> parseInherit();

    // Style-parsing functions
    std::unique_ptr<BaseNode> parseStyleBlock();
    std::unique_ptr<BaseNode> parseScriptBlock();
    std::unique_ptr<BaseNode> parseStyleContent();
    std::unique_ptr<BaseNode> parseStyleProperty();
    std::unique_ptr<BaseNode> parseStyleSelector();

    // Expression-parsing functions
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseTernary();
    std::unique_ptr<Expr> parseLogicalOr();
    std::unique_ptr<Expr> parseLogicalAnd();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parsePrimary();

    // Token manipulation helpers
    bool check(TokenType type) const;
    const Token& peek() const;
    const Token& advance();
    bool isAtEnd() const;
    bool match(std::initializer_list<TokenType> types);
    const Token& consume(TokenType type, const std::string& message);

    const std::string& m_source;
    const std::vector<Token>& m_tokens;
    CHTLContext& m_context;
    CHTLLoader& m_loader; // Reference to the loader for handling imports
    std::string m_current_path; // The path of the file being parsed
    std::string m_current_namespace; // The current active namespace
    size_t m_current = 0;
};

} // namespace CHTL
