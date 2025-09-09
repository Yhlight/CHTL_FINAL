#include "NamespaceNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& name, NamespaceType type)
    : BaseNode(NodeType::NAMESPACE, name), namespaceType(type), namespaceName(name), 
      isAbstract(false), isFinal(false), priority(0), depth(0), 
      isActive(true), isProcessed(false) {
}

std::string NamespaceNode::getNamespaceTypeName() const {
    switch (namespaceType) {
        case NamespaceType::GLOBAL: return "Global";
        case NamespaceType::LOCAL: return "Local";
        case NamespaceType::NESTED: return "Nested";
        case NamespaceType::ANONYMOUS: return "Anonymous";
        default: return "Unknown";
    }
}

void NamespaceNode::setNamespaceAttribute(const std::string& name, const std::string& value) {
    namespaceAttributes[name] = value;
}

std::string NamespaceNode::getNamespaceAttribute(const std::string& name) const {
    auto it = namespaceAttributes.find(name);
    return (it != namespaceAttributes.end()) ? it->second : "";
}

bool NamespaceNode::hasNamespaceAttribute(const std::string& name) const {
    return namespaceAttributes.find(name) != namespaceAttributes.end();
}

void NamespaceNode::removeNamespaceAttribute(const std::string& name) {
    namespaceAttributes.erase(name);
}

void NamespaceNode::addNamespaceContent(std::shared_ptr<BaseNode> content) {
    if (content) {
        namespaceContent.push_back(content);
    }
}

void NamespaceNode::removeNamespaceContent(std::shared_ptr<BaseNode> content) {
    auto it = std::find(namespaceContent.begin(), namespaceContent.end(), content);
    if (it != namespaceContent.end()) {
        namespaceContent.erase(it);
    }
}

void NamespaceNode::addNamedElement(const std::string& name, std::shared_ptr<BaseNode> element) {
    if (element) {
        namedElements[name] = element;
    }
}

void NamespaceNode::removeNamedElement(const std::string& name) {
    namedElements.erase(name);
}

std::shared_ptr<BaseNode> NamespaceNode::getNamedElement(const std::string& name) const {
    auto it = namedElements.find(name);
    return (it != namedElements.end()) ? it->second : nullptr;
}

bool NamespaceNode::hasNamedElement(const std::string& name) const {
    return namedElements.find(name) != namedElements.end();
}

void NamespaceNode::setNamespaceExport(const std::string& name, const std::string& value) {
    namespaceExports[name] = value;
}

std::string NamespaceNode::getNamespaceExport(const std::string& name) const {
    auto it = namespaceExports.find(name);
    return (it != namespaceExports.end()) ? it->second : "";
}

bool NamespaceNode::hasNamespaceExport(const std::string& name) const {
    return namespaceExports.find(name) != namespaceExports.end();
}

void NamespaceNode::removeNamespaceExport(const std::string& name) {
    namespaceExports.erase(name);
}

void NamespaceNode::addParentNamespace(const std::string& parentName) {
    if (std::find(parentNamespaces.begin(), parentNamespaces.end(), parentName) == parentNamespaces.end()) {
        parentNamespaces.push_back(parentName);
    }
}

void NamespaceNode::removeParentNamespace(const std::string& parentName) {
    auto it = std::find(parentNamespaces.begin(), parentNamespaces.end(), parentName);
    if (it != parentNamespaces.end()) {
        parentNamespaces.erase(it);
    }
}

bool NamespaceNode::hasParentNamespace(const std::string& parentName) const {
    return std::find(parentNamespaces.begin(), parentNamespaces.end(), parentName) != parentNamespaces.end();
}

void NamespaceNode::setInheritedProperty(const std::string& name, const std::string& value) {
    inheritedProperties[name] = value;
}

std::string NamespaceNode::getInheritedProperty(const std::string& name) const {
    auto it = inheritedProperties.find(name);
    return (it != inheritedProperties.end()) ? it->second : "";
}

bool NamespaceNode::hasInheritedProperty(const std::string& name) const {
    return inheritedProperties.find(name) != inheritedProperties.end();
}

void NamespaceNode::removeInheritedProperty(const std::string& name) {
    inheritedProperties.erase(name);
}

void NamespaceNode::addConstraint(const std::string& constraint) {
    if (std::find(constraints.begin(), constraints.end(), constraint) == constraints.end()) {
        constraints.push_back(constraint);
    }
}

void NamespaceNode::removeConstraint(const std::string& constraint) {
    auto it = std::find(constraints.begin(), constraints.end(), constraint);
    if (it != constraints.end()) {
        constraints.erase(it);
    }
}

