#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <filesystem>

namespace CHTL {

/**
 * @brief 导入类型枚举
 */
enum class ImportType {
    HTML,           // HTML 文件
    CSS,            // CSS 文件
    JAVASCRIPT,     // JavaScript 文件
    CHTL,           // CHTL 文件
    CJMOD,          // CJMOD 文件
    UNKNOWN         // 未知类型
};

/**
 * @brief 导入模式枚举
 */
enum class ImportMode {
    EXACT,          // 精确导入
    TYPE,           // 类型导入
    WILDCARD        // 通配导入
};

/**
 * @brief 导入项
 */
class ImportItem {
public:
    ImportItem(ImportType type, const std::string& name, const std::string& path, 
               const std::string& alias = "", ImportMode mode = ImportMode::EXACT);
    ~ImportItem() = default;

    ImportType getType() const { return m_type; }
    std::string getName() const { return m_name; }
    std::string getPath() const { return m_path; }
    std::string getAlias() const { return m_alias; }
    ImportMode getMode() const { return m_mode; }
    
    void setAlias(const std::string& alias) { m_alias = alias; }
    void setMode(ImportMode mode) { m_mode = mode; }
    
    // 路径解析
    std::string resolvePath(const std::string& basePath) const;
    bool isResolved() const { return m_resolved; }
    void setResolved(bool resolved) { m_resolved = resolved; }

private:
    ImportType m_type;
    std::string m_name;
    std::string m_path;
    std::string m_alias;
    ImportMode m_mode;
    bool m_resolved;
};

/**
 * @brief 命名空间
 */
class Namespace {
public:
    Namespace(const std::string& name, const std::string& parent = "");
    ~Namespace() = default;

    std::string getName() const { return m_name; }
    std::string getFullName() const;
    std::string getParent() const { return m_parent; }
    
    // 子命名空间管理
    void addChild(std::shared_ptr<Namespace> child);
    std::shared_ptr<Namespace> getChild(const std::string& name) const;
    const std::vector<std::shared_ptr<Namespace>>& getChildren() const;
    
    // 内容管理
    void addTemplate(const std::string& name, std::shared_ptr<BaseNode> template_);
    void addCustom(const std::string& name, std::shared_ptr<BaseNode> custom);
    void addOrigin(const std::string& name, std::shared_ptr<BaseNode> origin);
    
    std::shared_ptr<BaseNode> getTemplate(const std::string& name) const;
    std::shared_ptr<BaseNode> getCustom(const std::string& name) const;
    std::shared_ptr<BaseNode> getOrigin(const std::string& name) const;
    
    // 冲突检测
    bool hasConflict(const std::string& name) const;
    void resolveConflicts();

private:
    std::string m_name;
    std::string m_parent;
    std::vector<std::shared_ptr<Namespace>> m_children;
    
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> m_templates;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> m_customs;
    std::unordered_map<std::string, std::shared_ptr<BaseNode>> m_origins;
};

/**
 * @brief 导入管理器
 */
class ImportManager {
public:
    explicit ImportManager(std::shared_ptr<CHTLContext> context);
    ~ImportManager() = default;

    // 导入管理
    void addImport(std::shared_ptr<ImportItem> importItem);
    void addImport(const std::string& type, const std::string& name, 
                   const std::string& path, const std::string& alias = "");
    
    // 导入解析
    void resolveImports(const std::string& basePath);
    std::vector<std::shared_ptr<ImportItem>> getImports(ImportType type) const;
    std::vector<std::shared_ptr<ImportItem>> getAllImports() const;
    
    // 文件加载
    std::string loadFile(const std::string& path) const;
    bool fileExists(const std::string& path) const;
    
    // 路径解析
    std::string resolvePath(const std::string& path, const std::string& basePath) const;
    std::vector<std::string> findFiles(const std::string& pattern, const std::string& basePath) const;
    
    // 清理
    void clear();
    void removeImport(const std::string& name);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<std::shared_ptr<ImportItem>> m_imports;
    std::unordered_map<ImportType, std::vector<std::shared_ptr<ImportItem>>> m_importsByType;
    
