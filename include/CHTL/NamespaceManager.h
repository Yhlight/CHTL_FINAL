#pragma once

#include "CHTL/CHTLNode/NamespaceNode.h"
#include "CHTL/CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * @brief 命名空间管理器类
 * 
 * 负责管理命名空间、符号解析和冲突检测
 */
class NamespaceManager {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit NamespaceManager(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~NamespaceManager();
    
    /**
     * @brief 添加命名空间
     * @param namespaceNode 命名空间节点
     * @return 是否添加成功
     */
    bool addNamespace(std::shared_ptr<NamespaceNode> namespaceNode);
    
    /**
     * @brief 获取命名空间
     * @param name 命名空间名称
     * @return 命名空间节点
     */
    std::shared_ptr<NamespaceNode> getNamespace(const std::string& name);
    
    /**
     * @brief 检查命名空间是否存在
     * @param name 命名空间名称
     * @return 是否存在
     */
    bool hasNamespace(const std::string& name);
    
    /**
     * @brief 解析符号
     * @param symbolName 符号名称
     * @param currentNamespace 当前命名空间
     * @return 解析结果
     */
    std::string resolveSymbol(const std::string& symbolName, const std::string& currentNamespace = "");
    
    /**
     * @brief 检查符号冲突
     * @param symbolName 符号名称
     * @param namespace1 命名空间1
     * @param namespace2 命名空间2
     * @return 是否有冲突
     */
    bool checkSymbolConflict(const std::string& symbolName, const std::string& namespace1, const std::string& namespace2);
    
    /**
     * @brief 合并命名空间
     * @param targetNamespace 目标命名空间
     * @param sourceNamespace 源命名空间
     * @return 是否合并成功
     */
    bool mergeNamespaces(const std::string& targetNamespace, const std::string& sourceNamespace);
    
    /**
     * @brief 获取命名空间层次结构
     * @param rootNamespace 根命名空间
     * @return 层次结构字符串
     */
    std::string getNamespaceHierarchy(const std::string& rootNamespace = "");
    
    /**
     * @brief 获取所有命名空间
     * @return 命名空间列表
     */
    const std::vector<std::shared_ptr<NamespaceNode>>& getAllNamespaces() const;
    
    /**
     * @brief 清除所有命名空间
     */
    void clear();
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 解析完全限定名称
     * @param fullName 完全限定名称
     * @return 解析结果
     */
    std::pair<std::string, std::string> parseFullQualifiedName(const std::string& fullName);
    
    /**
     * @brief 查找符号
     * @param symbolName 符号名称
     * @param namespaceName 命名空间名称
     * @return 符号值
     */
    std::string findSymbol(const std::string& symbolName, const std::string& namespaceName);
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * @brief 添加警告信息
     * @param message 警告信息
     */
    void addWarning(const std::string& message);
    
    /**
     * @brief 构建层次结构字符串
     * @param node 命名空间节点
     * @param depth 深度
     * @return 层次结构字符串
     */
    std::string buildHierarchyString(std::shared_ptr<NamespaceNode> node, int depth);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<std::shared_ptr<NamespaceNode>> m_namespaces;
    std::unordered_map<std::string, std::shared_ptr<NamespaceNode>> m_namespaceMap;
    std::unordered_set<std::string> m_processedNamespaces;
    bool m_debugMode;
    
    // 统计信息
    size_t m_totalNamespaces;
    size_t m_totalSymbols;
    size_t m_conflictCount;
};

} // namespace CHTL