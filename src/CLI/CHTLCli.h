#ifndef CHTL_CLI_H
#define CHTL_CLI_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace CHTL {

// 命令行选项
struct CliOptions {
    // 输入输出
    std::string inputFile;
    std::string outputFile;
    std::string outputDir;
    std::string outputFormat = "html";
    
    // 编译选项
    bool enableMinification = false;
    bool enableSourceMap = false;
    bool enablePrettyPrint = true;
    bool enableComments = true;
    bool enableTemplateProcessing = true;
    bool enableModuleProcessing = true;
    bool enableCHTLJSProcessing = true;
    bool enableCSSProcessing = true;
    bool enableJSProcessing = true;
    
    // 输出选项
    bool outputHTML = true;
    bool outputCSS = true;
    bool outputJS = true;
    bool outputSourceMap = false;
    bool outputSeparateFiles = false;
    
    // 模块选项
    std::vector<std::string> modulePaths;
    std::vector<std::string> includePaths;
    std::vector<std::string> excludePaths;
    
    // 配置选项
    std::string configFile;
    std::unordered_map<std::string, std::string> configOverrides;
    
    // 调试选项
    bool verbose = false;
    bool debug = false;
    bool dryRun = false;
    bool showHelp = false;
    bool showVersion = false;
    
    // 其他选项
    bool watch = false;
    bool force = false;
    bool recursive = false;
    bool ignoreErrors = false;
};

// 命令行解析器
class CliParser {
public:
    CliParser(int argc, char* argv[]);
    ~CliParser() = default;
    
    // 解析命令行
    CliOptions parse();
    
    // 帮助信息
    void showHelp() const;
    void showVersion() const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    
private:
    int argc_;
    char** argv_;
    std::function<void(const std::string&)> errorHandler_;
    
    // 解析辅助方法
    bool isOption(const std::string& arg) const;
    bool isShortOption(const std::string& arg) const;
    bool isLongOption(const std::string& arg) const;
    std::string getOptionName(const std::string& arg) const;
    std::string getOptionValue(const std::string& arg) const;
    
    // 选项处理
    void handleInputOption(const std::string& value, CliOptions& options);
    void handleOutputOption(const std::string& value, CliOptions& options);
    void handleFormatOption(const std::string& value, CliOptions& options);
    void handleConfigOption(const std::string& value, CliOptions& options);
    void handleModulePathOption(const std::string& value, CliOptions& options);
    void handleIncludePathOption(const std::string& value, CliOptions& options);
    void handleExcludePathOption(const std::string& value, CliOptions& options);
    
    // 错误报告
    void reportError(const std::string& message) const;
};

// 命令行工具
class CHTLCli {
public:
    CHTLCli();
    ~CHTLCli() = default;
    
    // 主要接口
    int run(int argc, char* argv[]);
    int compile(const CliOptions& options);
    int watch(const CliOptions& options);
    int build(const CliOptions& options);
    int clean(const CliOptions& options);
    
    // 配置管理
    void loadConfig(const std::string& configFile);
    void saveConfig(const std::string& configFile, const CliOptions& options);
    void mergeConfig(CliOptions& options, const std::string& configFile);
    
    // 文件处理
    std::vector<std::string> findInputFiles(const CliOptions& options);
    bool processFile(const std::string& inputFile, const CliOptions& options);
    bool processDirectory(const std::string& inputDir, const CliOptions& options);
    
    // 输出管理
    bool writeOutput(const std::string& content, const std::string& outputFile, const CliOptions& options);
    bool writeHTML(const std::string& html, const std::string& outputFile, const CliOptions& options);
    bool writeCSS(const std::string& css, const std::string& outputFile, const CliOptions& options);
    bool writeJS(const std::string& js, const std::string& outputFile, const CliOptions& options);
    bool writeSourceMap(const std::string& sourceMap, const std::string& outputFile, const CliOptions& options);
    
