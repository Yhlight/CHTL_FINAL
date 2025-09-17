#include "CHTL/ElementNode.h"
#include "CHTL/StyleNode.h"
#include "CHTL/ScriptNode.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

// 属性管理
void ElementNode::setAttribute(const String& name, const String& value) {
    attributes_[name] = value;
}

String ElementNode::getAttribute(const String& name) const {
    auto it = attributes_.find(name);
    return it != attributes_.end() ? it->second : "";
}

bool ElementNode::hasAttribute(const String& name) const {
    return attributes_.find(name) != attributes_.end();
}

void ElementNode::removeAttribute(const String& name) {
    attributes_.erase(name);
}

void ElementNode::clearAttributes() {
    attributes_.clear();
}

// 元素类型检查
bool ElementNode::isBlockElement() const {
    static const std::unordered_set<String> blockElements = {
        "div", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "fieldset", "legend", "section", "article",
        "aside", "header", "footer", "nav", "main", "figure",
        "figcaption", "blockquote", "pre", "address"
    };
    return blockElements.find(tagName_) != blockElements.end();
}

bool ElementNode::isInlineElement() const {
    static const std::unordered_set<String> inlineElements = {
        "span", "a", "strong", "em", "b", "i", "u", "s",
        "small", "mark", "del", "ins", "sub", "sup",
        "code", "kbd", "samp", "var", "cite", "abbr",
        "time", "data", "q", "dfn"
    };
    return inlineElements.find(tagName_) != inlineElements.end();
}

bool ElementNode::isInlineBlockElement() const {
    static const std::unordered_set<String> inlineBlockElements = {
        "img", "input", "button", "select", "textarea",
        "label", "progress", "meter", "canvas", "video",
        "audio", "iframe", "object", "embed"
    };
    return inlineBlockElements.find(tagName_) != inlineBlockElements.end();
}

bool ElementNode::isSelfClosingElement() const {
    static const std::unordered_set<String> selfClosingElements = {
        "img", "input", "br", "hr", "meta", "link", "area",
        "base", "col", "embed", "param", "source", "track", "wbr"
    };
    return selfClosingElements.find(tagName_) != selfClosingElements.end();
}

bool ElementNode::isVoidElementType() const {
    return isSelfClosingElement();
}

// 查找子元素
ElementNodePtr ElementNode::findElement(const String& tagName) const {
    for (const auto& child : children_) {
        if (child->isElement()) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->getTagName() == tagName) {
                return element;
            }
        }
    }
    return nullptr;
}

std::vector<ElementNodePtr> ElementNode::findElements(const String& tagName) const {
    std::vector<ElementNodePtr> result;
    for (const auto& child : children_) {
        if (child->isElement()) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->getTagName() == tagName) {
                result.push_back(element);
            }
        }
    }
    return result;
}

ElementNodePtr ElementNode::findElementById(const String& id) const {
    for (const auto& child : children_) {
        if (child->isElement()) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->getId() == id) {
                return element;
            }
        }
    }
    return nullptr;
}

std::vector<ElementNodePtr> ElementNode::findElementsByClass(const String& className) const {
    std::vector<ElementNodePtr> result;
    for (const auto& child : children_) {
        if (child->isElement()) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            String classAttr = element->getClass();
            if (!classAttr.empty() && classAttr.find(className) != String::npos) {
                result.push_back(element);
            }
        }
    }
    return result;
}

// 查找样式和脚本节点
StyleNodePtr ElementNode::findStyleNode() const {
    for (const auto& child : children_) {
        if (child->isStyle()) {
            return std::static_pointer_cast<StyleNode>(child);
        }
    }
    return nullptr;
}

ScriptNodePtr ElementNode::findScriptNode() const {
    for (const auto& child : children_) {
        if (child->isScript()) {
            return std::static_pointer_cast<ScriptNode>(child);
        }
    }
    return nullptr;
}

