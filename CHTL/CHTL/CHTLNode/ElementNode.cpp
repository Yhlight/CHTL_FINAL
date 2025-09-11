#include "ElementNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName, bool selfClosing)
    : BaseNode(NodeType::ELEMENT, tagName), isSelfClosing(selfClosing), isVoidElement(false) {
    initializeVoidElements();
    isVoidElement = isVoidElementTag(tagName);
}

void ElementNode::initializeVoidElements() {
    voidElements = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
}

void ElementNode::setAttribute(const std::string& key, const std::string& value) {
    BaseNode::setAttribute(key, value);
    
    // 特殊处理class和id属性
    if (key == "class") {
        // class属性已设置，无需额外处理
    } else if (key == "id") {
        // id属性已设置，无需额外处理
    }
}

std::string ElementNode::getAttribute(const std::string& key) const {
    return BaseNode::getAttribute(key);
}

std::string ElementNode::getStyleProperty(const std::string& property) const {
    // 从style属性中解析CSS属性
    std::string styleAttr = getAttribute("style");
    if (styleAttr.empty()) {
        return "";
    }
    
    // 简单的CSS属性解析
    std::regex propertyRegex(property + "\\s*:\\s*([^;]+)");
    std::smatch match;
    if (std::regex_search(styleAttr, match, propertyRegex)) {
        std::string value = match[1].str();
        // 去除前后空格
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        return value;
    }
    
    return "";
}

bool ElementNode::isVoidElementTag(const std::string& tagName) const {
    return voidElements.find(tagName) != voidElements.end();
}

void ElementNode::setClass(const std::string& className) {
    setAttribute("class", className);
}

void ElementNode::addClass(const std::string& className) {
    std::string currentClasses = getAttribute("class");
    if (currentClasses.empty()) {
        setClass(className);
    } else {
        // 检查是否已存在该类名
        std::vector<std::string> classes = getClasses();
        if (std::find(classes.begin(), classes.end(), className) == classes.end()) {
            setClass(currentClasses + " " + className);
        }
    }
}

void ElementNode::removeClass(const std::string& className) {
    std::vector<std::string> classes = getClasses();
    classes.erase(std::remove(classes.begin(), classes.end(), className), classes.end());
    
    std::ostringstream oss;
    for (size_t i = 0; i < classes.size(); ++i) {
        if (i > 0) oss << " ";
        oss << classes[i];
    }
    setClass(oss.str());
}

bool ElementNode::hasClass(const std::string& className) const {
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
            classes.push_back(className);
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

void ElementNode::setStyle(const std::string& style) {
    setAttribute("style", style);
}

std::string ElementNode::getStyle() const {
    return getAttribute("style");
}

void ElementNode::setTextContent(const std::string& text) {
    // 清除现有文本子节点
    children.erase(
        std::remove_if(children.begin(), children.end(),
            [](const std::shared_ptr<BaseNode>& child) {
                return child->getNodeType() == NodeType::TEXT;
            }),
        children.end()
    );
    
    // 添加新的文本节点
    if (!text.empty()) {
        auto textNode = std::make_shared<BaseNode>(NodeType::TEXT, "text", text);
        addChild(textNode);
    }
}

std::string ElementNode::getTextContent() const {
    std::ostringstream oss;
    for (const auto& child : children) {
        if (child->getNodeType() == NodeType::TEXT) {
            oss << child->getValue();
        } else if (child->getNodeType() == NodeType::ELEMENT) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            oss << element->getTextContent();
        }
    }
    return oss.str();
}

void ElementNode::addElement(std::shared_ptr<ElementNode> element) {
    addChild(element);
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::getElements() const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    for (const auto& child : children) {
        if (child->getNodeType() == NodeType::ELEMENT) {
            elements.push_back(std::static_pointer_cast<ElementNode>(child));
        }
    }
    return elements;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::getElementsByTagName(const std::string& tagName) const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    for (const auto& child : children) {
        if (child->getNodeType() == NodeType::ELEMENT) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->getTagName() == tagName) {
                elements.push_back(element);
            }
            // 递归查找
            auto childElements = element->getElementsByTagName(tagName);
            elements.insert(elements.end(), childElements.begin(), childElements.end());
        }
    }
    return elements;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::getElementsByClassName(const std::string& className) const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    for (const auto& child : children) {
        if (child->getNodeType() == NodeType::ELEMENT) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->hasClass(className)) {
                elements.push_back(element);
            }
            // 递归查找
            auto childElements = element->getElementsByClassName(className);
            elements.insert(elements.end(), childElements.begin(), childElements.end());
        }
    }
    return elements;
}

