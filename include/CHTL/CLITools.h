#pragma once

#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/TemplateNode.h"
#include "CHTL/CustomNode.h"
#include "CHTL/ImportNode.h"
#include "CHTL/ModuleSystem.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTLJS/CJMODSystem.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace CHTL {

/**
 * CLI命令类型
 */
enum class CLICommandType {
    COMPILE,        // 编译CHTL文件
    PARSE,          // 解析CHTL文件
    LEX,            // 词法分析
    SCAN,           // 扫描分析
    TEMPLATE,       // 模板操作
    CUSTOM,         // 自定义操作
    IMPORT,         // 导入操作
    MODULE,         // 模块操作
    HELP,           // 帮助信息
    VERSION,        // 版本信息
    INIT,           // 初始化项目
    BUILD,          // 构建项目
    CLEAN,          // 清理项目
    TEST,           // 运行测试
    SERVE,          // 启动服务器
    WATCH,          // 监视文件变化
    FORMAT,         // 格式化代码
    LINT,           // 代码检查
    BUNDLE,         // 打包项目
    DEPLOY          // 部署项目
};

/**
 * CLI选项
 */
struct CLIOption {
    std::string name;
    std::string shortName;
    std::string description;
    std::string defaultValue;
    bool required;
    bool hasValue;
    
    CLIOption(const std::string& n, const std::string& sn = "", const std::string& desc = "",
              const std::string& def = "", bool req = false, bool hasVal = true)
        : name(n), shortName(sn), description(desc), defaultValue(def), required(req), hasValue(hasVal) {}
};

/**
 * CLI命令
 */
struct CLICommand {
    CLICommandType type;
    std::string name;
    std::string description;
    std::vector<CLIOption> options;
    std::function<int(const std::vector<std::string>&)> handler;
    
    CLICommand(CLICommandType t, const std::string& n, const std::string& desc)
        : type(t), name(n), description(desc) {}
};

/**
 * CLI参数解析器
 */
class CLIParser {
public:
    CLIParser();
    ~CLIParser();
    
    // 参数解析
    bool parse(int argc, char* argv[]);
    bool parse(const std::vector<std::string>& args);
    
    // 命令获取
    CLICommandType getCommand() const;
    std::string getCommandName() const;
    
    // 选项获取
    bool hasOption(const std::string& name) const;
    std::string getOptionValue(const std::string& name) const;
    std::vector<std::string> getOptionValues(const std::string& name) const;
    
    // 位置参数获取
    std::vector<std::string> getPositionalArgs() const;
    std::string getPositionalArg(size_t index) const;
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    
    // 帮助信息
    std::string getHelpText() const;
    std::string getCommandHelp(const std::string& commandName) const;
    
    // 清理
    void clear();

private:
    CLICommandType command_type_;
    std::string command_name_;
    std::map<std::string, std::vector<std::string>> options_;
    std::vector<std::string> positional_args_;
    std::vector<std::string> errors_;
    
    // 解析辅助函数
    bool parseCommand(const std::string& arg);
    bool parseOption(const std::string& arg);
    bool parseShortOption(const std::string& arg);
    bool parseLongOption(const std::string& arg);
    void addError(const std::string& error);
    
    // 选项验证
    bool validateOptions();
    bool validateRequiredOptions();
    
    // 帮助生成
    std::string generateHelpText() const;
    std::string generateCommandHelp(const std::string& commandName) const;
};

/**
 * CHTL编译器CLI
 */
class CHTLCompilerCLI {
public:
    CHTLCompilerCLI();
    ~CHTLCompilerCLI();
    
    // 主入口
    int run(int argc, char* argv[]);
    int run(const std::vector<std::string>& args);
    
    // 命令处理
    int handleCompile(const std::vector<std::string>& args);
    int handleParse(const std::vector<std::string>& args);
    int handleLex(const std::vector<std::string>& args);
    int handleScan(const std::vector<std::string>& args);
    int handleTemplate(const std::vector<std::string>& args);
    int handleCustom(const std::vector<std::string>& args);
    int handleImport(const std::vector<std::string>& args);
    int handleModule(const std::vector<std::string>& args);
    int handleHelp(const std::vector<std::string>& args);
    int handleVersion(const std::vector<std::string>& args);
    int handleInit(const std::vector<std::string>& args);
    int handleBuild(const std::vector<std::string>& args);
    int handleClean(const std::vector<std::string>& args);
    int handleTest(const std::vector<std::string>& args);
    int handleServe(const std::vector<std::string>& args);
    int handleWatch(const std::vector<std::string>& args);
    int handleFormat(const std::vector<std::string>& args);
    int handleLint(const std::vector<std::string>& args);
    int handleBundle(const std::vector<std::string>& args);
    int handleDeploy(const std::vector<std::string>& args);
    
