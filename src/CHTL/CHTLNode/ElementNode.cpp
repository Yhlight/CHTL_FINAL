#include "CHTL/CHTLNode/ElementNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName) 
    : BaseNode(NodeType::ELEMENT), m_tagName(tagName), m_selfClosing(false) {
}

ElementNode::~ElementNode() = default;

void ElementNode::setTagName(const std::string& tagName) {
    m_tagName = tagName;
}

std::string ElementNode::getTagName() const {
    return m_tagName;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    m_selfClosing = selfClosing;
}

bool ElementNode::isSelfClosing() const {
    return m_selfClosing;
}

bool ElementNode::isBlockElement() const {
    return isBlockElementTag(m_tagName);
}

bool ElementNode::isInlineElement() const {
    return isInlineElementTag(m_tagName);
}

void ElementNode::setTextContent(const std::string& text) {
    m_textContent = text;
}

std::string ElementNode::getTextContent() const {
    return m_textContent;
}

void ElementNode::addStyleProperty(const std::string& property, const std::string& value) {
    m_styleProperties[property] = value;
}

std::string ElementNode::getStyleProperty(const std::string& property) const {
    auto it = m_styleProperties.find(property);
    return (it != m_styleProperties.end()) ? it->second : "";
}

bool ElementNode::hasStyleProperty(const std::string& property) const {
    return m_styleProperties.find(property) != m_styleProperties.end();
}

void ElementNode::removeStyleProperty(const std::string& property) {
    m_styleProperties.erase(property);
}

const std::unordered_map<std::string, std::string>& ElementNode::getStyleProperties() const {
    return m_styleProperties;
}

void ElementNode::addClass(const std::string& className) {
    if (std::find(m_classes.begin(), m_classes.end(), className) == m_classes.end()) {
        m_classes.push_back(className);
    }
}

void ElementNode::removeClass(const std::string& className) {
    auto it = std::find(m_classes.begin(), m_classes.end(), className);
    if (it != m_classes.end()) {
        m_classes.erase(it);
    }
}

bool ElementNode::hasClass(const std::string& className) const {
    return std::find(m_classes.begin(), m_classes.end(), className) != m_classes.end();
}

std::vector<std::string> ElementNode::getClasses() const {
    return m_classes;
}

void ElementNode::setClasses(const std::vector<std::string>& classes) {
    m_classes = classes;
}

void ElementNode::setID(const std::string& id) {
    m_id = id;
}

std::string ElementNode::getID() const {
    return m_id;
}

bool ElementNode::hasID() const {
    return !m_id.empty();
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visitElement(*this);
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(m_tagName);
    cloned->m_selfClosing = m_selfClosing;
    cloned->m_textContent = m_textContent;
    cloned->m_styleProperties = m_styleProperties;
    cloned->m_classes = m_classes;
    cloned->m_id = m_id;
    cloned->m_attributes = m_attributes;
    cloned->m_content = m_content;
    cloned->m_name = m_name;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool ElementNode::validate() const {
    if (m_tagName.empty()) {
        return false;
    }
    
    // 验证自闭合标签
    if (m_selfClosing && !isSelfClosingTag(m_tagName)) {
        return false;
    }
    
    return BaseNode::validate();
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "ElementNode{tag='" << m_tagName << "', selfClosing=" << m_selfClosing;
    if (!m_textContent.empty()) {
        ss << ", text='" << m_textContent << "'";
    }
    if (!m_id.empty()) {
        ss << ", id='" << m_id << "'";
    }
    if (!m_classes.empty()) {
        ss << ", classes=[";
        for (size_t i = 0; i < m_classes.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "'" << m_classes[i] << "'";
        }
        ss << "]";
    }
    ss << "}";
    return ss.str();
}

bool ElementNode::isBlockElementTag(const std::string& tagName) const {
    static const std::vector<std::string> blockTags = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "dl", "dt", "dd",
        "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "fieldset", "legend",
        "article", "section", "aside", "nav", "header", "footer", "main",
        "blockquote", "pre", "address", "figure", "figcaption"
    };
    
    return std::find(blockTags.begin(), blockTags.end(), tagName) != blockTags.end();
}

bool ElementNode::isInlineElementTag(const std::string& tagName) const {
    static const std::vector<std::string> inlineTags = {
        "span", "a", "strong", "em", "b", "i", "u", "s", "small", "mark",
        "del", "ins", "sub", "sup", "code", "kbd", "samp", "var",
        "abbr", "acronym", "cite", "q", "dfn", "time"
    };
    
    return std::find(inlineTags.begin(), inlineTags.end(), tagName) != inlineTags.end();
}

bool ElementNode::isSelfClosingTag(const std::string& tagName) const {
    static const std::vector<std::string> selfClosingTags = {
        "img", "br", "hr", "input", "meta", "link", "area", "base",
        "col", "embed", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) != selfClosingTags.end();
}

} // namespace CHTL