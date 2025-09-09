#include "ElementNode.hpp"
#include <sstream>
#include <algorithm>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName, size_t line, size_t column)
    : BaseNode(NodeType::ELEMENT, line, column), tagName_(tagName), selfClosing_(false) {
    // 检查是否为自闭合标签
    selfClosing_ = isSelfClosing();
}

bool ElementNode::isSelfClosing() const {
    static const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    return isTagOfType(selfClosingTags);
}

bool ElementNode::isBlockElement() const {
    static const std::vector<std::string> blockTags = {
        "address", "article", "aside", "blockquote", "canvas", "dd", "div", "dl",
        "dt", "fieldset", "figcaption", "figure", "footer", "form", "h1", "h2",
        "h3", "h4", "h5", "h6", "header", "hr", "li", "main", "nav", "noscript",
        "ol", "p", "pre", "section", "table", "tfoot", "ul", "video"
    };
    return isTagOfType(blockTags);
}

bool ElementNode::isInlineElement() const {
    static const std::vector<std::string> inlineTags = {
        "a", "abbr", "acronym", "b", "bdi", "bdo", "big", "br", "button",
        "cite", "code", "dfn", "em", "i", "kbd", "label", "map", "mark",
        "meter", "noscript", "output", "progress", "q", "ruby", "s", "samp",
        "small", "span", "strong", "sub", "sup", "time", "tt", "u", "var",
        "wbr"
    };
    return isTagOfType(inlineTags);
}

bool ElementNode::isInlineBlockElement() const {
    static const std::vector<std::string> inlineBlockTags = {
        "audio", "canvas", "embed", "iframe", "img", "input", "object",
        "select", "textarea", "video"
    };
    return isTagOfType(inlineBlockTags);
}

std::string ElementNode::toHTML() const {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << tagName_;
    
    // 添加属性
    for (const auto& attr : getAttributes()) {
        oss << " " << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << attr.second << "\"";
        }
    }
    
    if (selfClosing_) {
        oss << " />";
    } else {
        oss << ">";
        
        // 添加子节点内容
        for (const auto& child : getChildren()) {
            if (child) {
                oss << child->toHTML();
            }
        }
        
        // 结束标签
        oss << "</" << tagName_ << ">";
    }
    
    return oss.str();
}

std::string ElementNode::toString() const {
    std::ostringstream oss;
    oss << "ElementNode(tag=" << tagName_ 
        << ", line=" << getLine() << ", column=" << getColumn()
        << ", children=" << getChildCount() << ")";
    return oss.str();
}

bool ElementNode::isTagOfType(const std::vector<std::string>& tagList) const {
    return std::find(tagList.begin(), tagList.end(), tagName_) != tagList.end();
}

} // namespace CHTL