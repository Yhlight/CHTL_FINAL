#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL编译器上下文类
 * 管理编译过程中的全局状态和配置
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;

    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;

    // 命名空间管理
    void addNamespace(const std::string& name, const std::string& content);
    std::string getNamespace(const std::string& name) const;
    bool hasNamespace(const std::string& name) const;

    // 模板管理
    void addTemplate(const std::string& name, const std::string& type, const std::string& content);
    std::string getTemplate(const std::string& name, const std::string& type) const;
    bool hasTemplate(const std::string& name, const std::string& type) const;

    // 自定义管理
    void addCustom(const std::string& name, const std::string& type, const std::string& content);
    std::string getCustom(const std::string& name, const std::string& type) const;
    bool hasCustom(const std::string& name, const std::string& type) const;

    // 变量管理
    void addVariable(const std::string& name, const std::string& value);
    std::string getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;

    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;

    // 清空上下文
    void clear();

private:
    std::unordered_map<std::string, std::string> configurations_;
    std::unordered_map<std::string, std::string> namespaces_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> templates_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> customs_;
    std::unordered_map<std::string, std::string> variables_;
    bool debug_mode_;
};

} // namespace CHTL