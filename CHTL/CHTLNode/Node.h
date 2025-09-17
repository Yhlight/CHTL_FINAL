#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// Forward-declare Node class
class Node;

// Define a type for a list of child nodes using smart pointers for automatic memory management.
using NodeList = std::vector<std::unique_ptr<Node>>;
using StyleMap = std::map<std::string, std::string>;

// Enum to identify the type of a node, crucial for casting and processing.
enum class NodeType {
    Root,
    Element,
    Text,
    Comment,
    Style,
    CssRule
};

// The base class for all AST nodes.
class Node {
public:
    virtual ~Node() = default;
    virtual NodeType getType() const = 0;

    const NodeList& getChildren() const { return m_children; }
    void addChild(std::unique_ptr<Node> child) { m_children.push_back(std::move(child)); }

protected:
    NodeList m_children;
};

// Represents a simple text node in the document.
class TextNode : public Node {
public:
    TextNode(std::string text) : m_text_content(std::move(text)) {}
    NodeType getType() const override { return NodeType::Text; }
    const std::string& getText() const { return m_text_content; }

private:
    std::string m_text_content;
};

// Represents an HTML element with a tag name, attributes, and children.
class ElementNode : public Node {
public:
    ElementNode(std::string tag) : m_tag_name(std::move(tag)) {}
    NodeType getType() const override { return NodeType::Element; }

    const std::string& getTagName() const { return m_tag_name; }

    const StyleMap& getAttributes() const { return m_attributes; }
    void setAttribute(const std::string& key, const std::string& value) { m_attributes[key] = value; }

private:
    std::string m_tag_name;
    StyleMap m_attributes;
};

// Represents a CSS rule with a selector and properties.
class CssRuleNode : public Node {
public:
    CssRuleNode(std::string selector) : m_selector(std::move(selector)) {}
    NodeType getType() const override { return NodeType::CssRule; }

    const std::string& getSelector() const { return m_selector; }
    const StyleMap& getProperties() const { return m_properties; }
    void setProperty(const std::string& key, const std::string& value) { m_properties[key] = value; }

private:
    std::string m_selector;
    StyleMap m_properties;
};

// Represents a style block, which can contain inline styles and CSS rule children.
class StyleNode : public Node {
public:
    StyleNode() = default;
    NodeType getType() const override { return NodeType::Style; }

    const StyleMap& getInlineStyles() const { return m_inline_styles; }
    void setInlineStyle(const std::string& key, const std::string& value) { m_inline_styles[key] = value; }
private:
    StyleMap m_inline_styles;
};


// Represents the root of the AST.
class RootNode : public Node {
public:
    RootNode() = default;
    NodeType getType() const override { return NodeType::Root; }
};

} // namespace CHTL

#endif // NODE_H
