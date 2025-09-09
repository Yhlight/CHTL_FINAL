#include "NamespaceManager.hpp"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

NamespaceManager& NamespaceManager::getInstance() {
    static NamespaceManager instance;
    return instance;
}

void NamespaceManager::createNamespace(const std::string& name, NamespaceType type) {
    if (!validateNamespaceName(name)) {
        return;
    }
    
    auto namespaceDef = std::make_shared<NamespaceDefinition>();
    namespaceDef->name = name;
    namespaceDef->fullName = generateFullName(name);
    namespaceDef->type = type;
    namespaceDef->parent = "";
    namespaceDef->isActive = true;
    
    namespaces_[name] = namespaceDef;
}

void NamespaceManager::createNestedNamespace(const std::string& name, const std::string& parent) {
    if (!validateNamespaceName(name) || !hasNamespace(parent)) {
        return;
    }
    
    auto namespaceDef = std::make_shared<NamespaceDefinition>();
    namespaceDef->name = name;
    namespaceDef->fullName = generateFullName(name, parent);
    namespaceDef->type = NamespaceType::NESTED;
    namespaceDef->parent = parent;
    namespaceDef->isActive = true;
    
    namespaces_[namespaceDef->fullName] = namespaceDef;
}

void NamespaceManager::enterNamespace(const std::string& name) {
    if (hasNamespace(name)) {
        namespaceStack_.push_back(name);
    }
}

void NamespaceManager::exitNamespace() {
    if (!namespaceStack_.empty()) {
        namespaceStack_.pop_back();
    }
}

std::string NamespaceManager::getCurrentNamespace() const {
    if (namespaceStack_.empty()) {
        return defaultNamespace_;
    }
    return namespaceStack_.back();
}

std::shared_ptr<NamespaceManager::NamespaceDefinition> NamespaceManager::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : nullptr;
}

