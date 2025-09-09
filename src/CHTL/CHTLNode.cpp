#include "CHTL/CHTLNode.h"
#include <sstream>

namespace CHTL {

CHTLNode::CHTLNode(NodeType type, const std::string& name)
    : m_type(type)
    , m_name(name)
    , m_line(0)
    , m_column(0)
{
}

void CHTLNode::addChild(std::shared_ptr<CHTLNode> child) {
    if (child) {
        m_children.push_back(child);
    }
}

void CHTLNode::removeChild(std::shared_ptr<CHTLNode> child) {
    auto it = std::find(m_children.begin(), m_children.end(), child);
    if (it != m_children.end()) {
        m_children.erase(it);
    }
}

const std::vector<std::shared_ptr<CHTLNode>>& CHTLNode::getChildren() const {
    return m_children;
}

std::vector<std::shared_ptr<CHTLNode>>& CHTLNode::getChildren() {
    return m_children;
}

void CHTLNode::setAttribute(const std::string& key, const AttributeValue& value) {
    m_attributes[key] = value;
}

AttributeValue CHTLNode::getAttribute(const std::string& key) const {
    auto it = m_attributes.find(key);
    if (it != m_attributes.end()) {
        return it->second;
    }
    return AttributeValue{};
}

bool CHTLNode::hasAttribute(const std::string& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

void CHTLNode::removeAttribute(const std::string& key) {
    m_attributes.erase(key);
}

const AttributeMap& CHTLNode::getAttributes() const {
    return m_attributes;
}

void CHTLNode::setTextContent(const std::string& text) {
    m_textContent = text;
}

const std::string& CHTLNode::getTextContent() const {
    return m_textContent;
}

void CHTLNode::setPosition(size_t line, size_t column) {
    m_line = line;
    m_column = column;
}

void CHTLNode::setNamespace(const std::string& ns) {
    m_namespace = ns;
}

const std::string& CHTLNode::getNamespace() const {
    return m_namespace;
}

std::string CHTLNode::toString() const {
    std::ostringstream oss;
    oss << "Node(" << static_cast<int>(m_type) << ", " << m_name << ")";
    return oss.str();
}

std::string CHTLNode::toHTML() const {
    return "";
}

std::string CHTLNode::toCSS() const {
    return "";
}

std::string CHTLNode::toJS() const {
    return "";
}

std::shared_ptr<CHTLNode> CHTLNode::findChild(const std::string& name) const {
    for (const auto& child : m_children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLNode::findChildren(const std::string& name) const {
    std::vector<std::shared_ptr<CHTLNode>> result;
    for (const auto& child : m_children) {
        if (child->getName() == name) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<CHTLNode> CHTLNode::findChildByType(NodeType type) const {
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLNode::findChildrenByType(NodeType type) const {
    std::vector<std::shared_ptr<CHTLNode>> result;
    for (const auto& child : m_children) {
        if (child->getType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

// ElementNode 实现
ElementNode::ElementNode(const std::string& tagName)
    : CHTLNode(NodeType::ELEMENT, tagName)
    , m_selfClosing(false)
{
}

const std::string& ElementNode::getTagName() const {
    return getName();
}

void ElementNode::setTagName(const std::string& tagName) {
    setName(tagName);
}

bool ElementNode::isSelfClosing() const {
    return m_selfClosing;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    m_selfClosing = selfClosing;
}

std::string ElementNode::toHTML() const {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << getTagName();
    
    // 属性
    for (const auto& attr : getAttributes()) {
        oss << " " << attr.first;
        if (std::holds_alternative<std::string>(attr.second)) {
            oss << "=\"" << std::get<std::string>(attr.second) << "\"";
        }
    }
    
    if (m_selfClosing) {
        oss << " />";
    } else {
        oss << ">";
        
        // 文本内容
        if (!getTextContent().empty()) {
            oss << getTextContent();
        }
        
        // 子节点
        for (const auto& child : getChildren()) {
            oss << child->toHTML();
        }
        
        // 结束标签
        oss << "</" << getTagName() << ">";
    }
    
    return oss.str();
}

// TextNode 实现
TextNode::TextNode(const std::string& text)
    : CHTLNode(NodeType::TEXT, "text")
{
    setTextContent(text);
}

std::string TextNode::toHTML() const {
    return getTextContent();
}

// StyleNode 实现
StyleNode::StyleNode()
    : CHTLNode(NodeType::STYLE, "style")
{
}

void StyleNode::addStyleProperty(const std::string& property, const std::string& value) {
    m_properties[property] = value;
}

void StyleNode::addStyleRule(const std::string& selector, const std::unordered_map<std::string, std::string>& properties) {
    m_rules[selector] = properties;
}

std::string StyleNode::toCSS() const {
    std::ostringstream oss;
    
    // 输出样式规则
    for (const auto& rule : m_rules) {
        oss << rule.first << " {\n";
        for (const auto& prop : rule.second) {
            oss << "  " << prop.first << ": " << prop.second << ";\n";
        }
        oss << "}\n\n";
    }
    
    // 输出内联样式
    if (!m_properties.empty()) {
        oss << "/* Inline styles */\n";
        for (const auto& prop : m_properties) {
            oss << prop.first << ": " << prop.second << ";\n";
        }
    }
    
    return oss.str();
}

// ScriptNode 实现
ScriptNode::ScriptNode()
    : CHTLNode(NodeType::SCRIPT, "script")
{
}

void ScriptNode::addScript(const std::string& script) {
    m_scripts.push_back(script);
}

std::string ScriptNode::toJS() const {
    std::ostringstream oss;
    
    for (const auto& script : m_scripts) {
        oss << script << "\n";
    }
    
    return oss.str();
}

// TemplateNode 实现
TemplateNode::TemplateNode(NodeType type, const std::string& name)
    : CHTLNode(type, name)
{
}

void TemplateNode::setTemplateType(const std::string& type) {
    m_templateType = type;
}

const std::string& TemplateNode::getTemplateType() const {
    return m_templateType;
}

std::string TemplateNode::toString() const {
    std::ostringstream oss;
    oss << "Template(" << m_templateType << ", " << getName() << ")";
    return oss.str();
}

// ImportNode 实现
ImportNode::ImportNode(NodeType type, const std::string& path)
    : CHTLNode(type, "import")
    , m_importPath(path)
{
}

void ImportNode::setImportPath(const std::string& path) {
    m_importPath = path;
}

const std::string& ImportNode::getImportPath() const {
    return m_importPath;
}

void ImportNode::setAlias(const std::string& alias) {
    m_alias = alias;
}

const std::string& ImportNode::getAlias() const {
    return m_alias;
}

std::string ImportNode::toString() const {
    std::ostringstream oss;
    oss << "Import(" << m_importPath;
    if (!m_alias.empty()) {
        oss << " as " << m_alias;
    }
    oss << ")";
    return oss.str();
}

} // namespace CHTL