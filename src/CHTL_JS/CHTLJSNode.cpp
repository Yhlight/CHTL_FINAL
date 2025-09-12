#include "CHTLJSNode.h"

namespace CHTL {

// CHTLJSBaseNode方法实现
CHTLJSBaseNode::CHTLJSBaseNode(CHTLJSNodeType nodeType) : type(nodeType) {}

CHTLJSBaseNode::~CHTLJSBaseNode() {}

CHTLJSNodeType CHTLJSBaseNode::getType() const {
    return type;
}

void CHTLJSBaseNode::setName(const std::string& nodeName) {
    name = nodeName;
}

std::string CHTLJSBaseNode::getName() const {
    return name;
}

std::string CHTLJSBaseNode::toString() const {
    return "CHTLJSNode(" + name + ")";
}

void CHTLJSBaseNode::setValue(const std::string& nodeValue) {
    value = nodeValue;
}

std::string CHTLJSBaseNode::getValue() const {
    return value;
}

void CHTLJSBaseNode::addChild(std::unique_ptr<CHTLJSBaseNode> child) {
    children.push_back(std::move(child));
}

const std::vector<std::unique_ptr<CHTLJSBaseNode>>& CHTLJSBaseNode::getChildren() const {
    return children;
}

void CHTLJSBaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string CHTLJSBaseNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

const std::map<std::string, std::string>& CHTLJSBaseNode::getAttributes() const {
    return attributes;
}

bool CHTLJSBaseNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void CHTLJSBaseNode::clearChildren() {
    children.clear();
}

void CHTLJSBaseNode::clearAttributes() {
    attributes.clear();
}

// CHTLJSElementNode方法实现
CHTLJSElementNode::CHTLJSElementNode(const std::string& tagName) 
    : CHTLJSBaseNode(CHTLJSNodeType::ELEMENT), tagName(tagName), selfClosing(false) {
    setName(tagName);
}

CHTLJSElementNode::~CHTLJSElementNode() {}

std::string CHTLJSElementNode::getTagName() const {
    return tagName;
}

void CHTLJSElementNode::setTagName(const std::string& newTagName) {
    tagName = newTagName;
    setName(newTagName);
}

bool CHTLJSElementNode::isSelfClosing() const {
    return selfClosing;
}

void CHTLJSElementNode::setSelfClosing(bool isSelfClosing) {
    selfClosing = isSelfClosing;
}

// CHTLJSTextNode方法实现
CHTLJSTextNode::CHTLJSTextNode(const std::string& text) : CHTLJSBaseNode(CHTLJSNodeType::TEXT) {
    setValue(text);
}

CHTLJSTextNode::~CHTLJSTextNode() {}

std::string CHTLJSTextNode::getText() const {
    return getValue();
}

void CHTLJSTextNode::setText(const std::string& text) {
    setValue(text);
}

// CHTLJSCommentNode方法实现
CHTLJSCommentNode::CHTLJSCommentNode(const std::string& comment) : CHTLJSBaseNode(CHTLJSNodeType::COMMENT) {
    setValue(comment);
}

CHTLJSCommentNode::~CHTLJSCommentNode() {}

std::string CHTLJSCommentNode::getComment() const {
    return getValue();
}

void CHTLJSCommentNode::setComment(const std::string& comment) {
    setValue(comment);
}

// CHTLJSTemplateNode方法实现
CHTLJSTemplateNode::CHTLJSTemplateNode(const std::string& templateName) : CHTLJSBaseNode(CHTLJSNodeType::TEMPLATE) {
    setName(templateName);
}

CHTLJSTemplateNode::~CHTLJSTemplateNode() {}

std::string CHTLJSTemplateNode::getTemplateName() const {
    return getName();
}

void CHTLJSTemplateNode::setTemplateName(const std::string& templateName) {
    setName(templateName);
}

std::string CHTLJSTemplateNode::getTemplateType() const {
    return getAttribute("type");
}

void CHTLJSTemplateNode::setTemplateType(const std::string& templateType) {
    setAttribute("type", templateType);
}

// CHTLJSCustomNode方法实现
CHTLJSCustomNode::CHTLJSCustomNode(const std::string& customName) : CHTLJSBaseNode(CHTLJSNodeType::CUSTOM) {
    setName(customName);
}

CHTLJSCustomNode::~CHTLJSCustomNode() {}

std::string CHTLJSCustomNode::getCustomName() const {
    return getName();
}

void CHTLJSCustomNode::setCustomName(const std::string& customName) {
    setName(customName);
}

// CHTLJSStyleNode方法实现
CHTLJSStyleNode::CHTLJSStyleNode() : CHTLJSBaseNode(CHTLJSNodeType::STYLE) {}

CHTLJSStyleNode::~CHTLJSStyleNode() {}

void CHTLJSStyleNode::addProperty(const std::string& property, const std::string& value) {
    properties[property] = value;
}

std::string CHTLJSStyleNode::getProperty(const std::string& property) const {
    auto it = properties.find(property);
    return it != properties.end() ? it->second : "";
}

const std::map<std::string, std::string>& CHTLJSStyleNode::getProperties() const {
    return properties;
}

bool CHTLJSStyleNode::hasProperty(const std::string& property) const {
    return properties.find(property) != properties.end();
}

void CHTLJSStyleNode::clearProperties() {
    properties.clear();
}

// CHTLJSScriptNode方法实现
CHTLJSScriptNode::CHTLJSScriptNode() : CHTLJSBaseNode(CHTLJSNodeType::SCRIPT) {}

CHTLJSScriptNode::~CHTLJSScriptNode() {}

std::string CHTLJSScriptNode::getScript() const {
    return getValue();
}

void CHTLJSScriptNode::setScript(const std::string& script) {
    setValue(script);
}

// CHTLJSOriginNode方法实现
CHTLJSOriginNode::CHTLJSOriginNode(const std::string& originType) 
    : CHTLJSBaseNode(CHTLJSNodeType::ORIGIN), originType(originType) {}

CHTLJSOriginNode::~CHTLJSOriginNode() {}

std::string CHTLJSOriginNode::getOriginType() const {
    return originType;
}

void CHTLJSOriginNode::setOriginType(const std::string& newOriginType) {
    originType = newOriginType;
}

std::string CHTLJSOriginNode::getOriginContent() const {
    return originContent;
}

void CHTLJSOriginNode::setOriginContent(const std::string& content) {
    originContent = content;
}

// CHTLJSImportNode方法实现
CHTLJSImportNode::CHTLJSImportNode(const std::string& importPath) : CHTLJSBaseNode(CHTLJSNodeType::IMPORT) {
    setValue(importPath);
}

CHTLJSImportNode::~CHTLJSImportNode() {}

std::string CHTLJSImportNode::getPath() const {
    return getValue();
}

void CHTLJSImportNode::setPath(const std::string& path) {
    setValue(path);
}

std::string CHTLJSImportNode::getImportType() const {
    return getAttribute("type");
}

void CHTLJSImportNode::setImportType(const std::string& importType) {
    setAttribute("type", importType);
}

// CHTLJSConfigNode方法实现
CHTLJSConfigNode::CHTLJSConfigNode() : CHTLJSBaseNode(CHTLJSNodeType::CONFIG) {}

CHTLJSConfigNode::~CHTLJSConfigNode() {}

void CHTLJSConfigNode::addConfig(const std::string& key, const std::string& value) {
    configs[key] = value;
}

std::string CHTLJSConfigNode::getConfig(const std::string& key) const {
    auto it = configs.find(key);
    return it != configs.end() ? it->second : "";
}

const std::map<std::string, std::string>& CHTLJSConfigNode::getConfigs() const {
    return configs;
}

bool CHTLJSConfigNode::hasConfig(const std::string& key) const {
    return configs.find(key) != configs.end();
}

// CHTLJSNamespaceNode方法实现
CHTLJSNamespaceNode::CHTLJSNamespaceNode(const std::string& namespaceName) : CHTLJSBaseNode(CHTLJSNodeType::NAMESPACE) {
    setName(namespaceName);
}

CHTLJSNamespaceNode::~CHTLJSNamespaceNode() {}

std::string CHTLJSNamespaceNode::getNamespaceName() const {
    return getName();
}

void CHTLJSNamespaceNode::setNamespaceName(const std::string& namespaceName) {
    setName(namespaceName);
}

// CHTLJSOperatorNode方法实现
CHTLJSOperatorNode::CHTLJSOperatorNode(const std::string& operatorType) : CHTLJSBaseNode(CHTLJSNodeType::OPERATOR) {
    setValue(operatorType);
}

CHTLJSOperatorNode::~CHTLJSOperatorNode() {}

std::string CHTLJSOperatorNode::getOperatorType() const {
    return getValue();
}

void CHTLJSOperatorNode::setOperatorType(const std::string& operatorType) {
    setValue(operatorType);
}

} // namespace CHTL