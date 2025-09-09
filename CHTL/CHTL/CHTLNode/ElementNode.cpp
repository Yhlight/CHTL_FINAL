#include "ElementNode.h"
#include <sstream>

namespace CHTL {

ElementNode::ElementNode(const std::string& tagName)
    : BaseNode(NodeType::ELEMENT, tagName), self_closing_(false) {
}

std::string ElementNode::toHTML() const {
    std::ostringstream oss;
    
    // 开始标签
    oss << "<" << getName();
    
    // 属性
    for (const auto& attr : getAttributes()) {
        oss << " " << attr.first;
        if (!attr.second.empty()) {
            oss << "=\"" << attr.second << "\"";
        }
    }
    
    if (isSelfClosing()) {
        oss << " />";
    } else {
        oss << ">";
        
        // 子节点
        for (const auto& child : getChildren()) {
            oss << child->toHTML();
        }
        
        // 结束标签
        oss << "</" << getName() << ">";
    }
    
    return oss.str();
}

std::string ElementNode::toCSS() const {
    // 元素节点不直接生成CSS，但可能包含样式子节点
    std::ostringstream oss;
    for (const auto& child : getChildren()) {
        std::string css = child->toCSS();
        if (!css.empty()) {
            oss << css << "\n";
        }
    }
    return oss.str();
}

std::string ElementNode::toJS() const {
    // 元素节点不直接生成JS，但可能包含脚本子节点
    std::ostringstream oss;
    for (const auto& child : getChildren()) {
        std::string js = child->toJS();
        if (!js.empty()) {
            oss << js << "\n";
        }
    }
    return oss.str();
}

std::shared_ptr<BaseNode> ElementNode::clone() const {
    auto cloned = std::make_shared<ElementNode>(getName());
    cloned->self_closing_ = self_closing_;
    
    // 复制属性
    for (const auto& attr : getAttributes()) {
        cloned->setAttribute(attr.first, attr.second);
    }
    
    // 复制子节点
    for (const auto& child : getChildren()) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool ElementNode::isSelfClosing() const {
    return self_closing_;
}

void ElementNode::setSelfClosing(bool selfClosing) {
    self_closing_ = selfClosing;
}

} // namespace CHTL