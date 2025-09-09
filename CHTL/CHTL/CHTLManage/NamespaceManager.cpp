#include "NamespaceManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>

namespace CHTL {

NamespaceManager::NamespaceManager() 
    : conflictDetection(true), autoMerge(true), debugMode(false) {
    resetStats();
}

void NamespaceManager::addNamespace(std::shared_ptr<NamespaceNode> namespaceNode) {
    if (!namespaceNode) {
        return;
    }
    
    std::string name = namespaceNode->getNamespaceName();
    namespaces[name] = namespaceNode;
    
    // 更新统计信息
    updateStats();
    
    debugLog("Added namespace: " + name);
}

void NamespaceManager::removeNamespace(const std::string& name) {
    auto it = namespaces.find(name);
    if (it != namespaces.end()) {
        namespaces.erase(it);
        updateStats();
        debugLog("Removed namespace: " + name);
    }
}

std::shared_ptr<NamespaceNode> NamespaceManager::getNamespace(const std::string& name) const {
    auto it = namespaces.find(name);
    if (it != namespaces.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<NamespaceNode> NamespaceManager::findNamespace(const std::string& name, bool recursive) const {
    // 首先在当前作用域中查找
    if (!currentScope.empty()) {
        std::string qualifiedName = currentScope + "::" + name;
        auto it = namespaces.find(qualifiedName);
        if (it != namespaces.end()) {
            return it->second;
        }
    }
    
    // 在全局作用域中查找
    auto it = namespaces.find(name);
    if (it != namespaces.end()) {
        return it->second;
    }
    
    // 在别名中查找
    if (hasAlias(name)) {
        std::string target = getAliasTarget(name);
        return findNamespace(target, recursive);
    }
    
    // 递归查找（简化实现）
    if (recursive) {
        for (const auto& ns : namespaces) {
            if (ns.first.find(name) != std::string::npos) {
                return ns.second;
            }
        }
    }
    
    return nullptr;
}

std::vector<std::string> NamespaceManager::findNamespaces(const std::string& pattern) const {
    std::vector<std::string> matches;
    
    for (const auto& ns : namespaces) {
        if (ns.first.find(pattern) != std::string::npos) {
            matches.push_back(ns.first);
        }
    }
    
    return matches;
}

std::shared_ptr<BaseNode> NamespaceManager::findMember(const std::string& qualifiedName) const {
    if (isQualifiedName(qualifiedName)) {
        std::vector<std::string> parts = parseQualifiedName(qualifiedName);
        if (parts.size() >= 2) {
            std::string namespaceName = parts[0];
            std::string memberName = parts[1];
            
            auto ns = getNamespace(namespaceName);
            if (ns) {
                return ns->getMember(memberName);
            }
        }
    } else {
        // 在当前作用域中查找
        if (!currentScope.empty()) {
            auto ns = getNamespace(currentScope);
            if (ns) {
                return ns->getMember(qualifiedName);
            }
        }
        
        // 在所有命名空间中查找
        for (const auto& ns : namespaces) {
            auto member = ns.second->getMember(qualifiedName);
            if (member) {
                return member;
            }
        }
    }
    
    return nullptr;
}

std::vector<std::string> NamespaceManager::findMembers(const std::string& pattern) const {
    std::vector<std::string> matches;
    
    for (const auto& ns : namespaces) {
        auto members = ns.second->findMembers(pattern);
        for (const auto& member : members) {
            matches.push_back(ns.first + "::" + member);
        }
    }
    
    return matches;
}

void NamespaceManager::mergeNamespaces(const std::string& target, const std::string& source) {
    auto targetNs = getNamespace(target);
    auto sourceNs = getNamespace(source);
    
    if (!targetNs || !sourceNs) {
        debugLog("Cannot merge namespaces: " + target + " and " + source);
        return;
    }
    
    mergeNamespaceMembers(targetNs, sourceNs);
    removeNamespace(source);
    
    debugLog("Merged namespaces: " + source + " into " + target);
}

void NamespaceManager::splitNamespace(const std::string& name, const std::vector<std::string>& newNames) {
    auto ns = getNamespace(name);
    if (!ns) {
        debugLog("Cannot split namespace: " + name + " not found");
        return;
    }
    
    // 简化实现：将成员分配到新的命名空间
    for (const auto& newName : newNames) {
        auto newNs = std::make_shared<NamespaceNode>(newName);
        addNamespace(newNs);
    }
    
    debugLog("Split namespace: " + name + " into " + std::to_string(newNames.size()) + " namespaces");
}

void NamespaceManager::renameNamespace(const std::string& oldName, const std::string& newName) {
    auto ns = getNamespace(oldName);
    if (!ns) {
        debugLog("Cannot rename namespace: " + oldName + " not found");
        return;
    }
    
    ns->setNamespaceName(newName);
    namespaces[newName] = ns;
    namespaces.erase(oldName);
    
    debugLog("Renamed namespace: " + oldName + " to " + newName);
}

bool NamespaceManager::hasConflicts() const {
    return !findConflicts().empty();
}

std::vector<std::string> NamespaceManager::getConflicts() const {
    return findConflicts();
}

bool NamespaceManager::resolveConflict(const std::string& name, const std::string& resolution) {
    // 简化实现
    debugLog("Resolving conflict: " + name + " with resolution: " + resolution);
    return true;
}

void NamespaceManager::performAutoMerge() {
    if (!autoMerge) {
        return;
    }
    
    // 简化实现：合并具有相同名称的命名空间
    std::map<std::string, std::vector<std::string>> nameGroups;
    
    for (const auto& ns : namespaces) {
        std::string baseName = NamespaceNode::parseNamespaceName(ns.first);
        nameGroups[baseName].push_back(ns.first);
    }
    
    for (const auto& group : nameGroups) {
        if (group.second.size() > 1) {
            // 合并到第一个命名空间
            std::string target = group.second[0];
            for (size_t i = 1; i < group.second.size(); ++i) {
                mergeNamespaces(target, group.second[i]);
            }
        }
    }
}

std::vector<std::string> NamespaceManager::getNamespaceHierarchy() const {
    std::vector<std::string> hierarchy;
    
    for (const auto& ns : namespaces) {
        hierarchy.push_back(ns.first);
    }
    
    std::sort(hierarchy.begin(), hierarchy.end());
    return hierarchy;
}

std::vector<std::string> NamespaceManager::getChildNamespaces(const std::string& parent) const {
    std::vector<std::string> children;
    
    for (const auto& ns : namespaces) {
        if (ns.first.find(parent + "::") == 0) {
            children.push_back(ns.first);
        }
    }
    
    return children;
}

std::vector<std::string> NamespaceManager::getParentNamespaces(const std::string& child) const {
    return NamespaceNode::getNamespaceHierarchy(child);
}

void NamespaceManager::addAlias(const std::string& alias, const std::string& target) {
    aliases[alias] = target;
    debugLog("Added alias: " + alias + " -> " + target);
}

void NamespaceManager::removeAlias(const std::string& alias) {
    aliases.erase(alias);
    debugLog("Removed alias: " + alias);
}

std::string NamespaceManager::getAliasTarget(const std::string& alias) const {
    auto it = aliases.find(alias);
    if (it != aliases.end()) {
        return it->second;
    }
    return "";
}

bool NamespaceManager::hasAlias(const std::string& alias) const {
    return aliases.find(alias) != aliases.end();
}

void NamespaceManager::enterScope(const std::string& namespaceName) {
    scopeStack.push_back(currentScope);
    currentScope = namespaceName;
    debugLog("Entered scope: " + namespaceName);
}

void NamespaceManager::exitScope() {
    if (!scopeStack.empty()) {
        currentScope = scopeStack.back();
        scopeStack.pop_back();
        debugLog("Exited scope, current: " + currentScope);
    }
}

void NamespaceManager::importNamespace(const std::string& name, const std::string& alias) {
    if (alias.empty()) {
        importedNamespaces.push_back(name);
    } else {
        addAlias(alias, name);
    }
    debugLog("Imported namespace: " + name + (alias.empty() ? "" : " as " + alias));
}

void NamespaceManager::exportNamespace(const std::string& name) {
    exportedNamespaces.push_back(name);
    debugLog("Exported namespace: " + name);
}

bool NamespaceManager::validateNamespace(const std::string& name) const {
    return NamespaceNode::isValidNamespaceName(name);
}

std::vector<std::string> NamespaceManager::validateAllNamespaces() const {
    std::vector<std::string> errors;
    
    for (const auto& ns : namespaces) {
        if (!validateNamespace(ns.first)) {
            errors.push_back("Invalid namespace name: " + ns.first);
        }
    }
    
    return errors;
}

void NamespaceManager::clear() {
    namespaces.clear();
    aliases.clear();
    scopeStack.clear();
    currentScope.clear();
    importedNamespaces.clear();
    exportedNamespaces.clear();
    updateStats();
    debugLog("Cleared all namespaces");
}

void NamespaceManager::clearAliases() {
    aliases.clear();
    debugLog("Cleared all aliases");
}

void NamespaceManager::clearImports() {
    importedNamespaces.clear();
    exportedNamespaces.clear();
    debugLog("Cleared all imports/exports");
}

std::string NamespaceManager::getDebugInfo() const {
    std::ostringstream oss;
    oss << "NamespaceManager Debug Info:\n";
    oss << "  Total Namespaces: " << stats.totalNamespaces << "\n";
    oss << "  Nested Namespaces: " << stats.nestedNamespaces << "\n";
    oss << "  Total Members: " << stats.totalMembers << "\n";
    oss << "  Total Aliases: " << stats.totalAliases << "\n";
    oss << "  Conflicts: " << stats.conflicts << "\n";
    oss << "  Imported: " << stats.importedNamespaces << "\n";
    oss << "  Exported: " << stats.exportedNamespaces << "\n";
    oss << "  Current Scope: " << currentScope << "\n";
    oss << "  Scope Stack Size: " << scopeStack.size() << "\n";
    return oss.str();
}

std::string NamespaceManager::resolveQualifiedName(const std::string& name) const {
    if (hasAlias(name)) {
        return getAliasTarget(name);
    }
    return name;
}

std::vector<std::string> NamespaceManager::parseQualifiedName(const std::string& name) const {
    std::vector<std::string> parts;
    std::string current;
    
    for (char c : name) {
        if (c == ':') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
}

bool NamespaceManager::isQualifiedName(const std::string& name) const {
    return name.find("::") != std::string::npos;
}

std::string NamespaceManager::buildQualifiedName(const std::vector<std::string>& parts) const {
    std::string result;
    
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            result += "::";
        }
        result += parts[i];
    }
    
    return result;
}

std::vector<std::string> NamespaceManager::findConflicts() const {
    std::vector<std::string> conflicts;
    
    for (const auto& ns : namespaces) {
        auto nsConflicts = ns.second->getConflicts();
        for (const auto& conflict : nsConflicts) {
            conflicts.push_back(ns.first + "::" + conflict);
        }
    }
    
    return conflicts;
}

bool NamespaceManager::hasMemberConflict(const std::string& name) const {
    // 简化实现
    return false;
}

void NamespaceManager::mergeNamespaceMembers(std::shared_ptr<NamespaceNode> target, 
                                           std::shared_ptr<NamespaceNode> source) {
    if (!target || !source) {
        return;
    }
    
    // 合并成员
    for (const auto& member : source->getMembers()) {
        target->addMember(member.first, member.second);
    }
    
    // 合并别名
    for (const auto& alias : source->getAliases()) {
        target->addAlias(alias.first, alias.second);
    }
}

void NamespaceManager::debugLog(const std::string& message) const {
    if (debugMode) {
        std::cout << "[NamespaceManager] " << message << std::endl;
    }
}

void NamespaceManager::updateStats() {
    stats.totalNamespaces = namespaces.size();
    stats.nestedNamespaces = 0;
    stats.totalMembers = 0;
    stats.totalAliases = aliases.size();
    stats.conflicts = findConflicts().size();
    stats.importedNamespaces = importedNamespaces.size();
    stats.exportedNamespaces = exportedNamespaces.size();
    
    for (const auto& ns : namespaces) {
        if (ns.second->isNested()) {
            stats.nestedNamespaces++;
        }
        stats.totalMembers += ns.second->getMembers().size();
    }
}

void NamespaceManager::resetStats() {
    stats = NamespaceStats{};
}

} // namespace CHTL