#ifndef CHTL_IMPORT_PROCESSOR_H
#define CHTL_IMPORT_PROCESSOR_H

#include "../CHTLNode/ImportNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <set>

namespace CHTL {

class CHTLImportProcessor {
private:
    std::map<std::string, std::shared_ptr<ImportNode>> importNodes;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 导入缓存
    std::map<std::string, std::string> importCache;
    std::map<std::string, std::vector<std::shared_ptr<BaseNode>>> nodeCache;
    
    // 路径搜索
    std::vector<std::string> searchPaths;
    std::set<std::string> processedPaths;

    void addError(const std::string& error);
    void addWarning(const std::string& warning);

    // 辅助方法
    std::shared_ptr<ImportNode> getImportNode(const std::string& path) const;
    void mergeImportProperties(std::shared_ptr<ImportNode> target, std::shared_ptr<ImportNode> source);

public:
    CHTLImportProcessor();
    ~CHTLImportProcessor() = default;

    void registerImportNode(std::shared_ptr<ImportNode> importNode);
    void unregisterImportNode(const std::string& path);
    bool hasImportNode(const std::string& path) const;

    std::shared_ptr<BaseNode> processImportNode(const std::string& path);
    std::shared_ptr<BaseNode> instantiateImportNode(const std::string& path);

    // 导入处理
    void processAllImports();
    void processImportByType(const std::string& type);
    void processRequiredImports();
    void processOptionalImports();

    // 路径搜索
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    std::vector<std::string> getSearchPaths() const { return searchPaths; }
    void clearSearchPaths();
    
    std::string findImportPath(const std::string& path) const;
    std::vector<std::string> findImportPaths(const std::string& pattern) const;
    std::vector<std::string> findWildcardImports(const std::string& pattern) const;

    // 导入缓存
    void addToCache(const std::string& path, const std::string& content);
    void addToNodeCache(const std::string& path, const std::vector<std::shared_ptr<BaseNode>>& nodes);
    std::string getFromCache(const std::string& path) const;
    std::vector<std::shared_ptr<BaseNode>> getFromNodeCache(const std::string& path) const;
    bool isInCache(const std::string& path) const;
    void clearCache();
    void clearNodeCache();

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

    // 导入验证
    bool validateImportNode(const std::string& path) const;
    bool validateAllImports() const;

    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    void clearMessages();
    void reset();
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_IMPORT_PROCESSOR_H