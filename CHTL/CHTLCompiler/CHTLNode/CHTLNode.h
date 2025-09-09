#ifndef CHTLNODE_H
#define CHTLNODE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

namespace CHTL {

// Forward declarations
class CHTLNode;
class TextNode;
class ElementNode;
class AttributeNode;
class StyleNode;
class ScriptNode;
class TemplateNode;
class CustomNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;
class UseNode;
class OriginNode;

// Node types
enum class NodeType {
    Text,
    Element,
    Attribute,
    Style,
    Script,
    Template,
    Custom,
    Import,
    Namespace,
    Configuration,
    Use,
    Origin,
    Root
};

// Base node class
class CHTLNode {
public:
    NodeType node_type;
    std::string value;
    size_t line;
    size_t column;
    std::vector<std::shared_ptr<CHTLNode>> children;
    std::map<std::string, std::string> attributes;

    CHTLNode(NodeType type, const std::string& val = "", size_t l = 0, size_t c = 0);
    virtual ~CHTLNode() = default;
    
    virtual std::string toString() const;
    virtual std::string toHTML() const;
    
    void addChild(std::shared_ptr<CHTLNode> child);
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
};

// Text node
class TextNode : public CHTLNode {
public:
    TextNode(const std::string& text, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
};

// Element node
class ElementNode : public CHTLNode {
public:
    std::string tag_name;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<CHTLNode>> content;
    
    ElementNode(const std::string& tag, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void addAttribute(std::shared_ptr<AttributeNode> attr);
    void addContent(std::shared_ptr<CHTLNode> node);
};

// Attribute node
class AttributeNode : public CHTLNode {
public:
    std::string attribute_name;
    std::string attribute_value;
    bool is_quoted;
    
    AttributeNode(const std::string& name, const std::string& value, bool quoted = true, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
};

// Style node
class StyleNode : public CHTLNode {
public:
    std::vector<std::shared_ptr<CHTLNode>> style_rules;
    
    StyleNode(size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void addStyleRule(std::shared_ptr<CHTLNode> rule);
};

// Script node
class ScriptNode : public CHTLNode {
public:
    std::vector<std::shared_ptr<CHTLNode>> script_content;
    
    ScriptNode(size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void addScriptContent(std::shared_ptr<CHTLNode> content);
};

// Template node
class TemplateNode : public CHTLNode {
public:
    std::string template_type; // Style, Element, Var
    std::string template_name;
    std::vector<std::shared_ptr<CHTLNode>> template_content;
    
    TemplateNode(const std::string& type, const std::string& name, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void addTemplateContent(std::shared_ptr<CHTLNode> content);
};

// Custom node
class CustomNode : public CHTLNode {
public:
    std::string custom_type; // Style, Element, Var
    std::string custom_name;
    std::vector<std::shared_ptr<CHTLNode>> custom_content;
    
    CustomNode(const std::string& type, const std::string& name, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void addCustomContent(std::shared_ptr<CHTLNode> content);
};

// Import node
class ImportNode : public CHTLNode {
public:
    std::string import_path;
    std::string import_type; // HTML, CSS, JS, CHTL
    std::string import_name;
    bool is_wildcard;
    
    ImportNode(const std::string& path, const std::string& type, const std::string& name = "", bool wildcard = false, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
};

// Namespace node
class NamespaceNode : public CHTLNode {
public:
    std::string namespace_name;
    std::vector<std::shared_ptr<CHTLNode>> namespace_content;
    
    NamespaceNode(const std::string& name, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void addNamespaceContent(std::shared_ptr<CHTLNode> content);
};

// Configuration node
class ConfigurationNode : public CHTLNode {
public:
    std::string config_name;
    std::map<std::string, std::string> config_options;
    
    ConfigurationNode(const std::string& name, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void setConfigOption(const std::string& key, const std::string& value);
};

// Use node
class UseNode : public CHTLNode {
public:
    std::string use_target; // html5, config group name
    std::vector<std::shared_ptr<CHTLNode>> use_content;
    
    UseNode(const std::string& target, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
    
    void addUseContent(std::shared_ptr<CHTLNode> content);
};

// Origin node
class OriginNode : public CHTLNode {
public:
    std::string origin_type; // HTML, CSS, JS
    std::string origin_name;
    std::string origin_content;
    
    OriginNode(const std::string& type, const std::string& name, const std::string& content, size_t l = 0, size_t c = 0);
    std::string toHTML() const override;
};

// Root node
class RootNode : public CHTLNode {
public:
    std::vector<std::shared_ptr<CHTLNode>> root_content;
    
    RootNode();
    std::string toHTML() const override;
    
    void addRootContent(std::shared_ptr<CHTLNode> content);
};

} // namespace CHTL

#endif // CHTLNODE_H