bool NamespaceNode::hasConstraint(const std::string& constraint) const {
    return std::find(constraints.begin(), constraints.end(), constraint) != constraints.end();
}

void NamespaceNode::setTypeConstraint(const std::string& type, const std::string& constraint) {
    typeConstraints[type] = constraint;
}

std::string NamespaceNode::getTypeConstraint(const std::string& type) const {
    auto it = typeConstraints.find(type);
    return (it != typeConstraints.end()) ? it->second : "";
}

bool NamespaceNode::hasTypeConstraint(const std::string& type) const {
    return typeConstraints.find(type) != typeConstraints.end();
}

void NamespaceNode::removeTypeConstraint(const std::string& type) {
    typeConstraints.erase(type);
}

void NamespaceNode::setAccessConstraint(const std::string& element, const std::string& constraint) {
    accessConstraints[element] = constraint;
}

std::string NamespaceNode::getAccessConstraint(const std::string& element) const {
    auto it = accessConstraints.find(element);
    return (it != accessConstraints.end()) ? it->second : "";
}

bool NamespaceNode::hasAccessConstraint(const std::string& element) const {
    return accessConstraints.find(element) != accessConstraints.end();
}

void NamespaceNode::removeAccessConstraint(const std::string& element) {
    accessConstraints.erase(element);
}

void NamespaceNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string NamespaceNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool NamespaceNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void NamespaceNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

void NamespaceNode::addNamespaceError(const std::string& error) {
    namespaceErrors.push_back(error);
}

void NamespaceNode::addNamespaceWarning(const std::string& warning) {
    namespaceWarnings.push_back(warning);
}

void NamespaceNode::clearNamespaceMessages() {
    namespaceErrors.clear();
    namespaceWarnings.clear();
}

void NamespaceNode::detectConflicts() {
    try {
        // 检测元素冲突
        detectElementConflicts();
        
        // 检测属性冲突
        detectPropertyConflicts();
        
        // 检测导出冲突
        detectExportConflicts();
        
    } catch (const std::exception& e) {
        addNamespaceError("检测冲突时发生错误: " + std::string(e.what()));
    }
}

void NamespaceNode::resolveConflicts() {
    try {
        // 解析元素冲突
        resolveElementConflicts();
        
        // 解析属性冲突
        resolvePropertyConflicts();
        
        // 解析导出冲突
        resolveExportConflicts();
        
    } catch (const std::exception& e) {
        addNamespaceError("解析冲突时发生错误: " + std::string(e.what()));
    }
}

void NamespaceNode::addConflict(const std::string& element, const std::string& conflict) {
    conflicts[element].push_back(conflict);
}

void NamespaceNode::removeConflict(const std::string& element) {
    conflicts.erase(element);
}

bool NamespaceNode::hasConflict(const std::string& element) const {
    return conflicts.find(element) != conflicts.end();
}

void NamespaceNode::setConflictResolution(const std::string& element, const std::string& resolution) {
    conflictResolutions[element] = resolution;
}

std::string NamespaceNode::getConflictResolution(const std::string& element) const {
    auto it = conflictResolutions.find(element);
    return (it != conflictResolutions.end()) ? it->second : "";
}

bool NamespaceNode::hasConflictResolution(const std::string& element) const {
    return conflictResolutions.find(element) != conflictResolutions.end();
}

void NamespaceNode::removeConflictResolution(const std::string& element) {
    conflictResolutions.erase(element);
}

void NamespaceNode::processNamespace() {
    try {
        // 处理继承
        processInheritance();
        
        // 处理约束
        processConstraints();
        
        // 处理冲突
        processConflicts();
        
        isProcessed = true;
        
    } catch (const std::exception& e) {
        addNamespaceError("处理命名空间时发生错误: " + std::string(e.what()));
    }
}

void NamespaceNode::processInheritance() {
    // 处理命名空间继承
    mergeParentProperties();
    resolveInheritanceConflicts();
}

void NamespaceNode::processConstraints() {
    // 处理命名空间约束
    // 简化实现
}

void NamespaceNode::processConflicts() {
    // 处理命名空间冲突
    detectConflicts();
    resolveConflicts();
}

bool NamespaceNode::validateNamespace() const {
    // 检查命名空间名称
    if (namespaceName.empty() && namespaceType != NamespaceType::ANONYMOUS) {
        return false;
    }
    
    // 检查命名空间路径
    if (namespacePath.empty()) {
        return false;
    }
    
    // 检查继承
    if (!validateInheritance()) {
        return false;
    }
    
    // 检查约束
    if (!validateConstraints()) {
        return false;
    }
    
    // 检查冲突
    if (!validateConflicts()) {
        return false;
    }
    
    return true;
}

