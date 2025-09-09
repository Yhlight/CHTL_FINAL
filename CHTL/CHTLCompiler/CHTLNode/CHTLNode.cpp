#include "CHTLNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// Base node implementation
CHTLNode::CHTLNode(NodeType type, const std::string& val, size_t l, size_t c)
    : node_type(type), value(val), line(l), column(c) {
}

std::string CHTLNode::toString() const {
    std::ostringstream oss;
    oss << "Node(" << static_cast<int>(node_type) << ", \"" << value << "\", " << line << ":" << column << ")";
    return oss.str();
}

std::string CHTLNode::toHTML() const {
    return value;
}

void CHTLNode::addChild(std::shared_ptr<CHTLNode> child) {
    children.push_back(child);
}

void CHTLNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string CHTLNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

bool CHTLNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

// Text node implementation
TextNode::TextNode(const std::string& text, size_t l, size_t c)
    : CHTLNode(NodeType::Text, text, l, c) {
}

std::string TextNode::toHTML() const {
    // Escape HTML special characters
    std::string escaped = value;
    for (size_t i = 0; i < escaped.length(); ++i) {
        switch (escaped[i]) {
            case '&':
                escaped.replace(i, 1, "&amp;");
                i += 4; // Skip the replaced characters
                break;
            case '<':
                escaped.replace(i, 1, "&lt;");
                i += 3;
                break;
            case '>':
                escaped.replace(i, 1, "&gt;");
                i += 3;
                break;
            case '"':
                escaped.replace(i, 1, "&quot;");
                i += 5;
                break;
            case '\'':
                escaped.replace(i, 1, "&#39;");
                i += 4;
                break;
        }
    }
    return escaped;
}

// Element node implementation
ElementNode::ElementNode(const std::string& tag, size_t l, size_t c)
    : CHTLNode(NodeType::Element, tag, l, c), tag_name(tag) {
}

std::string ElementNode::toHTML() const {
    std::ostringstream oss;
    oss << "<" << tag_name;
    
    // Add parsed attributes
    for (const auto& attr : attributes) {
        oss << " " << attr->attribute_name;
        if (!attr->attribute_value.empty()) {
            if (attr->is_quoted) {
                oss << "=\"" << attr->attribute_value << "\"";
            } else {
                oss << "=" << attr->attribute_value;
            }
        }
    }
    
    if (content.empty()) {
        oss << " />";
    } else {
        oss << ">";
        for (const auto& child : content) {
            oss << child->toHTML();
        }
        oss << "</" << tag_name << ">";
    }
    
    return oss.str();
}

void ElementNode::addAttribute(std::shared_ptr<AttributeNode> attr) {
    attributes.push_back(attr);
}

void ElementNode::addContent(std::shared_ptr<CHTLNode> node) {
    content.push_back(node);
}

// Attribute node implementation
AttributeNode::AttributeNode(const std::string& name, const std::string& value, bool quoted, size_t l, size_t c)
    : CHTLNode(NodeType::Attribute, value, l, c), attribute_name(name), attribute_value(value), is_quoted(quoted) {
}

std::string AttributeNode::toHTML() const {
    if (is_quoted) {
        return attribute_name + "=\"" + attribute_value + "\"";
    } else {
        return attribute_name + "=" + attribute_value;
    }
}

// Style node implementation
StyleNode::StyleNode(size_t l, size_t c)
    : CHTLNode(NodeType::Style, "", l, c) {
}

std::string StyleNode::toHTML() const {
    std::ostringstream oss;
    oss << "<style>";
    for (const auto& rule : style_rules) {
        oss << rule->toHTML();
    }
    oss << "</style>";
    return oss.str();
}

void StyleNode::addStyleRule(std::shared_ptr<CHTLNode> rule) {
    style_rules.push_back(rule);
}

// Script node implementation
ScriptNode::ScriptNode(size_t l, size_t c)
    : CHTLNode(NodeType::Script, "", l, c) {
}

