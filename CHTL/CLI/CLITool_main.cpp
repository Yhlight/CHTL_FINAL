#include "CLITool.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace CLI {

// Renderer 实现
Renderer::RenderMode Renderer::currentMode = Renderer::TEXT;

std::string Renderer::renderText(const std::string& content) {
    return content;
}

std::string Renderer::renderHTML(const std::string& content) {
    // 简单的HTML渲染
    std::string result = content;
    result = std::regex_replace(result, std::regex("\n"), "<br>");
    result = std::regex_replace(result, std::regex("\\*\\*(.*?)\\*\\*"), "<strong>$1</strong>");
    result = std::regex_replace(result, std::regex("\\*(.*?)\\*"), "<em>$1</em>");
    return result;
}

std::string Renderer::renderJSON(const std::string& content) {
    // 简单的JSON渲染
    return "{\n  \"content\": \"" + content + "\"\n}";
}

std::string Renderer::renderXML(const std::string& content) {
    // 简单的XML渲染
    return "<content>" + content + "</content>";
}

void Renderer::setRenderMode(RenderMode mode) {
    currentMode = mode;
}

Renderer::RenderMode Renderer::getRenderMode() {
    return currentMode;
}

// CLITool 实现
CLITool::CLITool() : debugMode(false), logLevel("info") {
    config.programName = "chtl";
    config.version = "1.0.0";
    config.description = "CHTL 命令行工具";
    initializeCommands();
}

CLITool::CLITool(const CLIConfig& config) : config(config), debugMode(false), logLevel("info") {
    initializeCommands();
}

CLITool::~CLITool() = default;

void CLITool::setConfig(const CLIConfig& config) {
    this->config = config;
}

CLIConfig CLITool::getConfig() const {
    return config;
}

void CLITool::addCommand(const Command& command) {
    commands.push_back(command);
}

void CLITool::removeCommand(CommandType type) {
    commands.erase(
        std::remove_if(commands.begin(), commands.end(),
            [type](const Command& cmd) { return cmd.type == type; }),
        commands.end()
    );
}

std::vector<Command> CLITool::getCommands() const {
    return commands;
}

Command CLITool::getCommand(CommandType type) const {
    for (const auto& command : commands) {
        if (command.type == type) {
            return command;
        }
    }
    return Command();
}

void CLITool::setGlobalOption(const std::string& name, const std::string& value) {
    globalOptions[name] = value;
}

std::string CLITool::getGlobalOption(const std::string& name, const std::string& defaultValue) const {
    auto it = globalOptions.find(name);
    return it != globalOptions.end() ? it->second : defaultValue;
}

std::map<std::string, std::string> CLITool::getGlobalOptions() const {
    return globalOptions;
}

int CLITool::execute(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return execute(args);
}

int CLITool::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        printUsage();
        return 1;
    }
    
    CLIParser parser(args);
    
    if (parser.getErrors().size() > 0) {
        for (const auto& error : parser.getErrors()) {
            notifyError(error);
        }
        return 1;
    }
    
    std::string commandName = parser.getPositionalArg(0);
    
    if (commandName == "help" || commandName == "--help" || commandName == "-h") {
        printHelp();
        return 0;
    }
    
    if (commandName == "version" || commandName == "--version" || commandName == "-v") {
        printVersion();
        return 0;
    }
    
    // 查找命令
    Command command;
    for (const auto& cmd : commands) {
        if (cmd.name == commandName) {
            command = cmd;
            break;
        }
    }
    
    if (command.type == CommandType::HELP) {
        notifyError("未知命令: " + commandName);
        printUsage();
        return 1;
    }
    
    // 验证参数
    parser.validateArguments(command);
    if (parser.getErrors().size() > 0) {
        for (const auto& error : parser.getErrors()) {
            notifyError(error);
        }
        return 1;
    }
    
    // 执行命令
    try {
        return command.handler(parser.getPositionalArgs());
    } catch (const std::exception& e) {
        notifyError("命令执行失败: " + std::string(e.what()));
        return 1;
    }
}

std::vector<std::string> CLITool::getGlobalErrors() const {
    return globalErrors;
}

std::vector<std::string> CLITool::getGlobalWarnings() const {
    return globalWarnings;
}

void CLITool::clearGlobalErrors() {
    globalErrors.clear();
}

void CLITool::clearGlobalWarnings() {
    globalWarnings.clear();
}

void CLITool::enableDebugMode(bool enable) {
    debugMode = enable;
}

bool CLITool::isDebugMode() const {
    return debugMode;
}

void CLITool::setLogLevel(const std::string& level) {
    logLevel = level;
}

std::string CLITool::getLogLevel() const {
    return logLevel;
}

void CLITool::setErrorCallback(std::function<void(const std::string&)> callback) {
    errorCallback = callback;
}

void CLITool::setWarningCallback(std::function<void(const std::string&)> callback) {
    warningCallback = callback;
}

void CLITool::setInfoCallback(std::function<void(const std::string&)> callback) {
    infoCallback = callback;
}

