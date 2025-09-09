#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>

namespace CHTL {

/**
 * @brief CHTL 编译上下文管理类
 * 
 * 负责管理编译过程中的全局状态、配置信息和编译选项
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
     * @brief 设置输入文件路径
     * @param filePath 输入文件路径
     */
    void setInputFile(const std::string& filePath);
    
    /**
     * @brief 设置输出文件路径
     * @param filePath 输出文件路径
     */
    void setOutputFile(const std::string& filePath);
    
    /**
     * @brief 设置模块搜索路径
     * @param path 模块搜索路径
     */
    void addModulePath(const std::string& path);
    
    /**
     * @brief 设置编译选项
     * @param option 选项名称
     * @param value 选项值
     */
    void setOption(const std::string& option, const std::string& value);
    
    /**
     * @brief 获取编译选项
     * @param option 选项名称
     * @return 选项值
     */
    std::string getOption(const std::string& option) const;
    
    /**
     * @brief 获取输入文件路径
     * @return 输入文件路径
     */
    const std::string& getInputFile() const { return inputFile_; }
    
    /**
     * @brief 获取输出文件路径
     * @return 输出文件路径
     */
    const std::string& getOutputFile() const { return outputFile_; }
    
    /**
     * @brief 获取模块搜索路径列表
     * @return 模块搜索路径列表
     */
    const std::vector<std::string>& getModulePaths() const { return modulePaths_; }
    
    /**
     * @brief 检查是否为调试模式
     * @return 是否为调试模式
     */
    bool isDebugMode() const;
    
    /**
     * @brief 设置调试模式
     * @param debug 调试模式标志
     */
    void setDebugMode(bool debug);
    
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
     * @brief 获取错误信息列表
     * @return 错误信息列表
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    /**
     * @brief 获取警告信息列表
     * @return 警告信息列表
     */
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    
    /**
     * @brief 检查是否有错误
     * @return 是否有错误
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief 清空错误和警告信息
     */
    void clearMessages();

private:
    std::string inputFile_;                    ///< 输入文件路径
    std::string outputFile_;                   ///< 输出文件路径
    std::vector<std::string> modulePaths_;     ///< 模块搜索路径列表
    std::unordered_map<std::string, std::string> options_; ///< 编译选项
    std::vector<std::string> errors_;          ///< 错误信息列表
    std::vector<std::string> warnings_;        ///< 警告信息列表
    bool debugMode_;                           ///< 调试模式标志
};

} // namespace CHTL