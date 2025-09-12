#include "CHTL/CHTLNode.h"
#include <iostream>
#include <sstream>

namespace CHTL {

// CHTLNode 实现
CHTLNode::CHTLNode(NodeType type) 
    : type_(type), line_(0), column_(0) {
}

CHTLNode::NodeType CHTLNode::getType() const {
    return type_;
}

void CHTLNode::setAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

std::string CHTLNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

void CHTLNode::removeAttribute(const std::string& key) {
    attributes_.erase(key);
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
        children_.erase(it);
    }
}

std::vector<std::shared_ptr<CHTLNode>> CHTLNode::getChildren() const {
    return children_;
}

std::shared_ptr<CHTLNode> CHTLNode::getChild(size_t index) const {
    if (index < children_.size()) {
        return children_[index];
    }
    return nullptr;
}

size_t CHTLNode::getChildCount() const {
    return children_.size();
}

void CHTLNode::setParent(std::shared_ptr<CHTLNode> parent) {
    parent_ = parent;
}

std::shared_ptr<CHTLNode> CHTLNode::getParent() const {
    return parent_.lock();
}

void CHTLNode::setContent(const std::string& content) {
    content_ = content;
}

std::string CHTLNode::getContent() const {
    return content_;
}

void CHTLNode::setPosition(size_t line, size_t column) {
    line_ = line;
    column_ = column;
}

size_t CHTLNode::getLine() const {
    return line_;
}

size_t CHTLNode::getColumn() const {
    return column_;
}

// ElementNode 实现
ElementNode::ElementNode(const std::string& tagName) 
    : CHTLNode(NodeType::ELEMENT), tag_name_(tagName), self_closing_(false) {
}

std::string ElementNode::getTagName() const {
    return tag_name_;
}

void ElementNode::setTagName(const std::string& tagName) {
    tag_name_ = tagName;
}

bool ElementNode::isSelfClosing() const {
    return self_closing_;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    self_closing_ = selfClosing;
}

std::shared_ptr<CHTLNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(tag_name_);
    cloned->self_closing_ = self_closing_;
    
    // 复制属性
    for (const auto& attr : attributes_) {
        cloned->setAttribute(attr.first, attr.second);
    }
    
    // 复制子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool ElementNode::validate() const {
    // 基本验证：标签名不能为空
    if (tag_name_.empty()) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : children_) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string ElementNode::generateHTML() const {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << tag_name_;
    
    // 属性
    for (const auto& attr : attributes_) {
        oss << " " << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << attr.second << "\"";
        }
    }
    
    if (self_closing_) {
        oss << " />";
    } else {
        oss << ">";
        
        // 子节点内容
        for (const auto& child : children_) {
            oss << child->generateHTML();
        }
        
        // 结束标签
        oss << "</" << tag_name_ << ">";
    }
    
    return oss.str();
}

// TextNode 实现
TextNode::TextNode(const std::string& text) 
    : CHTLNode(NodeType::TEXT) {
    setContent(text);
}

std::shared_ptr<CHTLNode> TextNode::clone() const {
    auto cloned = std::make_shared<TextNode>(content_);
    cloned->setPosition(line_, column_);
    return cloned;
}

bool TextNode::validate() const {
    return true; // 文本节点总是有效的
}