void CLITool::initializeCommands() {
    // 编译命令
    Command compileCmd(CommandType::COMPILE, "compile", "编译CHTL文件");
    compileCmd.arguments = {
        CommandArgument("input", "输入文件", true),
        CommandArgument("output", "输出文件", false),
        CommandArgument("format", "输出格式", false)
    };
    compileCmd.handler = [this](const std::vector<std::string>& args) {
        return handleCompileCommand(args);
    };
    commands.push_back(compileCmd);
    
    // 构建命令
    Command buildCmd(CommandType::BUILD, "build", "构建项目");
    buildCmd.arguments = {
        CommandArgument("project", "项目目录", false),
        CommandArgument("output", "输出目录", false)
    };
    buildCmd.handler = [this](const std::vector<std::string>& args) {
        return handleBuildCommand(args);
    };
    commands.push_back(buildCmd);
    
    // 监听命令
    Command watchCmd(CommandType::WATCH, "watch", "监听文件变化");
    watchCmd.arguments = {
        CommandArgument("directory", "监听目录", false),
        CommandArgument("pattern", "文件模式", false)
    };
    watchCmd.handler = [this](const std::vector<std::string>& args) {
        return handleWatchCommand(args);
    };
    commands.push_back(watchCmd);
    
    // 服务命令
    Command serveCmd(CommandType::SERVE, "serve", "启动开发服务器");
    serveCmd.arguments = {
        CommandArgument("port", "端口号", false),
        CommandArgument("directory", "服务目录", false)
    };
    serveCmd.handler = [this](const std::vector<std::string>& args) {
        return handleServeCommand(args);
    };
    commands.push_back(serveCmd);
    
    // 初始化命令
    Command initCmd(CommandType::INIT, "init", "初始化项目");
    initCmd.arguments = {
        CommandArgument("name", "项目名称", false),
        CommandArgument("template", "项目模板", false)
    };
    initCmd.handler = [this](const std::vector<std::string>& args) {
        return handleInitCommand(args);
    };
    commands.push_back(initCmd);
    
    // 清理命令
    Command cleanCmd(CommandType::CLEAN, "clean", "清理构建文件");
    cleanCmd.arguments = {
        CommandArgument("directory", "清理目录", false)
    };
    cleanCmd.handler = [this](const std::vector<std::string>& args) {
        return handleCleanCommand(args);
    };
    commands.push_back(cleanCmd);
    
    // 测试命令
    Command testCmd(CommandType::TEST, "test", "运行测试");
    testCmd.arguments = {
        CommandArgument("pattern", "测试模式", false),
        CommandArgument("verbose", "详细输出", false)
    };
    testCmd.handler = [this](const std::vector<std::string>& args) {
        return handleTestCommand(args);
    };
    commands.push_back(testCmd);
    
    // 帮助命令
    Command helpCmd(CommandType::HELP, "help", "显示帮助信息");
    helpCmd.handler = [this](const std::vector<std::string>& args) {
        return handleHelpCommand(args);
    };
    commands.push_back(helpCmd);
    
    // 版本命令
    Command versionCmd(CommandType::VERSION, "version", "显示版本信息");
    versionCmd.handler = [this](const std::vector<std::string>& args) {
        return handleVersionCommand(args);
    };
    commands.push_back(versionCmd);
    
    // 配置命令
    Command configCmd(CommandType::CONFIG, "config", "管理配置");
    configCmd.arguments = {
        CommandArgument("action", "操作类型", true),
        CommandArgument("key", "配置键", false),
        CommandArgument("value", "配置值", false)
    };
    configCmd.handler = [this](const std::vector<std::string>& args) {
        return handleConfigCommand(args);
    };
    commands.push_back(configCmd);
}

void CLITool::printBanner() {
    std::cout << "===============================================" << std::endl;
    std::cout << "  " << config.programName << " " << config.version << std::endl;
    std::cout << "  " << config.description << std::endl;
    std::cout << "===============================================" << std::endl;
}

void CLITool::printVersion() {
    std::cout << config.programName << " " << config.version << std::endl;
}

void CLITool::printHelp() {
    printBanner();
    std::cout << std::endl;
    std::cout << "用法: " << config.programName << " <命令> [选项] [参数]" << std::endl;
    std::cout << std::endl;
    std::cout << "可用命令:" << std::endl;
    
    for (const auto& command : commands) {
        std::cout << "  " << std::setw(12) << std::left << command.name 
                  << " " << command.description << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "使用 '" << config.programName << " <命令> --help' 获取特定命令的帮助信息" << std::endl;
}

void CLITool::printUsage() {
    std::cout << "用法: " << config.programName << " <命令> [选项] [参数]" << std::endl;
    std::cout << "使用 '" << config.programName << " help' 获取更多信息" << std::endl;
}

// 命令处理实现
int CLITool::handleCompileCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        notifyError("缺少输入文件");
        return 1;
    }
    
    std::string inputFile = args[1];
    std::string outputFile = args.size() > 2 ? args[2] : inputFile + ".html";
    
    if (!FileProcessor::fileExists(inputFile)) {
        notifyError("输入文件不存在: " + inputFile);
        return 1;
    }
    
    notifyInfo("编译文件: " + inputFile);
    notifyInfo("输出文件: " + outputFile);
    
    // 这里应该调用实际的编译器
    // 目前只是示例实现
    
    notifyInfo("编译完成");
    return 0;
}

