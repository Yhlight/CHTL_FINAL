#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>

namespace CHTL {

/**
 * @brief CHTL编译器上下文类
 * 
 * 负责管理编译过程中的全局状态、配置和资源
 */
class CHTLContext {
public:
    /**
     * @brief 构造函数
     */
    CHTLContext();
    
    /**
     * @brief 析构函数
     */
    ~CHTLContext();
    
    /**
     * @brief 初始化上下文
     * @param configPath 配置文件路径
     * @return 是否初始化成功
     */
    bool initialize(const std::string& configPath = "");
    
    /**
     * @brief 清理资源
     */
    void cleanup();
    
    // 配置管理
    /**
     * @brief 设置配置项
     * @param key 配置键
     * @param value 配置值
     */
    void setConfig(const std::string& key, const std::string& value);
    
    /**
     * @brief 获取配置项
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string getConfig(const std::string& key, const std::string& defaultValue = "") const;
    
    /**
     * @brief 检查配置项是否存在
     * @param key 配置键
     * @return 是否存在
     */
    bool hasConfig(const std::string& key) const;
    
    // 文件管理
    /**
     * @brief 添加源文件
     * @param filePath 文件路径
     * @return 是否添加成功
     */
    bool addSourceFile(const std::string& filePath);
    
    /**
     * @brief 获取源文件列表
     * @return 源文件路径列表
     */
    const std::vector<std::string>& getSourceFiles() const;
    
    /**
     * @brief 设置输出目录
     * @param outputDir 输出目录路径
     */
    void setOutputDirectory(const std::string& outputDir);
    
    /**
     * @brief 获取输出目录
     * @return 输出目录路径
     */
    const std::string& getOutputDirectory() const;
    
    // 模块管理
    /**
     * @brief 添加模块路径
     * @param modulePath 模块路径
     */
    void addModulePath(const std::string& modulePath);
    
    /**
     * @brief 获取模块路径列表
     * @return 模块路径列表
     */
    const std::vector<std::string>& getModulePaths() const;
    
    // 错误处理
    /**
     * @brief 添加错误信息
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * @brief 添加警告信息
     * @param warning 警告信息
     */
    void addWarning(const std::string& warning);
    
    /**
     * @brief 获取错误列表
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * @brief 获取警告列表
     * @return 警告信息列表
     */
    const std::vector<std::string>& getWarnings() const;
    
    /**
     * @brief 检查是否有错误
     * @return 是否有错误
     */
    bool hasErrors() const;
    
    /**
     * @brief 清空错误和警告
     */
    void clearMessages();
    
    // 调试模式
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;

private:
    // 配置存储
    std::unordered_map<std::string, std::string> m_configs;
    
    // 文件管理
    std::vector<std::string> m_sourceFiles;
    std::string m_outputDirectory;
    
    // 模块管理
    std::vector<std::string> m_modulePaths;
    
    // 错误处理
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    // 调试模式
    bool m_debugMode;
    
    // 初始化状态
    bool m_initialized;
};

} // namespace CHTL