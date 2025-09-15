#include "CHTL/CHTLNode/ImportNode.h"
#include <sstream>

namespace CHTL {

ImportNode::ImportNode(ImportType importType, const std::string& filePath)
    : BaseNode(NodeType::IMPORT)
    , m_importType(importType)
    , m_filePath(filePath)
    , m_importMode(ImportMode::UNKNOWN)
{
}

ImportNode::~ImportNode() {
}

void ImportNode::setImportType(ImportType importType) {
    m_importType = importType;
}

ImportType ImportNode::getImportType() const {
    return m_importType;
}

void ImportNode::setFilePath(const std::string& filePath) {
    m_filePath = filePath;
}

const std::string& ImportNode::getFilePath() const {
    return m_filePath;
}

void ImportNode::setImportMode(ImportMode mode) {
    m_importMode = mode;
}

ImportMode ImportNode::getImportMode() const {
    return m_importMode;
}

void ImportNode::addImportItem(const std::string& item) {
    m_importItems.push_back(item);
}

const std::vector<std::string>& ImportNode::getImportItems() const {
    return m_importItems;
}

void ImportNode::setAlias(const std::string& alias) {
    m_alias = alias;
}

const std::string& ImportNode::getAlias() const {
    return m_alias;
}

bool ImportNode::hasAlias() const {
    return !m_alias.empty();
}

void ImportNode::setNamespace(const std::string& ns) {
    m_namespace = ns;
}

const std::string& ImportNode::getNamespace() const {
    return m_namespace;
}

bool ImportNode::hasNamespace() const {
    return !m_namespace.empty();
}

void ImportNode::setImportContent(const std::string& content) {
    m_importContent = content;
}

const std::string& ImportNode::getImportContent() const {
    return m_importContent;
}

std::shared_ptr<BaseNode> ImportNode::clone() const {
    auto cloned = std::make_shared<ImportNode>(m_importType, m_filePath);
    cloned->m_importMode = m_importMode;
    cloned->m_importItems = m_importItems;
    cloned->m_alias = m_alias;
    cloned->m_namespace = m_namespace;
    cloned->m_importContent = m_importContent;
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

std::string ImportNode::toString() const {
    std::ostringstream oss;
    oss << "ImportNode(type=" << getImportTypeName(m_importType)
        << ", path=\"" << m_filePath << "\""
        << ", mode=" << getImportModeName(m_importMode)
        << ", items=" << m_importItems.size()
        << ", alias=\"" << m_alias << "\""
        << ", namespace=\"" << m_namespace << "\""
        << ", children=" << m_children.size()
        << ", line=" << m_line << ", column=" << m_column << ")";
    return oss.str();
}

std::string ImportNode::getImportTypeName(ImportType type) {
    switch (type) {
        case ImportType::CHTL:
            return "CHTL";
        case ImportType::HTML:
            return "HTML";
        case ImportType::CSS:
            return "CSS";
        case ImportType::JS:
            return "JS";
        case ImportType::CMOD:
            return "CMOD";
        case ImportType::CJMOD:
            return "CJMOD";
        case ImportType::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

std::string ImportNode::getImportModeName(ImportMode mode) {
    switch (mode) {
        case ImportMode::PRECISE:
            return "PRECISE";
        case ImportMode::TYPE:
            return "TYPE";
        case ImportMode::WILDCARD:
            return "WILDCARD";
        case ImportMode::UNKNOWN:
        default:
            return "UNKNOWN";
    }
}

} // namespace CHTL