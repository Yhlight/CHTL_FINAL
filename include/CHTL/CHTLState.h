#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief CHTL编译状态类
 * 
 * 管理编译过程中的状态信息
 */
class CHTLState {
public:
    CHTLState();
    ~CHTLState() = default;

    /**
     * @brief 设置当前文件路径
     * @param path 文件路径
     */
    void setCurrentFile(const std::string& path);

    /**
     * @brief 获取当前文件路径
     * @return 文件路径
     */
    const std::string& getCurrentFile() const { return m_currentFile; }

    /**
     * @brief 设置输出目录
     * @param dir 输出目录
     */
    void setOutputDirectory(const std::string& dir);

    /**
     * @brief 获取输出目录
     * @return 输出目录
     */
    const std::string& getOutputDirectory() const { return m_outputDirectory; }

    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);

    /**
     * @brief 获取调试模式
     * @return 是否启用调试模式
     */
    bool getDebugMode() const { return m_debugMode; }

    /**
     * @brief 添加全局变量
     * @param name 变量名
     * @param value 变量值
     */
    void addGlobalVariable(const std::string& name, const std::string& value);

    /**
     * @brief 获取全局变量
     * @param name 变量名
     * @return 变量值
     */
    std::string getGlobalVariable(const std::string& name) const;

    /**
     * @brief 检查是否有全局变量
     * @param name 变量名
     * @return 是否有该变量
     */
    bool hasGlobalVariable(const std::string& name) const;

    /**
     * @brief 添加模板定义
     * @param name 模板名
     * @param templateData 模板数据
     */
    void addTemplate(const std::string& name, const std::string& templateData);

    /**
     * @brief 获取模板定义
     * @param name 模板名
     * @return 模板数据
     */
    std::string getTemplate(const std::string& name) const;

    /**
     * @brief 检查是否有模板
     * @param name 模板名
     * @return 是否有该模板
     */
    bool hasTemplate(const std::string& name) const;

    /**
     * @brief 添加命名空间
     * @param name 命名空间名
     * @param namespaceData 命名空间数据
     */
    void addNamespace(const std::string& name, const std::string& namespaceData);

    /**
     * @brief 获取命名空间
     * @param name 命名空间名
     * @return 命名空间数据
     */
    std::string getNamespace(const std::string& name) const;

    /**
     * @brief 检查是否有命名空间
     * @param name 命名空间名
     * @return 是否有该命名空间
     */
    bool hasNamespace(const std::string& name) const;

    /**
     * @brief 清空所有状态
     */
    void clear();

private:
    std::string m_currentFile;
    std::string m_outputDirectory;
    bool m_debugMode;
    std::unordered_map<std::string, std::string> m_globalVariables;
    std::unordered_map<std::string, std::string> m_templates;
    std::unordered_map<std::string, std::string> m_namespaces;
};

} // namespace CHTL