bool NamespaceManager::hasNamespace(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

void NamespaceManager::addSymbol(const std::string& name, const std::string& value, 
                                const std::string& namespaceName) {
    if (!validateSymbolName(name)) {
        return;
    }
    
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    auto namespaceDef = getNamespace(targetNamespace);
    
    if (namespaceDef) {
        namespaceDef->symbols[name] = value;
    }
}

void NamespaceManager::addDefinition(const std::string& name, std::shared_ptr<BaseNode> definition,
                                    const std::string& namespaceName) {
    if (!validateDefinitionName(name)) {
        return;
    }
    
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    auto namespaceDef = getNamespace(targetNamespace);
    
    if (namespaceDef) {
        namespaceDef->definitions[name] = definition;
    }
}

std::string NamespaceManager::getSymbol(const std::string& name, const std::string& namespaceName) const {
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    return findSymbol(name, targetNamespace);
}

std::shared_ptr<BaseNode> NamespaceManager::getDefinition(const std::string& name, 
                                                         const std::string& namespaceName) const {
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    return findDefinition(name, targetNamespace);
}

bool NamespaceManager::hasSymbol(const std::string& name, const std::string& namespaceName) const {
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    return !findSymbol(name, targetNamespace).empty();
}

bool NamespaceManager::hasDefinition(const std::string& name, const std::string& namespaceName) const {
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    return findDefinition(name, targetNamespace) != nullptr;
}

std::string NamespaceManager::resolveSymbolName(const std::string& name) const {
    // 首先在当前命名空间中查找
    std::string currentNamespace = getCurrentNamespace();
    std::string result = findSymbol(name, currentNamespace);
    
    if (!result.empty()) {
        return result;
    }
    
    // 在全局命名空间中查找
    result = findSymbol(name, "global");
    if (!result.empty()) {
        return result;
    }
    
    // 在父命名空间中查找
    auto namespaceDef = getNamespace(currentNamespace);
    if (namespaceDef && !namespaceDef->parent.empty()) {
        result = findSymbol(name, namespaceDef->parent);
        if (!result.empty()) {
            return result;
        }
    }
    
    return "";
}

std::string NamespaceManager::resolveDefinitionName(const std::string& name) const {
    // 首先在当前命名空间中查找
    std::string currentNamespace = getCurrentNamespace();
    auto result = findDefinition(name, currentNamespace);
    
    if (result) {
        return name;
    }
    
    // 在全局命名空间中查找
    result = findDefinition(name, "global");
    if (result) {
        return name;
    }
    
    // 在父命名空间中查找
    auto namespaceDef = getNamespace(currentNamespace);
    if (namespaceDef && !namespaceDef->parent.empty()) {
        result = findDefinition(name, namespaceDef->parent);
        if (result) {
            return name;
        }
    }
    
    return "";
}

bool NamespaceManager::detectConflict(const std::string& name, const std::string& namespaceName) const {
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    return checkSymbolConflict(name, targetNamespace) || checkDefinitionConflict(name, targetNamespace);
}

std::string NamespaceManager::resolveConflict(const std::string& name, const std::string& namespaceName) const {
    if (!detectConflict(name, namespaceName)) {
        return name;
    }
    
    std::string targetNamespace = namespaceName.empty() ? getCurrentNamespace() : namespaceName;
    return generateConflictResolutionName(name, targetNamespace);
}

void NamespaceManager::mergeNamespace(const std::string& source, const std::string& target) {
    auto sourceNamespace = getNamespace(source);
    auto targetNamespace = getNamespace(target);
    
    if (!sourceNamespace || !targetNamespace) {
        return;
    }
    
    // 合并符号
    for (const auto& [name, value] : sourceNamespace->symbols) {
        if (!targetNamespace->symbols.count(name)) {
            targetNamespace->symbols[name] = value;
        }
    }
    
    // 合并定义
    for (const auto& [name, definition] : sourceNamespace->definitions) {
        if (!targetNamespace->definitions.count(name)) {
            targetNamespace->definitions[name] = definition;
        }
    }
    
    // 合并约束
    for (const auto& constraint : sourceNamespace->constraints) {
        targetNamespace->constraints.insert(constraint);
    }
}

std::vector<std::string> NamespaceManager::getAllNamespaceNames() const {
    std::vector<std::string> names;
    for (const auto& [name, namespaceDef] : namespaces_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> NamespaceManager::getNamespaceHierarchy(const std::string& name) const {
    std::vector<std::string> hierarchy;
    auto namespaceDef = getNamespace(name);
    
    if (namespaceDef) {
        hierarchy.push_back(name);
        std::string parent = namespaceDef->parent;
        
        while (!parent.empty()) {
            hierarchy.insert(hierarchy.begin(), parent);
            auto parentDef = getNamespace(parent);
            parent = parentDef ? parentDef->parent : "";
        }
    }
    
    return hierarchy;
}

std::map<std::string, std::string> NamespaceManager::getNamespaceSymbols(const std::string& name) const {
    auto namespaceDef = getNamespace(name);
    return namespaceDef ? namespaceDef->symbols : std::map<std::string, std::string>();
}

std::map<std::string, std::shared_ptr<BaseNode>> NamespaceManager::getNamespaceDefinitions(const std::string& name) const {
    auto namespaceDef = getNamespace(name);
    return namespaceDef ? namespaceDef->definitions : std::map<std::string, std::shared_ptr<BaseNode>>();
}

void NamespaceManager::clearNamespace(const std::string& name) {
    auto namespaceDef = getNamespace(name);
    if (namespaceDef) {
        namespaceDef->symbols.clear();
        namespaceDef->definitions.clear();
        namespaceDef->constraints.clear();
    }
}

void NamespaceManager::clear() {
    namespaces_.clear();
    namespaceStack_.clear();
    defaultNamespace_ = "";
}

void NamespaceManager::removeNamespace(const std::string& name) {
    namespaces_.erase(name);
    
    // 从命名空间栈中移除
    auto it = std::find(namespaceStack_.begin(), namespaceStack_.end(), name);
    if (it != namespaceStack_.end()) {
        namespaceStack_.erase(it);
    }
}

void NamespaceManager::setDefaultNamespace(const std::string& name) {
    if (hasNamespace(name)) {
        defaultNamespace_ = name;
    }
}

std::string NamespaceManager::getDefaultNamespace() const {
    return defaultNamespace_;
}

void NamespaceManager::addConstraint(const std::string& namespaceName, const std::string& constraint) {
    auto namespaceDef = getNamespace(namespaceName);
    if (namespaceDef) {
        namespaceDef->constraints.insert(constraint);
    }
}

bool NamespaceManager::checkConstraint(const std::string& namespaceName, const std::string& constraint) const {
    auto namespaceDef = getNamespace(namespaceName);
    return namespaceDef && namespaceDef->constraints.count(constraint) > 0;
}

std::set<std::string> NamespaceManager::getNamespaceConstraints(const std::string& name) const {
    auto namespaceDef = getNamespace(name);
    return namespaceDef ? namespaceDef->constraints : std::set<std::string>();
}

std::string NamespaceManager::generateFullName(const std::string& name, const std::string& parent) const {
    if (parent.empty()) {
        return name;
    }
    
    return parent + "::" + name;
}

std::string NamespaceManager::findSymbol(const std::string& name, const std::string& namespaceName) const {
    auto namespaceDef = getNamespace(namespaceName);
    if (namespaceDef) {
        auto it = namespaceDef->symbols.find(name);
        if (it != namespaceDef->symbols.end()) {
            return it->second;
        }
    }
    
    return "";
}

std::shared_ptr<BaseNode> NamespaceManager::findDefinition(const std::string& name, 
                                                          const std::string& namespaceName) const {
    auto namespaceDef = getNamespace(namespaceName);
    if (namespaceDef) {
        auto it = namespaceDef->definitions.find(name);
        if (it != namespaceDef->definitions.end()) {
            return it->second;
        }
    }
    
    return nullptr;
}

bool NamespaceManager::checkSymbolConflict(const std::string& name, const std::string& namespaceName) const {
    auto namespaceDef = getNamespace(namespaceName);
    if (namespaceDef) {
        return namespaceDef->symbols.count(name) > 0;
    }
    
    return false;
}

bool NamespaceManager::checkDefinitionConflict(const std::string& name, const std::string& namespaceName) const {
    auto namespaceDef = getNamespace(namespaceName);
    if (namespaceDef) {
        return namespaceDef->definitions.count(name) > 0;
    }
    
    return false;
}

std::string NamespaceManager::generateConflictResolutionName(const std::string& name, 
                                                           const std::string& namespaceName) const {
    std::string resolvedName = name;
    int counter = 1;
    
    while (detectConflict(resolvedName, namespaceName)) {
        resolvedName = name + "_" + std::to_string(counter);
        counter++;
    }
    
    return resolvedName;
}

bool NamespaceManager::validateNamespaceName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查命名空间名称格式
    std::regex namespaceRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(name, namespaceRegex);
}

bool NamespaceManager::validateSymbolName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查符号名称格式
    std::regex symbolRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(name, symbolRegex);
}

bool NamespaceManager::validateDefinitionName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    // 检查定义名称格式
    std::regex definitionRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(name, definitionRegex);
}

} // namespace CHTL