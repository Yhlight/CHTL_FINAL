#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL编译上下文
 * 管理编译过程中的全局状态和配置
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext();

    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    
    // 命名空间管理
    void addNamespace(const std::string& name, const std::string& content);
    std::string getNamespace(const std::string& name) const;
    
    // 模板管理
    void addTemplate(const std::string& name, const std::string& type, const std::string& content);
    std::string getTemplate(const std::string& name, const std::string& type) const;
    
    // 自定义元素管理
    void addCustomElement(const std::string& name, const std::string& content);
    std::string getCustomElement(const std::string& name) const;
    
    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    
    // 模块管理
    void addModule(const std::string& name, const std::string& path);
    std::string getModulePath(const std::string& name) const;
    
    // 错误处理
    void addError(const std::string& error);
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
    
    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;

private:
    std::unordered_map<std::string, std::string> m_configurations;
    std::unordered_map<std::string, std::string> m_namespaces;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_templates;
    std::unordered_map<std::string, std::string> m_customElements;
    std::unordered_map<std::string, std::string> m_variables;
    std::unordered_map<std::string, std::string> m_modules;
    std::vector<std::string> m_errors;
    bool m_debugMode;
};

} // namespace CHTL