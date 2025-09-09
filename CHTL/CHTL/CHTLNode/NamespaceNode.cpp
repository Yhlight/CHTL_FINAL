#include "NamespaceNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

NamespaceNode::NamespaceNode(const std::string& name) 
    : BaseNode(NodeType::Namespace, "namespace"), namespaceName(name), 
      autoMerge(true), conflictDetection(true) {
}

void NamespaceNode::addChildNamespace(const std::string& child) {
    childNamespaces.push_back(child);
}

void NamespaceNode::removeChildNamespace(const std::string& child) {
    auto it = std::find(childNamespaces.begin(), childNamespaces.end(), child);
    if (it != childNamespaces.end()) {
        childNamespaces.erase(it);
    }
}

void NamespaceNode::addMember(const std::string& name, std::shared_ptr<BaseNode> member) {
    if (member) {
        member->setParent(shared_from_this());
        namespaceMembers[name] = member;
    }
}

void NamespaceNode::removeMember(const std::string& name) {
    auto it = namespaceMembers.find(name);
    if (it != namespaceMembers.end()) {
        it->second->setParent(nullptr);
        namespaceMembers.erase(it);
    }
}

std::shared_ptr<BaseNode> NamespaceNode::getMember(const std::string& name) const {
    auto it = namespaceMembers.find(name);
    if (it != namespaceMembers.end()) {
        return it->second;
    }
    return nullptr;
}

bool NamespaceNode::hasMember(const std::string& name) const {
    return namespaceMembers.find(name) != namespaceMembers.end();
}

void NamespaceNode::addAlias(const std::string& alias, const std::string& target) {
    namespaceAliases[alias] = target;
}

void NamespaceNode::removeAlias(const std::string& alias) {
    namespaceAliases.erase(alias);
}

std::string NamespaceNode::getAliasTarget(const std::string& alias) const {
    auto it = namespaceAliases.find(alias);
    if (it != namespaceAliases.end()) {
        return it->second;
    }
    return "";
}

bool NamespaceNode::hasAlias(const std::string& alias) const {
    return namespaceAliases.find(alias) != namespaceAliases.end();
}

void NamespaceNode::addConflictResolution(const std::string& resolution) {
    conflictResolution.push_back(resolution);
}

std::string NamespaceNode::getFullName() const {
    if (parentNamespace.empty()) {
        return namespaceName;
    }
    return parentNamespace + "::" + namespaceName;
}

std::string NamespaceNode::getQualifiedName(const std::string& memberName) const {
    return getFullName() + "::" + memberName;
}

bool NamespaceNode::isNested() const {
    return !parentNamespace.empty();
}

int NamespaceNode::getDepth() const {
    return calculateDepth();
}

bool NamespaceNode::hasConflict(const std::string& name) const {
    return isMemberConflict(name);
}

std::vector<std::string> NamespaceNode::getConflicts() const {
    return findConflicts();
}

bool NamespaceNode::resolveConflict(const std::string& name, const std::string& resolution) {
    if (!hasConflict(name)) {
        return false;
    }
    
    // 简单的冲突解决：重命名
    if (resolution == "rename") {
        std::string newName = name + "_conflict";
        auto it = namespaceMembers.find(name);
        if (it != namespaceMembers.end()) {
            auto member = it->second;
            namespaceMembers.erase(it);
            namespaceMembers[newName] = member;
            return true;
        }
    }
    
    return false;
}

void NamespaceNode::mergeNamespace(std::shared_ptr<NamespaceNode> other) {
    if (!other) {
        return;
    }
    
    // 合并成员
    for (const auto& member : other->getMembers()) {
        if (hasMember(member.first)) {
            if (conflictDetection) {
                // 检测冲突
                if (hasConflict(member.first)) {
                    // 尝试解决冲突
                    resolveConflict(member.first, "rename");
                }
            }
        } else {
            addMember(member.first, member.second);
        }
    }
    
    // 合并别名
    for (const auto& alias : other->getAliases()) {
        namespaceAliases[alias.first] = alias.second;
    }
    
    // 合并子命名空间
    for (const auto& child : other->getChildNamespaces()) {
        addChildNamespace(child);
    }
}

void NamespaceNode::mergeMembers(const std::map<std::string, std::shared_ptr<BaseNode>>& members) {
    for (const auto& member : members) {
        addMember(member.first, member.second);
    }
}

