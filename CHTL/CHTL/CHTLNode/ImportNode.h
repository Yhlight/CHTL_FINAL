#ifndef CHTL_IMPORT_NODE_H
#define CHTL_IMPORT_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class ImportNode : public BaseNode {
private:
    // 导入类型
    enum class ImportType {
        HTML,       // HTML文件导入
        CSS,        // CSS文件导入
        JAVASCRIPT, // JavaScript文件导入
        CHTL,       // CHTL文件导入
        MIXED       // 混合文件导入
    };
    
    ImportType importType;
    std::string importPath;
    std::string importAlias;
    std::map<std::string, std::string> importOptions;
    
    // 导入模式
    enum class ImportMode {
        EXACT,      // 精确导入
        TYPE,       // 类型导入
        WILDCARD    // 通配导入
    };
    
    ImportMode importMode;
    std::vector<std::string> importTargets;
    std::vector<std::string> excludeTargets;
    
    // 导入内容
    std::string importedContent;
    std::map<std::string, std::string> importedExports;
    std::vector<std::shared_ptr<BaseNode>> importedNodes;
    
    // 导入状态
    bool isImported;
    bool isProcessed;
    std::vector<std::string> importErrors;
    std::vector<std::string> importWarnings;
    
    // 导入元数据
    std::map<std::string, std::string> metadata;
    int priority;
    bool isRequired;
    bool isOptional;

public:
    ImportNode(const std::string& path, ImportType type = ImportType::CHTL);
    virtual ~ImportNode() = default;
    
    // 导入类型
    ImportType getImportType() const { return importType; }
    void setImportType(ImportType type) { importType = type; }
    std::string getImportTypeName() const;
    
    // 导入路径
    const std::string& getImportPath() const { return importPath; }
    void setImportPath(const std::string& path) { importPath = path; }
    
    // 导入别名
    const std::string& getImportAlias() const { return importAlias; }
    void setImportAlias(const std::string& alias) { importAlias = alias; }
    
    // 导入选项
    void setImportOption(const std::string& key, const std::string& value);
    std::string getImportOption(const std::string& key) const;
    bool hasImportOption(const std::string& key) const;
    void removeImportOption(const std::string& key);
    const std::map<std::string, std::string>& getImportOptions() const { return importOptions; }
    
    // 导入模式
    ImportMode getImportMode() const { return importMode; }
    void setImportMode(ImportMode mode) { importMode = mode; }
    std::string getImportModeName() const;
    
    // 导入目标
    void addImportTarget(const std::string& target);
    void removeImportTarget(const std::string& target);
    const std::vector<std::string>& getImportTargets() const { return importTargets; }
    bool hasImportTarget(const std::string& target) const;
    
    void addExcludeTarget(const std::string& target);
    void removeExcludeTarget(const std::string& target);
    const std::vector<std::string>& getExcludeTargets() const { return excludeTargets; }
    bool hasExcludeTarget(const std::string& target) const;
    
    // 导入内容
    const std::string& getImportedContent() const { return importedContent; }
    void setImportedContent(const std::string& content) { importedContent = content; }
    
    void setImportedExport(const std::string& name, const std::string& value);
    std::string getImportedExport(const std::string& name) const;
    bool hasImportedExport(const std::string& name) const;
    void removeImportedExport(const std::string& name);
    const std::map<std::string, std::string>& getImportedExports() const { return importedExports; }
    
    void addImportedNode(std::shared_ptr<BaseNode> node);
    void removeImportedNode(std::shared_ptr<BaseNode> node);
    std::vector<std::shared_ptr<BaseNode>>& getImportedNodes() { return importedNodes; }
    const std::vector<std::shared_ptr<BaseNode>>& getImportedNodes() const { return importedNodes; }
    
    // 导入状态
    bool isImportedSuccessfully() const { return isImported; }
    void setImported(bool imported) { isImported = imported; }
    
    bool isProcessedSuccessfully() const { return isProcessed; }
    void setProcessed(bool processed) { isProcessed = processed; }
    
    // 导入错误和警告
    const std::vector<std::string>& getImportErrors() const { return importErrors; }
    const std::vector<std::string>& getImportWarnings() const { return importWarnings; }
    void addImportError(const std::string& error);
    void addImportWarning(const std::string& warning);
    void clearImportMessages();
    
    // 导入元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 导入特性
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    bool isRequiredImport() const { return isRequired; }
    void setRequired(bool required) { isRequired = required; }
    
    bool isOptionalImport() const { return isOptional; }
    void setOptional(bool optional) { isOptional = optional; }
    
    // 导入处理
    bool processImport();
    bool processHtmlImport();
    bool processCssImport();
    bool processJavaScriptImport();
    bool processChtlImport();
    bool processMixedImport();
    
    // 导入验证
    bool validateImport() const;
    bool validatePath() const;
    bool validateType() const;
    bool validateMode() const;
    bool validateTargets() const;
    
    // 导入解析
    std::string parseImportPath() const;
    std::string resolveImportPath() const;
    std::vector<std::string> parseImportTargets() const;
    std::vector<std::string> parseExcludeTargets() const;
    
    // 导入转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toImportString() const;
    
    // 导入格式化
    std::string formatImport() const;
    std::string minifyImport() const;
    std::string beautifyImport() const;
    
    // 导入压缩
    std::string compressImport() const;
    std::string decompressImport() const;
    
    // 导入编码
    std::string encodeImport() const;
    std::string decodeImport() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const ImportNode& other) const;
    bool operator!=(const ImportNode& other) const;