bool NamespaceNode::validateInheritance() const {
    // 检查父命名空间
    for (const auto& parent : parentNamespaces) {
        if (parent.empty()) {
            return false;
        }
    }
    
    return true;
}

bool NamespaceNode::validateConstraints() const {
    for (const auto& constraint : constraints) {
        if (!checkGeneralConstraint(constraint)) {
            return false;
        }
    }
    
    return true;
}

bool NamespaceNode::validateConflicts() const {
    // 检查是否有未解决的冲突
    for (const auto& conflict : conflicts) {
        if (!hasConflictResolution(conflict.first)) {
            return false;
        }
    }
    
    return true;
}

bool NamespaceNode::validateAccess() const {
    // 检查访问约束
    for (const auto& constraint : accessConstraints) {
        if (!checkAccessConstraint(constraint.first, constraint.second)) {
            return false;
        }
    }
    
    return true;
}

std::string NamespaceNode::toHTML() const {
    std::ostringstream oss;
    
    oss << "<!-- Namespace: " << namespaceName << " -->\n";
    
    for (const auto& content : namespaceContent) {
        oss << content->toHTML() << "\n";
    }
    
    return oss.str();
}

std::string NamespaceNode::toCSS() const {
    std::ostringstream oss;
    
    for (const auto& content : namespaceContent) {
        if (content->getNodeType() == NodeType::STYLE) {
            auto styleContent = std::static_pointer_cast<StyleNode>(content);
            oss << styleContent->toCSS() << "\n";
        }
    }
    
    return oss.str();
}

std::string NamespaceNode::toJavaScript() const {
    std::ostringstream oss;
    
    for (const auto& content : namespaceContent) {
        if (content->getNodeType() == NodeType::SCRIPT) {
            auto scriptContent = std::static_pointer_cast<ScriptNode>(content);
            oss << scriptContent->toJavaScript() << "\n";
        }
    }
    
    return oss.str();
}

std::string NamespaceNode::toNamespaceString() const {
    std::ostringstream oss;
    
    oss << "[" << getNamespaceTypeName() << "] " << namespaceName << "\n";
    
    if (!namespacePath.empty()) {
        oss << "Path: " << namespacePath << "\n";
    }
    
    if (!parentNamespaces.empty()) {
        oss << "Parents: " << parentNamespaces[0];
        for (size_t i = 1; i < parentNamespaces.size(); ++i) {
            oss << ", " << parentNamespaces[i];
        }
        oss << "\n";
    }
    
    if (!constraints.empty()) {
        oss << "Constraints: " << constraints[0];
        for (size_t i = 1; i < constraints.size(); ++i) {
            oss << ", " << constraints[i];
        }
        oss << "\n";
    }
    
    oss << "Content:\n";
    for (const auto& content : namespaceContent) {
        oss << "  " << content->toString() << "\n";
    }
    
    return oss.str();
}

std::string NamespaceNode::formatNamespace() const {
    return toNamespaceString();
}

std::string NamespaceNode::minifyNamespace() const {
    return toNamespaceString();
}

std::string NamespaceNode::beautifyNamespace() const {
    return formatNamespace();
}

std::string NamespaceNode::compressNamespace() const {
    return minifyNamespace();
}

std::string NamespaceNode::decompressNamespace() const {
    return toNamespaceString();
}

std::string NamespaceNode::encodeNamespace() const {
    return toNamespaceString();
}

std::string NamespaceNode::decodeNamespace() const {
    return toNamespaceString();
}

