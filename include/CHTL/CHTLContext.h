#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL 编译上下文
 * 
 * 管理编译过程中的全局状态、配置和错误信息
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;

    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;

    // 错误管理
    void addError(const std::string& message, int line = -1, int column = -1);
    void addWarning(const std::string& message, int line = -1, int column = -1);
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    void clearErrors();

    // 调试模式
    void setDebugMode(bool enabled);
    bool isDebugMode() const;

    // 输出管理
    void setOutputPath(const std::string& path);
    std::string getOutputPath() const;

private:
    std::unordered_map<std::string, std::string> m_configurations;
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    bool m_debugMode;
    std::string m_outputPath;
};

} // namespace CHTL