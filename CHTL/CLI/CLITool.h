#ifndef CLI_TOOL_H
#define CLI_TOOL_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <ctime>
#include <fstream>

namespace CHTL {
namespace CLI {

// 命令类型枚举
enum class CommandType {
    COMPILE,        // 编译命令
    BUILD,          // 构建命令
    WATCH,          // 监听命令
    SERVE,          // 服务命令
    INIT,           // 初始化命令
    CLEAN,          // 清理命令
    TEST,           // 测试命令
    HELP,           // 帮助命令
    VERSION,        // 版本命令
    CONFIG          // 配置命令
};

// 命令参数
struct CommandArgument {
    std::string name;
    std::string description;
    std::string defaultValue;
    bool required;
    bool hasValue;
    
    CommandArgument() : required(false), hasValue(true) {}
    CommandArgument(const std::string& n, const std::string& desc, bool req = false, bool hasVal = true)
        : name(n), description(desc), required(req), hasValue(hasVal) {}
};

// 命令定义
struct Command {
    CommandType type;
    std::string name;
    std::string description;
    std::vector<CommandArgument> arguments;
    std::function<int(const std::vector<std::string>&)> handler;
    
    Command() : type(CommandType::HELP) {}
    Command(CommandType t, const std::string& n, const std::string& desc)
        : type(t), name(n), description(desc) {}
};

// CLI配置
struct CLIConfig {
    std::string programName;
    std::string version;
    std::string description;
    std::string usage;
    bool enableColors;
    bool enableVerbose;
    std::string logLevel;
    std::map<std::string, std::string> defaultOptions;
    
    CLIConfig() : enableColors(true), enableVerbose(false), logLevel("info") {}
};

// 颜色输出
class ColorOutput {
public:
    enum Color {
        RED,
        GREEN,
        YELLOW,
        BLUE,
        MAGENTA,
        CYAN,
        WHITE,
        RESET
    };
    
    static std::string colorize(const std::string& text, Color color);
    static void printColored(const std::string& text, Color color, std::ostream& os = std::cout);
    static void printError(const std::string& text, std::ostream& os = std::cerr);
    static void printSuccess(const std::string& text, std::ostream& os = std::cout);
    static void printWarning(const std::string& text, std::ostream& os = std::cout);
    static void printInfo(const std::string& text, std::ostream& os = std::cout);
};

// 进度条
class ProgressBar {
private:
    int total;
    int current;
    int width;
    std::string prefix;
    std::string suffix;
    char fill;
    char empty;
    
public:
    ProgressBar(int total, int width = 50, const std::string& prefix = "", const std::string& suffix = "");
    
    void update(int current);
    void setTotal(int total);
    void setPrefix(const std::string& prefix);
    void setSuffix(const std::string& suffix);
    void setFill(char fill);
    void setEmpty(char empty);
    
    void display() const;
    void finish();
    
private:
    void updateDisplay() const;
};

// 文件处理器
class FileProcessor {
public:
    struct FileInfo {
        std::string path;
        std::string name;
        std::string extension;
        size_t size;
        time_t lastModified;
        bool exists;
        
        FileInfo() : size(0), lastModified(0), exists(false) {}
    };
    
    static FileInfo getFileInfo(const std::string& path);
    static bool fileExists(const std::string& path);
    static bool directoryExists(const std::string& path);
    static std::vector<std::string> listFiles(const std::string& directory, const std::string& pattern = "*");
    static std::vector<std::string> listDirectories(const std::string& directory);
    static bool createDirectory(const std::string& path);
    static bool removeFile(const std::string& path);
    static bool removeDirectory(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static std::string getRelativePath(const std::string& path, const std::string& base);
    static std::string getAbsolutePath(const std::string& path);
    static std::string normalizePath(const std::string& path);
};

// CLI解析器
class CLIParser {
private:
    std::vector<std::string> args;
    std::map<std::string, std::string> options;
    std::vector<std::string> positionalArgs;
    std::vector<std::string> errors;
    
    void parseArguments();
public:
    void validateArguments(const Command& command);
    
public:
    CLIParser(int argc, char* argv[]);
    CLIParser(const std::vector<std::string>& args);
    