std::string NamespaceNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "NamespaceType: " << getNamespaceTypeName() << "\n";
    oss << "NamespaceName: " << namespaceName << "\n";
    oss << "NamespacePath: " << namespacePath << "\n";
    oss << "Content: " << namespaceContent.size() << "\n";
    oss << "NamedElements: " << namedElements.size() << "\n";
    oss << "Exports: " << namespaceExports.size() << "\n";
    oss << "Parents: " << parentNamespaces.size() << "\n";
    oss << "Constraints: " << constraints.size() << "\n";
    oss << "Conflicts: " << conflicts.size() << "\n";
    oss << "IsAbstract: " << (isAbstract ? "true" : "false") << "\n";
    oss << "IsFinal: " << (isFinal ? "true" : "false") << "\n";
    oss << "Priority: " << priority << "\n";
    oss << "Depth: " << depth << "\n";
    oss << "IsActive: " << (isActive ? "true" : "false") << "\n";
    oss << "IsProcessed: " << (isProcessed ? "true" : "false") << "\n";
    oss << "Errors: " << namespaceErrors.size() << "\n";
    oss << "Warnings: " << namespaceWarnings.size() << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(namespaceName, namespaceType);
    cloned->namespacePath = namespacePath;
    cloned->namespaceAttributes = namespaceAttributes;
    cloned->namespaceExports = namespaceExports;
    cloned->parentNamespaces = parentNamespaces;
    cloned->inheritedProperties = inheritedProperties;
    cloned->constraints = constraints;
    cloned->typeConstraints = typeConstraints;
    cloned->accessConstraints = accessConstraints;
    cloned->metadata = metadata;
    cloned->conflicts = conflicts;
    cloned->conflictResolutions = conflictResolutions;
    cloned->namespaceErrors = namespaceErrors;
    cloned->namespaceWarnings = namespaceWarnings;
    cloned->isAbstract = isAbstract;
    cloned->isFinal = isFinal;
    cloned->priority = priority;
    cloned->depth = depth;
    cloned->isActive = isActive;
    cloned->isProcessed = isProcessed;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& content : namespaceContent) {
        cloned->addNamespaceContent(content->clone());
    }
    
    for (const auto& element : namedElements) {
        cloned->addNamedElement(element.first, element.second->clone());
    }
    
    return cloned;
}

bool NamespaceNode::operator==(const NamespaceNode& other) const {
    return BaseNode::operator==(other) &&
           namespaceType == other.namespaceType &&
           namespaceName == other.namespaceName &&
           namespacePath == other.namespacePath &&
           namespaceAttributes == other.namespaceAttributes &&
           parentNamespaces == other.parentNamespaces &&
           constraints == other.constraints &&
           isAbstract == other.isAbstract &&
           isFinal == other.isFinal &&
           priority == other.priority &&
           depth == other.depth &&
           isActive == other.isActive &&
           isProcessed == other.isProcessed;
}

bool NamespaceNode::operator!=(const NamespaceNode& other) const {
    return !(*this == other);
}

std::string NamespaceNode::processNamespacePath(const std::string& path) const {
    // 简化的命名空间路径处理
    return path;
}

std::string NamespaceNode::validateNamespacePath(const std::string& path) const {
    // 简化的命名空间路径验证
    return path;
}

std::string NamespaceNode::sanitizeNamespacePath(const std::string& path) const {
    // 简化的命名空间路径清理
    return path;
}

std::string NamespaceNode::resolveNamespacePath(const std::string& path) const {
    // 简化的命名空间路径解析
    return path;
}

std::string NamespaceNode::normalizeNamespacePath(const std::string& path) const {
    // 简化的命名空间路径规范化
    return path;
}

std::string NamespaceNode::expandNamespacePath(const std::string& path) const {
    // 简化的命名空间路径展开
    return path;
}

void NamespaceNode::mergeParentProperties() {
    // 合并父命名空间属性
    // 简化实现
}

void NamespaceNode::resolveInheritanceConflicts() {
    // 解决继承冲突
    // 简化实现
}

bool NamespaceNode::checkTypeConstraint(const std::string& type, const std::string& value) const {
    // 简化的类型约束检查
    return true;
}

bool NamespaceNode::checkGeneralConstraint(const std::string& constraint) const {
    // 简化的通用约束检查
    return true;
}

bool NamespaceNode::checkAccessConstraint(const std::string& element, const std::string& access) const {
    // 简化的访问约束检查
    return true;
}

void NamespaceNode::detectElementConflicts() {
    // 检测元素冲突
    // 简化实现
}

void NamespaceNode::detectPropertyConflicts() {
    // 检测属性冲突
    // 简化实现
}

void NamespaceNode::detectExportConflicts() {
    // 检测导出冲突
    // 简化实现
}

void NamespaceNode::resolveElementConflicts() {
    // 解析元素冲突
    // 简化实现
}

void NamespaceNode::resolvePropertyConflicts() {
    // 解析属性冲突
    // 简化实现
}

void NamespaceNode::resolveExportConflicts() {
    // 解析导出冲突
    // 简化实现
}

void NamespaceNode::handleNamespaceError(const std::string& error) {
    addNamespaceError(error);
}

void NamespaceNode::handleNamespaceWarning(const std::string& warning) {
    addNamespaceWarning(warning);
}

} // namespace CHTL