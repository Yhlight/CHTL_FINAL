#ifndef CHTL_NAMESPACE_NODE_H
#define CHTL_NAMESPACE_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class NamespaceNode : public BaseNode {
private:
    // 命名空间类型
    enum class NamespaceType {
        GLOBAL,     // 全局命名空间
        LOCAL,      // 局部命名空间
        NESTED,     // 嵌套命名空间
        ANONYMOUS   // 匿名命名空间
    };
    
    NamespaceType namespaceType;
    std::string namespaceName;
    std::string namespacePath;
    std::map<std::string, std::string> namespaceAttributes;
    
    // 命名空间内容
    std::vector<std::shared_ptr<BaseNode>> namespaceContent;
    std::map<std::string, std::shared_ptr<BaseNode>> namedElements;
    std::map<std::string, std::string> namespaceExports;
    
    // 命名空间继承
    std::vector<std::string> parentNamespaces;
    std::map<std::string, std::string> inheritedProperties;
    
    // 命名空间约束
    std::vector<std::string> constraints;
    std::map<std::string, std::string> typeConstraints;
    std::map<std::string, std::string> accessConstraints;
    
    // 命名空间元数据
    std::map<std::string, std::string> metadata;
    bool isAbstract;
    bool isFinal;
    int priority;
    int depth;
    
    // 命名空间状态
    bool isActive;
    bool isProcessed;
    std::vector<std::string> namespaceErrors;
    std::vector<std::string> namespaceWarnings;
    
    // 命名空间冲突检测
    std::map<std::string, std::vector<std::string>> conflicts;
    std::map<std::string, std::string> conflictResolutions;

