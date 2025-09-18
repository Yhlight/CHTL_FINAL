#include "ElementNode.h"
#include <algorithm>

namespace CHTL {

const std::vector<std::string> ElementNode::s_selfClosingTags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

const std::vector<std::string> ElementNode::s_html5Tags = {
    "article", "aside", "audio", "bdi", "canvas", "datalist", "details",
    "dialog", "embed", "figcaption", "figure", "footer", "header", "hgroup",
    "main", "mark", "meter", "nav", "output", "progress", "rp", "rt", "ruby",
    "section", "summary", "time", "video", "wbr"
};

ElementNode::ElementNode(const std::string& tagName) 
    : BaseNode(NodeType::ELEMENT), m_tagName(tagName), m_selfClosing(false) {
}

bool ElementNode::isHTML5Element() const {
    return std::find(s_html5Tags.begin(), s_html5Tags.end(), m_tagName) != s_html5Tags.end();
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(m_tagName);
    cloned->m_selfClosing = m_selfClosing;
    cloned->m_value = m_value;
    cloned->m_attributes = m_attributes;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ElementNode::toString() const {
    std::string result = "<" + m_tagName;
    
    // 添加属性
    for (const auto& attr : m_attributes) {
        result += " " + attr.first + "=\"" + attr.second + "\"";
    }
    
    if (m_selfClosing) {
        result += " />";
    } else {
        result += ">";
        
        // 添加子节点
        for (const auto& child : m_children) {
            result += child->toString();
        }
        
        result += "</" + m_tagName + ">";
    }
    
    return result;
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visitElement(*this);
}

} // namespace CHTL