std::string TextNode::generateHTML() const {
    // HTML转义
    std::string result = content_;
    
    // 替换特殊字符
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

// CommentNode 实现
CommentNode::CommentNode(const std::string& content, CommentType type) 
    : CHTLNode(NodeType::COMMENT), comment_type_(type) {
    setContent(content);
}

CommentNode::CommentType CommentNode::getCommentType() const {
    return comment_type_;
}

void CommentNode::setCommentType(CommentType type) {
    comment_type_ = type;
}

std::shared_ptr<CHTLNode> CommentNode::clone() const {
    auto cloned = std::make_shared<CommentNode>(content_, comment_type_);
    cloned->setPosition(line_, column_);
    return cloned;
}

bool CommentNode::validate() const {
    return true; // 注释节点总是有效的
}

std::string CommentNode::generateHTML() const {
    switch (comment_type_) {
        case CommentType::SINGLE_LINE:
            return "<!-- " + content_ + " -->";
        case CommentType::MULTI_LINE:
            return "<!-- " + content_ + " -->";
        case CommentType::GENERATOR:
            return "<!-- " + content_ + " -->";
        default:
            return "<!-- " + content_ + " -->";
    }
}

// StyleNode 实现
StyleNode::StyleNode(StyleType type) 
    : CHTLNode(NodeType::STYLE), style_type_(type) {
}

StyleNode::StyleType StyleNode::getStyleType() const {
    return style_type_;
}

void StyleNode::setStyleType(StyleType type) {
    style_type_ = type;
}

void StyleNode::addCSSProperty(const std::string& property, const std::string& value) {
    css_properties_[property] = value;
}

std::string StyleNode::getCSSProperty(const std::string& property) const {
    auto it = css_properties_.find(property);
    if (it != css_properties_.end()) {
        return it->second;
    }
    return "";
}

bool StyleNode::hasCSSProperty(const std::string& property) const {
    return css_properties_.find(property) != css_properties_.end();
}

void StyleNode::removeCSSProperty(const std::string& property) {
    css_properties_.erase(property);
}

void StyleNode::addSelector(const std::string& selector) {
    selectors_.push_back(selector);
}

std::vector<std::string> StyleNode::getSelectors() const {
    return selectors_;
}

std::shared_ptr<CHTLNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(style_type_);
    
    // 复制CSS属性
    for (const auto& prop : css_properties_) {
        cloned->addCSSProperty(prop.first, prop.second);
    }
    
    // 复制选择器
    for (const auto& selector : selectors_) {
        cloned->addSelector(selector);
    }
    
    // 复制子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool StyleNode::validate() const {
    // 验证CSS属性
    for (const auto& prop : css_properties_) {
        if (prop.first.empty() || prop.second.empty()) {
            return false;
        }
    }
    
    // 验证子节点
    for (const auto& child : children_) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string StyleNode::generateHTML() const {
    std::ostringstream oss;
    
    if (style_type_ == StyleType::GLOBAL) {
        oss << "<style>";
    }
    
    // 生成CSS规则
    for (const auto& selector : selectors_) {
        oss << selector << " {";
        for (const auto& prop : css_properties_) {
            oss << " " << prop.first << ": " << prop.second << ";";
        }
        oss << " }";
    }
    
    // 如果没有选择器，生成内联样式
    if (selectors_.empty() && !css_properties_.empty()) {
        oss << " style=\"";
        bool first = true;
        for (const auto& prop : css_properties_) {
            if (!first) oss << " ";
            oss << prop.first << ": " << prop.second << ";";
            first = false;
        }
        oss << "\"";
    }
    
    if (style_type_ == StyleType::GLOBAL) {
        oss << "</style>";
    }
    
    return oss.str();
}

// ScriptNode 实现
ScriptNode::ScriptNode(ScriptType type) 
    : CHTLNode(NodeType::SCRIPT), script_type_(type) {
}

ScriptNode::ScriptType ScriptNode::getScriptType() const {
    return script_type_;
}

void ScriptNode::setScriptType(ScriptType type) {
    script_type_ = type;
}

std::shared_ptr<CHTLNode> ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(script_type_);
    
    // 复制子节点
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    cloned->setContent(content_);
    cloned->setPosition(line_, column_);
    
    return cloned;
}

bool ScriptNode::validate() const {
    // 验证子节点
    for (const auto& child : children_) {
        if (!child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string ScriptNode::generateHTML() const {
    std::ostringstream oss;
    
    if (script_type_ == ScriptType::GLOBAL || script_type_ == ScriptType::CHTL_JS) {
        oss << "<script>";
    }
    
    // 生成脚本内容
    oss << content_;
    
    // 生成子节点内容
    for (const auto& child : children_) {
        oss << child->generateHTML();
    }
    
    if (script_type_ == ScriptType::GLOBAL || script_type_ == ScriptType::CHTL_JS) {
        oss << "</script>";
    }
    
    return oss.str();
}

} // namespace CHTL