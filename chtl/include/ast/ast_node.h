#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <variant>

namespace chtl {
namespace ast {

enum class NodeType {
    // Basic nodes
    ELEMENT,
    TEXT,
    COMMENT,
    
    // Style and script nodes
    STYLE,
    SCRIPT,
    
    // Template nodes
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    
    // Custom nodes
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    
    // Origin nodes
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    
    // Import nodes
    IMPORT_HTML,
    IMPORT_STYLE,
    IMPORT_JAVASCRIPT,
    IMPORT_CHTL,
    IMPORT_CJMOD,
    
    // Configuration nodes
    CONFIGURATION,
    NAMESPACE,
    
    // Operator nodes
    DELETE_OPERATOR,
    INSERT_OPERATOR,
    USE_OPERATOR,
    
    // Expression nodes
    BINARY_EXPRESSION,
    UNARY_EXPRESSION,
    CONDITIONAL_EXPRESSION,
    LITERAL_EXPRESSION,

    // Root node
    ROOT
};

class ASTNode : public std::enable_shared_from_this<ASTNode> {
public:
    using NodePtr = std::shared_ptr<ASTNode>;
    using NodeList = std::vector<NodePtr>;
    using AttributeMap = std::unordered_map<std::string, NodePtr>;
    
    NodeType type;
    std::string name;
    std::string value;
    AttributeMap attributes;
    NodeList children;
    NodePtr parent;
    
    // Position information
    size_t line;
    size_t column;
    size_t position;
    
    ASTNode(NodeType t, const std::string& n = "", const std::string& v = "")
        : type(t), name(n), value(v), parent(nullptr), line(0), column(0), position(0) {}
    
    virtual ~ASTNode() = default;
    
    // Tree operations
    void add_child(NodePtr child);
    void remove_child(NodePtr child);
    void insert_child(size_t index, NodePtr child);
    void clear_children();
    
    // Attribute operations
    void set_attribute(const std::string& key, NodePtr value);
    NodePtr get_attribute(const std::string& key) const;
    bool has_attribute(const std::string& key) const;
    void remove_attribute(const std::string& key);
    
    // Navigation
    NodePtr find_child_by_name(const std::string& name) const;
    NodeList find_children_by_type(NodeType type) const;
    NodePtr find_descendant_by_name(const std::string& name) const;
    NodeList find_descendants_by_type(NodeType type) const;
    
    // Utility
    bool is_leaf() const { return children.empty(); }
    size_t child_count() const { return children.size(); }
    bool has_children() const { return !children.empty(); }
    
    // Virtual methods for specific node types
    virtual std::string to_string() const;
    virtual std::string to_html() const;
    virtual std::string to_css() const;
    virtual std::string to_js() const;
    
    // Clone
    virtual NodePtr clone() const;
    
    // Visitor pattern
    virtual void accept(class ASTVisitor& visitor);
};

// Specific node types
class ElementNode : public ASTNode {
public:
    std::vector<std::string> constraints;

    ElementNode(const std::string& name = "", const std::string& value = "")
        : ASTNode(NodeType::ELEMENT, name, value) {}
    
