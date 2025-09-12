#include "CHTL/ConcreteNodes.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// ConcreteNode 实现
ConcreteNode::ConcreteNode(NodeType type) : CHTLNode(type) {
}

std::shared_ptr<CHTLNode> ConcreteNode::clone() const {
    auto cloned = std::make_shared<ConcreteNode>(type_);
    cloned->attributes_ = attributes_;
    cloned->content_ = content_;
    cloned->line_ = line_;
    cloned->column_ = column_;
    
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool ConcreteNode::validate() const {
    // 基础验证：检查是否有内容或子节点
    return !content_.empty() || !children_.empty();
}

std::string ConcreteNode::generateHTML() const {
    std::ostringstream oss;
    
    // 生成属性
    std::string attrs = generateAttributes();
    if (!attrs.empty()) {
        oss << " " << attrs;
    }
    
    // 生成内容
    if (!content_.empty()) {
        oss << content_;
    }
    
    // 生成子节点
    std::string children = generateChildren();
    if (!children.empty()) {
        oss << children;
    }
    
    return oss.str();
}

void ConcreteNode::setContent(const std::string& content) {
    content_ = content;
}

std::string ConcreteNode::getContent() const {
    return content_;
}

void ConcreteNode::addAttribute(const std::string& key, const std::string& value) {
    attributes_[key] = value;
}

void ConcreteNode::removeAttribute(const std::string& key) {
    attributes_.erase(key);
}

std::string ConcreteNode::getAttribute(const std::string& key) const {
    auto it = attributes_.find(key);
    return (it != attributes_.end()) ? it->second : "";
}

bool ConcreteNode::hasAttribute(const std::string& key) const {
    return attributes_.find(key) != attributes_.end();
}

std::string ConcreteNode::generateAttributes() const {
    std::ostringstream oss;
    
    for (const auto& attr : attributes_) {
        oss << attr.first << "=\"" << attr.second << "\" ";
    }
    
    std::string result = oss.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    return result;
}

std::string ConcreteNode::generateChildren() const {
    std::ostringstream oss;
    
    for (const auto& child : children_) {
        oss << child->generateHTML();
    }
    
    return oss.str();
}

// ConcreteElementNode 实现
ConcreteElementNode::ConcreteElementNode(const std::string& tag_name) 
    : ConcreteNode(NodeType::ELEMENT), tag_name_(tag_name), self_closing_(false) {
}

std::shared_ptr<CHTLNode> ConcreteElementNode::clone() const {
    auto cloned = std::make_shared<ConcreteElementNode>(tag_name_);
    cloned->attributes_ = attributes_;
    cloned->content_ = content_;
    cloned->line_ = line_;
    cloned->column_ = column_;
    cloned->self_closing_ = self_closing_;
    
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ConcreteElementNode::generateHTML() const {
    std::ostringstream oss;
    
    oss << "<" << tag_name_;
    
    // 生成属性
    std::string attrs = generateAttributes();
    if (!attrs.empty()) {
        oss << " " << attrs;
    }
    
    if (self_closing_) {
        oss << " />";
    } else {
        oss << ">";
        
        // 生成内容
        if (!content_.empty()) {
            oss << content_;
        }
        
        // 生成子节点
        std::string children = generateChildren();
        if (!children.empty()) {
            oss << children;
        }
        
        oss << "</" << tag_name_ << ">";
    }
    
    return oss.str();
}

void ConcreteElementNode::setTagName(const std::string& tag_name) {
    tag_name_ = tag_name;
}

std::string ConcreteElementNode::getTagName() const {
    return tag_name_;
}

void ConcreteElementNode::setSelfClosing(bool self_closing) {
    self_closing_ = self_closing;
}

bool ConcreteElementNode::isSelfClosing() const {
    return self_closing_;
}

// ConcreteTextNode 实现
ConcreteTextNode::ConcreteTextNode(const std::string& text) 
    : ConcreteNode(NodeType::TEXT) {
    setText(text);
}

std::shared_ptr<CHTLNode> ConcreteTextNode::clone() const {
    auto cloned = std::make_shared<ConcreteTextNode>(content_);
    cloned->line_ = line_;
    cloned->column_ = column_;
    return cloned;
}

std::string ConcreteTextNode::generateHTML() const {
    return escapeHTML(content_);
}

void ConcreteTextNode::setText(const std::string& text) {
    content_ = text;
}

std::string ConcreteTextNode::getText() const {
    return content_;
}

std::string ConcreteTextNode::escapeHTML(const std::string& text) const {
    std::string result = text;
    
    // 转义HTML特殊字符
    std::replace(result.begin(), result.end(), '&', '&');
    std::replace(result.begin(), result.end(), '<', '<');
    std::replace(result.begin(), result.end(), '>', '>');
    std::replace(result.begin(), result.end(), '"', '"');
    std::replace(result.begin(), result.end(), '\'', ''');
    
    return result;
}

// ConcreteCommentNode 实现
ConcreteCommentNode::ConcreteCommentNode(const std::string& comment) 
    : ConcreteNode(NodeType::COMMENT) {
    setComment(comment);
}

std::shared_ptr<CHTLNode> ConcreteCommentNode::clone() const {
    auto cloned = std::make_shared<ConcreteCommentNode>(content_);
    cloned->line_ = line_;
    cloned->column_ = column_;
    return cloned;
}

std::string ConcreteCommentNode::generateHTML() const {
    return "<!-- " + content_ + " -->";
}

void ConcreteCommentNode::setComment(const std::string& comment) {
    content_ = comment;
}

std::string ConcreteCommentNode::getComment() const {
    return content_;
}

// TemplateNode 实现
TemplateNode::TemplateNode(const std::string& name, TemplateType type) 
    : ConcreteNode(NodeType::TEMPLATE), template_name_(name), template_type_(type) {
}

std::shared_ptr<CHTLNode> TemplateNode::clone() const {
    auto cloned = std::make_shared<TemplateNode>(template_name_, template_type_);
    cloned->attributes_ = attributes_;
    cloned->content_ = content_;
    cloned->line_ = line_;
    cloned->column_ = column_;
    cloned->css_properties_ = css_properties_;
    cloned->variables_ = variables_;
    
    for (const auto& child : children_) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string TemplateNode::generateHTML() const {
    std::ostringstream oss;
    
    switch (template_type_) {
        case TemplateType::STYLE: {
            oss << "<style class=\"" << template_name_ << "\">";
            for (const auto& prop : css_properties_) {
                oss << prop.first << ": " << prop.second << "; ";
            }
            oss << "</style>";
            break;
        }
        case TemplateType::ELEMENT: {
            oss << "<div class=\"" << template_name_ << "\">";
            oss << content_;
            oss << "</div>";
            break;
        }
        case TemplateType::VAR: {
            oss << "<!-- Variable: " << template_name_ << " = " << content_ << " -->";
            break;
        }
        case TemplateType::CUSTOM: {
            oss << "<div class=\"custom-" << template_name_ << "\">";
            oss << content_;
            oss << "</div>";
            break;
        }
    }
    
    return oss.str();
}

void TemplateNode::setTemplateName(const std::string& name) {
    template_name_ = name;
}

std::string TemplateNode::getTemplateName() const {
    return template_name_;
}

void TemplateNode::setTemplateType(TemplateType type) {
    template_type_ = type;
}

TemplateNode::TemplateType TemplateNode::getTemplateType() const {
    return template_type_;
}

void TemplateNode::addCSSProperty(const std::string& property, const std::string& value) {
    css_properties_[property] = value;
}

std::string TemplateNode::getCSSProperty(const std::string& property) const {
    auto it = css_properties_.find(property);
    return (it != css_properties_.end()) ? it->second : "";
}

bool TemplateNode::hasCSSProperty(const std::string& property) const {
    return css_properties_.find(property) != css_properties_.end();
}

void TemplateNode::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string TemplateNode::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

bool TemplateNode::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

// ImportNode 实现
ImportNode::ImportNode(const std::string& import_path) 
    : ConcreteNode(NodeType::IMPORT), import_path_(import_path), import_type_("file") {
}

std::shared_ptr<CHTLNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(import_path_);
    cloned->import_type_ = import_type_;
    cloned->line_ = line_;
    cloned->column_ = column_;
    return cloned;
}

std::string ImportNode::generateHTML() const {
    return "<!-- Import: " + import_path_ + " -->";
}

void ImportNode::setImportPath(const std::string& path) {
    import_path_ = path;
}

std::string ImportNode::getImportPath() const {
    return import_path_;
}

void ImportNode::setImportType(const std::string& type) {
    import_type_ = type;
}

std::string ImportNode::getImportType() const {
    return import_type_;
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

void NamespaceNode::setNamespaceName(const std::string& name) {
    namespace_name_ = name;
}

std::string NamespaceNode::getNamespaceName() const {
    return namespace_name_;
}

// ConfigurationNode 实现
ConfigurationNode::ConfigurationNode(const std::string& config_name) 
    : ConcreteNode(NodeType::CONFIGURATION), config_name_(config_name) {
}

std::shared_ptr<CHTLNode> ConfigurationNode::clone() const {
    auto cloned = std::make_shared<ConfigurationNode>(config_name_);
    cloned->config_values_ = config_values_;
    cloned->line_ = line_;
    cloned->column_ = column_;
    return cloned;
}

std::string ConfigurationNode::generateHTML() const {
    std::ostringstream oss;
    oss << "<!-- Configuration: " << config_name_;
    
    for (const auto& config : config_values_) {
        oss << " " << config.first << "=" << config.second;
    }
    
    oss << " -->";
    return oss.str();
}

void ConfigurationNode::setConfigName(const std::string& name) {
    config_name_ = name;
}

std::string ConfigurationNode::getConfigName() const {
    return config_name_;
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

// OriginNode 实现
OriginNode::OriginNode(const std::string& content, OriginType type) 
    : ConcreteNode(NodeType::ORIGIN), origin_type_(type), origin_content_(content) {
}

std::shared_ptr<CHTLNode> OriginNode::clone() const {
    auto cloned = std::make_shared<OriginNode>(origin_content_, origin_type_);
    cloned->line_ = line_;
    cloned->column_ = column_;
    return cloned;
}

std::string OriginNode::generateHTML() const {
    return origin_content_;
}

void OriginNode::setOriginType(OriginType type) {
    origin_type_ = type;
}

OriginNode::OriginType OriginNode::getOriginType() const {
    return origin_type_;
}

void OriginNode::setOriginContent(const std::string& content) {
    origin_content_ = content;
}

std::string OriginNode::getOriginContent() const {
    return origin_content_;
}

} // namespace CHTL