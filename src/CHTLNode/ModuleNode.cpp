#include "CHTL/CHTLNode/ModuleNode.h"
#include <sstream>

namespace CHTL {

ModuleNode::ModuleNode(ModuleType type, const std::string& name)
    : BaseNode(NodeType::MODULE)
    , m_moduleType(type)
    , m_moduleName(name)
    , m_enabled(true)
{
}

ModuleNode::~ModuleNode() {
}

void ModuleNode::setModuleType(ModuleType type) {
    m_moduleType = type;
}

ModuleType ModuleNode::getModuleType() const {
    return m_moduleType;
}

void ModuleNode::setModuleName(const std::string& name) {
    m_moduleName = name;
}

const std::string& ModuleNode::getModuleName() const {
    return m_moduleName;
}

void ModuleNode::setModuleVersion(const std::string& version) {
    m_moduleVersion = version;
}

const std::string& ModuleNode::getModuleVersion() const {
    return m_moduleVersion;
}

void ModuleNode::setModulePath(const std::string& path) {
    m_modulePath = path;
}

const std::string& ModuleNode::getModulePath() const {
    return m_modulePath;
}

void ModuleNode::setModuleDescription(const std::string& description) {
    m_moduleDescription = description;
}

const std::string& ModuleNode::getModuleDescription() const {
    return m_moduleDescription;
}

void ModuleNode::addModuleDependency(const std::string& dependency, const std::string& version) {
    m_moduleDependencies[dependency] = version;
}

std::string ModuleNode::getModuleDependency(const std::string& dependency) const {
    auto it = m_moduleDependencies.find(dependency);
    if (it != m_moduleDependencies.end()) {
        return it->second;
    }
    return "";
}

bool ModuleNode::hasModuleDependency(const std::string& dependency) const {
    return m_moduleDependencies.find(dependency) != m_moduleDependencies.end();
}

const std::unordered_map<std::string, std::string>& ModuleNode::getModuleDependencies() const {
    return m_moduleDependencies;
}

void ModuleNode::setModuleContent(const std::string& content) {
    m_moduleContent = content;
}

const std::string& ModuleNode::getModuleContent() const {
    return m_moduleContent;
}

void ModuleNode::setModuleExport(const std::string& exportName, const std::string& exportValue) {
    m_moduleExports[exportName] = exportValue;
}

std::string ModuleNode::getModuleExport(const std::string& exportName) const {
    auto it = m_moduleExports.find(exportName);
    if (it != m_moduleExports.end()) {
        return it->second;
    }
    return "";
}

bool ModuleNode::hasModuleExport(const std::string& exportName) const {
    return m_moduleExports.find(exportName) != m_moduleExports.end();
}

const std::unordered_map<std::string, std::string>& ModuleNode::getModuleExports() const {
    return m_moduleExports;
}

void ModuleNode::setEnabled(bool enabled) {
    m_enabled = enabled;
}

bool ModuleNode::isEnabled() const {
    return m_enabled;
}

std::shared_ptr<BaseNode> ModuleNode::clone() const {
    auto cloned = std::make_shared<ModuleNode>(m_moduleType, m_moduleName);
    cloned->m_moduleVersion = m_moduleVersion;
    cloned->m_modulePath = m_modulePath;
    cloned->m_moduleDescription = m_moduleDescription;
    cloned->m_moduleDependencies = m_moduleDependencies;
    cloned->m_moduleContent = m_moduleContent;
    cloned->m_moduleExports = m_moduleExports;
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

std::string ModuleNode::toString() const {
    std::ostringstream oss;
    oss << "ModuleNode(type=" << getModuleTypeName(m_moduleType)
        << ", name=\"" << m_moduleName << "\""
        << ", version=\"" << m_moduleVersion << "\""
        << ", path=\"" << m_modulePath << "\""
        << ", dependencies=" << m_moduleDependencies.size()
        << ", exports=" << m_moduleExports.size()
        << ", enabled=" << (m_enabled ? "true" : "false")
        << ", children=" << m_children.size()
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

std::string ModuleNode::getModuleTypeName(ModuleType type) {
    switch (type) {
        case ModuleType::CMOD:
            return "CMOD";
        case ModuleType::CJMOD:
            return "CJMOD";
        case ModuleType::HTML:
            return "HTML";
        case ModuleType::CSS:
            return "CSS";
        case ModuleType::JS:
            return "JS";
        case ModuleType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

} // namespace CHTL