#include "ConfigNode.hpp"
#include <sstream>

namespace CHTL {

ConfigNode::ConfigNode(size_t line, size_t column)
    : BaseNode(NodeType::CONFIG, line, column) {}

void ConfigNode::addConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
}

std::string ConfigNode::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    if (it != configurations_.end()) {
        return it->second;
    }
    return "";
}

const std::map<std::string, std::string>& ConfigNode::getConfigurations() const {
    return configurations_;
}

void ConfigNode::addNameBlock(const std::string& oldName, const std::string& newName) {
    nameBlocks_[oldName] = newName;
}

const std::map<std::string, std::string>& ConfigNode::getNameBlocks() const {
    return nameBlocks_;
}

std::string ConfigNode::toHTML() const {
    // 配置节点不直接生成 HTML
    return "";
}

std::string ConfigNode::toString() const {
    std::ostringstream oss;
    oss << "ConfigNode(line=" << getLine() << ", column=" << getColumn()
        << ", configs=" << configurations_.size() 
        << ", nameBlocks=" << nameBlocks_.size() << ")";
    return oss.str();
}

} // namespace CHTL