#include "CHTL/CHTLNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// CHTLNode基类实现
CHTLNode::CHTLNode(NodeType type, const std::string& name)
    : type_(type), name_(name), line_(0), column_(0) {
}

void CHTLNode::addChild(std::shared_ptr<CHTLNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void CHTLNode::removeChild(std::shared_ptr<CHTLNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void CHTLNode::setAttribute(const std::string& key, const AttributeValue& value) {
    attributes_[key] = value;
}

AttributeValue CHTLNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return std::string("");
}

bool CHTLNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

void CHTLNode::removeAttribute(const std::string& key) {
    attributes_.erase(key);
}

bool CHTLNode::isTemplate() const {
    return type_ == NodeType::TEMPLATE || 
           type_ == NodeType::STYLE_TEMPLATE ||
           type_ == NodeType::ELEMENT_TEMPLATE ||
           type_ == NodeType::VAR_TEMPLATE;
}

bool CHTLNode::isCustom() const {
    return type_ == NodeType::CUSTOM ||
           type_ == NodeType::CUSTOM_STYLE ||
           type_ == NodeType::CUSTOM_ELEMENT ||
           type_ == NodeType::CUSTOM_VAR;
}

bool CHTLNode::isOrigin() const {
    return type_ == NodeType::ORIGIN ||
           type_ == NodeType::ORIGIN_HTML ||
           type_ == NodeType::ORIGIN_CSS ||
           type_ == NodeType::ORIGIN_JS;
}

bool CHTLNode::isImport() const {
    return type_ == NodeType::IMPORT ||
           type_ == NodeType::IMPORT_HTML ||
           type_ == NodeType::IMPORT_CSS ||
           type_ == NodeType::IMPORT_JS ||
           type_ == NodeType::IMPORT_CHTL ||
           type_ == NodeType::IMPORT_CJMOD;
}

std::string CHTLNode::toString() const {
    std::stringstream ss;
    ss << "Node(" << static_cast<int>(type_) << ", " << name_ << ")";
    return ss.str();
}

std::string CHTLNode::toDebugString() const {
    std::stringstream ss;
    ss << "Node {\n";
    ss << "  type: " << static_cast<int>(type_) << "\n";
    ss << "  name: " << name_ << "\n";
    ss << "  line: " << line_ << "\n";
    ss << "  column: " << column_ << "\n";
    ss << "  text_content: " << text_content_ << "\n";
    ss << "  namespace: " << namespace_ << "\n";
    ss << "  attributes: " << attributes_.size() << "\n";
    ss << "  children: " << children_.size() << "\n";
    ss << "}";
    return ss.str();
}

// ElementNode实现
ElementNode::ElementNode(const std::string& tag_name)
    : CHTLNode(NodeType::ELEMENT, tag_name) {
}

void ElementNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(getName());
    cloned->setPosition(getLine(), getColumn());
    cloned->setTextContent(getTextContent());
    cloned->setNamespace(getNamespace());
    
    // 复制属性
    for (const auto& pair : getAttributes()) {
        cloned->setAttribute(pair.first, pair.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// TextNode实现
TextNode::TextNode(const std::string& content)
    : CHTLNode(NodeType::TEXT, "text") {
    setTextContent(content);
}

void TextNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(getTextContent());
    cloned->setPosition(getLine(), getColumn());
    return cloned;
}

// CommentNode实现
CommentNode::CommentNode(const std::string& content)
    : CHTLNode(NodeType::COMMENT, "comment") {
    setTextContent(content);
}

void CommentNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> CommentNode::clone() const {
    auto cloned = std::make_shared<CommentNode>(getTextContent());
    cloned->setPosition(getLine(), getColumn());
    return cloned;
}

// TemplateNode实现
TemplateNode::TemplateNode(const std::string& template_name, const std::string& template_type)
    : CHTLNode(NodeType::TEMPLATE, template_name), template_type_(template_type) {
}

void TemplateNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(getName(), getTemplateType());
    cloned->setPosition(getLine(), getColumn());
    cloned->setTextContent(getTextContent());
    cloned->setNamespace(getNamespace());
    
    // 复制属性
    for (const auto& pair : getAttributes()) {
        cloned->setAttribute(pair.first, pair.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// CustomNode实现
CustomNode::CustomNode(const std::string& custom_name, const std::string& custom_type)
    : CHTLNode(NodeType::CUSTOM, custom_name), custom_type_(custom_type) {
}

void CustomNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> CustomNode::clone() const {
    auto cloned = std::make_shared<CustomNode>(getName(), getCustomType());
    cloned->setPosition(getLine(), getColumn());
    cloned->setTextContent(getTextContent());
    cloned->setNamespace(getNamespace());
    
    // 复制属性
    for (const auto& pair : getAttributes()) {
        cloned->setAttribute(pair.first, pair.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// StyleNode实现
StyleNode::StyleNode()
    : CHTLNode(NodeType::STYLE, "style"), is_local_(true) {
}

void StyleNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>();
    cloned->setPosition(getLine(), getColumn());
    cloned->setTextContent(getTextContent());
    cloned->setNamespace(getNamespace());
    cloned->setLocal(isLocal());
    
    // 复制属性
    for (const auto& pair : getAttributes()) {
        cloned->setAttribute(pair.first, pair.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ScriptNode实现
ScriptNode::ScriptNode()
    : CHTLNode(NodeType::SCRIPT, "script"), is_local_(true) {
}

void ScriptNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>();
    cloned->setPosition(getLine(), getColumn());
    cloned->setTextContent(getTextContent());
    cloned->setNamespace(getNamespace());
    cloned->setLocal(isLocal());
    
    // 复制属性
    for (const auto& pair : getAttributes()) {
        cloned->setAttribute(pair.first, pair.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// OriginNode实现
OriginNode::OriginNode(const std::string& origin_type)
    : CHTLNode(NodeType::ORIGIN, "origin"), origin_type_(origin_type) {
}

void OriginNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(getOriginType());
    cloned->setPosition(getLine(), getColumn());
    cloned->setTextContent(getTextContent());
    cloned->setNamespace(getNamespace());
    
    // 复制属性
    for (const auto& pair : getAttributes()) {
        cloned->setAttribute(pair.first, pair.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

// ImportNode实现
ImportNode::ImportNode(const std::string& import_type, const std::string& path)
    : CHTLNode(NodeType::IMPORT, "import"), import_type_(import_type), path_(path) {
}

void ImportNode::accept(CHTLNodeVisitor& visitor) {
    visitor.visit(*this);
}

std::shared_ptr<CHTLNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(getImportType(), getPath());
    cloned->setPosition(getLine(), getColumn());
    cloned->setTextContent(getTextContent());
    cloned->setNamespace(getNamespace());
    
    // 复制属性
    for (const auto& pair : getAttributes()) {
        cloned->setAttribute(pair.first, pair.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

} // namespace CHTL