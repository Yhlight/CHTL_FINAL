#include "ElementNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// 静态成员初始化
std::set<std::string> ElementNode::selfClosingTags_;
std::set<std::string> ElementNode::blockElements_;
std::set<std::string> ElementNode::inlineElements_;

ElementNode::ElementNode(const std::string& tagName)
    : BaseNode(NodeType::ELEMENT, tagName), isSelfClosing_(false) {
    initializeStaticData();
    isSelfClosing_ = isSelfClosingTag(tagName);
}

void ElementNode::addInlineStyle(const std::string& property, const std::string& value) {
    inlineStyles_[property] = value;
}

std::string ElementNode::getInlineStyle(const std::string& property) const {
    auto it = inlineStyles_.find(property);
    return (it != inlineStyles_.end()) ? it->second : "";
}

bool ElementNode::hasInlineStyle(const std::string& property) const {
    return inlineStyles_.find(property) != inlineStyles_.end();
}

void ElementNode::removeInlineStyle(const std::string& property) {
    inlineStyles_.erase(property);
}

std::string ElementNode::getInlineStyles() const {
    return formatInlineStyles();
}

void ElementNode::addClass(const std::string& className) {
    classes_.insert(className);
}

void ElementNode::removeClass(const std::string& className) {
    classes_.erase(className);
}

bool ElementNode::hasClass(const std::string& className) const {
    return classes_.find(className) != classes_.end();
}

std::string ElementNode::getClasses() const {
    std::ostringstream oss;
    bool first = true;
    for (const auto& className : classes_) {
        if (!first) oss << " ";
        oss << className;
        first = false;
    }
    return oss.str();
}

void ElementNode::setTextContent(const std::string& text) {
    content_ = text;
}

std::string ElementNode::getTextContent() const {
    return content_;
}

std::string ElementNode::toHTML() const {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << name_;
    
    // 属性
    std::string attrs = formatAttributes();
    if (!attrs.empty()) {
        oss << " " << attrs;
    }
    
    // 内联样式
    std::string styles = formatInlineStyles();
    if (!styles.empty()) {
        oss << " style=\"" << styles << "\"";
    }
    
    // 自闭合标签
    if (isSelfClosing_) {
        oss << " />";
        return oss.str();
    }
    
    oss << ">";
    
    // 文本内容
    if (!content_.empty()) {
        oss << escapeHTML(content_);
    }
    
    // 子节点
    for (const auto& child : children_) {
        oss << child->toHTML();
    }
    
    // 结束标签
    oss << "</" << name_ << ">";
    
    return oss.str();
}

std::string ElementNode::toCSS() const {
    std::ostringstream oss;
    
    // 生成基于类名的CSS规则
    for (const auto& className : classes_) {
        oss << "." << className << " {\n";
        
        // 内联样式转换为CSS规则
        for (const auto& style : inlineStyles_) {
            oss << "    " << style.first << ": " << style.second << ";\n";
        }
        
        oss << "}\n\n";
    }
    
    return oss.str();
}

std::string ElementNode::toJS() const {
    // 元素节点通常不直接生成JS代码
    // 但可以生成一些辅助代码，比如选择器
    std::ostringstream oss;
    
    if (!classes_.empty()) {
        oss << "// Element: " << name_ << " with classes: ";
        bool first = true;
        for (const auto& className : classes_) {
            if (!first) oss << ", ";
            oss << className;
            first = false;
        }
        oss << "\n";
    }
    
    return oss.str();
}

bool ElementNode::isValid() const {
    // 基本验证
    if (name_.empty()) {
        return false;
    }
    
    // 自闭合标签不能有子节点
    if (isSelfClosing_ && !children_.empty()) {
        return false;
    }
    
    return BaseNode::isValid();
}

bool ElementNode::isSelfClosingTag(const std::string& tagName) {
    return selfClosingTags_.find(tagName) != selfClosingTags_.end();
}

bool ElementNode::isBlockElement(const std::string& tagName) {
    return blockElements_.find(tagName) != blockElements_.end();
}

bool ElementNode::isInlineElement(const std::string& tagName) {
    return inlineElements_.find(tagName) != inlineElements_.end();
}

std::string ElementNode::formatAttributes() const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& attr : attributes_) {
        if (!first) oss << " ";
        oss << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
        first = false;
    }
    
    // 添加类名
    if (!classes_.empty()) {
        if (!first) oss << " ";
        oss << "class=\"" << getClasses() << "\"";
    }
    
    return oss.str();
}

std::string ElementNode::formatInlineStyles() const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& style : inlineStyles_) {
        if (!first) oss << "; ";
        oss << style.first << ": " << style.second;
        first = false;
    }
    
    return oss.str();
}

void ElementNode::initializeStaticData() {
    if (selfClosingTags_.empty()) {
        // 自闭合标签
        selfClosingTags_ = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        };
        
        // 块级元素
        blockElements_ = {
            "div", "p", "h1", "h2", "h3", "h4", "h5", "h6", "section", "article",
            "aside", "header", "footer", "nav", "main", "figure", "figcaption",
            "ul", "ol", "li", "dl", "dt", "dd", "table", "thead", "tbody",
            "tfoot", "tr", "th", "td", "form", "fieldset", "legend", "blockquote",
            "pre", "address", "canvas", "video", "audio"
        };
        
        // 行内元素
        inlineElements_ = {
            "span", "a", "strong", "em", "b", "i", "u", "s", "small", "mark",
            "del", "ins", "sub", "sup", "code", "kbd", "samp", "var", "cite",
            "abbr", "time", "data", "q", "dfn", "ruby", "rt", "rp", "bdi",
            "bdo", "wbr", "br", "img", "input", "button", "select", "textarea",
            "label", "output", "progress", "meter"
        };
    }
}

} // namespace CHTL