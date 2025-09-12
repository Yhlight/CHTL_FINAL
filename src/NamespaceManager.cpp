#include "CHTL/NamespaceManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

NamespaceManager::NamespaceManager(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_debugMode(false)
    , m_totalNamespaces(0)
    , m_totalSymbols(0)
    , m_conflictCount(0)
{
}

NamespaceManager::~NamespaceManager() {
}

bool NamespaceManager::addNamespace(std::shared_ptr<NamespaceNode> namespaceNode) {
    if (!namespaceNode) return false;
    
    m_totalNamespaces++;
    
    if (m_debugMode) {
        std::cout << "[NamespaceManager] Adding namespace: " << namespaceNode->getNamespaceName() << std::endl;
    }
    
    // 检查命名空间是否已存在
    std::string name = namespaceNode->getNamespaceName();
    if (m_namespaceMap.find(name) != m_namespaceMap.end()) {
        addWarning("Namespace already exists: " + name);
        return false;
    }
    
    // 添加命名空间
    m_namespaces.push_back(namespaceNode);
    m_namespaceMap[name] = namespaceNode;
    
    // 统计符号数量
    m_totalSymbols += namespaceNode->getSymbols().size();
    
    return true;
}

std::shared_ptr<NamespaceNode> NamespaceManager::getNamespace(const std::string& name) {
    auto it = m_namespaceMap.find(name);
    if (it != m_namespaceMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool NamespaceManager::hasNamespace(const std::string& name) {
    return m_namespaceMap.find(name) != m_namespaceMap.end();
}

std::string NamespaceManager::resolveSymbol(const std::string& symbolName, const std::string& currentNamespace) {
    if (m_debugMode) {
        std::cout << "[NamespaceManager] Resolving symbol: " << symbolName << " in namespace: " << currentNamespace << std::endl;
    }
    
    // 检查是否为完全限定名称
    if (symbolName.find("::") != std::string::npos) {
        auto [namespaceName, localName] = parseFullQualifiedName(symbolName);
        return findSymbol(localName, namespaceName);
    }
    
    // 在当前命名空间中查找
    if (!currentNamespace.empty()) {
        std::string result = findSymbol(symbolName, currentNamespace);
        if (!result.empty()) {
            return result;
        }
    }
    
    // 在全局命名空间中查找
    return findSymbol(symbolName, "");
}

bool NamespaceManager::checkSymbolConflict(const std::string& symbolName, const std::string& namespace1, const std::string& namespace2) {
    if (namespace1 == namespace2) return false;
    
    auto ns1 = getNamespace(namespace1);
    auto ns2 = getNamespace(namespace2);
    
    if (!ns1 || !ns2) return false;
    
    bool hasSymbol1 = ns1->hasSymbol(symbolName);
    bool hasSymbol2 = ns2->hasSymbol(symbolName);
    
    if (hasSymbol1 && hasSymbol2) {
        m_conflictCount++;
        addWarning("Symbol conflict detected: " + symbolName + " in namespaces " + namespace1 + " and " + namespace2);
        return true;
    }
    
    return false;
}

bool NamespaceManager::mergeNamespaces(const std::string& targetNamespace, const std::string& sourceNamespace) {
    auto target = getNamespace(targetNamespace);
    auto source = getNamespace(sourceNamespace);
    
    if (!target || !source) {
        addError("Cannot merge namespaces: one or both namespaces not found");
        return false;
    }
    
    if (m_debugMode) {
        std::cout << "[NamespaceManager] Merging namespace " << sourceNamespace << " into " << targetNamespace << std::endl;
    }
    
    // 合并符号
    const auto& sourceSymbols = source->getSymbols();
    for (const auto& symbol : sourceSymbols) {
        if (target->hasSymbol(symbol.first)) {
            addWarning("Symbol conflict during merge: " + symbol.first);
        } else {
            target->addSymbol(symbol.first, symbol.second.first, symbol.second.second);
        }
    }
    
    // 合并子命名空间
    const auto& sourceChildren = source->getChildNamespaces();
    for (const auto& child : sourceChildren) {
        target->addChildNamespace(child);
    }
    
    return true;
}

std::string NamespaceManager::getNamespaceHierarchy(const std::string& rootNamespace) {
    std::ostringstream oss;
    
    if (rootNamespace.empty()) {
        // 显示所有根命名空间
        for (const auto& ns : m_namespaces) {
            if (!ns->hasParentNamespace()) {
                oss << buildHierarchyString(ns, 0) << "\n";
            }
        }
    } else {
        auto ns = getNamespace(rootNamespace);
        if (ns) {
            oss << buildHierarchyString(ns, 0);
        }
    }
    
    return oss.str();
}

const std::vector<std::shared_ptr<NamespaceNode>>& NamespaceManager::getAllNamespaces() const {
    return m_namespaces;
}

void NamespaceManager::clear() {
    m_namespaces.clear();
    m_namespaceMap.clear();
    m_processedNamespaces.clear();
    m_totalNamespaces = 0;
    m_totalSymbols = 0;
    m_conflictCount = 0;
}

void NamespaceManager::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool NamespaceManager::isDebugMode() const {
    return m_debugMode;
}

std::string NamespaceManager::getStatistics() const {
    std::ostringstream oss;
    oss << "Namespace Statistics:\n";
    oss << "  Total namespaces: " << m_totalNamespaces << "\n";
    oss << "  Total symbols: " << m_totalSymbols << "\n";
    oss << "  Conflicts detected: " << m_conflictCount << "\n";
    return oss.str();
}

std::pair<std::string, std::string> NamespaceManager::parseFullQualifiedName(const std::string& fullName) {
    size_t pos = fullName.find("::");
    if (pos != std::string::npos) {
        return {fullName.substr(0, pos), fullName.substr(pos + 2)};
    }
    return {"", fullName};
}

std::string NamespaceManager::findSymbol(const std::string& symbolName, const std::string& namespaceName) {
    if (namespaceName.empty()) {
        // 在全局命名空间中查找
        for (const auto& ns : m_namespaces) {
            if (ns->hasSymbol(symbolName)) {
                return ns->getSymbol(symbolName);
            }
        }
    } else {
        auto ns = getNamespace(namespaceName);
        if (ns && ns->hasSymbol(symbolName)) {
            return ns->getSymbol(symbolName);
        }
    }
    
    return "";
}

std::string NamespaceManager::buildHierarchyString(std::shared_ptr<NamespaceNode> node, int depth) {
    std::ostringstream oss;
    std::string indent(depth * 2, ' ');
    
    oss << indent << node->getNamespaceName() << " (" << node->getSymbols().size() << " symbols)";
    
    const auto& children = node->getChildNamespaces();
    for (const auto& child : children) {
        oss << "\n" << buildHierarchyString(child, depth + 1);
    }
    
    return oss.str();
}

void NamespaceManager::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("NamespaceManager Error: " + message);
    }
}

void NamespaceManager::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("NamespaceManager Warning: " + message);
    }
}

} // namespace CHTL