#ifndef CHTLPARSER_H
#define CHTLPARSER_H

#include <vector>
#include <memory>
#include <string>
#include <map>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

class CHTLParser {
public:
    CHTLParser(const std::vector<Token>& tokens);
    ~CHTLParser() = default;
    
    std::shared_ptr<RootNode> parse();
    
private:
    std::vector<Token> tokens;
    size_t current_token;
    
    // Token management
    bool isAtEnd() const;
    Token current() const;
    Token peek() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    
    // Parsing methods
    std::shared_ptr<CHTLNode> parseNode();
    std::shared_ptr<TextNode> parseTextNode();
    std::shared_ptr<ElementNode> parseElementNode();
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<StyleNode> parseStyleNode();
    std::shared_ptr<ScriptNode> parseScriptNode();
    std::shared_ptr<TemplateNode> parseTemplateNode();
    std::shared_ptr<CustomNode> parseCustomNode();
    std::shared_ptr<ImportNode> parseImportNode();
    std::shared_ptr<NamespaceNode> parseNamespaceNode();
    std::shared_ptr<ConfigurationNode> parseConfigurationNode();
    std::shared_ptr<UseNode> parseUseNode();
    std::shared_ptr<OriginNode> parseOriginNode();
    
    // Helper methods
    std::string parseStringLiteral();
    std::string parseIdentifier();
    std::string parseUnquotedString();
    void parseAttributes(ElementNode* element);
    void parseStyleRules(StyleNode* style);
    void parseScriptContent(ScriptNode* script);
    void parseTemplateContent(TemplateNode* template_node);
    void parseCustomContent(CustomNode* custom_node);
    void parseNamespaceContent(NamespaceNode* namespace_node);
    void parseConfigurationOptions(ConfigurationNode* config);
    void parseUseContent(UseNode* use_node);
    
    // Error handling
    void error(const std::string& message);
    void synchronize();
    
    // Context management
    std::map<std::string, std::shared_ptr<TemplateNode>> templates;
    std::map<std::string, std::shared_ptr<CustomNode>> customs;
    std::map<std::string, std::shared_ptr<ConfigurationNode>> configurations;
    std::string current_namespace;
};

} // namespace CHTL

#endif // CHTLPARSER_H