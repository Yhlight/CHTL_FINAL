#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

// 命令行参数类型
enum class ArgType {
    String,
    Number,
    Boolean,
    File,
    Directory
};

// 命令行参数结构
struct CliArgument {
    std::string name;
    std::string shortName;
    ArgType type;
    std::string description;
    std::string defaultValue;
    bool isRequired;
    bool isMultiple;
};

// 命令行命令结构
struct CliCommand {
    std::string name;
    std::string description;
    std::vector<CliArgument> arguments;
    std::function<int(const std::map<std::string, std::string>&)> handler;
};

// CHTL CLI类
class CHTLCli {
public:
    CHTLCli();
    ~CHTLCli() = default;
    
    // 命令管理
    void addCommand(const CliCommand& command);
    void removeCommand(const std::string& name);
    bool hasCommand(const std::string& name) const;
    CliCommand getCommand(const std::string& name) const;
    std::vector<std::string> getAllCommandNames() const;
    
    // 参数管理
    void addGlobalArgument(const CliArgument& argument);
    void removeGlobalArgument(const std::string& name);
    bool hasGlobalArgument(const std::string& name) const;
    
    // 执行
    int execute(int argc, char* argv[]);
    int execute(const std::vector<std::string>& args);
    
    // 帮助
    void printHelp() const;
    void printCommandHelp(const std::string& commandName) const;
    void printVersion() const;
    
    // 配置
    void setProgramName(const std::string& name);
    void setVersion(const std::string& version);
    void setDescription(const std::string& description);
    
    // 工具方法
    static std::string getCurrentDirectory();
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);

private:
    std::string programName;
    std::string version;
    std::string description;
    std::map<std::string, CliCommand> commands;
    std::map<std::string, CliArgument> globalArguments;
    
    // 内置命令
    void initializeBuiltinCommands();
    int handleCompile(const std::map<std::string, std::string>& args);
    int handleWatch(const std::map<std::string, std::string>& args);
    int handleInit(const std::map<std::string, std::string>& args);
    int handleBuild(const std::map<std::string, std::string>& args);
    int handleClean(const std::map<std::string, std::string>& args);
    int handleTest(const std::map<std::string, std::string>& args);
    int handleServe(const std::map<std::string, std::string>& args);
    int handleHelp(const std::map<std::string, std::string>& args);
    int handleVersion(const std::map<std::string, std::string>& args);
    
    // 参数解析
    std::map<std::string, std::string> parseArguments(const std::vector<std::string>& args, 
                                                      const std::vector<CliArgument>& commandArgs) const;
    bool validateArguments(const std::map<std::string, std::string>& parsedArgs,
                          const std::vector<CliArgument>& commandArgs) const;
    
    // 输出
    void printError(const std::string& message) const;
    void printInfo(const std::string& message) const;
    void printSuccess(const std::string& message) const;
};

} // namespace CHTL