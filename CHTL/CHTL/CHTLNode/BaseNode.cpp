#include "BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

// 静态成员定义
std::vector<std::unique_ptr<BaseNode>> BaseNode::nodePool;
size_t BaseNode::poolSize = 0;

BaseNode::BaseNode(NodeType type, const std::string& name) 
    : nodeType(type), name(name), line(0), column(0), position(0) {
}

void BaseNode::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string BaseNode::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    if (it != attributes.end()) {
        return it->second;
    }
    return "";
}

bool BaseNode::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void BaseNode::removeAttribute(const std::string& key) {
    attributes.erase(key);
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        children.push_back(child);
        child->setParent(shared_from_this());
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
    }
}

void BaseNode::insertChild(size_t index, std::shared_ptr<BaseNode> child) {
    if (child && index <= children.size()) {
        children.insert(children.begin() + index, child);
        child->setParent(shared_from_this());
    }
}

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

std::shared_ptr<BaseNode> BaseNode::getParent() const {
    return parent.lock();
}

void BaseNode::setParent(std::shared_ptr<BaseNode> parent) {
    this->parent = parent;
}

void BaseNode::setPosition(size_t line, size_t column, size_t position) {
    this->line = line;
    this->column = column;
    this->position = position;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(const std::string& name) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children) {
        if (child->getName() == name) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findFirstChild(const std::string& name) const {
    for (const auto& child : children) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildrenByType(NodeType type) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children) {
        if (child->getNodeType() == type) {
            result.push_back(child);
        }
    }
    return result;
}

void BaseNode::traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor) {
    visitor(shared_from_this());
    for (const auto& child : children) {
        child->traverse(visitor);
    }
}

void BaseNode::traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor) const {
    visitor(std::const_pointer_cast<BaseNode>(shared_from_this()));
    for (const auto& child : children) {
        child->traverse(visitor);
    }
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = createNode(nodeType, name);
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    cloned->attributes = attributes;
    
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "Node[" << static_cast<int>(nodeType) << "](" << name << ")";
    return oss.str();
}

std::string BaseNode::toHTML() const {
    return "";
}

std::string BaseNode::toCSS() const {
    return "";
}

std::string BaseNode::toJS() const {
    return "";
}

bool BaseNode::isValid() const {
    return !name.empty();
}

void BaseNode::accept(NodeVisitor& visitor) {
    visitor.visit(shared_from_this());
}

void BaseNode::accept(const NodeVisitor& visitor) const {
    visitor.visit(std::const_pointer_cast<BaseNode>(shared_from_this()));
}

std::string BaseNode::debugString() const {
    std::ostringstream oss;
    oss << "BaseNode {\n";
    oss << "  type: " << static_cast<int>(nodeType) << "\n";
    oss << "  name: " << name << "\n";
    oss << "  line: " << line << ", column: " << column << "\n";
    oss << "  attributes: " << attributes.size() << "\n";
    oss << "  children: " << children.size() << "\n";
    oss << "}";
    return oss.str();
}

// 内存管理实现
std::shared_ptr<BaseNode> BaseNode::createNode(NodeType type, const std::string& name) {
    // 尝试从对象池获取
    if (!nodePool.empty()) {
        auto node = std::move(nodePool.back());
        nodePool.pop_back();
        poolSize--;
        
        // 重置节点状态
        node->nodeType = type;
        node->name = name;
        node->line = 0;
        node->column = 0;
        node->position = 0;
        node->attributes.clear();
        node->children.clear();
        node->parent.reset();
        
        return std::shared_ptr<BaseNode>(node.release(), [](BaseNode* n) {
            returnNode(std::shared_ptr<BaseNode>(n));
        });
    }
    
    // 创建新节点
    return std::shared_ptr<BaseNode>(new BaseNode(type, name), [](BaseNode* n) {
        returnNode(std::shared_ptr<BaseNode>(n));
    });
}

void BaseNode::returnNode(std::shared_ptr<BaseNode> node) {
    if (!node) return;
    
    // 清理节点状态
    node->attributes.clear();
    node->children.clear();
    node->parent.reset();
    
    // 如果对象池未满，回收节点
    if (poolSize < MAX_POOL_SIZE) {
        nodePool.push_back(std::unique_ptr<BaseNode>(node.get()));
        poolSize++;
    }
    // 否则让节点自然销毁
}

void BaseNode::clearPool() {
    nodePool.clear();
    poolSize = 0;
}

BaseNode::MemoryStats BaseNode::getMemoryStats() {
    MemoryStats stats;
    stats.poolNodes = poolSize;
    stats.totalNodes = poolSize; // 简化实现
    stats.activeNodes = stats.totalNodes - stats.poolNodes;
    stats.memoryUsage = sizeof(BaseNode) * stats.totalNodes;
    return stats;
}

} // namespace CHTL