#ifndef CHTLCLI_H
#define CHTLCLI_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * CLI命令类型
 */
enum class CommandType {
    COMPILE,
    BUILD,
    WATCH,
    INIT,
    MODULE,
    HELP,
    VERSION,
    UNKNOWN
};

/**
 * CLI选项
 */
struct CLIOption {
    std::string name;
    std::string short_name;
    std::string description;
    bool has_value;
    std::string default_value;
    bool required;
};

/**
 * CLI命令
 */
struct CLICommand {
    CommandType type;
    std::string name;
    std::string description;
    std::vector<CLIOption> options;
    std::vector<std::string> arguments;
};

/**
 * CHTL CLI工具
 */
class CHTLCli {
public:
    CHTLCli();
    ~CHTLCli();
    
    // 主要方法
    int run(int argc, char* argv[]);
    void parseArguments(int argc, char* argv[]);
    int executeCommand();
    
    // 命令执行
    int compileCommand();
    int buildCommand();
    int watchCommand();
    int initCommand();
    int moduleCommand();
    int helpCommand();
    int versionCommand();
    
    // 配置
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    
    void setVerboseMode(bool verbose);
    bool isVerboseMode() const;
    
    // 帮助
    void printHelp();
    void printVersion();
    void printCommandHelp(const std::string& command);

private:
    // 命令行参数
    std::vector<std::string> args_;
    std::map<std::string, std::string> options_;
    std::vector<std::string> positional_args_;
    
    // 配置
    bool debug_mode_;
    bool verbose_mode_;
    
    // 命令定义
    std::map<std::string, CLICommand> commands_;
    
    // 内部方法
    void initializeCommands();
    void parseOption(const std::string& arg);
    void parsePositionalArg(const std::string& arg);
    bool isOption(const std::string& arg);
    std::string getOptionValue(const std::string& option);
    bool hasOption(const std::string& option);
    
    // 编译相关
    bool compileFile(const std::string& inputFile, const std::string& outputFile);
    bool compileDirectory(const std::string& inputDir, const std::string& outputDir);
    std::string getOutputPath(const std::string& inputPath);
    
    // 模块相关
    bool installModule(const std::string& moduleName);
    bool uninstallModule(const std::string& moduleName);
    bool listModules();
    bool searchModules(const std::string& query);
    
    // 项目初始化
    bool initializeProject(const std::string& projectName, const std::string& projectPath);
    bool createProjectStructure(const std::string& projectPath);
    
    // 文件操作
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    std::string readFile(const std::string& path);
    bool writeFile(const std::string& path, const std::string& content);
    std::vector<std::string> getFilesInDirectory(const std::string& dir, const std::string& extension = "");
    
    // 错误处理
    void printError(const std::string& message);
    void printWarning(const std::string& message);
    void printInfo(const std::string& message);
    void printDebug(const std::string& message);
};

} // namespace CHTL

#endif // CHTLCLI_H