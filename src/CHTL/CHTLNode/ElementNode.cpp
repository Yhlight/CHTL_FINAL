#include "ElementNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName) 
    : BaseNode(NodeType::ELEMENT), m_tagName(tagName), m_selfClosing(false) {}

void ElementNode::addAttribute(const std::string& name, const std::string& value) {
    m_attributes[name] = value;
}

std::string ElementNode::getAttribute(const std::string& name) const {
    auto it = m_attributes.find(name);
    return it != m_attributes.end() ? it->second : "";
}

bool ElementNode::hasAttribute(const std::string& name) const {
    return m_attributes.find(name) != m_attributes.end();
}

bool ElementNode::removeAttribute(const std::string& name) {
    auto it = m_attributes.find(name);
    if (it != m_attributes.end()) {
        m_attributes.erase(it);
        return true;
    }
    return false;
}

void ElementNode::setTextContent(const std::string& text) {
    m_textContent = text;
}

bool ElementNode::isSelfClosing() const {
    return m_selfClosing || isSelfClosingTag(m_tagName);
}

bool ElementNode::isBlockElement() const {
    return isBlockElement(m_tagName);
}

bool ElementNode::isInlineElement() const {
    return isInlineElement(m_tagName);
}

bool ElementNode::isInlineBlockElement() const {
    return isInlineBlockElement(m_tagName);
}

std::string ElementNode::getId() const {
    return getAttribute("id");
}

void ElementNode::setId(const std::string& id) {
    addAttribute("id", id);
}

std::string ElementNode::getClass() const {
    return getAttribute("class");
}

void ElementNode::setClass(const std::string& className) {
    addAttribute("class", className);
}

void ElementNode::addClass(const std::string& className) {
    std::string currentClass = getClass();
    if (currentClass.empty()) {
        setClass(className);
    } else {
        setClass(currentClass + " " + className);
    }
}

void ElementNode::removeClass(const std::string& className) {
    std::string currentClass = getClass();
    if (currentClass.empty()) return;
    
    std::vector<std::string> classes;
    std::stringstream ss(currentClass);
    std::string token;
    
    while (ss >> token) {
        if (token != className) {
            classes.push_back(token);
        }
    }
    
    if (classes.empty()) {
        removeAttribute("class");
    } else {
        std::string newClass;
        for (size_t i = 0; i < classes.size(); ++i) {
            if (i > 0) newClass += " ";
            newClass += classes[i];
        }
        setClass(newClass);
    }
}

bool ElementNode::hasClass(const std::string& className) const {
    std::string currentClass = getClass();
    if (currentClass.empty()) return false;
    
    std::stringstream ss(currentClass);
    std::string token;
    while (ss >> token) {
        if (token == className) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ElementNode::getClasses() const {
    std::vector<std::string> classes;
    std::string currentClass = getClass();
    if (currentClass.empty()) return classes;
    
    std::stringstream ss(currentClass);
    std::string token;
    while (ss >> token) {
        classes.push_back(token);
    }
    return classes;
}

std::unique_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_unique<ElementNode>(m_tagName);
    cloned->m_attributes = m_attributes;
    cloned->m_textContent = m_textContent;
    cloned->m_selfClosing = m_selfClosing;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    if (m_token) {
        cloned->m_token = std::make_unique<Token>(*m_token);
    }
    
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

void ElementNode::accept(NodeVisitor* visitor) {
    if (visitor) {
        visitor->visitElement(this);
    }
}

std::string ElementNode::toString() const {
    std::stringstream ss;
    ss << "ElementNode(" << m_tagName;
    if (!m_attributes.empty()) {
        ss << ", " << m_attributes.size() << " attributes";
    }
    if (!m_textContent.empty()) {
        ss << ", text: \"" << m_textContent << "\"";
    }
    ss << ")";
    return ss.str();
}

std::string ElementNode::toHTML() const {
    std::stringstream ss;
    ss << getStartTagHTML();
    
    if (!m_textContent.empty()) {
        ss << escapeHTML(m_textContent);
    }
    
    for (const auto& child : m_children) {
        if (child->getType() == NodeType::ELEMENT) {
            auto* elementChild = static_cast<ElementNode*>(child.get());
            ss << elementChild->toHTML();
        } else if (child->getType() == NodeType::TEXT) {
            auto* textChild = static_cast<TextNode*>(child.get());
            ss << textChild->toHTML();
        }
    }
    
    if (!isSelfClosing()) {
        ss << getEndTagHTML();
    }
    
    return ss.str();
}

std::string ElementNode::getStartTagHTML() const {
    std::stringstream ss;
    ss << "<" << m_tagName;
    
    if (!m_attributes.empty()) {
        ss << " " << formatAttributesHTML();
    }
    
    if (isSelfClosing()) {
        ss << " />";
    } else {
        ss << ">";
    }
    
    return ss.str();
}

std::string ElementNode::getEndTagHTML() const {
    return "</" + m_tagName + ">";
}

bool ElementNode::isHTML5Tag(const std::string& tagName) {
    static const std::unordered_set<std::string> html5Tags = {
        "article", "aside", "audio", "bdi", "canvas", "datalist", "details",
        "dialog", "embed", "figcaption", "figure", "footer", "header", "hgroup",
        "keygen", "main", "mark", "meter", "nav", "output", "progress", "rp",
        "rt", "ruby", "section", "source", "summary", "time", "track", "video",
        "wbr"
    };
    return html5Tags.find(tagName) != html5Tags.end();
}

bool ElementNode::isSelfClosingTag(const std::string& tagName) {
    static const std::unordered_set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return selfClosingTags.find(tagName) != selfClosingTags.end();
}

bool ElementNode::isBlockElement(const std::string& tagName) {
    static const std::unordered_set<std::string> blockElements = {
        "address", "article", "aside", "blockquote", "body", "canvas", "dd",
        "div", "dl", "dt", "fieldset", "figcaption", "figure", "footer",
        "form", "h1", "h2", "h3", "h4", "h5", "h6", "head", "header", "hgroup",
        "hr", "html", "li", "main", "nav", "noscript", "ol", "output", "p",
        "pre", "section", "table", "tbody", "td", "tfoot", "th", "thead",
        "tr", "ul", "video"
    };
    return blockElements.find(tagName) != blockElements.end();
}

bool ElementNode::isInlineElement(const std::string& tagName) {
    static const std::unordered_set<std::string> inlineElements = {
        "a", "abbr", "acronym", "b", "bdi", "bdo", "big", "br", "button",
        "cite", "code", "dfn", "em", "i", "img", "input", "kbd", "label",
        "map", "mark", "meter", "noscript", "object", "output", "q", "ruby",
        "s", "samp", "script", "select", "small", "span", "strong", "sub",
        "sup", "textarea", "time", "tt", "u", "var", "wbr"
    };
    return inlineElements.find(tagName) != inlineElements.end();
}

bool ElementNode::isInlineBlockElement(const std::string& tagName) {
    static const std::unordered_set<std::string> inlineBlockElements = {
        "button", "input", "select", "textarea"
    };
    return inlineBlockElements.find(tagName) != inlineBlockElements.end();
}

std::string ElementNode::formatAttributesHTML() const {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& pair : m_attributes) {
        if (!first) {
            ss << " ";
        }
        ss << pair.first << "=\"" << escapeHTML(pair.second) << "\"";
        first = false;
    }
    
    return ss.str();
}

std::string ElementNode::escapeHTML(const std::string& text) {
    std::string result = text;
    
    // 替换HTML特殊字符
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

} // namespace CHTL