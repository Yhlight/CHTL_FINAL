#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    
    // 模板管理
    void registerTemplate(const std::string& name, const std::string& content);
    std::string getTemplate(const std::string& name) const;
    bool hasTemplate(const std::string& name) const;
    
    // 自定义管理
    void registerCustom(const std::string& name, const std::string& content);
    std::string getCustom(const std::string& name) const;
    bool hasCustom(const std::string& name) const;
    
    // 命名空间管理
    void enterNamespace(const std::string& name);
    void exitNamespace();
    std::string getCurrentNamespace() const;
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const { return errors_; }
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    bool hasErrors() const { return !errors_.empty(); }
    bool hasWarnings() const { return !warnings_.empty(); }
    
    // 清理
    void clear();
    
private:
    std::unordered_map<std::string, std::string> configurations_;
    std::unordered_map<std::string, std::string> templates_;
    std::unordered_map<std::string, std::string> customs_;
    std::vector<std::string> namespaceStack_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
};

} // namespace CHTL

#endif // CHTL_CONTEXT_H