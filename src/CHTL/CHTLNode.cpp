#include "CHTLNode.h"

namespace CHTL {

// BaseNode方法实现
BaseNode::BaseNode(NodeType nodeType) : type(nodeType) {}

BaseNode::~BaseNode() {}

NodeType BaseNode::getType() const {
    return type;
}

std::string BaseNode::toString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    return indentStr + "BaseNode(" + std::to_string(static_cast<int>(type)) + ")";
}

void BaseNode::setName(const std::string& nodeName) {
    name = nodeName;
}

std::string BaseNode::getName() const {
    return name;
}

void BaseNode::setValue(const std::string& nodeValue) {
    value = nodeValue;
}

std::string BaseNode::getValue() const {
    return value;
}

void BaseNode::addChild(std::unique_ptr<BaseNode> child) {
    children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<BaseNode>>& BaseNode::getChildren() const {
    return children;
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

const std::map<std::string, std::string>& BaseNode::getAttributes() const {
    return attributes;
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void BaseNode::clearChildren() {
    children.clear();
}

void BaseNode::clearAttributes() {
    attributes.clear();
}

// ElementNode方法实现
ElementNode::ElementNode(const std::string& tagName) 
    : BaseNode(NodeType::ELEMENT), tagName(tagName), selfClosing(false) {
    setName(tagName);
}

ElementNode::~ElementNode() {}

std::string ElementNode::getTagName() const {
    return tagName;
}

void ElementNode::setTagName(const std::string& newTagName) {
    tagName = newTagName;
    setName(newTagName);
}

bool ElementNode::isSelfClosing() const {
    return selfClosing;
}

void ElementNode::setSelfClosing(bool isSelfClosing) {
    selfClosing = isSelfClosing;
}

// TextNode方法实现
TextNode::TextNode(const std::string& text) : BaseNode(NodeType::TEXT) {
    setValue(text);
}

TextNode::~TextNode() {}

std::string TextNode::getText() const {
    return getValue();
}

void TextNode::setText(const std::string& text) {
    setValue(text);
}

// CommentNode方法实现
CommentNode::CommentNode(const std::string& comment) : BaseNode(NodeType::COMMENT) {
    setValue(comment);
}

CommentNode::~CommentNode() {}

std::string CommentNode::getComment() const {
    return getValue();
}

void CommentNode::setComment(const std::string& comment) {
    setValue(comment);
}

// TemplateNode方法实现
TemplateNode::TemplateNode(const std::string& templateName) : BaseNode(NodeType::TEMPLATE) {
    setName(templateName);
}

TemplateNode::~TemplateNode() {}

std::string TemplateNode::getTemplateName() const {
    return getName();
}

void TemplateNode::setTemplateName(const std::string& templateName) {
    setName(templateName);
}

std::string TemplateNode::getTemplateType() const {
    return getAttribute("type");
}

void TemplateNode::setTemplateType(const std::string& templateType) {
    setAttribute("type", templateType);
}

// CustomNode方法实现
CustomNode::CustomNode(const std::string& customName) : BaseNode(NodeType::CUSTOM) {
    setName(customName);
}

CustomNode::~CustomNode() {}

std::string CustomNode::getCustomName() const {
    return getName();
}

void CustomNode::setCustomName(const std::string& customName) {
    setName(customName);
}

// StyleNode方法实现
StyleNode::StyleNode() : BaseNode(NodeType::STYLE) {}

StyleNode::~StyleNode() {}

void StyleNode::addProperty(const std::string& property, const std::string& value) {
    properties[property] = value;
}

std::string StyleNode::getProperty(const std::string& property) const {
    auto it = properties.find(property);
    return it != properties.end() ? it->second : "";
}

const std::map<std::string, std::string>& StyleNode::getProperties() const {
    return properties;
}

bool StyleNode::hasProperty(const std::string& property) const {
    return properties.find(property) != properties.end();
}

void StyleNode::clearProperties() {
    properties.clear();
}

// ScriptNode方法实现
ScriptNode::ScriptNode() : BaseNode(NodeType::SCRIPT) {}

ScriptNode::~ScriptNode() {}

std::string ScriptNode::getScript() const {
    return getValue();
}

void ScriptNode::setScript(const std::string& script) {
    setValue(script);
}

// OriginNode方法实现
OriginNode::OriginNode(const std::string& originType) 
    : BaseNode(NodeType::ORIGIN), originType(originType) {}

OriginNode::~OriginNode() {}

std::string OriginNode::getOriginType() const {
    return originType;
}

void OriginNode::setOriginType(const std::string& newOriginType) {
    originType = newOriginType;
}

std::string OriginNode::getOriginContent() const {
    return originContent;
}

void OriginNode::setOriginContent(const std::string& content) {
    originContent = content;
}

// ImportNode方法实现
ImportNode::ImportNode(const std::string& importPath) : BaseNode(NodeType::IMPORT) {
    setValue(importPath);
}

ImportNode::~ImportNode() {}

std::string ImportNode::getPath() const {
    return getValue();
}

void ImportNode::setPath(const std::string& path) {
    setValue(path);
}

std::string ImportNode::getImportType() const {
    return getAttribute("type");
}

void ImportNode::setImportType(const std::string& importType) {
    setAttribute("type", importType);
}

// ConfigNode方法实现
ConfigNode::ConfigNode() : BaseNode(NodeType::CONFIG) {}

ConfigNode::~ConfigNode() {}

void ConfigNode::addConfig(const std::string& key, const std::string& value) {
    configs[key] = value;
}

std::string ConfigNode::getConfig(const std::string& key) const {
    auto it = configs.find(key);
    return it != configs.end() ? it->second : "";
}

const std::map<std::string, std::string>& ConfigNode::getConfigs() const {
    return configs;
}

bool ConfigNode::hasConfig(const std::string& key) const {
    return configs.find(key) != configs.end();
}

// NamespaceNode方法实现
NamespaceNode::NamespaceNode(const std::string& namespaceName) : BaseNode(NodeType::NAMESPACE) {
    setName(namespaceName);
}

NamespaceNode::~NamespaceNode() {}

std::string NamespaceNode::getNamespaceName() const {
    return getName();
}

void NamespaceNode::setNamespaceName(const std::string& namespaceName) {
    setName(namespaceName);
}

// OperatorNode方法实现
OperatorNode::OperatorNode(const std::string& operatorType) : BaseNode(NodeType::OPERATOR) {
    setValue(operatorType);
}

OperatorNode::~OperatorNode() {}

std::string OperatorNode::getOperatorType() const {
    return getValue();
}

void OperatorNode::setOperatorType(const std::string& operatorType) {
    setValue(operatorType);
}

} // namespace CHTL