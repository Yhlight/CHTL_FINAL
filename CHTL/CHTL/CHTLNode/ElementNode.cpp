#include "ElementNode.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace CHTL {

const std::vector<std::string> ElementNode::VOID_ELEMENTS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

ElementNode::ElementNode(const std::string& tagName) 
    : BaseNode(NodeType::ELEMENT, tagName), isSelfClosing(false), isVoidElement(false) {
    if (!tagName.empty()) {
        isVoidElement = isVoidElementType();
        isSelfClosing = isSelfClosingElement();
    }
}

void ElementNode::setTagName(const std::string& tagName) {
    name = tagName;
    isVoidElement = isVoidElementType();
    isSelfClosing = isSelfClosingElement();
}

void ElementNode::addClass(const std::string& className) {
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

void ElementNode::removeClass(const std::string& className) {
    std::string currentClasses = getAttribute("class");
    if (!currentClasses.empty()) {
        std::vector<std::string> classes = getClasses();
        classes.erase(std::remove(classes.begin(), classes.end(), className), classes.end());
        
        if (classes.empty()) {
            removeAttribute("class");
        } else {
            std::string newClasses;
            for (size_t i = 0; i < classes.size(); ++i) {
                if (i > 0) newClasses += " ";
                newClasses += classes[i];
            }
            setAttribute("class", newClasses);
        }
    }
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

bool ElementNode::hasId() const {
    return hasAttribute("id");
}

void ElementNode::setTextContent(const std::string& text) {
    // 清除所有子节点
    children.clear();
    
    // 创建文本节点
    auto textNode = std::make_shared<BaseNode>(NodeType::TEXT, "text");
    textNode->setAttribute("content", text);
    addChild(textNode);
}

std::string ElementNode::getTextContent() const {
    std::string content;
    for (const auto& child : children) {
        if (child->getNodeType() == NodeType::TEXT) {
            content += child->getAttribute("content");
        } else if (child->getNodeType() == NodeType::ELEMENT) {
            // 递归获取子元素的文本内容
            auto elementChild = std::static_pointer_cast<ElementNode>(child);
            content += elementChild->getTextContent();
        }
    }
    return content;
}

std::string ElementNode::toHTML() const {
    return toHTML(false, 0);
}

std::string ElementNode::toHTML(bool prettyPrint, int indentLevel) const {
    std::ostringstream oss;
    std::string indent = prettyPrint ? std::string(indentLevel * 2, ' ') : "";
    std::string newline = prettyPrint ? "\n" : "";
    
    // 开始标签
    oss << indent << "<" << name;
    
    // 属性
    for (const auto& attr : attributes) {
        oss << " " << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << attr.second << "\"";
        }
    }
    
    if (isVoidElement || isSelfClosing) {
        oss << " />" << newline;
    } else {
        oss << ">" << newline;
        
        // 子节点内容
        for (const auto& child : children) {
            if (child->getNodeType() == NodeType::ELEMENT) {
                auto elementChild = std::static_pointer_cast<ElementNode>(child);
                oss << elementChild->toHTML(prettyPrint, indentLevel + 1);
            } else if (child->getNodeType() == NodeType::TEXT) {
                std::string textContent = child->getAttribute("content");
                if (prettyPrint) {
                    oss << indent << "  " << textContent << newline;
                } else {
                    oss << textContent;
                }
            }
        }
        
        // 结束标签
        oss << indent << "</" << name << ">" << newline;
    }
    
    return oss.str();
}

bool ElementNode::isValid() const {
    if (name.empty()) {
        return false;
    }
    
    // 检查标签名是否有效
    for (char c : name) {
        if (!std::isalnum(c) && c != '-') {
            return false;
        }
    }
    
    return BaseNode::isValid();
}

bool ElementNode::isValidHTML5() const {
    // 基本的HTML5验证
    if (!isValid()) {
        return false;
    }
    
    // 检查是否为有效的HTML5标签
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);
    
    // 这里可以添加更严格的HTML5标签验证
    return true;
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(name);
    cloned->attributes = attributes;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    cloned->isSelfClosing = isSelfClosing;
    cloned->isVoidElement = isVoidElement;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string ElementNode::debugString() const {
    std::ostringstream oss;
    oss << "ElementNode: <" << name << "> (Line: " << line << ", Column: " << column << ")";
    if (isVoidElement) {
        oss << " [VOID]";
    }
    if (isSelfClosing) {
        oss << " [SELF-CLOSING]";
    }
    return oss.str();
}

bool ElementNode::isVoidElement(const std::string& tagName) {
    std::string lowerTag = tagName;
    std::transform(lowerTag.begin(), lowerTag.end(), lowerTag.begin(), ::tolower);
    return std::find(VOID_ELEMENTS.begin(), VOID_ELEMENTS.end(), lowerTag) != VOID_ELEMENTS.end();
}

bool ElementNode::isSelfClosingElement(const std::string& tagName) {
    // 在CHTL中，某些元素可能被标记为自闭合
    return isVoidElement(tagName);
}

std::vector<std::string> ElementNode::getVoidElements() {
    return VOID_ELEMENTS;
}

} // namespace CHTL