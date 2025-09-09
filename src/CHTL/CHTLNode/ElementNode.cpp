#include "CHTL/CHTLNode/ElementNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName, size_t line, size_t column)
    : BaseNode(NodeType::ELEMENT, line, column), tagName_(tagName), isSelfClosing_(false) {
    isSelfClosing_ = isSelfClosingTag(tagName);
}

void ElementNode::accept(Visitor& visitor) {
    visitor.visit(*this);
}

std::string ElementNode::generate() {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << tagName_;
    
    // 添加属性
    for (const auto& attr : attributes_) {
        oss << " " << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << attr.second << "\"";
        }
    }
    
    if (isSelfClosing_) {
        oss << " />";
    } else {
        oss << ">";
        
        // 添加子节点
        for (const auto& child : children_) {
            oss << child->generate();
        }
        
        // 结束标签
        oss << "</" << tagName_ << ">";
    }
    
    return oss.str();
}

bool ElementNode::isBlockElement() const {
    return isBlockTag(tagName_);
}

bool ElementNode::isInlineElement() const {
    return isInlineTag(tagName_);
}

bool ElementNode::isInlineBlockElement() const {
    return isInlineBlockTag(tagName_);
}

std::string ElementNode::toString() const {
    std::ostringstream oss;
    oss << "ElementNode(" << tagName_ << ", " << line_ << ":" << column_ 
        << ", self-closing: " << (isSelfClosing_ ? "true" : "false") 
        << ", children: " << children_.size() << ")";
    return oss.str();
}

bool ElementNode::isSelfClosingTag(const std::string& tagName) const {
    static const std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tagName) != selfClosingTags.end();
}

bool ElementNode::isBlockTag(const std::string& tagName) const {
    static const std::vector<std::string> blockTags = {
        "address", "article", "aside", "blockquote", "canvas", "dd", "div", "dl",
        "dt", "fieldset", "figcaption", "figure", "footer", "form", "h1", "h2",
        "h3", "h4", "h5", "h6", "header", "hr", "li", "main", "nav", "noscript",
        "ol", "p", "pre", "section", "table", "tfoot", "ul", "video"
    };
    
    return std::find(blockTags.begin(), blockTags.end(), tagName) != blockTags.end();
}

bool ElementNode::isInlineTag(const std::string& tagName) const {
    static const std::vector<std::string> inlineTags = {
        "a", "abbr", "acronym", "b", "bdi", "bdo", "big", "br", "button",
        "cite", "code", "dfn", "em", "i", "kbd", "label", "map", "mark",
        "meter", "noscript", "output", "q", "ruby", "s", "samp", "script",
        "select", "small", "span", "strong", "sub", "sup", "svg", "textarea",
        "time", "tt", "u", "var", "wbr"
    };
    
    return std::find(inlineTags.begin(), inlineTags.end(), tagName) != inlineTags.end();
}

bool ElementNode::isInlineBlockTag(const std::string& tagName) const {
    static const std::vector<std::string> inlineBlockTags = {
        "button", "input", "select", "textarea"
    };
    
    return std::find(inlineBlockTags.begin(), inlineBlockTags.end(), tagName) != inlineBlockTags.end();
}

} // namespace CHTL