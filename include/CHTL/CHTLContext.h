#pragma once

#include "CHTLCommon.h"
#include "CHTLNode.h"
#include <memory>
#include <unordered_map>

namespace CHTL {

// 编译上下文
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 配置管理
    const ConfigOptions& getConfig() const { return config_; }
    void setConfig(const ConfigOptions& config) { config_ = config; }
    
    // 根节点管理
    std::shared_ptr<CHTLNode> getRootNode() const { return rootNode_; }
    void setRootNode(std::shared_ptr<CHTLNode> node) { rootNode_ = node; }
    
    // 模板管理
    void addTemplate(const String& name, std::shared_ptr<TemplateNode> template_);
    std::shared_ptr<TemplateNode> getTemplate(const String& name) const;
    bool hasTemplate(const String& name) const;
    void removeTemplate(const String& name);
    const std::unordered_map<String, std::shared_ptr<TemplateNode>>& getTemplates() const { return templates_; }
    
    // 自定义管理
    void addCustom(const String& name, std::shared_ptr<CustomNode> custom);
    std::shared_ptr<CustomNode> getCustom(const String& name) const;
    bool hasCustom(const String& name) const;
    void removeCustom(const String& name);
    const std::unordered_map<String, std::shared_ptr<CustomNode>>& getCustoms() const { return customs_; }
    
    // 变量管理
    void setVariable(const String& name, const String& value);
    String getVariable(const String& name) const;
    bool hasVariable(const String& name) const;
    void removeVariable(const String& name);
    const std::unordered_map<String, String>& getVariables() const { return variables_; }
    
    // 命名空间管理
    void addNamespace(const String& name, std::shared_ptr<NamespaceNode> namespace_);
    std::shared_ptr<NamespaceNode> getNamespace(const String& name) const;
    bool hasNamespace(const String& name) const;
    void removeNamespace(const String& name);
    const std::unordered_map<String, std::shared_ptr<NamespaceNode>>& getNamespaces() const { return namespaces_; }
    
    // 导入管理
    void addImport(const String& path, std::shared_ptr<CHTLNode> import_);
    std::shared_ptr<CHTLNode> getImport(const String& path) const;
    bool hasImport(const String& path) const;
    void removeImport(const String& path);
    const std::unordered_map<String, std::shared_ptr<CHTLNode>>& getImports() const { return imports_; }
    
    // 错误管理
    void addError(const ErrorInfo& error);
    void addError(const String& message, const Position& pos, const String& severity = "error");
    const ErrorList& getErrors() const { return errors_; }
    void clearErrors();
    bool hasErrors() const { return !errors_.empty(); }
    
    // 警告管理
    void addWarning(const String& message, const Position& pos);
    const ErrorList& getWarnings() const { return warnings_; }
    void clearWarnings();
    bool hasWarnings() const { return !warnings_.empty(); }
    
    // 模块管理
    void addModule(const String& name, const ModuleInfo& info);
    const ModuleInfo* getModule(const String& name) const;
    bool hasModule(const String& name) const;
    void removeModule(const String& name);
    const std::unordered_map<String, ModuleInfo>& getModules() const { return modules_; }
    
    // 导出管理
    void addExport(const String& name, const ExportInfo& info);
    const ExportInfo* getExport(const String& name) const;
    bool hasExport(const String& name) const;
    void removeExport(const String& name);
    const std::unordered_map<String, ExportInfo>& getExports() const { return exports_; }
    
    // 文件管理
    void setCurrentFile(const String& file);
    const String& getCurrentFile() const { return currentFile_; }
    
    // 路径解析
    String resolvePath(const String& path) const;
    String resolveModulePath(const String& moduleName) const;
    String resolveImportPath(const String& importPath) const;
    
    // 清理
    void clear();
    void clearTemplates();
    void clearCustoms();
    void clearVariables();
    void clearNamespaces();
    void clearImports();
    void clearModules();
    void clearExports();
    
    // 验证
    bool validate() const;
    bool validateTemplates() const;
    bool validateCustoms() const;
    bool validateNamespaces() const;
    bool validateImports() const;
    
    // 调试
    void printDebugInfo() const;
    void printTemplates() const;
    void printCustoms() const;
    void printVariables() const;
    void printNamespaces() const;
    void printImports() const;
    void printModules() const;
    void printExports() const;

private:
    ConfigOptions config_;
    std::shared_ptr<CHTLNode> rootNode_;
    
    // 存储
    std::unordered_map<String, std::shared_ptr<TemplateNode>> templates_;
    std::unordered_map<String, std::shared_ptr<CustomNode>> customs_;
    std::unordered_map<String, String> variables_;
    std::unordered_map<String, std::shared_ptr<NamespaceNode>> namespaces_;
    std::unordered_map<String, std::shared_ptr<CHTLNode>> imports_;
    std::unordered_map<String, ModuleInfo> modules_;
    std::unordered_map<String, ExportInfo> exports_;
    
    // 错误和警告
    ErrorList errors_;
    ErrorList warnings_;
    
    // 当前文件
    String currentFile_;
    
    // 路径解析
    StringList modulePaths_;
    StringList importPaths_;
    
    // 内部方法
    void initializeDefaultConfig();
    void initializeDefaultPaths();
    StringList splitPath(const String& path) const;
    String joinPath(const StringList& parts) const;
    bool isAbsolutePath(const String& path) const;
    String getDirectory(const String& file) const;
    String getFileName(const String& file) const;
    String getFileExtension(const String& file) const;
};

} // namespace CHTL