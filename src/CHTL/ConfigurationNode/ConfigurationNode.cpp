#include "CHTL/ConfigurationNode.h"

namespace CHTL {

// 设置管理
void ConfigurationNode::setSetting(const String& name, const String& value) {
    settings_[name] = value;
}

String ConfigurationNode::getSetting(const String& name) const {
    auto it = settings_.find(name);
    return it != settings_.end() ? it->second : "";
}

bool ConfigurationNode::hasSetting(const String& name) const {
    return settings_.find(name) != settings_.end();
}

void ConfigurationNode::removeSetting(const String& name) {
    settings_.erase(name);
}

void ConfigurationNode::clearSettings() {
    settings_.clear();
}

// 实现基类方法
String ConfigurationNode::toString() const {
    return "[" + configName_ + "]";
}

String ConfigurationNode::toHTML() const {
    return "";
}

bool ConfigurationNode::isValid() const {
    return !configName_.empty();
}

void ConfigurationNode::accept(NodeVisitor& visitor) {
    visitor.visit(*this);
}

// 工厂方法
ConfigurationNodePtr ConfigurationNode::create(const String& name, const SourceLocation& loc) {
    return std::make_shared<ConfigurationNode>(name, loc);
}

} // namespace CHTL