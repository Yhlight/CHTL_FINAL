#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <fstream>

namespace CHTL {

/**
 * @brief CHTL编译上下文，管理编译过程中的状态和配置
 */
class CHTLContext {
public:
    CHTLContext();
    ~CHTLContext() = default;

    // 配置管理
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    
    void setIndexInitialCount(int count);
    int getIndexInitialCount() const;
    
    void setDisableStyleAutoAddClass(bool disable);
    bool isDisableStyleAutoAddClass() const;
    
    void setDisableStyleAutoAddId(bool disable);
    bool isDisableStyleAutoAddId() const;
    
    void setDisableDefaultNamespace(bool disable);
    bool isDisableDefaultNamespace() const;

    // 文件管理
    void addSourceFile(const std::string& filePath);
    const std::vector<std::string>& getSourceFiles() const;
    
    void setOutputDirectory(const std::string& dir);
    const std::string& getOutputDirectory() const;

    // 模块管理
    void addModulePath(const std::string& path);
    const std::vector<std::string>& getModulePaths() const;
    
    void setOfficialModulePath(const std::string& path);
    const std::string& getOfficialModulePath() const;

    // 命名空间管理
    void setCurrentNamespace(const std::string& ns);
    const std::string& getCurrentNamespace() const;
    
    void pushNamespace(const std::string& ns);
    void popNamespace();
    const std::vector<std::string>& getNamespaceStack() const;

    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    
    bool hasErrors() const;
    void clearErrors();

    // 编译状态
    void setCompilationState(const std::string& state);
    const std::string& getCompilationState() const;

private:
    // 配置选项
    bool m_debugMode;
    int m_indexInitialCount;
    bool m_disableStyleAutoAddClass;
    bool m_disableStyleAutoAddId;
    bool m_disableDefaultNamespace;
    
    // 文件管理
    std::vector<std::string> m_sourceFiles;
    std::string m_outputDirectory;
    
    // 模块管理
    std::vector<std::string> m_modulePaths;
    std::string m_officialModulePath;
    
    // 命名空间管理
    std::string m_currentNamespace;
    std::vector<std::string> m_namespaceStack;
    
    // 错误处理
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
    
    // 编译状态
    std::string m_compilationState;
};

} // namespace CHTL