std::shared_ptr<BaseNode> NamespaceNode::findMember(const std::string& name, bool recursive) const {
    // 在当前命名空间中查找
    auto it = namespaceMembers.find(name);
    if (it != namespaceMembers.end()) {
        return it->second;
    }
    
    // 在别名中查找
    if (hasAlias(name)) {
        std::string target = getAliasTarget(name);
        return findMember(target, recursive);
    }
    
    // 递归查找（这里简化实现）
    if (recursive) {
        for (const auto& child : childNamespaces) {
            // 在实际实现中，这里应该递归查找子命名空间
        }
    }
    
    return nullptr;
}

std::vector<std::string> NamespaceNode::findMembers(const std::string& pattern) const {
    std::vector<std::string> matches;
    std::regex regexPattern(pattern);
    
    for (const auto& member : namespaceMembers) {
        if (std::regex_match(member.first, regexPattern)) {
            matches.push_back(member.first);
        }
    }
    
    return matches;
}

std::shared_ptr<BaseNode> NamespaceNode::clone() const {
    auto cloned = std::make_shared<NamespaceNode>(namespaceName);
    cloned->parentNamespace = parentNamespace;
    cloned->childNamespaces = childNamespaces;
    cloned->namespaceAliases = namespaceAliases;
    cloned->autoMerge = autoMerge;
    cloned->conflictDetection = conflictDetection;
    cloned->conflictResolution = conflictResolution;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    // 克隆成员
    for (const auto& member : namespaceMembers) {
        cloned->addMember(member.first, member.second->clone());
    }
    
    return cloned;
}

std::string NamespaceNode::toString() const {
    std::ostringstream oss;
    oss << "NamespaceNode(\"" << namespaceName << "\")";
    return oss.str();
}

std::string NamespaceNode::toHTML() const {
    // 命名空间节点不直接生成HTML
    return "";
}

std::string NamespaceNode::toCSS() const {
    // 命名空间节点不直接生成CSS
    return "";
}

std::string NamespaceNode::toJS() const {
    // 命名空间节点不直接生成JS
    return "";
}

bool NamespaceNode::isValid() const {
    return BaseNode::isValid() && !namespaceName.empty();
}

std::string NamespaceNode::debugString() const {
    std::ostringstream oss;
    oss << "NamespaceNode: " << namespaceName 
        << " (Members: " << namespaceMembers.size() 
        << ", Aliases: " << namespaceAliases.size() 
        << ", Line: " << line << ", Column: " << column << ")";
    return oss.str();
}

std::string NamespaceNode::parseNamespaceName(const std::string& fullName) {
    size_t lastColon = fullName.find_last_of("::");
    if (lastColon != std::string::npos) {
        return fullName.substr(lastColon + 2);
    }
    return fullName;
}

std::string NamespaceNode::getParentNamespace(const std::string& fullName) {
    size_t lastColon = fullName.find_last_of("::");
    if (lastColon != std::string::npos) {
        return fullName.substr(0, lastColon);
    }
    return "";
}

std::vector<std::string> NamespaceNode::getNamespaceHierarchy(const std::string& fullName) {
    std::vector<std::string> hierarchy;
    std::string current = fullName;
    
    while (!current.empty()) {
        hierarchy.push_back(current);
        current = getParentNamespace(current);
    }
    
    std::reverse(hierarchy.begin(), hierarchy.end());
    return hierarchy;
}

bool NamespaceNode::isValidNamespaceName(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    
    // 检查是否包含有效字符
    for (char c : name) {
        if (!std::isalnum(c) && c != '_' && c != ':') {
            return false;
        }
    }
    
    // 检查是否以::开头或结尾
    if (name.front() == ':' || name.back() == ':') {
        return false;
    }
    
    return true;
}

int NamespaceNode::calculateDepth() const {
    if (parentNamespace.empty()) {
        return 0;
    }
    
    int depth = 1;
    std::string current = parentNamespace;
    
    while (!current.empty()) {
        depth++;
        current = getParentNamespace(current);
    }
    
    return depth;
}

std::vector<std::string> NamespaceNode::findConflicts() const {
    std::vector<std::string> conflicts;
    
    for (const auto& member : namespaceMembers) {
        if (isMemberConflict(member.first)) {
            conflicts.push_back(member.first);
        }
    }
    
    return conflicts;
}

bool NamespaceNode::isMemberConflict(const std::string& name) const {
    // 检查是否与别名冲突
    if (hasAlias(name)) {
        return true;
    }
    
    // 检查是否与子命名空间冲突
    for (const auto& child : childNamespaces) {
        if (child == name) {
            return true;
        }
    }
    
    return false;
}

} // namespace CHTL