std::shared_ptr<ElementNode> ElementNode::getElementById(const std::string& id) const {
    for (const auto& child : children) {
        if (child->getNodeType() == NodeType::ELEMENT) {
            auto element = std::static_pointer_cast<ElementNode>(child);
            if (element->getId() == id) {
                return element;
            }
            // 递归查找
            auto found = element->getElementById(id);
            if (found) {
                return found;
            }
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<ElementNode>> ElementNode::querySelectorAll(const std::string& selector) const {
    // 简化的选择器实现，仅支持基本选择器
    std::vector<std::shared_ptr<ElementNode>> elements;
    
    if (selector.empty()) {
        return elements;
    }
    
    // 标签选择器
    if (selector[0] != '.' && selector[0] != '#') {
        return getElementsByTagName(selector);
    }
    
    // 类选择器
    if (selector[0] == '.') {
        std::string className = selector.substr(1);
        return getElementsByClassName(className);
    }
    
    // ID选择器
    if (selector[0] == '#') {
        std::string id = selector.substr(1);
        auto element = getElementById(id);
        if (element) {
            elements.push_back(element);
        }
        return elements;
    }
    
    return elements;
}

std::shared_ptr<ElementNode> ElementNode::querySelector(const std::string& selector) const {
    auto elements = querySelectorAll(selector);
    return elements.empty() ? nullptr : elements[0];
}

bool ElementNode::isValid() const {
    // 检查标签名是否为空
    if (name.empty()) {
        return false;
    }
    
    // 检查void元素是否有子节点
    if (isVoidElement && !children.empty()) {
        return false;
    }
    
    // 检查自闭合标签
    if (isSelfClosing && !isVoidElement) {
        return false;
    }
    
    return true;
}

std::string ElementNode::toHTML() const {
    return toHTML(0);
}

std::string ElementNode::toHTML(int indentLevel) const {
    std::ostringstream oss;
    
    // 开始标签
    oss << indent(indentLevel) << "<" << name;
    
    // 属性
    std::string attrs = attributesToHTML();
    if (!attrs.empty()) {
        oss << " " << attrs;
    }
    
    // 自闭合或void元素
    if (isSelfClosing || isVoidElement) {
        oss << " />";
        return oss.str();
    }
    
    oss << ">";
    
    // 子节点
    if (!children.empty()) {
        bool hasBlockChildren = false;
        for (const auto& child : children) {
            if (child->getNodeType() == NodeType::ELEMENT) {
                auto element = std::static_pointer_cast<ElementNode>(child);
                if (isBlockElement(element->getTagName())) {
                    hasBlockChildren = true;
                    break;
                }
            }
        }
        
        if (hasBlockChildren) {
            oss << "\n";
            for (const auto& child : children) {
                if (child->getNodeType() == NodeType::ELEMENT) {
                    auto element = std::static_pointer_cast<ElementNode>(child);
                    oss << element->toHTML(indentLevel + 1) << "\n";
                } else if (child->getNodeType() == NodeType::TEXT) {
                    oss << child->getValue();
                }
            }
            oss << indent(indentLevel);
        } else {
            for (const auto& child : children) {
                if (child->getNodeType() == NodeType::ELEMENT) {
                    auto element = std::static_pointer_cast<ElementNode>(child);
                    oss << element->toHTML(indentLevel);
                } else if (child->getNodeType() == NodeType::TEXT) {
                    oss << child->getValue();
                }
            }
        }
    }
    
    // 结束标签
    oss << "</" << name << ">";
    
    return oss.str();
}

std::string ElementNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "TagName: " << name << "\n";
    oss << "SelfClosing: " << (isSelfClosing ? "true" : "false") << "\n";
    oss << "VoidElement: " << (isVoidElement ? "true" : "false") << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(name, isSelfClosing);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    cloned->isVoidElement = isVoidElement;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool ElementNode::operator==(const ElementNode& other) const {
    return BaseNode::operator==(other) &&
           isSelfClosing == other.isSelfClosing &&
           isVoidElement == other.isVoidElement;
}

bool ElementNode::operator!=(const ElementNode& other) const {
    return !(*this == other);
}

std::string ElementNode::attributesToHTML() const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& attr : attributes) {
        if (!first) oss << " ";
        oss << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << attr.second << "\"";
        }
        first = false;
    }
    
    return oss.str();
}

std::string ElementNode::indent(int level) const {
    return std::string(level * 2, ' ');
}

bool ElementNode::isInlineElement(const std::string& tagName) const {
    static const std::set<std::string> inlineElements = {
        "a", "abbr", "acronym", "b", "bdo", "big", "br", "button", "cite",
        "code", "dfn", "em", "i", "img", "input", "kbd", "label", "map",
        "object", "q", "samp", "select", "small", "span", "strong", "sub",
        "sup", "textarea", "tt", "var"
    };
    return inlineElements.find(tagName) != inlineElements.end();
}

bool ElementNode::isBlockElement(const std::string& tagName) const {
    static const std::set<std::string> blockElements = {
        "address", "article", "aside", "blockquote", "canvas", "dd", "div",
        "dl", "dt", "fieldset", "figcaption", "figure", "footer", "form",
        "h1", "h2", "h3", "h4", "h5", "h6", "header", "hr", "li", "main",
        "nav", "noscript", "ol", "output", "p", "pre", "section", "table",
        "tfoot", "ul", "video"
    };
    return blockElements.find(tagName) != blockElements.end();
}

bool ElementNode::isInlineBlockElement(const std::string& tagName) const {
    static const std::set<std::string> inlineBlockElements = {
        "img", "input", "button", "select", "textarea"
    };
    return inlineBlockElements.find(tagName) != inlineBlockElements.end();
}

} // namespace CHTL