    // 配置
    void setVerbose(bool verbose);
    void setDebug(bool debug);
    void setOutputDirectory(const std::string& dir);
    void setInputFile(const std::string& file);
    void setConfigFile(const std::string& file);
    
    // 状态
    bool isVerbose() const;
    bool isDebug() const;
    std::string getOutputDirectory() const;
    std::string getInputFile() const;
    std::string getConfigFile() const;
    
    // 清理
    void clear();

private:
    CLIParser parser_;
    CHTLContext context_;
    UnifiedScanner scanner_;
    CHTLLexer lexer_;
    CHTLParser parser_chtl_;
    TemplateManager template_manager_;
    CustomManager custom_manager_;
    ImportManager import_manager_;
    ModuleManager module_manager_;
    CJMODSystem cjmod_system_;
    
    // 配置
    bool verbose_;
    bool debug_;
    std::string output_directory_;
    std::string input_file_;
    std::string config_file_;
    
    // 命令注册
    void registerCommands();
    void registerOptions();
    
    // 文件操作
    bool readFile(const std::string& path, std::string& content);
    bool writeFile(const std::string& path, const std::string& content);
    bool createDirectory(const std::string& path);
    bool fileExists(const std::string& path);
    
    // 编译操作
    bool compileFile(const std::string& inputPath, const std::string& outputPath);
    bool parseFile(const std::string& inputPath);
    bool lexFile(const std::string& inputPath);
    bool scanFile(const std::string& inputPath);
    
    // 模板操作
    bool createTemplate(const std::string& name, const std::string& type);
    bool listTemplates();
    bool deleteTemplate(const std::string& name);
    
    // 自定义操作
    bool createCustom(const std::string& name, const std::string& type);
    bool listCustoms();
    bool deleteCustom(const std::string& name);
    
    // 导入操作
    bool importFile(const std::string& path, const std::string& alias);
    bool listImports();
    bool resolveImports();
    
    // 模块操作
    bool createModule(const std::string& name, const std::string& type);
    bool buildModule(const std::string& name);
    bool installModule(const std::string& name);
    bool uninstallModule(const std::string& name);
    bool listModules();
    
    // 项目操作
    bool initProject(const std::string& name);
    bool buildProject();
    bool cleanProject();
    bool testProject();
    
    // 服务器操作
    bool startServer(int port);
    bool stopServer();
    bool restartServer();
    
    // 监视操作
    bool startWatching(const std::string& path);
    bool stopWatching();
    bool isWatching() const;
    
    // 格式化操作
    bool formatFile(const std::string& path);
    bool formatDirectory(const std::string& path);
    
    // 代码检查
    bool lintFile(const std::string& path);
    bool lintDirectory(const std::string& path);
    
    // 打包操作
    bool bundleProject(const std::string& outputPath);
    bool deployProject(const std::string& target);
    
    // 输出
    void printInfo(const std::string& message);
    void printWarning(const std::string& message);
    void printError(const std::string& message);
    void printDebug(const std::string& message);
    void printSuccess(const std::string& message);
    
    // 帮助
    void printUsage();
    void printVersion();
    void printHelp();
    void printCommandHelp(const std::string& command);
};

/**
 * 命令行程序（带渲染功能）
 */
class CHTLCommandLineProgram {
public:
    CHTLCommandLineProgram();
    ~CHTLCommandLineProgram();
    
    // 主入口
    int run(int argc, char* argv[]);
    int run(const std::vector<std::string>& args);
    
    // 渲染功能
    void enableRendering(bool enable);
    bool isRenderingEnabled() const;
    
    // 预览功能
    void enablePreview(bool enable);
    bool isPreviewEnabled() const;
    
    // 实时编译
    void enableLiveCompilation(bool enable);
    bool isLiveCompilationEnabled() const;
    
    // 清理
    void clear();

private:
    CHTLCompilerCLI cli_;
    bool rendering_enabled_;
    bool preview_enabled_;
    bool live_compilation_enabled_;
    
    // 渲染操作
    bool renderFile(const std::string& inputPath, const std::string& outputPath);
    bool renderDirectory(const std::string& inputPath, const std::string& outputPath);
    
    // 预览操作
    bool startPreview(const std::string& path, int port);
    bool stopPreview();
    bool refreshPreview();
    
    // 实时编译
    bool startLiveCompilation(const std::string& path);
    bool stopLiveCompilation();
    bool isLiveCompilationActive() const;
};

} // namespace CHTL