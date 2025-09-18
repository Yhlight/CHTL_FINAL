#include "BaseNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

BaseNode::BaseNode(NodeType type, const std::string& name)
    : type_(type), name_(name), line_(0), column_(0) {
}

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        child->setParent(shared_from_this());
        children_.push_back(child);
    }
}

void BaseNode::removeChild(std::shared_ptr<BaseNode> child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->setParent(nullptr);
        children_.erase(it);
    }
}

void BaseNode::setAttribute(const std::string& name, const std::string& value) {
    attributes_[name] = value;
}

std::string BaseNode::getAttribute(const std::string& name) const {
    auto it = attributes_.find(name);
    return (it != attributes_.end()) ? it->second : "";
}

bool BaseNode::hasAttribute(const std::string& name) const {
    return attributes_.find(name) != attributes_.end();
}

void BaseNode::removeAttribute(const std::string& name) {
    attributes_.erase(name);
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << "Node(" << static_cast<int>(type_) << ", \"" << name_ << "\", " 
        << line_ << ":" << column_ << ")";
    return oss.str();
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findChildren(const std::string& name) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    for (const auto& child : children_) {
        if (child->getName() == name) {
            result.push_back(child);
        }
    }
    return result;
}

std::shared_ptr<BaseNode> BaseNode::findFirstChild(const std::string& name) const {
    for (const auto& child : children_) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::findDescendants(const std::string& name) const {
    std::vector<std::shared_ptr<BaseNode>> result;
    
    // 查找直接子节点
    auto children = findChildren(name);
    result.insert(result.end(), children.begin(), children.end());
    
    // 递归查找子节点的后代
    for (const auto& child : children_) {
        auto descendants = child->findDescendants(name);
        result.insert(result.end(), descendants.begin(), descendants.end());
    }
    
    return result;
}

std::string BaseNode::escapeHTML(const std::string& text) const {
    std::string result = text;
    
    // 替换HTML特殊字符
    size_t pos = 0;
    while ((pos = result.find("&", pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = result.find("<", pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find(">", pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = result.find("'", pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string BaseNode::escapeCSS(const std::string& text) const {
    std::string result = text;
    
    // 替换CSS特殊字符
    size_t pos = 0;
    while ((pos = result.find("\\", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    return result;
}

std::string BaseNode::escapeJS(const std::string& text) const {
    std::string result = text;
    
    // 替换JavaScript特殊字符
    size_t pos = 0;
    while ((pos = result.find("\\", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\"", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\\"");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\n", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\r", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\r");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = result.find("\t", pos)) != std::string::npos) {
        result.replace(pos, 1, "\\t");
        pos += 2;
    }
    
    return result;
}

} // namespace CHTL