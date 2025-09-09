#include "CHTL/CHTLNode.h"

namespace CHTL {

// BaseNode 实现
BaseNode::BaseNode(NodeType type, int line, int column)
    : m_type(type), m_line(line), m_column(column) {
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    m_children.push_back(child);
}

const std::vector<std::shared_ptr<BaseNode>>& BaseNode::getChildren() const {
    return m_children;
}

void BaseNode::clearChildren() {
    m_children.clear();
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    m_attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = m_attributes.find(key);
    return (it != m_attributes.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

const std::unordered_map<std::string, std::string>& BaseNode::getAttributes() const {
    return m_attributes;
}

// ElementNode 实现
ElementNode::ElementNode(const std::string& tagName, int line, int column)
    : BaseNode(NodeType::ELEMENT, line, column), m_tagName(tagName) {
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// TextNode 实现
TextNode::TextNode(const std::string& content, int line, int column)
    : BaseNode(NodeType::TEXT, line, column), m_content(content) {
}

void TextNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// CommentNode 实现
CommentNode::CommentNode(const std::string& content, bool isGenerator, int line, int column)
    : BaseNode(NodeType::COMMENT, line, column), m_content(content), m_isGenerator(isGenerator) {
}

void CommentNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// TemplateNode 实现
TemplateNode::TemplateNode(NodeType type, const std::string& name, int line, int column)
    : BaseNode(type, line, column), m_name(name) {
}

void TemplateNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// CustomNode 实现
CustomNode::CustomNode(NodeType type, const std::string& name, int line, int column)
    : BaseNode(type, line, column), m_name(name) {
}

void CustomNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// StyleNode 实现
StyleNode::StyleNode(int line, int column)
    : BaseNode(NodeType::STYLE, line, column) {
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// ScriptNode 实现
ScriptNode::ScriptNode(int line, int column)
    : BaseNode(NodeType::SCRIPT, line, column) {
}

void ScriptNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// OriginNode 实现
OriginNode::OriginNode(const std::string& type, const std::string& name, int line, int column)
    : BaseNode(NodeType::ORIGIN, line, column), m_originType(type), m_name(name) {
}

void OriginNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// ImportNode 实现
ImportNode::ImportNode(const std::string& importType, const std::string& path, 
                       const std::string& alias, int line, int column)
    : BaseNode(NodeType::IMPORT, line, column), m_importType(importType), m_path(path), m_alias(alias) {
}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// ConfigNode 实现
ConfigNode::ConfigNode(const std::string& name, int line, int column)
    : BaseNode(NodeType::CONFIGURATION, line, column), m_name(name) {
}

void ConfigNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// NamespaceNode 实现
NamespaceNode::NamespaceNode(const std::string& name, int line, int column)
    : BaseNode(NodeType::NAMESPACE, line, column), m_name(name) {
}

void NamespaceNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// OperatorNode 实现
OperatorNode::OperatorNode(OperatorType opType, int line, int column)
    : BaseNode(NodeType::OPERATOR, line, column), m_operatorType(opType) {
}

void OperatorNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace CHTL