int CLITool::handleBuildCommand(const std::vector<std::string>& args) {
    std::string projectDir = args.size() > 1 ? args[1] : ".";
    std::string outputDir = args.size() > 2 ? args[2] : "./dist";
    
    notifyInfo("构建项目: " + projectDir);
    notifyInfo("输出目录: " + outputDir);
    
    // 这里应该调用实际的构建器
    // 目前只是示例实现
    
    notifyInfo("构建完成");
    return 0;
}

int CLITool::handleWatchCommand(const std::vector<std::string>& args) {
    std::string directory = args.size() > 1 ? args[1] : ".";
    std::string pattern = args.size() > 2 ? args[2] : "*.chtl";
    
    notifyInfo("监听目录: " + directory);
    notifyInfo("文件模式: " + pattern);
    
    // 这里应该实现文件监听
    // 目前只是示例实现
    
    notifyInfo("开始监听文件变化...");
    notifyInfo("按 Ctrl+C 停止监听");
    
    // 简单的监听循环
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // 检查文件变化
    }
    
    return 0;
}

int CLITool::handleServeCommand(const std::vector<std::string>& args) {
    int port = args.size() > 1 ? std::stoi(args[1]) : 8080;
    std::string directory = args.size() > 2 ? args[2] : ".";
    
    notifyInfo("启动开发服务器");
    notifyInfo("端口: " + std::to_string(port));
    notifyInfo("目录: " + directory);
    
    // 这里应该启动HTTP服务器
    // 目前只是示例实现
    
    notifyInfo("服务器启动成功");
    notifyInfo("访问 http://localhost:" + std::to_string(port));
    
    return 0;
}

int CLITool::handleInitCommand(const std::vector<std::string>& args) {
    std::string projectName = args.size() > 1 ? args[1] : "my-chtl-project";
    std::string templateName = args.size() > 2 ? args[2] : "basic";
    
    notifyInfo("初始化项目: " + projectName);
    notifyInfo("使用模板: " + templateName);
    
    // 这里应该创建项目结构
    // 目前只是示例实现
    
    if (FileProcessor::createDirectory(projectName)) {
        notifyInfo("项目创建成功: " + projectName);
    } else {
        notifyError("项目创建失败");
        return 1;
    }
    
    return 0;
}

int CLITool::handleCleanCommand(const std::vector<std::string>& args) {
    std::string directory = args.size() > 1 ? args[1] : "./dist";
    
    notifyInfo("清理目录: " + directory);
    
    if (FileProcessor::removeDirectory(directory)) {
        notifyInfo("清理完成");
    } else {
        notifyError("清理失败");
        return 1;
    }
    
    return 0;
}

int CLITool::handleTestCommand(const std::vector<std::string>& args) {
    std::string pattern = args.size() > 1 ? args[1] : "*";
    bool verbose = args.size() > 2 && args[2] == "true";
    
    notifyInfo("运行测试");
    notifyInfo("模式: " + pattern);
    
    // 这里应该运行测试
    // 目前只是示例实现
    
    notifyInfo("测试完成");
    return 0;
}

int CLITool::handleHelpCommand(const std::vector<std::string>& args) {
    printHelp();
    return 0;
}

int CLITool::handleVersionCommand(const std::vector<std::string>& args) {
    printVersion();
    return 0;
}

int CLITool::handleConfigCommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        notifyError("缺少操作类型");
        return 1;
    }
    
    std::string action = args[1];
    
    if (action == "list") {
        notifyInfo("当前配置:");
        for (const auto& option : globalOptions) {
            std::cout << "  " << option.first << " = " << option.second << std::endl;
        }
    } else if (action == "set" && args.size() > 3) {
        std::string key = args[2];
        std::string value = args[3];
        setGlobalOption(key, value);
        notifyInfo("配置已设置: " + key + " = " + value);
    } else if (action == "get" && args.size() > 2) {
        std::string key = args[2];
        std::string value = getGlobalOption(key);
        std::cout << key << " = " << value << std::endl;
    } else {
        notifyError("未知操作: " + action);
        return 1;
    }
    
    return 0;
}

void CLITool::notifyError(const std::string& error) {
    globalErrors.push_back(error);
    if (errorCallback) {
        errorCallback(error);
    } else {
        ColorOutput::printError(error);
    }
}

void CLITool::notifyWarning(const std::string& warning) {
    globalWarnings.push_back(warning);
    if (warningCallback) {
        warningCallback(warning);
    } else {
        ColorOutput::printWarning(warning);
    }
}

void CLITool::notifyInfo(const std::string& info) {
    if (infoCallback) {
        infoCallback(info);
    } else {
        ColorOutput::printInfo(info);
    }
}

void CLITool::log(const std::string& message, const std::string& level) {
    if (debugMode || level == "error") {
        std::cout << "[" << level << "] " << message << std::endl;
    }
}

} // namespace CLI
} // namespace CHTL