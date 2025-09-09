#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// Defines the different types of nodes in the AST.
enum class NodeType {
    Element,
    Text,
    Attribute,
    StyleBlock,
    CssRule,
    CssProperty,

    // Expression Nodes
    Literal,
    PropertyAccess,
    BinaryOperation,
    TernaryOperation
};

// Forward declarations for node classes.
class ElementNode;
class TextNode;
class AttributeNode;
class StyleBlockNode;
class CssRuleNode;
class CssPropertyNode;
class ExpressionNode;
class LiteralNode;
class PropertyAccessNode;
class BinaryOperationNode;
class TernaryOperationNode;

// Base class for all AST nodes.
class Node {
public:
    virtual ~Node() = default;
    virtual NodeType getType() const = 0;
};

// Represents a key-value attribute, e.g., `id: "box"`.
class AttributeNode : public Node {
public:
    AttributeNode(const std::string& key, const std::string& value)
        : m_key(key), m_value(value) {}

    NodeType getType() const override { return NodeType::Attribute; }
    const std::string& getKey() const { return m_key; }
    const std::string& getValue() const { return m_value; }

private:
    std::string m_key;
    std::string m_value;
};

// Represents a text block, e.g., `text { "some text" }`.
class TextNode : public Node {
public:
    TextNode(const std::string& text) : m_text(text) {}

    NodeType getType() const override { return NodeType::Text; }
    const std::string& getText() const { return m_text; }

private:
    std::string m_text;
};

// Represents an element, e.g., `div { ... }`.
class ElementNode : public Node {
public:
    ElementNode(const std::string& tagName) : m_tagName(tagName) {}

    NodeType getType() const override { return NodeType::Element; }
    const std::string& getTagName() const { return m_tagName; }

    const std::vector<std::unique_ptr<AttributeNode>>& getAttributes() const { return m_attributes; }
    const std::vector<std::unique_ptr<Node>>& getChildren() const { return m_children; }
    const StyleBlockNode* getStyleBlock() const { return m_styleBlock.get(); }

    void addAttribute(std::unique_ptr<AttributeNode> attr) {
        m_attributes.push_back(std::move(attr));
    }

    void addChild(std::unique_ptr<Node> child) {
        m_children.push_back(std::move(child));
    }

    void setStyleBlock(std::unique_ptr<StyleBlockNode> styleBlock) {
        m_styleBlock = std::move(styleBlock);
    }

private:
    std::string m_tagName;
    std::vector<std::unique_ptr<AttributeNode>> m_attributes;
    std::vector<std::unique_ptr<Node>> m_children;
    std::unique_ptr<StyleBlockNode> m_styleBlock;
};

// Represents a CSS property, e.g., `width: 100px`.
// Base class for all expression-related nodes
class ExpressionNode : public Node {};

class PropertyAccessNode : public ExpressionNode {
public:
    PropertyAccessNode(const std::string& selector, const std::string& prop_name)
        : m_selector(selector), m_property_name(prop_name) {}

    NodeType getType() const override { return NodeType::PropertyAccess; }
    const std::string& getSelector() const { return m_selector; }
    const std::string& getPropertyName() const { return m_property_name; }
private:
    std::string m_selector;
    std::string m_property_name;
};

class LiteralNode : public ExpressionNode {
public:
    LiteralNode(const std::string& value) : m_value(value) {}
    NodeType getType() const override { return NodeType::Literal; }
    const std::string& getValue() const { return m_value; }
private:
    std::string m_value;
};

class BinaryOperationNode : public ExpressionNode {
public:
    BinaryOperationNode(std::unique_ptr<ExpressionNode> left, Token op, std::unique_ptr<ExpressionNode> right)
        : m_left(std::move(left)), m_operator(op), m_right(std::move(right)) {}
    NodeType getType() const override { return NodeType::BinaryOperation; }
    const ExpressionNode* getLeft() const { return m_left.get(); }
    const ExpressionNode* getRight() const { return m_right.get(); }
    const Token& getOperator() const { return m_operator; }
private:
    std::unique_ptr<ExpressionNode> m_left;
    Token m_operator;
    std::unique_ptr<ExpressionNode> m_right;
};

class TernaryOperationNode : public ExpressionNode {
public:
    TernaryOperationNode(std::unique_ptr<ExpressionNode> cond, std::unique_ptr<ExpressionNode> true_val, std::unique_ptr<ExpressionNode> false_val)
        : m_condition(std::move(cond)), m_true_value(std::move(true_val)), m_false_value(std::move(false_val)) {}
    NodeType getType() const override { return NodeType::TernaryOperation; }
    const ExpressionNode* getCondition() const { return m_condition.get(); }
    const ExpressionNode* getTrueValue() const { return m_true_value.get(); }
    const ExpressionNode* getFalseValue() const { return m_false_value.get(); }
private:
     std::unique_ptr<ExpressionNode> m_condition;
     std::unique_ptr<ExpressionNode> m_true_value;
     std::unique_ptr<ExpressionNode> m_false_value;
};


class CssPropertyNode : public Node {
public:
    CssPropertyNode(const std::string& key, std::unique_ptr<ExpressionNode> value)
        : m_key(key), m_value(std::move(value)) {}

    NodeType getType() const override { return NodeType::CssProperty; }
    const std::string& getKey() const { return m_key; }
    const ExpressionNode* getValue() const { return m_value.get(); }
private:
    std::string m_key;
    std::unique_ptr<ExpressionNode> m_value;
};

// Represents a CSS rule with a selector, e.g., `.box { ... }`.
class CssRuleNode : public Node {
public:
    CssRuleNode(const std::string& selector) : m_selector(selector) {}

    NodeType getType() const override { return NodeType::CssRule; }
    const std::string& getSelector() const { return m_selector; }
    const std::vector<std::unique_ptr<CssPropertyNode>>& getProperties() const { return m_properties; }

    void addProperty(std::unique_ptr<CssPropertyNode> prop) {
        m_properties.push_back(std::move(prop));
    }
private:
    std::string m_selector;
    std::vector<std::unique_ptr<CssPropertyNode>> m_properties;
};

// Represents a `style { ... }` block within an element.
class StyleBlockNode : public Node {
public:
    StyleBlockNode() = default;
    NodeType getType() const override { return NodeType::StyleBlock; }

    const std::vector<std::unique_ptr<CssPropertyNode>>& getInlineProperties() const { return m_inline_properties; }
    const std::vector<std::unique_ptr<CssRuleNode>>& getCssRules() const { return m_rules; }

    void addInlineProperty(std::unique_ptr<CssPropertyNode> prop) {
        m_inline_properties.push_back(std::move(prop));
    }
    void addCssRule(std::unique_ptr<CssRuleNode> rule) {
        m_rules.push_back(std::move(rule));
    }
private:
    std::vector<std::unique_ptr<CssPropertyNode>> m_inline_properties;
    std::vector<std::unique_ptr<CssRuleNode>> m_rules;
};

} // namespace CHTL
