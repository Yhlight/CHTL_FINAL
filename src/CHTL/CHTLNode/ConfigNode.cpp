#include "CHTL/CHTLNode/ConfigNode.h"
#include <sstream>

namespace CHTL {

ConfigNode::ConfigNode(const std::string& name) 
    : BaseNode(NodeType::CONFIG), m_configName(name), m_styleAutoAddClass(false), 
      m_styleAutoAddID(false), m_scriptAutoAddClass(true), m_scriptAutoAddID(true),
      m_disableDefaultNamespace(false), m_disableCustomOriginType(false) {
}

ConfigNode::~ConfigNode() = default;

void ConfigNode::setConfigName(const std::string& name) {
    m_configName = name;
}

std::string ConfigNode::getConfigName() const {
    return m_configName;
}

bool ConfigNode::hasConfigName() const {
    return !m_configName.empty();
}

void ConfigNode::setConfigValue(const std::string& key, const std::string& value) {
    m_configValues[key] = value;
}

std::string ConfigNode::getConfigValue(const std::string& key) const {
    auto it = m_configValues.find(key);
    return (it != m_configValues.end()) ? it->second : "";
}

bool ConfigNode::hasConfigValue(const std::string& key) const {
    return m_configValues.find(key) != m_configValues.end();
}

const std::unordered_map<std::string, std::string>& ConfigNode::getConfigValues() const {
    return m_configValues;
}

void ConfigNode::setNameConfig(const std::string& key, const std::string& value) {
    m_nameConfigs[key] = value;
}

std::string ConfigNode::getNameConfig(const std::string& key) const {
    auto it = m_nameConfigs.find(key);
    return (it != m_nameConfigs.end()) ? it->second : "";
}

bool ConfigNode::hasNameConfig(const std::string& key) const {
    return m_nameConfigs.find(key) != m_nameConfigs.end();
}

const std::unordered_map<std::string, std::string>& ConfigNode::getNameConfigs() const {
    return m_nameConfigs;
}

void ConfigNode::setGroupOption(const std::string& key, const std::vector<std::string>& values) {
    m_groupOptions[key] = values;
}

std::vector<std::string> ConfigNode::getGroupOption(const std::string& key) const {
    auto it = m_groupOptions.find(key);
    return (it != m_groupOptions.end()) ? it->second : std::vector<std::string>();
}

bool ConfigNode::hasGroupOption(const std::string& key) const {
    return m_groupOptions.find(key) != m_groupOptions.end();
}

const std::unordered_map<std::string, std::vector<std::string>>& ConfigNode::getGroupOptions() const {
    return m_groupOptions;
}

void ConfigNode::setOriginType(const std::string& typeName, const std::string& prefix) {
    m_originTypes[typeName] = prefix;
}

std::string ConfigNode::getOriginType(const std::string& typeName) const {
    auto it = m_originTypes.find(typeName);
    return (it != m_originTypes.end()) ? it->second : "";
}

bool ConfigNode::hasOriginType(const std::string& typeName) const {
    return m_originTypes.find(typeName) != m_originTypes.end();
}

const std::unordered_map<std::string, std::string>& ConfigNode::getOriginTypes() const {
    return m_originTypes;
}

void ConfigNode::setStyleAutoAddClass(bool enabled) {
    m_styleAutoAddClass = enabled;
}

bool ConfigNode::getStyleAutoAddClass() const {
    return m_styleAutoAddClass;
}

void ConfigNode::setStyleAutoAddID(bool enabled) {
    m_styleAutoAddID = enabled;
}

bool ConfigNode::getStyleAutoAddID() const {
    return m_styleAutoAddID;
}

void ConfigNode::setScriptAutoAddClass(bool enabled) {
    m_scriptAutoAddClass = enabled;
}

bool ConfigNode::getScriptAutoAddClass() const {
    return m_scriptAutoAddClass;
}

void ConfigNode::setScriptAutoAddID(bool enabled) {
    m_scriptAutoAddID = enabled;
}

bool ConfigNode::getScriptAutoAddID() const {
    return m_scriptAutoAddID;
}

void ConfigNode::setDisableDefaultNamespace(bool disabled) {
    m_disableDefaultNamespace = disabled;
}

bool ConfigNode::isDefaultNamespaceDisabled() const {
    return m_disableDefaultNamespace;
}

void ConfigNode::setDisableCustomOriginType(bool disabled) {
    m_disableCustomOriginType = disabled;
}

bool ConfigNode::isCustomOriginTypeDisabled() const {
    return m_disableCustomOriginType;
}

void ConfigNode::accept(NodeVisitor& visitor) {
    visitor.visitConfig(*this);
}

std::shared_ptr<BaseNode> ConfigNode::clone() const {
    auto cloned = std::make_shared<ConfigNode>(m_configName);
    cloned->m_configValues = m_configValues;
    cloned->m_nameConfigs = m_nameConfigs;
    cloned->m_groupOptions = m_groupOptions;
    cloned->m_originTypes = m_originTypes;
    cloned->m_styleAutoAddClass = m_styleAutoAddClass;
    cloned->m_styleAutoAddID = m_styleAutoAddID;
    cloned->m_scriptAutoAddClass = m_scriptAutoAddClass;
    cloned->m_scriptAutoAddID = m_scriptAutoAddID;
    cloned->m_disableDefaultNamespace = m_disableDefaultNamespace;
    cloned->m_disableCustomOriginType = m_disableCustomOriginType;
    cloned->m_content = m_content;
    cloned->m_name = m_name;
    cloned->m_line = m_line;
    cloned->m_column = m_column;
    cloned->m_attributes = m_attributes;
    
    // 克隆子节点
    for (const auto& child : m_children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

bool ConfigNode::validate() const {
    return BaseNode::validate();
}

std::string ConfigNode::toString() const {
    std::stringstream ss;
    ss << "ConfigNode{name='" << m_configName << "', configValues=" << m_configValues.size() 
       << ", nameConfigs=" << m_nameConfigs.size() << ", groupOptions=" << m_groupOptions.size() 
       << ", originTypes=" << m_originTypes.size() << ", styleAutoAddClass=" << m_styleAutoAddClass 
       << ", styleAutoAddID=" << m_styleAutoAddID << ", scriptAutoAddClass=" << m_scriptAutoAddClass 
       << ", scriptAutoAddID=" << m_scriptAutoAddID << ", disableDefaultNamespace=" << m_disableDefaultNamespace 
       << ", disableCustomOriginType=" << m_disableCustomOriginType << "}";
    return ss.str();
}

} // namespace CHTL