    std::map<std::string, std::string> getOptions() const;
    std::vector<std::string> getPositionalArgs() const;
    std::vector<std::string> getErrors() const;
    
    bool hasOption(const std::string& name) const;
    std::string getOption(const std::string& name, const std::string& defaultValue = "") const;
    int getIntOption(const std::string& name, int defaultValue = 0) const;
    bool getBoolOption(const std::string& name, bool defaultValue = false) const;
    
    std::string getPositionalArg(int index, const std::string& defaultValue = "") const;
    int getPositionalArgCount() const;
    
    void printUsage(const Command& command) const;
    void printHelp(const std::vector<Command>& commands) const;
};

// 渲染器
class Renderer {
public:
    enum RenderMode {
        TEXT,
        HTML,
        JSON,
        XML
    };
    
    static std::string renderText(const std::string& content);
    static std::string renderHTML(const std::string& content);
    static std::string renderJSON(const std::string& content);
    static std::string renderXML(const std::string& content);
    
    static void setRenderMode(RenderMode mode);
    static RenderMode getRenderMode();
    
private:
    static RenderMode currentMode;
};

// CLI工具主类
class CLITool {
private:
    CLIConfig config;
    std::vector<Command> commands;
    std::map<std::string, std::string> globalOptions;
    std::vector<std::string> globalErrors;
    std::vector<std::string> globalWarnings;
    
    // 命令处理
    int handleCompileCommand(const std::vector<std::string>& args);
    int handleBuildCommand(const std::vector<std::string>& args);
    int handleWatchCommand(const std::vector<std::string>& args);
    int handleServeCommand(const std::vector<std::string>& args);
    int handleInitCommand(const std::vector<std::string>& args);
    int handleCleanCommand(const std::vector<std::string>& args);
    int handleTestCommand(const std::vector<std::string>& args);
    int handleHelpCommand(const std::vector<std::string>& args);
    int handleVersionCommand(const std::vector<std::string>& args);
    int handleConfigCommand(const std::vector<std::string>& args);
    
    // 辅助方法
    void initializeCommands();
    void printBanner();
    void printVersion();
    void printHelp();
    void printUsage();
    
public:
    CLITool();
    CLITool(const CLIConfig& config);
    ~CLITool();
    
    // 配置管理
    void setConfig(const CLIConfig& config);
    CLIConfig getConfig() const;
    
    // 命令管理
    void addCommand(const Command& command);
    void removeCommand(CommandType type);
    std::vector<Command> getCommands() const;
    Command getCommand(CommandType type) const;
    
    // 选项管理
    void setGlobalOption(const std::string& name, const std::string& value);
    std::string getGlobalOption(const std::string& name, const std::string& defaultValue = "") const;
    std::map<std::string, std::string> getGlobalOptions() const;
    
    // 执行
    int execute(int argc, char* argv[]);
    int execute(const std::vector<std::string>& args);
    
    // 错误和警告管理
    std::vector<std::string> getGlobalErrors() const;
    std::vector<std::string> getGlobalWarnings() const;
    void clearGlobalErrors();
    void clearGlobalWarnings();
    
    // 调试和诊断
    void enableDebugMode(bool enable);
    bool isDebugMode() const;
    void setLogLevel(const std::string& level);
    std::string getLogLevel() const;
    
    // 回调函数
    void setErrorCallback(std::function<void(const std::string&)> callback);
    void setWarningCallback(std::function<void(const std::string&)> callback);
    void setInfoCallback(std::function<void(const std::string&)> callback);
    
private:
    std::function<void(const std::string&)> errorCallback;
    std::function<void(const std::string&)> warningCallback;
    std::function<void(const std::string&)> infoCallback;
    bool debugMode;
    std::string logLevel;
    
    // 内部辅助方法
    void notifyError(const std::string& error);
    void notifyWarning(const std::string& warning);
    void notifyInfo(const std::string& info);
    void log(const std::string& message, const std::string& level = "info");
};

} // namespace CLI
} // namespace CHTL

#endif // CLI_TOOL_H