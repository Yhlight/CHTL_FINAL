#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL编译上下文
 * 管理编译过程中的全局状态和配置
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // 命名空间管理
    void addNamespace(const std::string& name, const std::string& content);
    std::string getNamespace(const std::string& name) const;
    
    // 模板管理
    void addTemplate(const std::string& name, const std::string& type, const std::string& content);
    std::string getTemplate(const std::string& name, const std::string& type) const;
    
    // 自定义管理
    void addCustom(const std::string& name, const std::string& type, const std::string& content);
    std::string getCustom(const std::string& name, const std::string& type) const;
    
    // 变量管理
    void setVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    
    // 引用管理
    void setReference(const std::string& name, const std::string& value);
    std::string getReference(const std::string& name) const;
    
    // 模块管理
    void addModule(const std::string& name, const std::string& path);
    std::string getModule(const std::string& name) const;
    
    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;
    
private:
    std::unordered_map<std::string, std::string> configurations_;
    std::unordered_map<std::string, std::string> namespaces_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> templates_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> customs_;
    std::unordered_map<std::string, std::string> variables_;
    std::unordered_map<std::string, std::string> modules_;
    bool debugMode_;
};

} // namespace CHTL