public:
    NamespaceNode(const std::string& name, NamespaceType type = NamespaceType::LOCAL);
    virtual ~NamespaceNode() = default;
    
    // 命名空间类型
    NamespaceType getNamespaceType() const { return namespaceType; }
    void setNamespaceType(NamespaceType type) { namespaceType = type; }
    std::string getNamespaceTypeName() const;
    
    // 命名空间名称
    const std::string& getNamespaceName() const { return namespaceName; }
    void setNamespaceName(const std::string& name) { namespaceName = name; }
    
    // 命名空间路径
    const std::string& getNamespacePath() const { return namespacePath; }
    void setNamespacePath(const std::string& path) { namespacePath = path; }
    
    // 命名空间属性
    void setNamespaceAttribute(const std::string& name, const std::string& value);
    std::string getNamespaceAttribute(const std::string& name) const;
    bool hasNamespaceAttribute(const std::string& name) const;
    void removeNamespaceAttribute(const std::string& name);
    const std::map<std::string, std::string>& getNamespaceAttributes() const { return namespaceAttributes; }
    
    // 命名空间内容
    void addNamespaceContent(std::shared_ptr<BaseNode> content);
    void removeNamespaceContent(std::shared_ptr<BaseNode> content);
    std::vector<std::shared_ptr<BaseNode>>& getNamespaceContent() { return namespaceContent; }
    const std::vector<std::shared_ptr<BaseNode>>& getNamespaceContent() const { return namespaceContent; }
    
    // 命名元素
    void addNamedElement(const std::string& name, std::shared_ptr<BaseNode> element);
    void removeNamedElement(const std::string& name);
    std::shared_ptr<BaseNode> getNamedElement(const std::string& name) const;
    bool hasNamedElement(const std::string& name) const;
    const std::map<std::string, std::shared_ptr<BaseNode>>& getNamedElements() const { return namedElements; }
    
    // 命名空间导出
    void setNamespaceExport(const std::string& name, const std::string& value);
    std::string getNamespaceExport(const std::string& name) const;
    bool hasNamespaceExport(const std::string& name) const;
    void removeNamespaceExport(const std::string& name);
    const std::map<std::string, std::string>& getNamespaceExports() const { return namespaceExports; }
    
    // 命名空间继承
    void addParentNamespace(const std::string& parentName);
    void removeParentNamespace(const std::string& parentName);
    const std::vector<std::string>& getParentNamespaces() const { return parentNamespaces; }
    bool hasParentNamespace(const std::string& parentName) const;
    
    void setInheritedProperty(const std::string& name, const std::string& value);
    std::string getInheritedProperty(const std::string& name) const;
    bool hasInheritedProperty(const std::string& name) const;
    void removeInheritedProperty(const std::string& name);
    const std::map<std::string, std::string>& getInheritedProperties() const { return inheritedProperties; }
    
    // 命名空间约束
    void addConstraint(const std::string& constraint);
    void removeConstraint(const std::string& constraint);
    const std::vector<std::string>& getConstraints() const { return constraints; }
    bool hasConstraint(const std::string& constraint) const;
    
    void setTypeConstraint(const std::string& type, const std::string& constraint);
    std::string getTypeConstraint(const std::string& type) const;
    bool hasTypeConstraint(const std::string& type) const;
    void removeTypeConstraint(const std::string& type);
    const std::map<std::string, std::string>& getTypeConstraints() const { return typeConstraints; }
    
    void setAccessConstraint(const std::string& element, const std::string& constraint);
    std::string getAccessConstraint(const std::string& element) const;
    bool hasAccessConstraint(const std::string& element) const;
    void removeAccessConstraint(const std::string& element);
    const std::map<std::string, std::string>& getAccessConstraints() const { return accessConstraints; }
    
    // 命名空间元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 命名空间特性
    bool isAbstractNamespace() const { return isAbstract; }
    void setAbstract(bool abstract) { isAbstract = abstract; }
    
    bool isFinalNamespace() const { return isFinal; }
    void setFinal(bool final) { isFinal = final; }
    
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    int getDepth() const { return depth; }
    void setDepth(int d) { depth = d; }
    
    // 命名空间状态
    bool isActiveNamespace() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    bool isProcessedNamespace() const { return isProcessed; }
    void setProcessed(bool processed) { isProcessed = processed; }
    
    // 命名空间错误和警告
    const std::vector<std::string>& getNamespaceErrors() const { return namespaceErrors; }
    const std::vector<std::string>& getNamespaceWarnings() const { return namespaceWarnings; }
    void addNamespaceError(const std::string& error);
    void addNamespaceWarning(const std::string& warning);
    void clearNamespaceMessages();
    
    // 命名空间冲突检测
    void detectConflicts();
    void resolveConflicts();
    void addConflict(const std::string& element, const std::string& conflict);
    void removeConflict(const std::string& element);
    const std::map<std::string, std::vector<std::string>>& getConflicts() const { return conflicts; }
    bool hasConflict(const std::string& element) const;
    
    void setConflictResolution(const std::string& element, const std::string& resolution);
    std::string getConflictResolution(const std::string& element) const;
    bool hasConflictResolution(const std::string& element) const;
    void removeConflictResolution(const std::string& element);
    const std::map<std::string, std::string>& getConflictResolutions() const { return conflictResolutions; }
    
    // 命名空间处理
    void processNamespace();
    void processInheritance();
    void processConstraints();
    void processConflicts();
    
    // 命名空间验证
    bool validateNamespace() const;
    bool validateInheritance() const;
    bool validateConstraints() const;
    bool validateConflicts() const;
    bool validateAccess() const;
    
    // 命名空间转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toNamespaceString() const;
    
    // 命名空间格式化
    std::string formatNamespace() const;
    std::string minifyNamespace() const;
    std::string beautifyNamespace() const;
    
    // 命名空间压缩
    std::string compressNamespace() const;
    std::string decompressNamespace() const;
    
    // 命名空间编码
    std::string encodeNamespace() const;
    std::string decodeNamespace() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const NamespaceNode& other) const;
    bool operator!=(const NamespaceNode& other) const;

private:
    // 辅助方法
    std::string processNamespacePath(const std::string& path) const;
    std::string validateNamespacePath(const std::string& path) const;
    std::string sanitizeNamespacePath(const std::string& path) const;
    
    // 路径处理
    std::string resolveNamespacePath(const std::string& path) const;
    std::string normalizeNamespacePath(const std::string& path) const;
    std::string expandNamespacePath(const std::string& path) const;
    
    // 继承处理
    void mergeParentProperties();
    void resolveInheritanceConflicts();
    
    // 约束处理
    bool checkTypeConstraint(const std::string& type, const std::string& value) const;
    bool checkGeneralConstraint(const std::string& constraint) const;
    bool checkAccessConstraint(const std::string& element, const std::string& access) const;
    void detectElementConflicts();
    void detectPropertyConflicts();
    void detectExportConflicts();
    void resolveElementConflicts();
    void resolvePropertyConflicts();
    void resolveExportConflicts();
    
    // 错误处理
    void handleNamespaceError(const std::string& error);
    void handleNamespaceWarning(const std::string& warning);
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_NODE_H