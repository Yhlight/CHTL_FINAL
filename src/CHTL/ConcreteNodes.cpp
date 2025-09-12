#include "CHTL/ConcreteNodes.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// 辅助函数
std::string escapeHTMLString(const std::string& text) {
    std::string result = text;
    
    // 替换HTML特殊字符
    std::replace(result.begin(), result.end(), '&', '&');
    std::replace(result.begin(), result.end(), '<', '<');
    std::replace(result.begin(), result.end(), '>', '>');
    
    // 处理单引号
    std::string temp;
    for (char c : result) {
        if (c == '\'') {
            temp += "&apos;";
        } else {
            temp += c;
        }
    }
    result = temp;
    
    return result;
}

// ConcreteElementNode 实现
ConcreteElementNode::ConcreteElementNode(const std::string& tag_name) 
    : ConcreteNode(NodeType::ELEMENT), tag_name_(tag_name) {
}

std::shared_ptr<CHTLNode> ConcreteElementNode::clone() const {
    auto cloned = std::make_shared<ConcreteElementNode>(tag_name_);
    cloned->attributes_ = attributes_;
    cloned->content_ = content_;
    cloned->line_ = line_;
    cloned->column_ = column_;
    
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ConcreteElementNode::generateHTML() const {
    std::ostringstream oss;
    oss << "<" << tag_name_;
    
    // 添加属性
    for (const auto& attr : attributes_) {
        oss << " " << attr.first << "=\"" << escapeHTMLString(attr.second) << "\"";
    }
    
    if (children_.empty() && content_.empty()) {
        oss << " />";
    } else {
        oss << ">";
        if (!content_.empty()) {
            oss << escapeHTMLString(content_);
        }
        for (const auto& child : children_) {
            oss << child->generateHTML();
        }
        oss << "</" << tag_name_ << ">";
    }
    
    return oss.str();
}

std::string ConcreteElementNode::getTagName() const {
    return tag_name_;
}

void ConcreteElementNode::setTagName(const std::string& name) {
    tag_name_ = name;
}

// ConcreteTextNode 实现
ConcreteTextNode::ConcreteTextNode(const std::string& text) 
    : ConcreteNode(NodeType::TEXT) {
    setContent(text);
}

std::shared_ptr<CHTLNode> ConcreteTextNode::clone() const {
    return std::make_shared<ConcreteTextNode>(getContent());
}

std::string ConcreteTextNode::generateHTML() const {
    return escapeHTMLString(getContent());
}

std::string ConcreteTextNode::getText() const {
    return getContent();
}

void ConcreteTextNode::setText(const std::string& text) {
    setContent(text);
}

std::string ConcreteTextNode::escapeHTML(const std::string& text) const {
    std::string result = text;
    
    // 替换HTML特殊字符
    std::replace(result.begin(), result.end(), '&', '&');
    std::replace(result.begin(), result.end(), '<', '<');
    std::replace(result.begin(), result.end(), '>', '>');
    
    // 处理单引号
    std::string temp;
    for (char c : result) {
        if (c == '\'') {
            temp += "&apos;";
        } else {
            temp += c;
        }
    }
    result = temp;
    
    return result;
}

// ConcreteCommentNode 实现
ConcreteCommentNode::ConcreteCommentNode(const std::string& comment) 
    : ConcreteNode(NodeType::COMMENT) {
    setContent(comment);
}

std::shared_ptr<CHTLNode> ConcreteCommentNode::clone() const {
    return std::make_shared<ConcreteCommentNode>(getContent());
}

std::string ConcreteCommentNode::generateHTML() const {
    return "<!-- " + getContent() + " -->";
}

std::string ConcreteCommentNode::getComment() const {
    return getContent();
}

// NamespaceNode 实现
NamespaceNode::NamespaceNode(const std::string& namespace_name) 
    : ConcreteNode(NodeType::NAMESPACE), namespace_name_(namespace_name) {
}

std::shared_ptr<CHTLNode> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(namespace_name_);
    cloned->line_ = line_;
    cloned->column_ = column_;
    return cloned;
}

std::string NamespaceNode::generateHTML() const {
    return "<!-- Namespace: " + namespace_name_ + " -->";
}

std::string NamespaceNode::getNamespaceName() const {
    return namespace_name_;
}

void NamespaceNode::setNamespaceName(const std::string& name) {
    namespace_name_ = name;
}

// ConfigurationNode 实现
ConfigurationNode::ConfigurationNode(const std::string& config_name) 
    : ConcreteNode(NodeType::CONFIG), config_name_(config_name) {
}

std::shared_ptr<CHTLNode> ConfigurationNode::clone() const {
    auto cloned = std::make_shared<ConfigurationNode>(config_name_);
    cloned->line_ = line_;
    cloned->column_ = column_;
    cloned->config_values_ = config_values_;
    return cloned;
}

std::string ConfigurationNode::generateHTML() const {
    return "<!-- Configuration: " + config_name_ + " -->";
}

std::string ConfigurationNode::getConfigName() const {
    return config_name_;
}

void ConfigurationNode::setConfigName(const std::string& name) {
    config_name_ = name;
}

void ConfigurationNode::addConfigValue(const std::string& key, const std::string& value) {
    config_values_[key] = value;
}

std::string ConfigurationNode::getConfigValue(const std::string& key) const {
    auto it = config_values_.find(key);
    return (it != config_values_.end()) ? it->second : "";
}

bool ConfigurationNode::hasConfigValue(const std::string& key) const {
    return config_values_.find(key) != config_values_.end();
}

bool ConfigurationNode::validate() const {
    return !config_name_.empty();
}

// UseStatement 实现已移至单独文件

} // namespace CHTL