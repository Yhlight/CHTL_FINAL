#ifndef CHTL_NAMESPACE_PROCESSOR_H
#define CHTL_NAMESPACE_PROCESSOR_H

#include "../CHTLNode/NamespaceNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include <stack>

namespace CHTL {

class CHTLNamespaceProcessor {
private:
    std::map<std::string, std::shared_ptr<NamespaceNode>> namespaceNodes;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 作用域管理
    std::stack<std::string> scopeStack;
    std::map<std::string, std::vector<std::string>> scopeHierarchy;
    std::map<std::string, std::set<std::string>> scopeElements;
    
    // 冲突检测
    std::map<std::string, std::vector<std::string>> globalConflicts;
    std::map<std::string, std::string> conflictResolutions;

    void addError(const std::string& error);
    void addWarning(const std::string& warning);

    // 辅助方法
    std::shared_ptr<NamespaceNode> getNamespaceNode(const std::string& name) const;
    void mergeNamespaceProperties(std::shared_ptr<NamespaceNode> target, std::shared_ptr<NamespaceNode> source);

public:
    CHTLNamespaceProcessor();
    ~CHTLNamespaceProcessor() = default;

    void registerNamespaceNode(std::shared_ptr<NamespaceNode> namespaceNode);
    void unregisterNamespaceNode(const std::string& name);
    bool hasNamespaceNode(const std::string& name) const;

    std::shared_ptr<BaseNode> processNamespaceNode(const std::string& name);
    std::shared_ptr<BaseNode> instantiateNamespaceNode(const std::string& name);

    // 命名空间管理
    void processAllNamespaces();
    void processNamespaceByType(const std::string& type);
    void processActiveNamespaces();
    void processInactiveNamespaces();

    // 作用域管理
    void enterScope(const std::string& namespaceName);
    void exitScope();
    std::string getCurrentScope() const;
    std::vector<std::string> getScopeHierarchy() const;
    void clearScopeStack();
    
    void addToScope(const std::string& namespaceName, const std::string& elementName);
    void removeFromScope(const std::string& namespaceName, const std::string& elementName);
    std::set<std::string> getScopeElements(const std::string& namespaceName) const;
    bool isInScope(const std::string& namespaceName, const std::string& elementName) const;

    // 嵌套管理
    void createNestedNamespace(const std::string& parentName, const std::string& childName);
    void removeNestedNamespace(const std::string& parentName, const std::string& childName);
    std::vector<std::string> getNestedNamespaces(const std::string& parentName) const;
    std::string getParentNamespace(const std::string& childName) const;
    bool isNestedNamespace(const std::string& namespaceName) const;

    // 冲突检测
    void detectConflicts();
    void detectNamespaceConflicts(const std::string& namespaceName);
    void detectElementConflicts(const std::string& elementName);
    void detectScopeConflicts();
    
    void resolveConflict(const std::string& conflictKey, const std::string& resolution);
    std::vector<std::string> getConflicts(const std::string& namespaceName) const;
    std::string getConflictResolution(const std::string& conflictKey) const;
    bool hasConflict(const std::string& namespaceName) const;

    // 命名空间查找
    std::shared_ptr<BaseNode> findElement(const std::string& elementName) const;
    std::shared_ptr<BaseNode> findElementInNamespace(const std::string& namespaceName, const std::string& elementName) const;
    std::vector<std::string> findNamespacesContaining(const std::string& elementName) const;
    std::vector<std::string> findElementsInNamespace(const std::string& namespaceName) const;

    // 命名空间验证
    bool validateNamespaceNode(const std::string& name) const;
    bool validateAllNamespaces() const;
    bool validateScopeIntegrity() const;
    bool validateConflictResolutions() const;

    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    void clearMessages();
    void reset();
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_PROCESSOR_H