#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 命名空间管理器
 * 
 * 管理命名空间定义、嵌套和冲突检测
 */
class NamespaceManager {
public:
    enum class NamespaceType {
        GLOBAL,         // 全局命名空间
        LOCAL,          // 局部命名空间
        NESTED,         // 嵌套命名空间
        MODULE          // 模块命名空间
    };
    
    struct NamespaceDefinition {
        std::string name;
        std::string fullName;
        NamespaceType type;
        std::string parent;
        std::map<std::string, std::string> symbols;
        std::map<std::string, std::shared_ptr<BaseNode>> definitions;
        std::set<std::string> constraints;
        bool isActive;
    };
    
    // 单例模式
    static NamespaceManager& getInstance();
    
    // 创建命名空间
    void createNamespace(const std::string& name, NamespaceType type = NamespaceType::LOCAL);
    
    // 创建嵌套命名空间
    void createNestedNamespace(const std::string& name, const std::string& parent);
    
    // 进入命名空间
    void enterNamespace(const std::string& name);
    
    // 退出当前命名空间
    void exitNamespace();
    
    // 获取当前命名空间
    std::string getCurrentNamespace() const;
    
    // 获取命名空间定义
    std::shared_ptr<NamespaceDefinition> getNamespace(const std::string& name) const;
    
    // 检查命名空间是否存在
    bool hasNamespace(const std::string& name) const;
    
    // 添加符号到命名空间
    void addSymbol(const std::string& name, const std::string& value, 
                  const std::string& namespaceName = "");
    
    // 添加定义到命名空间
    void addDefinition(const std::string& name, std::shared_ptr<BaseNode> definition,
                      const std::string& namespaceName = "");
    
    // 获取符号值
    std::string getSymbol(const std::string& name, const std::string& namespaceName = "") const;
    
    // 获取定义
    std::shared_ptr<BaseNode> getDefinition(const std::string& name, 
                                           const std::string& namespaceName = "") const;
    
    // 检查符号是否存在
    bool hasSymbol(const std::string& name, const std::string& namespaceName = "") const;
    
    // 检查定义是否存在
    bool hasDefinition(const std::string& name, const std::string& namespaceName = "") const;
    
    // 解析符号名称
    std::string resolveSymbolName(const std::string& name) const;
    
    // 解析定义名称
    std::string resolveDefinitionName(const std::string& name) const;
    
    // 检测命名冲突
    bool detectConflict(const std::string& name, const std::string& namespaceName = "") const;
    
    // 解决命名冲突
    std::string resolveConflict(const std::string& name, const std::string& namespaceName = "") const;
    
    // 合并命名空间
    void mergeNamespace(const std::string& source, const std::string& target);
    
    // 获取所有命名空间名称
    std::vector<std::string> getAllNamespaceNames() const;
    
    // 获取命名空间层次结构
    std::vector<std::string> getNamespaceHierarchy(const std::string& name) const;
    
    // 获取命名空间的所有符号
    std::map<std::string, std::string> getNamespaceSymbols(const std::string& name) const;
    
    // 获取命名空间的所有定义
    std::map<std::string, std::shared_ptr<BaseNode>> getNamespaceDefinitions(const std::string& name) const;
    
    // 清除命名空间
    void clearNamespace(const std::string& name);
    
    // 清除所有命名空间
    void clear();
    
    // 移除命名空间
    void removeNamespace(const std::string& name);
    
    // 设置默认命名空间
    void setDefaultNamespace(const std::string& name);
    
    // 获取默认命名空间
    std::string getDefaultNamespace() const;
    
    // 添加约束
    void addConstraint(const std::string& namespaceName, const std::string& constraint);
    
    // 检查约束
    bool checkConstraint(const std::string& namespaceName, const std::string& constraint) const;
    
    // 获取命名空间约束
    std::set<std::string> getNamespaceConstraints(const std::string& name) const;
    
private:
    NamespaceManager() = default;
    ~NamespaceManager() = default;
    NamespaceManager(const NamespaceManager&) = delete;
    NamespaceManager& operator=(const NamespaceManager&) = delete;
    
    std::map<std::string, std::shared_ptr<NamespaceDefinition>> namespaces_;
    std::vector<std::string> namespaceStack_;
    std::string defaultNamespace_;
    
    // 生成完整命名空间名称
    std::string generateFullName(const std::string& name, const std::string& parent = "") const;
    
    // 查找符号
    std::string findSymbol(const std::string& name, const std::string& namespaceName) const;
    
    // 查找定义
    std::shared_ptr<BaseNode> findDefinition(const std::string& name, 
                                            const std::string& namespaceName) const;
    
    // 检查符号冲突
    bool checkSymbolConflict(const std::string& name, const std::string& namespaceName) const;
    
    // 检查定义冲突
    bool checkDefinitionConflict(const std::string& name, const std::string& namespaceName) const;
    
    // 生成冲突解决名称
    std::string generateConflictResolutionName(const std::string& name, 
                                             const std::string& namespaceName) const;
    
    // 验证命名空间名称
    bool validateNamespaceName(const std::string& name) const;
    
    // 验证符号名称
    bool validateSymbolName(const std::string& name) const;
    
    // 验证定义名称
    bool validateDefinitionName(const std::string& name) const;
};

} // namespace CHTL