#include "CHTL/BaseNode.h"
#include "CHTL/ElementNode.h"
#include "CHTL/TextNode.h"
#include "CHTL/StyleNode.h"
#include "CHTL/ScriptNode.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
#include "CHTL/NamespaceNode.h"
#include "CHTL/ConfigurationNode.h"
#include "CHTL/OriginNode.h"
#include "CHTL/OperatorNode.h"
#include <algorithm>

namespace CHTL {

// 子节点管理
void BaseNode::addChild(const NodePtr& child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void BaseNode::removeChild(const NodePtr& child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void BaseNode::insertChild(size_t index, const NodePtr& child) {
    if (child && index <= children_.size()) {
        child->setParent(shared_from_this());
        children_.insert(children_.begin() + index, child);
    }
}

void BaseNode::clearChildren() {
    for (auto& child : children_) {
        child->setParent(nullptr);
    }
    children_.clear();
}

// 查找子节点
NodePtr BaseNode::findChild(NodeType type) const {
    for (const auto& child : children_) {
        if (child->getNodeType() == type) {
            return child;
        }
    }
    return nullptr;
}

std::vector<NodePtr> BaseNode::findChildren(NodeType type) const {
    std::vector<NodePtr> result;
    for (const auto& child : children_) {
        if (child->getNodeType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

NodePtr BaseNode::findChildByType(NodeType type, size_t index) const {
    size_t count = 0;
    for (const auto& child : children_) {
        if (child->getNodeType() == type) {
            if (count == index) {
                return child;
            }
            count++;
        }
    }
    return nullptr;
}

// 默认访问者实现
void DefaultNodeVisitor::visit(BaseNode& node) {
    // 默认实现：遍历所有子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(ElementNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(TextNode& node) {
    // 文本节点没有子节点
}

void DefaultNodeVisitor::visit(StyleNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(ScriptNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(TemplateNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(CustomNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(ImportNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(NamespaceNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(ConfigurationNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(OriginNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

void DefaultNodeVisitor::visit(OperatorNode& node) {
    // 遍历子节点
    for (auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

} // namespace CHTL