private:
    // 辅助方法
    std::string processImportPath(const std::string& path) const;
    std::string validateImportPath(const std::string& path) const;
    std::string sanitizeImportPath(const std::string& path) const;
    
    // 路径处理
    std::string resolvePath(const std::string& path) const;
    std::string normalizePath(const std::string& path) const;
    std::string expandPath(const std::string& path) const;
    
    // 内容处理
    std::string processHtmlContent(const std::string& content) const;
    std::string processCssContent(const std::string& content) const;
    std::string processJavaScriptContent(const std::string& content) const;
    std::string processChtlContent(const std::string& content) const;
    std::string processMixedContent(const std::string& content) const;
    
    // 内容验证
    bool validateHtmlContent(const std::string& content) const;
    bool validateCssContent(const std::string& content) const;
    bool validateJavaScriptContent(const std::string& content) const;
    bool validateChtlContent(const std::string& content) const;
    bool validateMixedContent(const std::string& content) const;
    
    // 内容清理
    std::string sanitizeHtmlContent(const std::string& content) const;
    std::string sanitizeCssContent(const std::string& content) const;
    std::string sanitizeJavaScriptContent(const std::string& content) const;
    std::string sanitizeChtlContent(const std::string& content) const;
    std::string sanitizeMixedContent(const std::string& content) const;
    
    // 内容格式化
    std::string formatHtmlContent(const std::string& content) const;
    std::string formatCssContent(const std::string& content) const;
    std::string formatJavaScriptContent(const std::string& content) const;
    std::string formatChtlContent(const std::string& content) const;
    std::string formatMixedContent(const std::string& content) const;
    
    // 内容压缩
    std::string compressHtmlContent(const std::string& content) const;
    std::string compressCssContent(const std::string& content) const;
    std::string compressJavaScriptContent(const std::string& content) const;
    std::string compressChtlContent(const std::string& content) const;
    std::string compressMixedContent(const std::string& content) const;
    
    // 内容编码
    std::string encodeHtmlContent(const std::string& content) const;
    std::string encodeCssContent(const std::string& content) const;
    std::string encodeJavaScriptContent(const std::string& content) const;
    std::string encodeChtlContent(const std::string& content) const;
    std::string encodeMixedContent(const std::string& content) const;
    
    // 错误处理
    void handleImportError(const std::string& error);
    void handleImportWarning(const std::string& warning);
};

} // namespace CHTL

#endif // CHTL_IMPORT_NODE_H