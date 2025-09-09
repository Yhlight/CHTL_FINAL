#pragma once

#include <string>
#include <vector>
#include <map>

namespace CHTL {

/**
 * @brief CHTL命令行工具
 * 提供命令行接口来编译CHTL文件
 */
class CHTLCli {
public:
    CHTLCli();
    ~CHTLCli();
    
    // 主入口点
    int run(int argc, char* argv[]);
    
    // 命令处理
    int compileCommand(const std::vector<std::string>& args);
    int helpCommand(const std::vector<std::string>& args);
    int versionCommand(const std::vector<std::string>& args);
    int initCommand(const std::vector<std::string>& args);
    
    // 选项处理
    void setOutputPath(const std::string& path);
    void setDebugMode(bool enabled);
    void setPrettyPrint(bool enabled);
    void setIncludeComments(bool enabled);
    
    // 获取帮助信息
    std::string getHelpText() const;
    std::string getVersionText() const;

private:
    // 配置选项
    std::string m_outputPath;
    bool m_debugMode;
    bool m_prettyPrint;
    bool m_includeComments;
    
    // 命令映射
    std::map<std::string, int (CHTLCli::*)(const std::vector<std::string>&)> m_commands;
    
    // 辅助方法
    void initializeCommands();
    void printUsage() const;
    void printError(const std::string& message) const;
    void printInfo(const std::string& message) const;
    bool validateInputFile(const std::string& filePath) const;
    std::string getOutputFileName(const std::string& inputFile) const;
};

} // namespace CHTL