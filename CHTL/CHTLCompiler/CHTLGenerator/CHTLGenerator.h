#ifndef CHTLGENERATOR_H
#define CHTLGENERATOR_H

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

class CHTLGenerator {
public:
    CHTLGenerator();
    ~CHTLGenerator() = default;
    
    // Main generation method
    std::string generateHTML(std::shared_ptr<RootNode> ast);
    
    // Node-specific generation methods
    std::string generateTextNode(std::shared_ptr<TextNode> node);
    std::string generateElementNode(std::shared_ptr<ElementNode> node);
    std::string generateStyleNode(std::shared_ptr<StyleNode> node);
    std::string generateScriptNode(std::shared_ptr<ScriptNode> node);
    std::string generateTemplateNode(std::shared_ptr<TemplateNode> node);
    std::string generateCustomNode(std::shared_ptr<CustomNode> node);
    std::string generateImportNode(std::shared_ptr<ImportNode> node);
    std::string generateNamespaceNode(std::shared_ptr<NamespaceNode> node);
    std::string generateConfigurationNode(std::shared_ptr<ConfigurationNode> node);
    std::string generateUseNode(std::shared_ptr<UseNode> node);
    std::string generateOriginNode(std::shared_ptr<OriginNode> node);
    
    // Template and custom resolution
    void registerTemplate(std::shared_ptr<TemplateNode> template_node);
    void registerCustom(std::shared_ptr<CustomNode> custom_node);
    std::shared_ptr<TemplateNode> resolveTemplate(const std::string& name);
    std::shared_ptr<CustomNode> resolveCustom(const std::string& name);
    
    // HTML structure generation
    std::string generateHTMLDocument(std::shared_ptr<RootNode> ast);
    std::string generateHTMLHead();
    std::string generateHTMLBody(const std::string& content);
    
    // CSS generation
    std::string generateCSS(std::shared_ptr<StyleNode> style_node);
    std::string generateCSSRules(const std::vector<std::shared_ptr<CHTLNode>>& rules);
    
    // JavaScript generation
    std::string generateJavaScript(std::shared_ptr<ScriptNode> script_node);
    
    // Utility methods
    std::string escapeHTML(const std::string& text);
    std::string formatHTML(const std::string& html);
    bool shouldFormatOutput() const;
    void setFormatOutput(bool format);
    
private:
    // Template and custom registries
    std::map<std::string, std::shared_ptr<TemplateNode>> templates;
    std::map<std::string, std::shared_ptr<CustomNode>> customs;
    
    // Configuration
    bool format_output;
    int indent_level;
    std::string indent_string;
    
    // Helper methods
    std::string getIndent();
    void increaseIndent();
    void decreaseIndent();
    std::string generateAttributes(const std::vector<std::shared_ptr<AttributeNode>>& attributes);
    std::string generateElementContent(const std::vector<std::shared_ptr<CHTLNode>>& content);
    std::string resolveElementReference(const std::string& ref_type, const std::string& ref_name);
    std::string generateTemplateContent(std::shared_ptr<TemplateNode> template_node);
    std::string generateCustomContent(std::shared_ptr<CustomNode> custom_node);
};

} // namespace CHTL

#endif // CHTLGENERATOR_H