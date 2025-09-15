#ifndef CHTLJSCONTEXT_H
#define CHTLJSCONTEXT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS上下文类
 * 管理CHTL JS编译过程中的全局状态和配置
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();

    // 配置管理
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    
    void setOutputPath(const std::string& path);
    const std::string& getOutputPath() const;
    
    // 模块管理
    void addModule(const std::string& name, const std::string& content);
    std::string getModule(const std::string& name) const;
    bool hasModule(const std::string& name) const;
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    
    // 命名空间管理
    void setCurrentNamespace(const std::string& namespace_name);
    const std::string& getCurrentNamespace() const;
    
    // 错误处理
    void addError(const std::string& error);
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
    void clearErrors();
    
    // 警告处理
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getWarnings() const;
    bool hasWarnings() const;
    void clearWarnings();

private:
    bool debug_mode_;
    std::string output_path_;
    std::string current_namespace_;
    
    std::map<std::string, std::string> modules_;
    std::map<std::string, std::string> variables_;
    
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

} // namespace CHTLJS

#endif // CHTLJSCONTEXT_H