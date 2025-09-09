#include "CHTL/CHTLNode/ImportNode.h"
#include <sstream>

namespace CHTL {

ImportNode::ImportNode(ImportType type, const std::string& path) 
    : BaseNode(NodeType::IMPORT), m_importType(type), m_importPath(path) {
}

ImportNode::~ImportNode() = default;

ImportType ImportNode::getImportType() const {
    return m_importType;
}

void ImportNode::setImportPath(const std::string& path) {
    m_importPath = path;
}

std::string ImportNode::getImportPath() const {
    return m_importPath;
}

void ImportNode::setImportMode(ImportMode mode) {
    m_importMode = mode;
}

ImportMode ImportNode::getImportMode() const {
    return m_importMode;
}

void ImportNode::setImportName(const std::string& name) {
    m_importName = name;
}

std::string ImportNode::getImportName() const {
    return m_importName;
}

bool ImportNode::hasImportName() const {
    return !m_importName.empty();
}

void ImportNode::setAlias(const std::string& alias) {
    m_alias = alias;
}

std::string ImportNode::getAlias() const {
    return m_alias;
}

bool ImportNode::hasAlias() const {
    return !m_alias.empty();
}

void ImportNode::setNamespace(const std::string& namespaceName) {
    m_namespace = namespaceName;
}

std::string ImportNode::getNamespace() const {
    return m_namespace;
}

bool ImportNode::hasNamespace() const {
    return !m_namespace.empty();
}

void ImportNode::setTargetName(const std::string& name) {
    m_targetName = name;
}

std::string ImportNode::getTargetName() const {
    return m_targetName;
}

bool ImportNode::hasTargetName() const {
    return !m_targetName.empty();
}

void ImportNode::setTargetType(const std::string& type) {
    m_targetType = type;
}

std::string ImportNode::getTargetType() const {
    return m_targetType;
}

bool ImportNode::hasTargetType() const {
    return !m_targetType.empty();
}

void ImportNode::setWildcardPattern(const std::string& pattern) {
    m_wildcardPattern = pattern;
}

std::string ImportNode::getWildcardPattern() const {
    return m_wildcardPattern;
}

bool ImportNode::hasWildcardPattern() const {
    return !m_wildcardPattern.empty();
}

void ImportNode::accept(NodeVisitor& visitor) {
    visitor.visitImport(*this);
}

std::shared_ptr<BaseNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(m_importType, m_importPath);
    cloned->m_importMode = m_importMode;
    cloned->m_importName = m_importName;
    cloned->m_alias = m_alias;
    cloned->m_namespace = m_namespace;
    cloned->m_targetName = m_targetName;
    cloned->m_targetType = m_targetType;
    cloned->m_wildcardPattern = m_wildcardPattern;
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

bool ImportNode::validate() const {
    if (m_importPath.empty()) {
        return false;
    }
    
    return BaseNode::validate();
}

std::string ImportNode::toString() const {
    std::stringstream ss;
    ss << "ImportNode{type=" << static_cast<int>(m_importType) 
       << ", path='" << m_importPath << "', mode=" << static_cast<int>(m_importMode) 
       << ", name='" << m_importName << "', alias='" << m_alias 
       << "', namespace='" << m_namespace << "', targetName='" << m_targetName 
       << "', targetType='" << m_targetType << "', wildcardPattern='" << m_wildcardPattern << "'}";
    return ss.str();
}

} // namespace CHTL