    // 模块管理
    void setupModulePaths(const CliOptions& options);
    void loadModules(const CliOptions& options);
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    void setWarningHandler(std::function<void(const std::string&, size_t, size_t)> handler);
    
    // 日志
    void setLogLevel(int level) { logLevel_ = level; }
    void log(const std::string& message, int level = 0);
    void logError(const std::string& message);
    void logWarning(const std::string& message);
    void logInfo(const std::string& message);
    void logDebug(const std::string& message);
    
private:
    std::function<void(const std::string&, size_t, size_t)> errorHandler_;
    std::function<void(const std::string&, size_t, size_t)> warningHandler_;
    int logLevel_;
    
    // 编译状态
    size_t processedFiles_;
    size_t errorCount_;
    size_t warningCount_;
    
    // 文件监控
    std::unordered_map<std::string, std::time_t> fileTimestamps_;
    bool isFileModified(const std::string& filePath);
    void updateFileTimestamp(const std::string& filePath);
    
    // 辅助方法
    std::string getOutputFileName(const std::string& inputFile, const std::string& extension, const CliOptions& options);
    std::string getRelativePath(const std::string& path, const std::string& base);
    bool createDirectory(const std::string& path);
    bool fileExists(const std::string& path);
    std::string readFile(const std::string& path);
    bool writeFile(const std::string& path, const std::string& content);
    
    // 错误报告
    void reportError(const std::string& message, size_t line = 0, size_t column = 0);
    void reportWarning(const std::string& message, size_t line = 0, size_t column = 0);
    
    // 配置解析
    CliOptions parseConfigFile(const std::string& configFile);
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "");
    void setConfigValue(const std::string& key, const std::string& value);
    
    // 模块处理
    void processModules(const CliOptions& options);
    void loadModule(const std::string& modulePath);
    
    // 清理
    void reset();
};

// 高级渲染CLI
class AdvancedCHTLCli : public CHTLCli {
public:
    AdvancedCHTLCli();
    ~AdvancedCHTLCli() = default;
    
    // 高级功能
    int interactiveMode();
    int projectMode(const std::string& projectPath);
    int serverMode(const CliOptions& options);
    
    // 项目管理
    bool createProject(const std::string& projectName, const std::string& projectPath);
    bool initProject(const std::string& projectPath);
    bool buildProject(const std::string& projectPath);
    bool testProject(const std::string& projectPath);
    
    // 模板管理
    bool createTemplate(const std::string& templateName, const std::string& templatePath);
    bool listTemplates();
    bool showTemplate(const std::string& templateName);
    
    // 模块管理
    bool installModule(const std::string& moduleName);
    bool uninstallModule(const std::string& moduleName);
    bool listModules();
    bool showModule(const std::string& moduleName);
    bool updateModules();
    
    // 配置管理
    bool showConfig();
    bool editConfig();
    bool resetConfig();
    
    // 调试工具
    bool showAST(const std::string& inputFile);
    bool showTokens(const std::string& inputFile);
    bool showDependencies(const std::string& inputFile);
    bool showTimeline(const std::string& inputFile);
    
private:
    // 交互式模式
    void showInteractiveMenu();
    void handleInteractiveCommand(const std::string& command);
    
    // 项目管理
    bool createProjectStructure(const std::string& projectPath);
    bool createProjectConfig(const std::string& projectPath);
    bool createProjectReadme(const std::string& projectPath);
    
    // 服务器模式
    void startServer(const CliOptions& options);
    void handleServerRequest(const std::string& request);
    void stopServer();
    
    // 调试辅助
    void printAST(NodePtr node, int depth = 0);
    void printTokens(const std::vector<TokenPtr>& tokens);
    void printDependencies(const std::vector<std::string>& dependencies);
    void printTimeline(const std::vector<std::string>& timeline);
};

} // namespace CHTL

#endif // CHTL_CLI_H