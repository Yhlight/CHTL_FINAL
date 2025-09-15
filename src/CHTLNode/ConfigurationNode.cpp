#include "CHTL/CHTLNode/ConfigurationNode.h"
#include <sstream>

namespace CHTL {

ConfigurationNode::ConfigurationNode(ConfigurationType type, const std::string& name)
    : BaseNode(NodeType::CONFIG)
    , m_configurationType(type)
    , m_configurationName(name)
    , m_enabled(true)
{
}

ConfigurationNode::~ConfigurationNode() {
}

void ConfigurationNode::setConfigurationType(ConfigurationType type) {
    m_configurationType = type;
}

ConfigurationType ConfigurationNode::getConfigurationType() const {
    return m_configurationType;
}

void ConfigurationNode::setConfigurationName(const std::string& name) {
    m_configurationName = name;
}

const std::string& ConfigurationNode::getConfigurationName() const {
    return m_configurationName;
}

void ConfigurationNode::addConfigurationItem(const std::string& key, const std::string& value) {
    m_configurationItems[key] = value;
}

std::string ConfigurationNode::getConfigurationItem(const std::string& key) const {
    auto it = m_configurationItems.find(key);
    if (it != m_configurationItems.end()) {
        return it->second;
    }
    return "";
}

bool ConfigurationNode::hasConfigurationItem(const std::string& key) const {
    return m_configurationItems.find(key) != m_configurationItems.end();
}

const std::unordered_map<std::string, std::string>& ConfigurationNode::getConfigurationItems() const {
    return m_configurationItems;
}

void ConfigurationNode::setConfigurationGroup(const std::string& group) {
    m_configurationGroup = group;
}

const std::string& ConfigurationNode::getConfigurationGroup() const {
    return m_configurationGroup;
}

bool ConfigurationNode::hasConfigurationGroup() const {
    return !m_configurationGroup.empty();
}

void ConfigurationNode::setConfigurationContent(const std::string& content) {
    m_configurationContent = content;
}

const std::string& ConfigurationNode::getConfigurationContent() const {
    return m_configurationContent;
}

void ConfigurationNode::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool ConfigurationNode::isEnabled() const {
    return m_enabled;
}

std::shared_ptr<BaseNode> ConfigurationNode::clone() const {
    auto cloned = std::make_shared<ConfigurationNode>(m_configurationType, m_configurationName);
    cloned->m_configurationItems = m_configurationItems;
    cloned->m_configurationGroup = m_configurationGroup;
    cloned->m_configurationContent = m_configurationContent;
    cloned->m_enabled = m_enabled;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        if (child) {
            cloned->addChild(child->clone());
        }
    }
    
    return cloned;
}

std::string ConfigurationNode::toString() const {
    std::ostringstream oss;
    oss << "ConfigurationNode(type=" << getConfigurationTypeName(m_configurationType)
        << ", name=\"" << m_configurationName << "\""
        << ", items=" << m_configurationItems.size()
        << ", group=\"" << m_configurationGroup << "\""
        << ", enabled=" << (m_enabled ? "true" : "false")
        << ", children=" << m_children.size()
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

std::string ConfigurationNode::getConfigurationTypeName(ConfigurationType type) {
    switch (type) {
        case ConfigurationType::KEYWORD:
            return "KEYWORD";
        case ConfigurationType::MODULE:
            return "MODULE";
        case ConfigurationType::COMPILER:
            return "COMPILER";
        case ConfigurationType::OUTPUT:
            return "OUTPUT";
        case ConfigurationType::DEBUG:
            return "DEBUG";
        case ConfigurationType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

} // namespace CHTL