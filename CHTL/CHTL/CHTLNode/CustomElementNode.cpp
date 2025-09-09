#include "CustomElementNode.hpp"
#include <CHTL/CHTLNode/ElementNode.hpp>
#include <CHTL/CHTLManage/TemplateManager.hpp>
#include <sstream>
#include <algorithm>

namespace CHTL {

CustomElementNode::CustomElementNode(const std::string& name, size_t line, size_t column)
    : BaseNode(NodeType::CUSTOM, line, column), customName_(name) {}

void CustomElementNode::addDeletedElement(const std::string& elementSelector) {
    deletedElements_.push_back(elementSelector);
}

bool CustomElementNode::isDeletedElement(const std::string& elementSelector) const {
    return std::find(deletedElements_.begin(), deletedElements_.end(), elementSelector) 
           != deletedElements_.end();
}

void CustomElementNode::addDeletedInheritance(const std::string& templateName) {
    deletedInheritances_.push_back(templateName);
}

bool CustomElementNode::isDeletedInheritance(const std::string& templateName) const {
    return std::find(deletedInheritances_.begin(), deletedInheritances_.end(), templateName) 
           != deletedInheritances_.end();
}

void CustomElementNode::addInsertedElement(const std::string& position, const std::string& selector, 
                                          std::shared_ptr<BaseNode> element) {
    insertedElements_.push_back({position, element});
}

void CustomElementNode::addInheritedTemplate(const std::string& templateName) {
    inheritedTemplates_.push_back(templateName);
}

bool CustomElementNode::inheritsFrom(const std::string& templateName) const {
    return std::find(inheritedTemplates_.begin(), inheritedTemplates_.end(), templateName) 
           != inheritedTemplates_.end();
}

void CustomElementNode::applySpecialization() {
    // 应用特例化操作
    // 1. 删除指定的元素
    for (const auto& elementSelector : deletedElements_) {
        // 删除匹配的元素
        auto it = children_.begin();
        while (it != children_.end()) {
            if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(*it)) {
                if (elementNode->getTagName() == elementSelector) {
                    it = children_.erase(it);
                } else {
                    ++it;
                }
            } else {
                ++it;
            }
        }
    }
    
    // 2. 删除指定的继承
    for (const auto& inheritance : deletedInheritances_) {
        inheritedTemplates_.erase(
            std::remove(inheritedTemplates_.begin(), inheritedTemplates_.end(), inheritance),
            inheritedTemplates_.end()
        );
    }
    
    // 3. 插入新元素
    for (const auto& [position, element] : insertedElements_) {
        // 根据位置插入元素
        if (position == "at top") {
            // 插入到顶部
        } else if (position == "at bottom") {
            // 插入到底部
        } else if (position == "after") {
            // 插入到指定元素后
        } else if (position == "before") {
            // 插入到指定元素前
        } else if (position == "replace") {
            // 替换指定元素
        }
    }
    
    // 4. 合并继承的模板内容
    for (const auto& inheritedTemplateName : inheritedTemplates_) {
        auto& templateManager = TemplateManager::getInstance();
        auto inheritedTemplate = templateManager.getElementTemplate(inheritedTemplateName);
        
        if (inheritedTemplate) {
            // 合并继承的子节点
            for (const auto& child : inheritedTemplate->getChildren()) {
                addChild(child);
            }
        }
    }
}

std::string CustomElementNode::toHTML() const {
    std::ostringstream oss;
    
    // 生成元素内容
    for (const auto& child : getChildren()) {
        if (child) {
            oss << child->toHTML();
        }
    }
    
    return oss.str();
}

std::string CustomElementNode::toString() const {
    std::ostringstream oss;
    oss << "CustomElementNode(name=" << customName_
        << ", children=" << getChildCount()
        << ", deleted=" << deletedElements_.size()
        << ", inserted=" << insertedElements_.size()
        << ", line=" << getLine() << ", column=" << getColumn() << ")";
    return oss.str();
}

std::string CustomElementNode::generateElementContent() const {
    return toHTML();
}

} // namespace CHTL