// 实现基类方法
String ElementNode::toString() const {
    std::ostringstream oss;
    oss << "<" << tagName_;
    
    for (const auto& attr : attributes_) {
        oss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    if (isSelfClosing_ || isVoidElement_) {
        oss << " />";
    } else {
        oss << ">";
        if (!textContent_.empty()) {
            oss << textContent_;
        }
        for (const auto& child : children_) {
            oss << child->toString();
        }
        oss << "</" << tagName_ << ">";
    }
    
    return oss.str();
}

String ElementNode::toHTML() const {
    return toString();
}

bool ElementNode::isValid() const {
    // 基本验证：标签名不能为空
    if (tagName_.empty()) {
        return false;
    }
    
    // 自闭合元素不能有子节点
    if (isSelfClosing_ && !children_.empty()) {
        return false;
    }
    
    // void元素不能有子节点
    if (isVoidElement_ && !children_.empty()) {
        return false;
    }
    
    return true;
}

void ElementNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
ElementNodePtr ElementNode::create(const String& tagName, const SourceLocation& loc) {
    return std::make_shared<ElementNode>(tagName, loc);
}

ElementNodePtr ElementNode::createHTML(const SourceLocation& loc) {
    return create("html", loc);
}

ElementNodePtr ElementNode::createHead(const SourceLocation& loc) {
    return create("head", loc);
}

ElementNodePtr ElementNode::createBody(const SourceLocation& loc) {
    return create("body", loc);
}

ElementNodePtr ElementNode::createDiv(const SourceLocation& loc) {
    return create("div", loc);
}

ElementNodePtr ElementNode::createSpan(const SourceLocation& loc) {
    return create("span", loc);
}

ElementNodePtr ElementNode::createP(const SourceLocation& loc) {
    return create("p", loc);
}

ElementNodePtr ElementNode::createA(const String& href, const SourceLocation& loc) {
    auto element = create("a", loc);
    element->setAttribute("href", href);
    return element;
}

ElementNodePtr ElementNode::createImg(const String& src, const String& alt, const SourceLocation& loc) {
    auto element = create("img", loc);
    element->setAttribute("src", src);
    element->setAttribute("alt", alt);
    element->setSelfClosing(true);
    return element;
}

ElementNodePtr ElementNode::createInput(const String& type, const SourceLocation& loc) {
    auto element = create("input", loc);
    element->setAttribute("type", type);
    element->setSelfClosing(true);
    return element;
}

ElementNodePtr ElementNode::createButton(const String& text, const SourceLocation& loc) {
    auto element = create("button", loc);
    element->setTextContent(text);
    return element;
}

ElementNodePtr ElementNode::createForm(const String& action, const SourceLocation& loc) {
    auto element = create("form", loc);
    element->setAttribute("action", action);
    return element;
}

ElementNodePtr ElementNode::createTable(const SourceLocation& loc) {
    return create("table", loc);
}

ElementNodePtr ElementNode::createTr(const SourceLocation& loc) {
    return create("tr", loc);
}

ElementNodePtr ElementNode::createTd(const SourceLocation& loc) {
    return create("td", loc);
}

ElementNodePtr ElementNode::createTh(const SourceLocation& loc) {
    return create("th", loc);
}

ElementNodePtr ElementNode::createUl(const SourceLocation& loc) {
    return create("ul", loc);
}

ElementNodePtr ElementNode::createOl(const SourceLocation& loc) {
    return create("ol", loc);
}

ElementNodePtr ElementNode::createLi(const SourceLocation& loc) {
    return create("li", loc);
}

ElementNodePtr ElementNode::createH1(const SourceLocation& loc) {
    return create("h1", loc);
}

ElementNodePtr ElementNode::createH2(const SourceLocation& loc) {
    return create("h2", loc);
}

ElementNodePtr ElementNode::createH3(const SourceLocation& loc) {
    return create("h3", loc);
}

ElementNodePtr ElementNode::createH4(const SourceLocation& loc) {
    return create("h4", loc);
}

ElementNodePtr ElementNode::createH5(const SourceLocation& loc) {
    return create("h5", loc);
}

ElementNodePtr ElementNode::createH6(const SourceLocation& loc) {
    return create("h6", loc);
}

} // namespace CHTL