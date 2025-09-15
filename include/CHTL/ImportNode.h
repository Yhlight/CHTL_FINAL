#pragma once

#include "CHTL/CHTLNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * 导入类型
 */
enum class ImportType {
    HTML,           // HTML文件导入
    CSS,            // CSS文件导入
    JS,             // JavaScript文件导入
    CHTL,           // CHTL文件导入
    CJMOD,          // CJMOD文件导入
    TEMPLATE,       // 模板导入
    CUSTOM,         // 自定义导入
    ORIGIN,         // 原始嵌入导入
    CONFIGURATION   // 配置导入
};

/**
 * 导入模式
 */
enum class ImportMode {
    EXACT,          // 精确导入
    TYPE,           // 类型导入
    WILDCARD        // 通配导入
};

/**
 * 导入节点
 */
class ImportNode : public CHTLNode {
public:
    ImportNode(ImportType type, const std::string& path);
    
    // 导入类型
    ImportType getImportType() const;
    void setImportType(ImportType type);
    
    // 导入路径
    std::string getImportPath() const;
    void setImportPath(const std::string& path);
    
    // 导入模式
    ImportMode getImportMode() const;
    void setImportMode(ImportMode mode);
    
    // 导入名称
    std::string getImportName() const;
    void setImportName(const std::string& name);
    
    // 导入别名
    std::string getImportAlias() const;
    void setImportAlias(const std::string& alias);
    
    // 命名空间
    std::string getNamespace() const;
    void setNamespace(const std::string& namespaceName);
    
    // 克隆
    std::shared_ptr<CHTLNode> clone() const override;
    
    // 验证
    bool validate() const override;
    
    // 生成HTML
    std::string generateHTML() const override;

private:
    ImportType import_type_;
    std::string import_path_;
    ImportMode import_mode_;
    std::string import_name_;
    std::string import_alias_;
    std::string namespace_;
};

/**
 * 导入管理器
 */
class ImportManager {
public:
    ImportManager();
    ~ImportManager();
    
    // 导入处理
    bool processImport(std::shared_ptr<ImportNode> importNode);
    bool processHTMLImport(const std::string& path, const std::string& alias);
    bool processCSSImport(const std::string& path, const std::string& alias);
    bool processJSImport(const std::string& path, const std::string& alias);
    bool processCHTLImport(const std::string& path, const std::string& alias);
    bool processCJMODImport(const std::string& path, const std::string& alias);
    
    // 模板导入
    bool processTemplateImport(ImportType type, const std::string& name, 
                              const std::string& path, const std::string& alias);
    
    // 通配导入
    bool processWildcardImport(ImportType type, const std::string& path, 
                              const std::string& alias);
    
    // 路径解析
    std::string resolvePath(const std::string& path) const;
    std::vector<std::string> findFiles(const std::string& pattern) const;
    
    // 命名空间管理
    void setCurrentNamespace(const std::string& namespaceName);
    std::string getCurrentNamespace() const;
    void addNamespaceMapping(const std::string& alias, const std::string& namespaceName);
    std::string getNamespaceMapping(const std::string& alias) const;
    
    // 导入状态
    bool isImported(const std::string& path) const;
    void markAsImported(const std::string& path);
    void clearImported();
    
    // 配置
    void setBasePath(const std::string& path);
    std::string getBasePath() const;
    void addSearchPath(const std::string& path);
    std::vector<std::string> getSearchPaths() const;
    
    // 清理
    void clear();

private:
    // 导入状态
    std::unordered_set<std::string> imported_files_;
    std::unordered_map<std::string, std::string> namespace_mappings_;
    
    // 路径配置
    std::string base_path_;
    std::vector<std::string> search_paths_;
    std::string current_namespace_;
    
    // 文件系统操作
    bool fileExists(const std::string& path) const;
    std::string readFile(const std::string& path) const;
    std::vector<std::string> listFiles(const std::string& directory) const;
    std::vector<std::string> findFilesByPattern(const std::string& directory, 
                                                const std::string& pattern) const;
    
    // 路径处理
    std::string normalizePath(const std::string& path) const;
    std::string joinPath(const std::string& base, const std::string& relative) const;
    bool isAbsolutePath(const std::string& path) const;
    std::string getDirectory(const std::string& path) const;
    std::string getFileName(const std::string& path) const;
    std::string getFileExtension(const std::string& path) const;
    
    // 通配符处理
    bool matchesPattern(const std::string& filename, const std::string& pattern) const;
    std::vector<std::string> expandWildcard(const std::string& pattern) const;
};

/**
 * 命名空间管理器
 */
class NamespaceManager {
public:
    NamespaceManager();
    ~NamespaceManager();
    
    // 命名空间管理
    void createNamespace(const std::string& name, const std::string& parent = "");
    void deleteNamespace(const std::string& name);
    bool hasNamespace(const std::string& name) const;
    std::string getFullNamespace(const std::string& name) const;
    
    // 命名空间嵌套
    void addNestedNamespace(const std::string& parent, const std::string& child);
    std::vector<std::string> getNestedNamespaces(const std::string& parent) const;
    
    // 命名空间合并
    void mergeNamespaces(const std::string& name1, const std::string& name2);
    bool hasConflict(const std::string& name1, const std::string& name2) const;
    
    // 默认命名空间
    void setDefaultNamespace(const std::string& name);
    std::string getDefaultNamespace() const;
    
    // 命名空间查询
    std::vector<std::string> getAllNamespaces() const;
    std::string getParentNamespace(const std::string& name) const;
    
    // 清理
    void clear();

private:
    std::unordered_map<std::string, std::string> namespaces_;
    std::unordered_map<std::string, std::vector<std::string>> nested_namespaces_;
    std::string default_namespace_;
    
    // 冲突检测
    bool checkConflict(const std::string& name1, const std::string& name2) const;
    void resolveConflict(const std::string& name1, const std::string& name2);
};

} // namespace CHTL