std::string ScriptNode::toHTML() const {
    std::ostringstream oss;
    oss << "<script>";
    for (const auto& content : script_content) {
        oss << content->toHTML();
    }
    oss << "</script>";
    return oss.str();
}

void ScriptNode::addScriptContent(std::shared_ptr<CHTLNode> content) {
    script_content.push_back(content);
}

// Template node implementation
TemplateNode::TemplateNode(const std::string& type, const std::string& name, size_t l, size_t c)
    : CHTLNode(NodeType::Template, name, l, c), template_type(type), template_name(name) {
}

std::string TemplateNode::toHTML() const {
    // Templates don't generate HTML directly, they're used for code generation
    return "";
}

void TemplateNode::addTemplateContent(std::shared_ptr<CHTLNode> content) {
    template_content.push_back(content);
}

// Custom node implementation
CustomNode::CustomNode(const std::string& type, const std::string& name, size_t l, size_t c)
    : CHTLNode(NodeType::Custom, name, l, c), custom_type(type), custom_name(name) {
}

std::string CustomNode::toHTML() const {
    // Custom nodes don't generate HTML directly, they're used for code generation
    return "";
}

void CustomNode::addCustomContent(std::shared_ptr<CHTLNode> content) {
    custom_content.push_back(content);
}

// Import node implementation
ImportNode::ImportNode(const std::string& path, const std::string& type, const std::string& name, bool wildcard, size_t l, size_t c)
    : CHTLNode(NodeType::Import, path, l, c), import_path(path), import_type(type), import_name(name), is_wildcard(wildcard) {
}

std::string ImportNode::toHTML() const {
    if (import_type == "CSS") {
        return "<link rel=\"stylesheet\" href=\"" + import_path + "\">";
    } else if (import_type == "JS") {
        return "<script src=\"" + import_path + "\"></script>";
    } else if (import_type == "HTML") {
        // HTML imports are handled differently
        return "<!-- Import: " + import_path + " -->";
    }
    return "";
}

// Namespace node implementation
NamespaceNode::NamespaceNode(const std::string& name, size_t l, size_t c)
    : CHTLNode(NodeType::Namespace, name, l, c), namespace_name(name) {
}

std::string NamespaceNode::toHTML() const {
    // Namespaces don't generate HTML directly
    return "";
}

void NamespaceNode::addNamespaceContent(std::shared_ptr<CHTLNode> content) {
    namespace_content.push_back(content);
}

// Configuration node implementation
ConfigurationNode::ConfigurationNode(const std::string& name, size_t l, size_t c)
    : CHTLNode(NodeType::Configuration, name, l, c), config_name(name) {
}

std::string ConfigurationNode::toHTML() const {
    // Configuration nodes don't generate HTML directly
    return "";
}

void ConfigurationNode::setConfigOption(const std::string& key, const std::string& value) {
    config_options[key] = value;
}

// Use node implementation
UseNode::UseNode(const std::string& target, size_t l, size_t c)
    : CHTLNode(NodeType::Use, target, l, c), use_target(target) {
}

std::string UseNode::toHTML() const {
    if (use_target == "html5") {
        return "<!DOCTYPE html>";
    }
    return "";
}

void UseNode::addUseContent(std::shared_ptr<CHTLNode> content) {
    use_content.push_back(content);
}

// Origin node implementation
OriginNode::OriginNode(const std::string& type, const std::string& name, const std::string& content, size_t l, size_t c)
    : CHTLNode(NodeType::Origin, content, l, c), origin_type(type), origin_name(name), origin_content(content) {
}

std::string OriginNode::toHTML() const {
    return origin_content;
}

// Root node implementation
RootNode::RootNode() : CHTLNode(NodeType::Root, "", 0, 0) {
}

std::string RootNode::toHTML() const {
    std::ostringstream oss;
    for (const auto& content : root_content) {
        oss << content->toHTML();
    }
    return oss.str();
}

void RootNode::addRootContent(std::shared_ptr<CHTLNode> content) {
    root_content.push_back(content);
}

} // namespace CHTL