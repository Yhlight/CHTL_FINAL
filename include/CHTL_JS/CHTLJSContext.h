#pragma once

#include "CHTLJSCommon.h"
#include "CHTLJSNode.h"
#include <memory>
#include <unordered_map>

namespace CHTL_JS {

// 编译上下文
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext() = default;
    
    // 根节点管理
    std::shared_ptr<CHTLJSNode> getRootNode() const { return rootNode_; }
    void setRootNode(std::shared_ptr<CHTLJSNode> node) { rootNode_ = node; }
    
    // 函数管理
    void addFunction(const String& name, std::shared_ptr<FunctionNode> func);
    std::shared_ptr<FunctionNode> getFunction(const String& name) const;
    bool hasFunction(const String& name) const;
    void removeFunction(const String& name);
    const std::unordered_map<String, std::shared_ptr<FunctionNode>>& getFunctions() const { return functions_; }
    
    // 变量管理
    void setVariable(const String& name, const String& value);
    String getVariable(const String& name) const;
    bool hasVariable(const String& name) const;
    void removeVariable(const String& name);
    const std::unordered_map<String, String>& getVariables() const { return variables_; }
    
    // 对象管理
    void addObject(const String& name, std::shared_ptr<ObjectNode> obj);
    std::shared_ptr<ObjectNode> getObject(const String& name) const;
    bool hasObject(const String& name) const;
    void removeObject(const String& name);
    const std::unordered_map<String, std::shared_ptr<ObjectNode>>& getObjects() const { return objects_; }
    
    // 数组管理
    void addArray(const String& name, std::shared_ptr<ArrayNode> arr);
    std::shared_ptr<ArrayNode> getArray(const String& name) const;
    bool hasArray(const String& name) const;
    void removeArray(const String& name);
    const std::unordered_map<String, std::shared_ptr<ArrayNode>>& getArrays() const { return arrays_; }
    
    // 虚对象管理
    void addVirtualObject(const String& name, std::shared_ptr<VirtualObjectNode> obj);
    std::shared_ptr<VirtualObjectNode> getVirtualObject(const String& name) const;
    bool hasVirtualObject(const String& name) const;
    void removeVirtualObject(const String& name);
    const std::unordered_map<String, std::shared_ptr<VirtualObjectNode>>& getVirtualObjects() const { return virtualObjects_; }
    
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
    
    // 文件管理
    void setCurrentFile(const String& file);
    const String& getCurrentFile() const { return currentFile_; }
    
    // 路径解析
    String resolvePath(const String& path) const;
    String resolveModulePath(const String& moduleName) const;
    String resolveImportPath(const String& importPath) const;
    
    // 清理
    void clear();
    void clearFunctions();
    void clearVariables();
    void clearObjects();
    void clearArrays();
    void clearVirtualObjects();
    
    // 验证
    bool validate() const;
    bool validateFunctions() const;
    bool validateVariables() const;
    bool validateObjects() const;
    bool validateArrays() const;
    bool validateVirtualObjects() const;
    
    // 调试
    void printDebugInfo() const;
    void printFunctions() const;
    void printVariables() const;
    void printObjects() const;
    void printArrays() const;
    void printVirtualObjects() const;

private:
    std::shared_ptr<CHTLJSNode> rootNode_;
    
    // 存储
    std::unordered_map<String, std::shared_ptr<FunctionNode>> functions_;
    std::unordered_map<String, String> variables_;
    std::unordered_map<String, std::shared_ptr<ObjectNode>> objects_;
    std::unordered_map<String, std::shared_ptr<ArrayNode>> arrays_;
    std::unordered_map<String, std::shared_ptr<VirtualObjectNode>> virtualObjects_;
    
    // 错误和警告
    ErrorList errors_;
    ErrorList warnings_;
    
    // 当前文件
    String currentFile_;
    
    // 路径解析
    StringList modulePaths_;
    StringList importPaths_;
    
    // 内部方法
    void initializeDefaultPaths();
    StringList splitPath(const String& path) const;
    String joinPath(const StringList& parts) const;
    bool isAbsolutePath(const String& path) const;
    String getDirectory(const String& file) const;
    String getFileName(const String& file) const;
    String getFileExtension(const String& file) const;
};

} // namespace CHTL_JS