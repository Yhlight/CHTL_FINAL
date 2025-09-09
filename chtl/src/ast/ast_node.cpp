#include "ast/ast_node.h"
#include <sstream>
#include <algorithm>
#include <unordered_set>

namespace chtl {
namespace ast {

// ASTNode implementation
void ASTNode::add_child(NodePtr child) {
    if (child) {
        child->parent = shared_from_this();
        children.push_back(child);
    }
}

void ASTNode::remove_child(NodePtr child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
    }
}

void ASTNode::insert_child(size_t index, NodePtr child) {
    if (child && index <= children.size()) {
        child->parent = shared_from_this();
        children.insert(children.begin() + index, child);
    }
}

void ASTNode::clear_children() {
    for (auto& child : children) {
        child->parent = nullptr;
    }
    children.clear();
}

void ASTNode::set_attribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string ASTNode::get_attribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

bool ASTNode::has_attribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void ASTNode::remove_attribute(const std::string& key) {
    attributes.erase(key);
}

ASTNode::NodePtr ASTNode::find_child_by_name(const std::string& name) const {
    for (const auto& child : children) {
        if (child->name == name) {
            return child;
        }
    }
    return nullptr;
}

ASTNode::NodeList ASTNode::find_children_by_type(NodeType type) const {
    NodeList result;
    for (const auto& child : children) {
        if (child->type == type) {
            result.push_back(child);
        }
    }
    return result;
}

ASTNode::NodePtr ASTNode::find_descendant_by_name(const std::string& name) const {
    for (const auto& child : children) {
        if (child->name == name) {
            return child;
        }
        auto descendant = child->find_descendant_by_name(name);
        if (descendant) {
            return descendant;
        }
    }
    return nullptr;
}

ASTNode::NodeList ASTNode::find_descendants_by_type(NodeType type) const {
    NodeList result;
    for (const auto& child : children) {
        if (child->type == type) {
            result.push_back(child);
        }
        auto descendants = child->find_descendants_by_type(type);
        result.insert(result.end(), descendants.begin(), descendants.end());
    }
    return result;
}

std::string ASTNode::to_string() const {
    std::ostringstream oss;
    oss << "ASTNode(type=" << static_cast<int>(type) 
        << ", name=\"" << name << "\""
        << ", value=\"" << value << "\""
        << ", children=" << children.size() << ")";
    return oss.str();
}

std::string ASTNode::to_html() const {
    if (type == NodeType::ROOT) {
        std::ostringstream oss;
        for (const auto& child : children) {
            oss << child->to_html();
        }
        return oss.str();
    } else if (type == NodeType::TEMPLATE_STYLE || 
               type == NodeType::TEMPLATE_ELEMENT || 
               type == NodeType::TEMPLATE_VAR ||
               type == NodeType::CUSTOM_STYLE || 
               type == NodeType::CUSTOM_ELEMENT || 
               type == NodeType::CUSTOM_VAR ||
               type == NodeType::ORIGIN_HTML || 
               type == NodeType::ORIGIN_STYLE || 
               type == NodeType::ORIGIN_JAVASCRIPT ||
               type == NodeType::IMPORT_HTML || 
               type == NodeType::IMPORT_STYLE || 
               type == NodeType::IMPORT_JAVASCRIPT ||
               type == NodeType::IMPORT_CHTL || 
               type == NodeType::IMPORT_CJMOD) {
        // For now, just return the content attribute
        auto it = attributes.find("content");
        if (it != attributes.end()) {
            return it->second;
        }
        return "";
    }
    return "";
}

std::string ASTNode::to_css() const {
    return "";
}

std::string ASTNode::to_js() const {
    return "";
}

ASTNode::NodePtr ASTNode::clone() const {
    auto cloned = std::make_shared<ASTNode>(type, name, value);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void ASTNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ElementNode implementation
std::string ElementNode::to_html() const {
    std::ostringstream oss;
    
    // Self-closing tags
    static const std::unordered_set<std::string> self_closing_tags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    bool is_self_closing = self_closing_tags.find(name) != self_closing_tags.end();
    
    oss << "<" << name;
    
    // Add attributes
    for (const auto& attr : attributes) {
        oss << " " << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << attr.second << "\"";
        }
    }
    
    if (is_self_closing) {
        oss << " />";
    } else {
        oss << ">";
        
        // Add children
        for (const auto& child : children) {
            oss << child->to_html();
        }
        
        oss << "</" << name << ">";
    }
    
    return oss.str();
}

ASTNode::NodePtr ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(name, value);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void ElementNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// TextNode implementation
std::string TextNode::to_html() const {
    return value;
}

ASTNode::NodePtr TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(value);
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

void TextNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// StyleNode implementation
std::string StyleNode::to_html() const {
    return "<style>" + value + "</style>";
}

std::string StyleNode::to_css() const {
    return value;
}

ASTNode::NodePtr StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(value);
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

void StyleNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ScriptNode implementation
std::string ScriptNode::to_js() const {
    return value;
}

ASTNode::NodePtr ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(value);
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    return cloned;
}

void ScriptNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateNode implementation
ASTNode::NodePtr TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(template_type, name);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void TemplateNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// CustomNode implementation
ASTNode::NodePtr CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(custom_type, name);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void CustomNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// OriginNode implementation
ASTNode::NodePtr OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(origin_type, name);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void OriginNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ImportNode implementation
ASTNode::NodePtr ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(import_type, file_path, alias);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void ImportNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// ConfigurationNode implementation
void ConfigurationNode::set_config_value(const std::string& key, const std::string& value) {
    config_values[key] = value;
}

std::string ConfigurationNode::get_config_value(const std::string& key) const {
    auto it = config_values.find(key);
    return it != config_values.end() ? it->second : "";
}

bool ConfigurationNode::has_config_value(const std::string& key) const {
    return config_values.find(key) != config_values.end();
}

ASTNode::NodePtr ConfigurationNode::clone() const {
    auto cloned = std::make_shared<ConfigurationNode>(config_name);
    cloned->config_values = config_values;
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void ConfigurationNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

// NamespaceNode implementation
ASTNode::NodePtr NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(namespace_name);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& child : children) {
        cloned->add_child(child->clone());
    }
    
    return cloned;
}

void NamespaceNode::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace ast
} // namespace chtl