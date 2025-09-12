#include "CHTL/CHTLNode/ElementNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName)
    : BaseNode(NodeType::ELEMENT)
    , m_tagName(tagName)
    , m_selfClosing(false)
{
    m_selfClosing = isSelfClosingTag(tagName);
}

ElementNode::~ElementNode() {
}

void ElementNode::setTagName(const std::string& tagName) {
    m_tagName = tagName;
    m_selfClosing = isSelfClosingTag(tagName);
}

const std::string& ElementNode::getTagName() const {
    return m_tagName;
}

bool ElementNode::isSelfClosing() const {
    return m_selfClosing;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    m_selfClosing = selfClosing;
}

bool ElementNode::isBlockElement() const {
    return isBlockTag(m_tagName);
}

bool ElementNode::isInlineElement() const {
    return isInlineTag(m_tagName);
}

bool ElementNode::isInlineBlockElement() const {
    return isInlineBlockTag(m_tagName);
}

void ElementNode::addClass(const std::string& className) {
    if (className.empty()) return;
    
    std::string currentClasses = getAttribute("class");
    if (currentClasses.empty()) {
        setAttribute("class", className);
    } else {
        // 检查是否已存在
        if (!hasClass(className)) {
            setAttribute("class", currentClasses + " " + className);
        }
    }
}

bool ElementNode::removeClass(const std::string& className) {
    if (className.empty()) return false;
    
    std::string currentClasses = getAttribute("class");
    if (currentClasses.empty()) return false;
    
    std::vector<std::string> classes = getClasses();
    auto it = std::find(classes.begin(), classes.end(), className);
    if (it != classes.end()) {
        classes.erase(it);
        
        // 重新设置class属性
        if (classes.empty()) {
            removeAttribute("class");
        } else {
            std::ostringstream oss;
            for (size_t i = 0; i < classes.size(); ++i) {
                if (i > 0) oss << " ";
                oss << classes[i];
            }
            setAttribute("class", oss.str());
        }
        return true;
    }
    
    return false;
}

bool ElementNode::hasClass(const std::string& className) const {
    if (className.empty()) return false;
    
    std::vector<std::string> classes = getClasses();
    return std::find(classes.begin(), classes.end(), className) != classes.end();
}

std::vector<std::string> ElementNode::getClasses() const {
    std::vector<std::string> classes;
    std::string classAttr = getAttribute("class");
    
    if (!classAttr.empty()) {
        std::istringstream iss(classAttr);
        std::string className;
        while (iss >> className) {
            if (!className.empty()) {
                classes.push_back(className);
            }
        }
    }
    
    return classes;
}

void ElementNode::setId(const std::string& id) {
    setAttribute("id", id);
}

std::string ElementNode::getId() const {
    return getAttribute("id");
}

bool ElementNode::hasId() const {
    return hasAttribute("id");
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(m_tagName);
    cloned->m_selfClosing = m_selfClosing;
    
    // 复制基类属性
    cloned->m_name = m_name;
    cloned->m_value = m_value;
    cloned->m_attributes = m_attributes;
    cloned->m_metadata = m_metadata;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        if (child) {
            cloned->addChild(child->clone());
        }
    }
    
    return cloned;
}

std::string ElementNode::toString() const {
    std::ostringstream oss;
    oss << "ElementNode(tag=\"" << m_tagName << "\""
        << ", selfClosing=" << (m_selfClosing ? "true" : "false")
        << ", children=" << m_children.size()
        << ", attributes=" << m_attributes.size()
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

// void ElementNode::accept(NodeVisitor& visitor) {
//     visitor.visit(*this);
// }

bool ElementNode::isSelfClosingTag(const std::string& tagName) {
    static const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) != selfClosingTags.end();
}

bool ElementNode::isBlockTag(const std::string& tagName) {
    static const std::vector<std::string> blockTags = {
        "address", "article", "aside", "blockquote", "body", "canvas",
        "dd", "div", "dl", "dt", "fieldset", "figcaption", "figure",
        "footer", "form", "h1", "h2", "h3", "h4", "h5", "h6", "header",
        "hr", "li", "main", "nav", "noscript", "ol", "output", "p",
        "pre", "section", "table", "tfoot", "ul", "video"
    };
    
    return std::find(blockTags.begin(), blockTags.end(), tagName) != blockTags.end();
}

bool ElementNode::isInlineTag(const std::string& tagName) {
    static const std::vector<std::string> inlineTags = {
        "a", "abbr", "acronym", "b", "bdi", "bdo", "big", "br", "button",
        "cite", "code", "dfn", "em", "i", "kbd", "label", "map", "mark",
        "meter", "noscript", "object", "output", "progress", "q", "ruby",
        "s", "samp", "script", "select", "small", "span", "strong", "sub",
        "sup", "textarea", "time", "tt", "u", "var", "wbr"
    };
    
    return std::find(inlineTags.begin(), inlineTags.end(), tagName) != inlineTags.end();
}

bool ElementNode::isInlineBlockTag(const std::string& tagName) {
    static const std::vector<std::string> inlineBlockTags = {
        "img", "input", "select", "textarea", "button"
    };
    
    return std::find(inlineBlockTags.begin(), inlineBlockTags.end(), tagName) != inlineBlockTags.end();
}

} // namespace CHTL