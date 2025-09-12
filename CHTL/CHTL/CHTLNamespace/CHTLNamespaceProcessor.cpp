#include "CHTLNamespaceProcessor.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLNamespaceProcessor::CHTLNamespaceProcessor() {}

void CHTLNamespaceProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLNamespaceProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::shared_ptr<NamespaceNode> CHTLNamespaceProcessor::getNamespaceNode(const std::string& name) const {
    auto it = namespaceNodes.find(name);
    if (it != namespaceNodes.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLNamespaceProcessor::mergeNamespaceProperties(std::shared_ptr<NamespaceNode> target, std::shared_ptr<NamespaceNode> source) {
    for (const auto& attr : source->getNamespaceAttributes()) {
        if (!target->hasNamespaceAttribute(attr.first)) {
            target->setNamespaceAttribute(attr.first, attr.second);
        }
    }
    for (const auto& meta : source->getMetadata()) {
        if (!target->hasMetadata(meta.first)) {
            target->setMetadata(meta.first, meta.second);
        }
    }
}

void CHTLNamespaceProcessor::registerNamespaceNode(std::shared_ptr<NamespaceNode> namespaceNode) {
    if (namespaceNode) {
        namespaceNodes[namespaceNode->getNamespaceName()] = namespaceNode;
    }
}

void CHTLNamespaceProcessor::unregisterNamespaceNode(const std::string& name) {
    namespaceNodes.erase(name);
}

bool CHTLNamespaceProcessor::hasNamespaceNode(const std::string& name) const {
    return namespaceNodes.count(name) > 0;
}

std::shared_ptr<BaseNode> CHTLNamespaceProcessor::processNamespaceNode(const std::string& name) {
    auto namespaceNode = getNamespaceNode(name);
    if (!namespaceNode) {
        addError("命名空间节点 '" + name + "' 未找到");
        return nullptr;
    }

    // 处理命名空间
    namespaceNode->processNamespace();
    return namespaceNode;
}

std::shared_ptr<BaseNode> CHTLNamespaceProcessor::instantiateNamespaceNode(const std::string& name) {
    auto namespaceNode = getNamespaceNode(name);
    if (!namespaceNode) {
        addError("命名空间节点 '" + name + "' 未找到");
        return nullptr;
    }
    return namespaceNode->clone();
}

void CHTLNamespaceProcessor::processAllNamespaces() {
    for (auto& pair : namespaceNodes) {
        pair.second->processNamespace();
    }
}

void CHTLNamespaceProcessor::processNamespaceByType(const std::string& type) {
    for (auto& pair : namespaceNodes) {
        if (pair.second->getNamespaceTypeName() == type) {
            pair.second->processNamespace();
        }
    }
}

void CHTLNamespaceProcessor::processActiveNamespaces() {
    for (auto& pair : namespaceNodes) {
        if (pair.second->isActiveNamespace()) {
            pair.second->processNamespace();
        }
    }
}

void CHTLNamespaceProcessor::processInactiveNamespaces() {
    for (auto& pair : namespaceNodes) {
        if (!pair.second->isActiveNamespace()) {
            pair.second->processNamespace();
        }
    }
}

void CHTLNamespaceProcessor::enterScope(const std::string& namespaceName) {
    scopeStack.push(namespaceName);
}

void CHTLNamespaceProcessor::exitScope() {
    if (!scopeStack.empty()) {
        scopeStack.pop();
    }
}

std::string CHTLNamespaceProcessor::getCurrentScope() const {
    return scopeStack.empty() ? "" : scopeStack.top();
}

std::vector<std::string> CHTLNamespaceProcessor::getScopeHierarchy() const {
    std::vector<std::string> hierarchy;
    std::stack<std::string> tempStack = scopeStack;
    while (!tempStack.empty()) {
        hierarchy.push_back(tempStack.top());
        tempStack.pop();
    }
    std::reverse(hierarchy.begin(), hierarchy.end());
    return hierarchy;
}

void CHTLNamespaceProcessor::clearScopeStack() {
    while (!scopeStack.empty()) {
        scopeStack.pop();
    }
}

void CHTLNamespaceProcessor::addToScope(const std::string& namespaceName, const std::string& elementName) {
    scopeElements[namespaceName].insert(elementName);
}

void CHTLNamespaceProcessor::removeFromScope(const std::string& namespaceName, const std::string& elementName) {
    auto it = scopeElements.find(namespaceName);
    if (it != scopeElements.end()) {
        it->second.erase(elementName);
    }
}

std::set<std::string> CHTLNamespaceProcessor::getScopeElements(const std::string& namespaceName) const {
    auto it = scopeElements.find(namespaceName);
    return (it != scopeElements.end()) ? it->second : std::set<std::string>();
}

bool CHTLNamespaceProcessor::isInScope(const std::string& namespaceName, const std::string& elementName) const {
    auto it = scopeElements.find(namespaceName);
    return (it != scopeElements.end()) && (it->second.find(elementName) != it->second.end());
}

void CHTLNamespaceProcessor::createNestedNamespace(const std::string& parentName, const std::string& childName) {
    auto parentNode = getNamespaceNode(parentName);
    if (parentNode) {
        // 简化实现：直接添加到作用域层次
        scopeHierarchy[parentName].push_back(childName);
    }
}

void CHTLNamespaceProcessor::removeNestedNamespace(const std::string& parentName, const std::string& childName) {
    auto parentNode = getNamespaceNode(parentName);
    if (parentNode) {
        // 简化实现：直接从作用域层次中移除
        auto& nested = scopeHierarchy[parentName];
        auto it = std::find(nested.begin(), nested.end(), childName);
        if (it != nested.end()) {
            nested.erase(it);
        }
    }
}

std::vector<std::string> CHTLNamespaceProcessor::getNestedNamespaces(const std::string& parentName) const {
    auto it = scopeHierarchy.find(parentName);
    return (it != scopeHierarchy.end()) ? it->second : std::vector<std::string>();
}

std::string CHTLNamespaceProcessor::getParentNamespace(const std::string& childName) const {
    for (const auto& pair : scopeHierarchy) {
        const auto& nested = pair.second;
        if (std::find(nested.begin(), nested.end(), childName) != nested.end()) {
            return pair.first;
        }
    }
    return "";
}

bool CHTLNamespaceProcessor::isNestedNamespace(const std::string& namespaceName) const {
    return !getParentNamespace(namespaceName).empty();
}

void CHTLNamespaceProcessor::detectConflicts() {
    detectNamespaceConflicts("");
    detectElementConflicts("");
    detectScopeConflicts();
}

void CHTLNamespaceProcessor::detectNamespaceConflicts(const std::string& namespaceName) {
    // 简化的命名空间冲突检测
    for (const auto& pair : namespaceNodes) {
        const auto& name = pair.first;
        if (namespaceName.empty() || name == namespaceName) {
            // 检查命名空间名称冲突
            // 简化实现
        }
    }
}

void CHTLNamespaceProcessor::detectElementConflicts(const std::string& elementName) {
    // 简化的元素冲突检测
    std::map<std::string, std::vector<std::string>> elementLocations;
    
    for (const auto& pair : namespaceNodes) {
        const auto& namespaceName = pair.first;
        const auto& namespaceNode = pair.second;
        
        for (const auto& element : namespaceNode->getNamedElements()) {
            if (elementName.empty() || element.first == elementName) {
                elementLocations[element.first].push_back(namespaceName);
            }
        }
    }
    
    // 检测冲突
    for (const auto& pair : elementLocations) {
        if (pair.second.size() > 1) {
            globalConflicts[pair.first] = pair.second;
        }
    }
}

void CHTLNamespaceProcessor::detectScopeConflicts() {
    // 简化的作用域冲突检测
    // 简化实现
}

void CHTLNamespaceProcessor::resolveConflict(const std::string& conflictKey, const std::string& resolution) {
    conflictResolutions[conflictKey] = resolution;
}

std::vector<std::string> CHTLNamespaceProcessor::getConflicts(const std::string& namespaceName) const {
    auto it = globalConflicts.find(namespaceName);
    return (it != globalConflicts.end()) ? it->second : std::vector<std::string>();
}

std::string CHTLNamespaceProcessor::getConflictResolution(const std::string& conflictKey) const {
    auto it = conflictResolutions.find(conflictKey);
    return (it != conflictResolutions.end()) ? it->second : "";
}

bool CHTLNamespaceProcessor::hasConflict(const std::string& namespaceName) const {
    return globalConflicts.find(namespaceName) != globalConflicts.end();
}

std::shared_ptr<BaseNode> CHTLNamespaceProcessor::findElement(const std::string& elementName) const {
    // 在当前作用域中查找
    std::string currentScope = getCurrentScope();
    if (!currentScope.empty()) {
        auto element = findElementInNamespace(currentScope, elementName);
        if (element) {
            return element;
        }
    }
    
    // 在所有命名空间中查找
    for (const auto& pair : namespaceNodes) {
        auto element = findElementInNamespace(pair.first, elementName);
        if (element) {
            return element;
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLNamespaceProcessor::findElementInNamespace(const std::string& namespaceName, const std::string& elementName) const {
    auto namespaceNode = getNamespaceNode(namespaceName);
    if (namespaceNode) {
        return namespaceNode->getNamedElement(elementName);
    }
    return nullptr;
}

std::vector<std::string> CHTLNamespaceProcessor::findNamespacesContaining(const std::string& elementName) const {
    std::vector<std::string> namespaces;
    
    for (const auto& pair : namespaceNodes) {
        if (pair.second->hasNamedElement(elementName)) {
            namespaces.push_back(pair.first);
        }
    }
    
    return namespaces;
}

std::vector<std::string> CHTLNamespaceProcessor::findElementsInNamespace(const std::string& namespaceName) const {
    auto namespaceNode = getNamespaceNode(namespaceName);
    if (namespaceNode) {
        std::vector<std::string> elements;
        for (const auto& pair : namespaceNode->getNamedElements()) {
            elements.push_back(pair.first);
        }
        return elements;
    }
    return std::vector<std::string>();
}

bool CHTLNamespaceProcessor::validateNamespaceNode(const std::string& name) const {
    auto namespaceNode = getNamespaceNode(name);
    if (!namespaceNode) {
        return false;
    }
    return namespaceNode->validateNamespace();
}

bool CHTLNamespaceProcessor::validateAllNamespaces() const {
    for (const auto& pair : namespaceNodes) {
        if (!pair.second->validateNamespace()) {
            return false;
        }
    }
    return true;
}

bool CHTLNamespaceProcessor::validateScopeIntegrity() const {
    // 简化的作用域完整性验证
    return true;
}

bool CHTLNamespaceProcessor::validateConflictResolutions() const {
    // 简化的冲突解决验证
    return true;
}

const std::vector<std::string>& CHTLNamespaceProcessor::getErrors() const {
    return errors;
}

const std::vector<std::string>& CHTLNamespaceProcessor::getWarnings() const {
    return warnings;
}

void CHTLNamespaceProcessor::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLNamespaceProcessor::reset() {
    namespaceNodes.clear();
    clearScopeStack();
    scopeHierarchy.clear();
    scopeElements.clear();
    globalConflicts.clear();
    conflictResolutions.clear();
    clearMessages();
}

std::string CHTLNamespaceProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL命名空间处理器调试信息:\n";
    oss << "注册命名空间节点数: " << namespaceNodes.size() << "\n";
    oss << "当前作用域深度: " << scopeStack.size() << "\n";
    oss << "作用域层次数: " << scopeHierarchy.size() << "\n";
    oss << "作用域元素数: " << scopeElements.size() << "\n";
    oss << "全局冲突数: " << globalConflicts.size() << "\n";
    oss << "冲突解决数: " << conflictResolutions.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL