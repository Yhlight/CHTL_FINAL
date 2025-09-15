#include "CHTLCli.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace CHTL {

CHTLCli::CHTLCli() : debug_mode_(false), verbose_mode_(false) {
    initializeCommands();
}

CHTLCli::~CHTLCli() {
}

int CHTLCli::run(int argc, char* argv[]) {
    try {
        parseArguments(argc, argv);
        return executeCommand();
    } catch (const std::exception& e) {
        printError("Error: " + std::string(e.what()));
        return 1;
    }
}

void CHTLCli::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (isOption(arg)) {
            parseOption(arg);
        } else {
            parsePositionalArg(arg);
        }
    }
}

int CHTLCli::executeCommand() {
    if (positional_args_.empty()) {
        printHelp();
        return 0;
    }
    
    std::string command = positional_args_[0];
    
    if (command == "compile" || command == "c") {
        return compileCommand();
    } else if (command == "build" || command == "b") {
        return buildCommand();
    } else if (command == "watch" || command == "w") {
        return watchCommand();
    } else if (command == "init" || command == "i") {
        return initCommand();
    } else if (command == "module" || command == "m") {
        return moduleCommand();
    } else if (command == "help" || command == "h") {
        return helpCommand();
    } else if (command == "version" || command == "v") {
        return versionCommand();
    } else {
        printError("Unknown command: " + command);
        printHelp();
        return 1;
    }
}

int CHTLCli::compileCommand() {
    if (positional_args_.size() < 2) {
        printError("No input file specified");
        return 1;
    }
    
    std::string inputFile = positional_args_[1];
    std::string outputFile = getOptionValue("output");
    
    if (outputFile.empty()) {
        outputFile = getOutputPath(inputFile);
    }
    
    printInfo("Compiling " + inputFile + " to " + outputFile);
    
    if (fileExists(inputFile)) {
        if (compileFile(inputFile, outputFile)) {
            printInfo("Compilation successful");
            return 0;
        } else {
            printError("Compilation failed");
            return 1;
        }
    } else if (directoryExists(inputFile)) {
        if (compileDirectory(inputFile, outputFile)) {
            printInfo("Compilation successful");
            return 0;
        } else {
            printError("Compilation failed");
            return 1;
        }
    } else {
        printError("Input file or directory not found: " + inputFile);
        return 1;
    }
}

int CHTLCli::buildCommand() {
    printInfo("Building project...");
    
    // 查找项目文件
    std::vector<std::string> chtlFiles = getFilesInDirectory(".", ".chtl");
    
    if (chtlFiles.empty()) {
        printError("No CHTL files found in current directory");
        return 1;
    }
    
    bool success = true;
    for (const std::string& file : chtlFiles) {
        std::string outputFile = getOutputPath(file);
        if (!compileFile(file, outputFile)) {
            success = false;
        }
    }
    
    if (success) {
        printInfo("Build successful");
        return 0;
    } else {
        printError("Build failed");
        return 1;
    }
}

int CHTLCli::watchCommand() {
    printInfo("Watching for file changes...");
    printWarning("Watch mode not implemented yet");
    return 0;
}

int CHTLCli::initCommand() {
    if (positional_args_.size() < 2) {
        printError("Project name required");
        return 1;
    }
    
    std::string projectName = positional_args_[1];
    std::string projectPath = getOptionValue("path");
    
    if (projectPath.empty()) {
        projectPath = "./" + projectName;
    }
    
    printInfo("Initializing project: " + projectName);
    
    if (initializeProject(projectName, projectPath)) {
        printInfo("Project initialized successfully");
        return 0;
    } else {
        printError("Failed to initialize project");
        return 1;
    }
}

int CHTLCli::moduleCommand() {
    if (positional_args_.size() < 2) {
        printError("Module command required");
        return 1;
    }
    
    std::string subCommand = positional_args_[1];
    
    if (subCommand == "install" || subCommand == "i") {
        if (positional_args_.size() < 3) {
            printError("Module name required");
            return 1;
        }
        std::string moduleName = positional_args_[2];
        if (installModule(moduleName)) {
            printInfo("Module installed: " + moduleName);
            return 0;
        } else {
            printError("Failed to install module: " + moduleName);
            return 1;
        }
    } else if (subCommand == "uninstall" || subCommand == "u") {
        if (positional_args_.size() < 3) {
            printError("Module name required");
            return 1;
        }
        std::string moduleName = positional_args_[2];
        if (uninstallModule(moduleName)) {
            printInfo("Module uninstalled: " + moduleName);
            return 0;
        } else {
            printError("Failed to uninstall module: " + moduleName);
            return 1;
        }
    } else if (subCommand == "list" || subCommand == "l") {
        if (listModules()) {
            return 0;
        } else {
            printError("Failed to list modules");
            return 1;
        }
    } else if (subCommand == "search" || subCommand == "s") {
        if (positional_args_.size() < 3) {
            printError("Search query required");
            return 1;
        }
        std::string query = positional_args_[2];
        if (searchModules(query)) {
            return 0;
        } else {
            printError("Failed to search modules");
            return 1;
        }
    } else {
        printError("Unknown module command: " + subCommand);
        return 1;
    }
}

int CHTLCli::helpCommand() {
    if (positional_args_.size() > 1) {
        printCommandHelp(positional_args_[1]);
    } else {
        printHelp();
    }
    return 0;
}

int CHTLCli::versionCommand() {
    printVersion();
    return 0;
}

void CHTLCli::setDebugMode(bool debug) {
    debug_mode_ = debug;
}

bool CHTLCli::isDebugMode() const {
    return debug_mode_;
}

void CHTLCli::setVerboseMode(bool verbose) {
    verbose_mode_ = verbose;
}

bool CHTLCli::isVerboseMode() const {
    return verbose_mode_;
}

void CHTLCli::printHelp() {
    std::cout << "CHTL - C++ HyperText Language\n";
    std::cout << "Usage: chtl <command> [options] [arguments]\n\n";
    std::cout << "Commands:\n";
    std::cout << "  compile, c    Compile CHTL files\n";
    std::cout << "  build, b      Build entire project\n";
    std::cout << "  watch, w      Watch for file changes\n";
    std::cout << "  init, i       Initialize new project\n";
    std::cout << "  module, m     Manage modules\n";
    std::cout << "  help, h       Show help\n";
    std::cout << "  version, v    Show version\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output  Output file or directory\n";
    std::cout << "  -d, --debug   Enable debug mode\n";
    std::cout << "  -v, --verbose Enable verbose output\n";
    std::cout << "  -h, --help    Show help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  chtl compile input.chtl\n";
    std::cout << "  chtl compile input.chtl -o output.html\n";
    std::cout << "  chtl build\n";
    std::cout << "  chtl init my-project\n";
    std::cout << "  chtl module install chtholly\n";
}

void CHTLCli::printVersion() {
    std::cout << "CHTL version 1.0.0\n";
    std::cout << "Copyright (c) 2024 CHTL Team\n";
    std::cout << "MIT License\n";
}

void CHTLCli::printCommandHelp(const std::string& command) {
    auto it = commands_.find(command);
    if (it != commands_.end()) {
        const CLICommand& cmd = it->second;
        std::cout << "Command: " << cmd.name << "\n";
        std::cout << "Description: " << cmd.description << "\n";
        std::cout << "Usage: chtl " << cmd.name << " [options] [arguments]\n";
        
        if (!cmd.options.empty()) {
            std::cout << "\nOptions:\n";
            for (const CLIOption& option : cmd.options) {
                std::cout << "  -" << option.short_name << ", --" << option.name;
                if (option.has_value) {
                    std::cout << " <value>";
                }
                std::cout << "  " << option.description << "\n";
            }
        }
        
        if (!cmd.arguments.empty()) {
            std::cout << "\nArguments:\n";
            for (const std::string& arg : cmd.arguments) {
                std::cout << "  " << arg << "\n";
            }
        }
    } else {
        printError("Unknown command: " + command);
    }
}

void CHTLCli::initializeCommands() {
    // Compile command
    CLICommand compileCmd;
    compileCmd.type = CommandType::COMPILE;
    compileCmd.name = "compile";
    compileCmd.description = "Compile CHTL files to HTML";
    compileCmd.arguments = {"<input-file>", "[output-file]"};
    compileCmd.options = {
        {"output", "o", "Output file or directory", true, "", false},
        {"debug", "d", "Enable debug mode", false, "", false}
    };
    commands_["compile"] = compileCmd;
    
    // Build command
    CLICommand buildCmd;
    buildCmd.type = CommandType::BUILD;
    buildCmd.name = "build";
    buildCmd.description = "Build entire project";
    buildCmd.options = {
        {"debug", "d", "Enable debug mode", false, "", false}
    };
    commands_["build"] = buildCmd;
    
    // Watch command
    CLICommand watchCmd;
    watchCmd.type = CommandType::WATCH;
    watchCmd.name = "watch";
    watchCmd.description = "Watch for file changes and recompile";
    watchCmd.options = {
        {"debug", "d", "Enable debug mode", false, "", false}
    };
    commands_["watch"] = watchCmd;
    
    // Init command
    CLICommand initCmd;
    initCmd.type = CommandType::INIT;
    initCmd.name = "init";
    initCmd.description = "Initialize new CHTL project";
    initCmd.arguments = {"<project-name>"};
    initCmd.options = {
        {"path", "p", "Project path", true, "", false}
    };
    commands_["init"] = initCmd;
    
    // Module command
    CLICommand moduleCmd;
    moduleCmd.type = CommandType::MODULE;
    moduleCmd.name = "module";
    moduleCmd.description = "Manage CHTL modules";
    moduleCmd.arguments = {"<subcommand>", "[module-name]"};
    commands_["module"] = moduleCmd;
}

void CHTLCli::parseOption(const std::string& arg) {
    if (arg.length() < 2) {
        return;
    }
    
    if (arg[0] == '-' && arg[1] == '-') {
        // Long option
        std::string option = arg.substr(2);
        size_t equalPos = option.find('=');
        if (equalPos != std::string::npos) {
            std::string name = option.substr(0, equalPos);
            std::string value = option.substr(equalPos + 1);
            options_[name] = value;
        } else {
            options_[option] = "true";
        }
    } else if (arg[0] == '-') {
        // Short option
        std::string option = arg.substr(1);
        if (option == "d") {
            debug_mode_ = true;
        } else if (option == "v") {
            verbose_mode_ = true;
        } else if (option == "h") {
            printHelp();
        } else {
            options_[option] = "true";
        }
    }
}

void CHTLCli::parsePositionalArg(const std::string& arg) {
    positional_args_.push_back(arg);
}

bool CHTLCli::isOption(const std::string& arg) {
    return arg.length() > 1 && arg[0] == '-';
}

std::string CHTLCli::getOptionValue(const std::string& option) {
    auto it = options_.find(option);
    if (it != options_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLCli::hasOption(const std::string& option) {
    return options_.find(option) != options_.end();
}

bool CHTLCli::compileFile(const std::string& inputFile, const std::string& outputFile) {
    printDebug("Compiling file: " + inputFile);
    
    // 这里应该调用实际的编译器
    // 暂时只是复制文件
    std::string content = readFile(inputFile);
    if (content.empty()) {
        printError("Failed to read input file: " + inputFile);
        return false;
    }
    
    // 简单的转换（实际应该调用CHTL编译器）
    std::string htmlContent = "<!DOCTYPE html>\n<html>\n<head>\n<title>CHTL Output</title>\n</head>\n<body>\n";
    htmlContent += "<!-- Generated from " + inputFile + " -->\n";
    htmlContent += "<pre>" + content + "</pre>\n";
    htmlContent += "</body>\n</html>";
    
    if (!writeFile(outputFile, htmlContent)) {
        printError("Failed to write output file: " + outputFile);
        return false;
    }
    
    return true;
}

bool CHTLCli::compileDirectory(const std::string& inputDir, const std::string& outputDir) {
    printDebug("Compiling directory: " + inputDir);
    
    std::vector<std::string> chtlFiles = getFilesInDirectory(inputDir, ".chtl");
    bool success = true;
    
    for (const std::string& file : chtlFiles) {
        std::string inputPath = inputDir + "/" + file;
        std::string outputPath = outputDir + "/" + file.substr(0, file.find_last_of('.')) + ".html";
        
        if (!compileFile(inputPath, outputPath)) {
            success = false;
        }
    }
    
    return success;
}

std::string CHTLCli::getOutputPath(const std::string& inputPath) {
    size_t lastDot = inputPath.find_last_of('.');
    if (lastDot != std::string::npos) {
        return inputPath.substr(0, lastDot) + ".html";
    }
    return inputPath + ".html";
}

bool CHTLCli::installModule(const std::string& moduleName) {
    printDebug("Installing module: " + moduleName);
    // 这里应该实现模块安装逻辑
    return true;
}

bool CHTLCli::uninstallModule(const std::string& moduleName) {
    printDebug("Uninstalling module: " + moduleName);
    // 这里应该实现模块卸载逻辑
    return true;
}

bool CHTLCli::listModules() {
    printInfo("Installed modules:");
    // 这里应该列出已安装的模块
    return true;
}

bool CHTLCli::searchModules(const std::string& query) {
    printInfo("Searching for modules: " + query);
    // 这里应该实现模块搜索逻辑
    return true;
}

bool CHTLCli::initializeProject(const std::string& projectName, const std::string& projectPath) {
    printDebug("Initializing project: " + projectName + " at " + projectPath);
    
    if (!createProjectStructure(projectPath)) {
        return false;
    }
    
    // 创建示例文件
    std::string exampleContent = R"(html
{
    head
    {
        title
        {
            "Welcome to " + projectName
        }
    }
    
    body
    {
        h1
        {
            "Hello, CHTL!"
        }
        
        p
        {
            "This is your first CHTL project."
        }
    }
})";
    
    std::string exampleFile = projectPath + "/index.chtl";
    if (!writeFile(exampleFile, exampleContent)) {
        printError("Failed to create example file");
        return false;
    }
    
    return true;
}

bool CHTLCli::createProjectStructure(const std::string& projectPath) {
    try {
        std::filesystem::create_directories(projectPath);
        std::filesystem::create_directories(projectPath + "/src");
        std::filesystem::create_directories(projectPath + "/modules");
        std::filesystem::create_directories(projectPath + "/dist");
        
        // 创建README.md
        std::string readmeContent = "# " + projectPath + "\n\nA CHTL project.\n\n## Usage\n\n```bash\nchtl compile src/index.chtl -o dist/index.html\n```\n";
        writeFile(projectPath + "/README.md", readmeContent);
        
        return true;
    } catch (const std::exception& e) {
        printError("Failed to create project structure: " + std::string(e.what()));
        return false;
    }
}

bool CHTLCli::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool CHTLCli::directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

std::string CHTLCli::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLCli::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::vector<std::string> CHTLCli::getFilesInDirectory(const std::string& dir, const std::string& extension) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (extension.empty() || filename.ends_with(extension)) {
                    files.push_back(filename);
                }
            }
        }
    } catch (const std::exception& e) {
        printError("Error reading directory: " + std::string(e.what()));
    }
    
    return files;
}

void CHTLCli::printError(const std::string& message) {
    std::cerr << "\033[31mError: " << message << "\033[0m" << std::endl;
}

void CHTLCli::printWarning(const std::string& message) {
    std::cerr << "\033[33mWarning: " << message << "\033[0m" << std::endl;
}

void CHTLCli::printInfo(const std::string& message) {
    std::cout << "\033[32m" << message << "\033[0m" << std::endl;
}

void CHTLCli::printDebug(const std::string& message) {
    if (debug_mode_) {
        std::cout << "\033[36mDebug: " << message << "\033[0m" << std::endl;
    }
}

} // namespace CHTL