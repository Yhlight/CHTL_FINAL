#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/ScriptBlockNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ConfigurationNode.h"
#include "../CHTLNode/PropertyValue.h"
#include "ParserContext.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <set>

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const std::string& source, CHTLLoader& loader, const std::string& initial_path, std::shared_ptr<ParserContext> context);
    std::unique_ptr<RootNode> parse();

private:
    const std::string source_;
    std::vector<Token> tokens_;
    void applySpecializations(std::vector<std::unique_ptr<Node>>& target_nodes);
    std::vector<std::unique_ptr<Node>> parseDeclaration();
    std::unique_ptr<NamespaceNode> parseNamespaceBlock();
    std::unique_ptr<ImportNode> parseImportStatement();
    void parseExportStatement();
    std::unique_ptr<OriginNode> parseOriginBlock();
    std::unique_ptr<ConfigurationNode> parseConfigurationBlock();
    void parseTemplateDefinition(bool is_custom);
    std::unique_ptr<ElementNode> parseElement();
    void parseElementBody(ElementNode& element);
    std::vector<PropertyValue> parsePropertyValue();
    std::unique_ptr<TextNode> parseText();
    std::unique_ptr<StyleBlockNode> parseStyleBlock();
    std::unique_ptr<ScriptBlockNode> parseScriptBlock();
    std::unique_ptr<CommentNode> parseGeneratorComment();
    void consumeColonOrEquals();
    void checkConstraints(const ElementNode& parent, const Node& child);
    void applyStyleTemplate(
        StyleBlockNode& styleNode,
        const std::string& template_name,
        const std::unordered_map<std::string, std::vector<PropertyValue>>& provided_values,
        const std::unordered_set<std::string>& deleted_properties,
        const std::unordered_set<std::string>& deleted_templates,
        std::set<std::string>& visited_templates
    );


    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    const Token& peekNext() const;
    const Token& consume(TokenType type, const std::string& message);
    void synchronize();

    enum class TemplateType { Style, Element, Var };
    std::string resolveUnqualifiedName(const std::string& name, TemplateType type);

    CHTLLoader& loader_;
    std::string current_path_;
    std::shared_ptr<ParserContext> context_;
    size_t current_ = 0;
public:
    std::string current_namespace_;
    bool hadError_ = false;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
