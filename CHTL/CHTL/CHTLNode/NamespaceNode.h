#pragma once

#include "BaseNode.h"

namespace CHTL {

class NamespaceNode : public BaseNode {
private:
    std::string namespaceName;
    std::string parentNamespace;
    std::vector<std::string> childNamespaces;
    std::map<std::string, std::shared_ptr<BaseNode>> namespaceMembers;
    std::map<std::string, std::string> namespaceAliases;
    
    // 命名空间配置
    bool autoMerge;
    bool conflictDetection;
    std::vector<std::string> conflictResolution;
    
public:
    NamespaceNode(const std::string& name);
    virtual ~NamespaceNode() = default;
    
    // 命名空间名称
    const std::string& getNamespaceName() const { return namespaceName; }
    void setNamespaceName(const std::string& name) { namespaceName = name; }
    
    // 父命名空间
    const std::string& getParentNamespace() const { return parentNamespace; }
    void setParentNamespace(const std::string& parent) { parentNamespace = parent; }
    
    // 子命名空间
    void addChildNamespace(const std::string& child);
    void removeChildNamespace(const std::string& child);
    const std::vector<std::string>& getChildNamespaces() const { return childNamespaces; }
    
    // 命名空间成员
    void addMember(const std::string& name, std::shared_ptr<BaseNode> member);
    void removeMember(const std::string& name);
    std::shared_ptr<BaseNode> getMember(const std::string& name) const;
    bool hasMember(const std::string& name) const;
    const std::map<std::string, std::shared_ptr<BaseNode>>& getMembers() const { return namespaceMembers; }
    
    // 命名空间别名
    void addAlias(const std::string& alias, const std::string& target);
    void removeAlias(const std::string& alias);
    std::string getAliasTarget(const std::string& alias) const;
    bool hasAlias(const std::string& alias) const;
    const std::map<std::string, std::string>& getAliases() const { return namespaceAliases; }
    
    // 配置选项
    void setAutoMerge(bool enable) { autoMerge = enable; }
    bool isAutoMerge() const { return autoMerge; }
    
    void setConflictDetection(bool enable) { conflictDetection = enable; }
    bool isConflictDetection() const { return conflictDetection; }
    
    void addConflictResolution(const std::string& resolution);
    const std::vector<std::string>& getConflictResolution() const { return conflictResolution; }
    
    // 命名空间操作
    std::string getFullName() const;
    std::string getQualifiedName(const std::string& memberName) const;
    bool isNested() const;
    int getDepth() const;
    
    // 冲突检测
    bool hasConflict(const std::string& name) const;
    std::vector<std::string> getConflicts() const;
    bool resolveConflict(const std::string& name, const std::string& resolution);
    
    // 合并操作
    void mergeNamespace(std::shared_ptr<NamespaceNode> other);
    void mergeMembers(const std::map<std::string, std::shared_ptr<BaseNode>>& members);
    
    // 查找操作
    std::shared_ptr<BaseNode> findMember(const std::string& name, bool recursive = true) const;
    std::vector<std::string> findMembers(const std::string& pattern) const;
    
    // 克隆
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 序列化
    virtual std::string toString() const override;
    virtual std::string toHTML() const override;
    virtual std::string toCSS() const override;
    virtual std::string toJS() const override;
    
    // 验证
    virtual bool isValid() const override;
    
    // 调试
    virtual std::string debugString() const override;
    
    // 静态方法
    static std::string parseNamespaceName(const std::string& fullName);
    static std::string getParentNamespace(const std::string& fullName);
    static std::vector<std::string> getNamespaceHierarchy(const std::string& fullName);
    static bool isValidNamespaceName(const std::string& name);
    
private:
    int calculateDepth() const;
    std::vector<std::string> findConflicts() const;
    bool isMemberConflict(const std::string& name) const;
};

} // namespace CHTL