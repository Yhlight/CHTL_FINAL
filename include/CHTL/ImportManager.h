#pragma once

#include "CHTL/CHTLNode/ImportNode.h"
#include "CHTL/CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * @brief 导入管理器类
 * 
 * 负责管理文件导入、依赖解析和内容合并
 */
class ImportManager {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit ImportManager(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~ImportManager();
    
    /**
     * @brief 添加导入路径
     * @param path 导入路径
     */
    void addImportPath(const std::string& path);
    
    /**
     * @brief 获取导入路径列表
     * @return 导入路径列表
     */
    const std::vector<std::string>& getImportPaths() const;
    
    /**
     * @brief 解析导入节点
     * @param importNode 导入节点
     * @return 是否解析成功
     */
    bool resolveImport(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 解析所有导入
     * @param importNodes 导入节点列表
     * @return 是否解析成功
     */
    bool resolveAllImports(const std::vector<std::shared_ptr<ImportNode>>& importNodes);
    
    /**
     * @brief 获取导入内容
     * @param importNode 导入节点
     * @return 导入内容
     */
    std::string getImportContent(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool fileExists(const std::string& filePath);
    
    /**
     * @brief 查找文件
     * @param fileName 文件名
     * @return 完整文件路径
     */
    std::string findFile(const std::string& fileName);
    
    /**
     * @brief 读取文件内容
     * @param filePath 文件路径
     * @return 文件内容
     */
    std::string readFile(const std::string& filePath);
    
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
     * @brief 清除缓存
     */
    void clearCache();
    
    /**
     * @brief 获取导入统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 解析CHTL文件导入
     * @param importNode 导入节点
     * @return 是否解析成功
     */
    bool resolveCHTLImport(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 解析HTML文件导入
     * @param importNode 导入节点
     * @return 是否解析成功
     */
    bool resolveHTMLImport(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 解析CSS文件导入
     * @param importNode 导入节点
     * @return 是否解析成功
     */
    bool resolveCSSImport(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 解析JavaScript文件导入
     * @param importNode 导入节点
     * @return 是否解析成功
     */
    bool resolveJSImport(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 解析CMOD文件导入
     * @param importNode 导入节点
     * @return 是否解析成功
     */
    bool resolveCMODImport(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 解析CJMOD文件导入
     * @param importNode 导入节点
     * @return 是否解析成功
     */
    bool resolveCJMODImport(std::shared_ptr<ImportNode> importNode);
    
    /**
     * @brief 根据文件扩展名确定导入类型
     * @param filePath 文件路径
     * @return 导入类型
     */
    ImportType determineImportType(const std::string& filePath);
    
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

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<std::string> m_importPaths;
    std::unordered_map<std::string, std::string> m_fileCache;
    std::unordered_set<std::string> m_processedFiles;
    bool m_debugMode;
    
    // 统计信息
    size_t m_totalImports;
    size_t m_successfulImports;
    size_t m_failedImports;
};

} // namespace CHTL