#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/ConfigNode.h"
#include "../CHTLContext/Configuration.h"
#include "../Expression/Expr.h"
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(const std::string& source, const std::vector<Token>& tokens, const std::string& file_path, std::shared_ptr<Configuration> config);
    std::unique_ptr<BaseNode> parse();
    std::unique_ptr<ConfigNode> preParseConfiguration();
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& getTemplateDefinitions() const;
    std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& getMutableTemplateDefinitions();

private:
    const std::string& source;
    const std::vector<Token>& tokens;
    const std::string file_path;
    std::string current_namespace;
    int current = 0;
    std::shared_ptr<Configuration> config;

    std::unique_ptr<ConfigNode> parseConfigurationBlock();

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    Token consume(TokenType type, const std::string& message);
    bool match(const std::vector<TokenType>& types);
    void error(const Token& token, const std::string& message);

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
    std::unique_ptr<BaseNode> parseOriginBlock();

    std::map<std::string, std::map<std::string, TemplateDefinitionNode>> template_definitions;
    void parseSymbolDeclaration(bool is_custom);
    void parseImportStatement();
    void parseStyleTemplateUsage(StyleNode* styleNode);
    std::vector<std::unique_ptr<BaseNode>> parseElementTemplateUsage();
};

} // namespace CHTL

#endif // CHTL_PARSER_H
