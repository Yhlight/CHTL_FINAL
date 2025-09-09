#pragma once

#include "CHTLNode/NamespaceNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

class NamespaceManager {
public:
    NamespaceManager();
    virtual ~NamespaceManager() = default;
    
    // 命名空间管理
    void addNamespace(std::shared_ptr<NamespaceNode> namespaceNode);
    void removeNamespace(const std::string& name);
    std::shared_ptr<NamespaceNode> getNamespace(const std::string& name) const;
    std::vector<std::shared_ptr<NamespaceNode>> getAllNamespaces() const { return namespaces; }
    
    // 命名空间查找
    std::shared_ptr<NamespaceNode> findNamespace(const std::string& name, bool recursive = true) const;
    std::vector<std::string> findNamespaces(const std::string& pattern) const;
    
    // 成员查找
    std::shared_ptr<BaseNode> findMember(const std::string& qualifiedName) const;
    std::vector<std::string> findMembers(const std::string& pattern) const;
    
    // 命名空间操作
    void mergeNamespaces(const std::string& target, const std::string& source);
    void splitNamespace(const std::string& name, const std::vector<std::string>& newNames);
    void renameNamespace(const std::string& oldName, const std::string& newName);
    
    // 冲突检测和解决
    bool hasConflicts() const;
    std::vector<std::string> getConflicts() const;
    bool resolveConflict(const std::string& name, const std::string& resolution);
    void enableConflictDetection(bool enable) { conflictDetection = enable; }
    bool isConflictDetectionEnabled() const { return conflictDetection; }
    
    // 自动合并
    void enableAutoMerge(bool enable) { autoMerge = enable; }
    bool isAutoMergeEnabled() const { return autoMerge; }
    void performAutoMerge();
    
    // 命名空间层次结构
    std::vector<std::string> getNamespaceHierarchy() const;
    std::vector<std::string> getChildNamespaces(const std::string& parent) const;
    std::vector<std::string> getParentNamespaces(const std::string& child) const;
    
    // 别名管理
    void addAlias(const std::string& alias, const std::string& target);
    void removeAlias(const std::string& alias);
    std::string getAliasTarget(const std::string& alias) const;
    bool hasAlias(const std::string& alias) const;
    const std::map<std::string, std::string>& getAliases() const { return aliases; }
    
    // 作用域管理
    void enterScope(const std::string& namespaceName);
    void exitScope();
    std::string getCurrentScope() const { return currentScope; }
    std::vector<std::string> getScopeStack() const { return scopeStack; }
    
    // 导入和导出
    void importNamespace(const std::string& name, const std::string& alias = "");
    void exportNamespace(const std::string& name);
    std::vector<std::string> getImportedNamespaces() const { return importedNamespaces; }
    std::vector<std::string> getExportedNamespaces() const { return exportedNamespaces; }
    
    // 验证
    bool validateNamespace(const std::string& name) const;
    std::vector<std::string> validateAllNamespaces() const;
    
    // 清理
    void clear();
    void clearAliases();
    void clearImports();
    
    // 统计信息
    struct NamespaceStats {
        size_t totalNamespaces;
        size_t nestedNamespaces;
        size_t totalMembers;
        size_t totalAliases;
        size_t conflicts;
        size_t importedNamespaces;
        size_t exportedNamespaces;
    };
    
    NamespaceStats getStats() const { return stats; }
    void resetStats();
    
    // 调试
    void enableDebugMode(bool enable) { debugMode = enable; }
    bool isDebugMode() const { return debugMode; }
    std::string getDebugInfo() const;
    
private:
    std::map<std::string, std::shared_ptr<NamespaceNode>> namespaces;
    std::map<std::string, std::string> aliases;
    std::vector<std::string> scopeStack;
    std::string currentScope;
    std::vector<std::string> importedNamespaces;
    std::vector<std::string> exportedNamespaces;
    bool conflictDetection;
    bool autoMerge;
    bool debugMode;
    NamespaceStats stats;
    
    // 内部方法
    std::string resolveQualifiedName(const std::string& name) const;
    std::vector<std::string> parseQualifiedName(const std::string& name) const;
    bool isQualifiedName(const std::string& name) const;
    std::string buildQualifiedName(const std::vector<std::string>& parts) const;
    
    // 冲突检测
    std::vector<std::string> findConflicts() const;
    bool hasMemberConflict(const std::string& name) const;
    
    // 合并操作
    void mergeNamespaceMembers(std::shared_ptr<NamespaceNode> target, 
                              std::shared_ptr<NamespaceNode> source);
    
    // 调试
    void debugLog(const std::string& message) const;
    void updateStats();
};

} // namespace CHTL