    // 辅助方法
    ImportType parseImportType(const std::string& typeString) const;
    std::string getFileExtension(const std::string& path) const;
    bool isWildcardPattern(const std::string& pattern) const;
    std::vector<std::string> expandWildcard(const std::string& pattern, const std::string& basePath) const;
};

/**
 * @brief 命名空间管理器
 */
class NamespaceManager {
public:
    explicit NamespaceManager(std::shared_ptr<CHTLContext> context);
    ~NamespaceManager() = default;

    // 命名空间管理
    void createNamespace(const std::string& name, const std::string& parent = "");
    std::shared_ptr<Namespace> getNamespace(const std::string& name) const;
    std::shared_ptr<Namespace> getNamespace(const std::string& name, const std::string& parent) const;
    
    // 内容注册
    void registerTemplate(const std::string& namespaceName, const std::string& name, 
                         std::shared_ptr<BaseNode> template_);
    void registerCustom(const std::string& namespaceName, const std::string& name, 
                       std::shared_ptr<BaseNode> custom);
    void registerOrigin(const std::string& namespaceName, const std::string& name, 
                       std::shared_ptr<BaseNode> origin);
    
    // 内容查找
    std::shared_ptr<BaseNode> findTemplate(const std::string& name, 
                                          const std::string& namespaceName = "") const;
    std::shared_ptr<BaseNode> findCustom(const std::string& name, 
                                        const std::string& namespaceName = "") const;
    std::shared_ptr<BaseNode> findOrigin(const std::string& name, 
                                        const std::string& namespaceName = "") const;
    
    // 冲突解决
    void resolveConflicts();
    bool hasConflict(const std::string& name, const std::string& namespaceName = "") const;
    
    // 清理
    void clear();
    void removeNamespace(const std::string& name);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<Namespace>> m_namespaces;
    
    // 辅助方法
    std::string parseNamespaceName(const std::string& fullName) const;
    std::string parseParentName(const std::string& fullName) const;
    void mergeNamespaces(std::shared_ptr<Namespace> target, std::shared_ptr<Namespace> source);
};

/**
 * @brief 导入解析器
 */
class ImportParser {
public:
    explicit ImportParser(std::shared_ptr<CHTLContext> context);
    ~ImportParser() = default;

    // 解析导入
    std::shared_ptr<ImportItem> parseImport(std::shared_ptr<ImportNode> node);
    std::vector<std::shared_ptr<ImportItem>> parseImports(std::shared_ptr<BaseNode> root);
    
    // 解析命名空间
    std::shared_ptr<Namespace> parseNamespace(std::shared_ptr<NamespaceNode> node);
    std::vector<std::shared_ptr<Namespace>> parseNamespaces(std::shared_ptr<BaseNode> root);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    ImportType parseImportType(const std::string& typeString) const;
    ImportMode parseImportMode(const std::string& modeString) const;
    std::string parseImportPath(const std::string& pathString) const;
    std::string parseImportAlias(const std::string& aliasString) const;
};

/**
 * @brief 导入处理器
 */
class ImportProcessor {
public:
    explicit ImportProcessor(std::shared_ptr<CHTLContext> context);
    ~ImportProcessor() = default;

    // 处理导入
    void processImports(std::shared_ptr<BaseNode> root, const std::string& basePath);
    
    // 内容合并
    void mergeImportedContent(std::shared_ptr<BaseNode> root);
    
    // 命名空间处理
    void processNamespaces(std::shared_ptr<BaseNode> root);
    
    // 冲突解决
    void resolveConflicts();

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<ImportManager> m_importManager;
    std::unique_ptr<NamespaceManager> m_namespaceManager;
    std::unique_ptr<ImportParser> m_parser;
    
    // 辅助方法
    void processHtmlImport(std::shared_ptr<ImportItem> importItem);
    void processCssImport(std::shared_ptr<ImportItem> importItem);
    void processJavaScriptImport(std::shared_ptr<ImportItem> importItem);
    void processChtlImport(std::shared_ptr<ImportItem> importItem);
    void processCjmodImport(std::shared_ptr<ImportItem> importItem);
    
    void mergeHtmlContent(const std::string& content, std::shared_ptr<BaseNode> root);
    void mergeCssContent(const std::string& content, std::shared_ptr<BaseNode> root);
    void mergeJavaScriptContent(const std::string& content, std::shared_ptr<BaseNode> root);
    void mergeChtlContent(const std::string& content, std::shared_ptr<BaseNode> root);
};

} // namespace CHTL