    std::string to_html() const override;
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class TextNode : public ASTNode {
public:
    TextNode(const std::string& value = "")
        : ASTNode(NodeType::TEXT, "text", value) {}
    
    std::string to_html() const override;
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class StyleNode : public ASTNode {
public:
    StyleNode(const std::string& value = "")
        : ASTNode(NodeType::STYLE, "style", value) {}
    
    std::string to_html() const override;
    std::string to_css() const override;
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class ScriptNode : public ASTNode {
public:
    ScriptNode(const std::string& value = "")
        : ASTNode(NodeType::SCRIPT, "script", value) {}
    
    std::string to_js() const override;
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class TemplateNode : public ASTNode {
public:
    enum class TemplateType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    TemplateType template_type;
    
    TemplateNode(TemplateType type, const std::string& name = "")
        : ASTNode(NodeType::TEMPLATE_STYLE, name), template_type(type) {
        switch (type) {
            case TemplateType::STYLE:
                this->type = NodeType::TEMPLATE_STYLE;
                break;
            case TemplateType::ELEMENT:
                this->type = NodeType::TEMPLATE_ELEMENT;
                break;
            case TemplateType::VAR:
                this->type = NodeType::TEMPLATE_VAR;
                break;
        }
    }
    
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class CustomNode : public ASTNode {
public:
    enum class CustomType {
        STYLE,
        ELEMENT,
        VAR
    };
    
    CustomType custom_type;
    
    CustomNode(CustomType type, const std::string& name = "")
        : ASTNode(NodeType::CUSTOM_STYLE, name), custom_type(type) {
        switch (type) {
            case CustomType::STYLE:
                this->type = NodeType::CUSTOM_STYLE;
                break;
            case CustomType::ELEMENT:
                this->type = NodeType::CUSTOM_ELEMENT;
                break;
            case CustomType::VAR:
                this->type = NodeType::CUSTOM_VAR;
                break;
        }
    }
    
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class OriginNode : public ASTNode {
public:
    enum class OriginType {
        HTML,
        STYLE,
        JAVASCRIPT
    };
    
    OriginType origin_type;
    
    OriginNode(OriginType type, const std::string& name = "")
        : ASTNode(NodeType::ORIGIN_HTML, name), origin_type(type) {
        switch (type) {
            case OriginType::HTML:
                this->type = NodeType::ORIGIN_HTML;
                break;
            case OriginType::STYLE:
                this->type = NodeType::ORIGIN_STYLE;
                break;
            case OriginType::JAVASCRIPT:
                this->type = NodeType::ORIGIN_JAVASCRIPT;
                break;
        }
    }
    
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class ImportNode : public ASTNode {
public:
    enum class ImportType {
        HTML,
        STYLE,
        JAVASCRIPT,
        CHTL,
        CJMOD
    };
    
    ImportType import_type;
    std::string file_path;
    std::string alias;
    
    // For precise imports, e.g. [Import] [Custom] @Element Box
    std::string import_category; // "Custom", "Template", "Origin"
    std::string import_specifier; // "@Element", "@Style", "@Html"
    std::string imported_item_name; // "Box"

    ImportNode(ImportType type, const std::string& file_path = "", const std::string& alias = "")
        : ASTNode(NodeType::IMPORT_HTML, ""), import_type(type), file_path(file_path), alias(alias) {
        switch (type) {
            case ImportType::HTML:
                this->type = NodeType::IMPORT_HTML;
                break;
            case ImportType::STYLE:
                this->type = NodeType::IMPORT_STYLE;
                break;
            case ImportType::JAVASCRIPT:
                this->type = NodeType::IMPORT_JAVASCRIPT;
                break;
            case ImportType::CHTL:
                this->type = NodeType::IMPORT_CHTL;
                break;
            case ImportType::CJMOD:
                this->type = NodeType::IMPORT_CJMOD;
                break;
        }
    }
    
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class ConfigurationNode : public ASTNode {
public:
    std::string config_name;
    std::unordered_map<std::string, std::string> config_values;
    
    ConfigurationNode(const std::string& name = "")
        : ASTNode(NodeType::CONFIGURATION, name), config_name(name) {}
    
    void set_config_value(const std::string& key, const std::string& value);
    std::string get_config_value(const std::string& key) const;
    bool has_config_value(const std::string& key) const;
    
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

class NamespaceNode : public ASTNode {
public:
    std::string namespace_name;
    
    NamespaceNode(const std::string& name = "")
        : ASTNode(NodeType::NAMESPACE, name), namespace_name(name) {}
    
    NodePtr clone() const override;
    void accept(class ASTVisitor& visitor) override;
};

// Visitor pattern
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(ASTNode& node) {}
    virtual void visit(ElementNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(TextNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(StyleNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(ScriptNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(TemplateNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(CustomNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(OriginNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(ImportNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(ConfigurationNode& node) { visit(static_cast<ASTNode&>(node)); }
    virtual void visit(NamespaceNode& node) { visit(static_cast<ASTNode&>(node)); }
};

// Expression Nodes
class LiteralNode : public ASTNode {
public:
    LiteralNode(const std::string& value = "")
        : ASTNode(NodeType::LITERAL_EXPRESSION, "Literal", value) {}

    std::string to_html() const override { return value; }
    NodePtr clone() const override { return std::make_shared<LiteralNode>(value); }
};

class BinaryExpressionNode : public ASTNode {
public:
    NodePtr left;
    std::string op;
    NodePtr right;

    BinaryExpressionNode(NodePtr l, const std::string& o, NodePtr r)
        : ASTNode(NodeType::BINARY_EXPRESSION, o), left(l), op(o), right(r) {}

    // to_html would need an evaluation context, returning a string for now
    std::string to_html() const override {
        return left->to_html() + " " + op + " " + right->to_html();
    }

    NodePtr clone() const override {
        return std::make_shared<BinaryExpressionNode>(left->clone(), op, right->clone());
    }
};

class UnaryExpressionNode : public ASTNode {
public:
    std::string op;
    NodePtr operand;

    UnaryExpressionNode(const std::string& o, NodePtr r)
        : ASTNode(NodeType::UNARY_EXPRESSION, o), op(o), operand(r) {}

    std::string to_html() const override {
        return op + operand->to_html();
    }

    NodePtr clone() const override {
        return std::make_shared<UnaryExpressionNode>(op, operand->clone());
    }
};

class ConditionalExpressionNode : public ASTNode {
public:
    NodePtr condition;
    NodePtr true_branch;
    NodePtr false_branch;

    ConditionalExpressionNode(NodePtr c, NodePtr t, NodePtr f)
        : ASTNode(NodeType::CONDITIONAL_EXPRESSION, "?:"), condition(c), true_branch(t), false_branch(f) {}

    std::string to_html() const override {
        return condition->to_html() + " ? " + true_branch->to_html() + " : " + false_branch->to_html();
    }

    NodePtr clone() const override {
        return std::make_shared<ConditionalExpressionNode>(condition->clone(), true_branch->clone(), false_branch->clone());
    }
};

} // namespace ast
} // namespace chtl