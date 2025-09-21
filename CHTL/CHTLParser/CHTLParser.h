#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h" // Include full definition for std::unique_ptr
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/CustomDeclarationNode.h"
#include "../CHTLNode/ImportNode.h"
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// The parser takes a stream of tokens and produces an Abstract Syntax Tree.
class CHTLParser {
public:
    bool suppress_not_found_errors = false;
    explicit CHTLParser(const std::string& source, const std::vector<Token>& tokens, std::map<std::string, TemplateDefinitionNode>& templates, std::string default_namespace_alias = "");

    // The main entry point for parsing.
    std::unique_ptr<BaseNode> parse();

    // This performs a lightweight scan to find all import paths and their aliases.
    std::vector<std::unique_ptr<ImportNode>> discoverImports();

    std::map<std::string, TemplateDefinitionNode> takeTemplateDefinitions() { return std::move(template_definitions); }

private:
    const std::string& source;
    const std::vector<Token>& tokens;
    int current = 0;
    std::string current_namespace = "";
    std::string default_namespace_alias = "";

    // --- Helper Methods ---
    // These methods provide a clean interface for consuming and looking at tokens.

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool match(const std::vector<TokenType>& types);

    // --- Expression Parsing Methods ---
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseConditional();
    std::unique_ptr<Expr> parseLogicalOr();
    std::unique_ptr<Expr> parseLogicalAnd();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parsePower();
    std::unique_ptr<Expr> parsePrimary();

    // --- Document Parsing Methods ---
    std::vector<std::unique_ptr<BaseNode>> parseDeclaration();
    std::unique_ptr<ElementNode> parseElement();
    void parseAttribute(ElementNode* element);
    std::unique_ptr<StyleNode> parseStyleBlock();
    std::unique_ptr<ImportNode> parseImportStatement();

    // --- Error Handling ---
    void error(const Token& token, const std::string& message);

    // --- Symbol Table for Templates ---
    std::map<std::string, TemplateDefinitionNode>& template_definitions;
    std::unique_ptr<BaseNode> parseTopLevelDeclaration(); // New method
    std::unique_ptr<TemplateDeclarationNode> parseTemplateDeclaration();
    std::unique_ptr<CustomDeclarationNode> parseCustomDeclaration();
    std::unique_ptr<NamespaceNode> parseNamespace();
    void parseStyleTemplateUsage(StyleNode* styleNode);
    std::vector<std::unique_ptr<BaseNode>> parseElementTemplateUsage();
    std::unique_ptr<BaseNode> parseOriginBlock();
};

} // namespace CHTL

#endif // CHTL_PARSER_H
