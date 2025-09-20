#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/CustomDeclarationNode.h"
#include "../CHTLNode/ImportNode.h"
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

class CHTLParser {
public:
    bool suppress_not_found_errors = false;
    explicit CHTLParser(const std::string& source, const std::vector<Token>& tokens, std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>& templates);

    std::unique_ptr<BaseNode> parse();
    std::vector<std::string> discoverImports();

private:
    const std::string& source;
    const std::vector<Token>& tokens;
    size_t current = 0;

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool match(const std::vector<TokenType>& types);

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

    std::vector<std::unique_ptr<BaseNode>> parseDeclaration();
    std::unique_ptr<ElementNode> parseElement();
    void parseAttribute(ElementNode* element);
    std::unique_ptr<StyleNode> parseStyleBlock();
    std::unique_ptr<ImportNode> parseImportStatement();

    void error(const Token& token, const std::string& message);

    std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>& template_definitions;
    std::unique_ptr<BaseNode> parseTopLevelDeclaration();
    std::unique_ptr<TemplateDeclarationNode> parseTemplateDeclaration();
    std::unique_ptr<CustomDeclarationNode> parseCustomDeclaration();
    void parseStyleTemplateUsage(StyleNode* styleNode);
    std::vector<std::unique_ptr<BaseNode>> parseElementTemplateUsage();
    std::unique_ptr<BaseNode> parseOriginBlock();
};

} // namespace CHTL

#endif // CHTL_PARSER_H
