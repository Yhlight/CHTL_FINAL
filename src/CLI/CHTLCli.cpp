#include "CLI/CHTLCli.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>

namespace CHTL {

CHTLCli::CHTLCli() : debug_mode_(false) {
    // 初始化编译器组件
    compiler_ = std::make_unique<CompilerDispatcher>();
    cmod_system_ = std::make_unique<CMODSystem>();
    cjmod_system_ = std::make_unique<CJMODSystem>();
    
    // 初始化命令列表
    initializeCommands();
}

CommandType CHTLCli::parseArguments(int argc, char* argv[]) {
    args_.clear();
    errors_.clear();
    
    // 复制参数
    for (int i = 1; i < argc; ++i) {
        args_.push_back(argv[i]);
    }
    
    if (args_.empty()) {
        return CommandType::HELP;
    }
    
    std::string command = args_[0];
    
    // 解析选项（跳过第一个参数，因为它是命令）
    for (size_t i = 1; i < args_.size(); ++i) {
        if (args_[i].length() > 0 && args_[i][0] == '-') {
            parseOption(args_[i]);
        }
    }
    
    // 查找命令
    const CLICommand* cmd = findCommand(command);
    if (cmd) {
        return cmd->type;
    }
    
    // 如果没有找到命令，可能是文件路径
    if (validateFilePath(command)) {
        return CommandType::COMPILE;
    }
    
    // 检查是否是编译命令但没有指定文件
    if (command == "compile" || command == "c") {
        return CommandType::COMPILE;
    }
    
    addError("Unknown command: " + command);
    return CommandType::UNKNOWN;
}

int CHTLCli::executeCommand(CommandType command_type) {
    switch (command_type) {
        case CommandType::COMPILE: {
            std::string input_file = "";
            std::string output_file = "";
            
            // 查找输入文件和输出文件
            for (size_t i = 0; i < args_.size(); ++i) {
                if (args_[i] == "-o" && i + 1 < args_.size()) {
                    output_file = args_[i + 1];
                    i++; // 跳过输出文件名
                } else if (!(args_[i].length() > 0 && args_[i][0] == '-') && input_file.empty()) {
                    input_file = args_[i];
                }
            }
            
            if (input_file.empty()) {
                addError("No input file specified");
                return 1;
            }
            
            return compileFile(input_file, output_file);
        }
        
        case CommandType::WATCH: {
            if (args_.size() < 2) {
                addError("No input path specified for watch command");
                return 1;
            }
            
            return watchFiles(args_[1]);
        }
        
        case CommandType::MODULE: {
            if (args_.size() < 2) {
                addError("No module action specified");
                return 1;
            }
            
            std::string action = args_[1];
            std::string module_name = args_.size() > 2 ? args_[2] : "";
            
            return manageModule(action, module_name);
        }
        
        case CommandType::INIT: {
            std::string project_name = args_.size() > 1 ? args_[1] : "chtl-project";
            return initializeProject(project_name);
        }
        
        case CommandType::BUILD: {
            std::string project_dir = args_.size() > 1 ? args_[1] : ".";
            return buildProject(project_dir);
        }
        
        case CommandType::CLEAN: {
            std::string project_dir = args_.size() > 1 ? args_[1] : ".";
            return cleanProject(project_dir);
        }
        
        case CommandType::SERVE: {
            int port = 8080;
            std::string root_dir = ".";
            
            // 解析端口参数
            for (size_t i = 1; i < args_.size(); ++i) {
                if (args_[i] == "-p" && i + 1 < args_.size()) {
                    try {
                        port = std::stoi(args_[i + 1]);
                    } catch (const std::exception& e) {
                        addError("Invalid port number: " + args_[i + 1]);
                        return 1;
                    }
                } else if (args_[i] == "-d" && i + 1 < args_.size()) {
                    root_dir = args_[i + 1];
                }
            }
            
            return startServer(port, root_dir);
        }
        
        case CommandType::HELP:
            return showHelp(args_.size() > 1 ? args_[1] : "");
            
        case CommandType::VERSION:
            return showVersion();
            
        default:
            addError("Unknown command type");
            return 1;
    }
}

int CHTLCli::compileFile(const std::string& input_file, const std::string& output_file) {
    if (!validateFilePath(input_file)) {
        addError("Input file does not exist: " + input_file);
        return 1;
    }
    
    // 读取输入文件
    std::ifstream file(input_file);
    if (!file.is_open()) {
        addError("Cannot open input file: " + input_file);
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source_code = buffer.str();
    file.close();
    
    // 设置编译器选项
    compiler_->setDebugMode(options_.debug);
    
    // 编译代码
    auto result = compiler_->compile(source_code);
    
    if (!result.success) {
        std::cerr << "Compilation failed:" << std::endl;
        for (const auto& error : result.errors) {
            std::cerr << "Error: " << error << std::endl;
        }
        return 1;
    }
    
    // 生成输出文件名
    std::string output_filename = output_file;
    if (output_filename.empty()) {
        std::string base_name = std::filesystem::path(input_file).stem().string();
        output_filename = options_.output_dir + "/" + base_name + ".html";
    }
    
    // 创建输出目录
    std::string output_dir = std::filesystem::path(output_filename).parent_path().string();
    if (!output_dir.empty() && !createOutputDirectory(output_dir)) {
        addError("Cannot create output directory: " + output_dir);
        return 1;
    }
    
    // 写入输出文件
    std::ofstream out_file(output_filename);
    if (!out_file.is_open()) {
        addError("Cannot create output file: " + output_filename);
        return 1;
    }
    
    out_file << result.html;
    out_file.close();
    
    if (options_.verbose) {
        std::cout << "Successfully compiled: " << input_file << " -> " << output_filename << std::endl;
    }
    
    return 0;
}

int CHTLCli::compileDirectory(const std::string& input_dir, const std::string& output_dir) {
    if (!std::filesystem::exists(input_dir)) {
        addError("Input directory does not exist: " + input_dir);
        return 1;
    }
    
    if (!createOutputDirectory(output_dir)) {
        addError("Cannot create output directory: " + output_dir);
        return 1;
    }
    
    // 扫描CHTL文件
    auto chtl_files = scanCHTLFiles(input_dir);
    
    int success_count = 0;
    int total_count = chtl_files.size();
    
    for (const auto& file : chtl_files) {
        std::string relative_path = std::filesystem::relative(file, input_dir).string();
        std::string output_file = output_dir + "/" + std::filesystem::path(relative_path).stem().string() + ".html";
        
        if (compileFile(file, output_file) == 0) {
            success_count++;
        }
    }
    
    std::cout << "Compiled " << success_count << "/" << total_count << " files" << std::endl;
    
    return success_count == total_count ? 0 : 1;
}

int CHTLCli::watchFiles(const std::string& input_path) {
    std::cout << "Watching files in: " << input_path << std::endl;
    std::cout << "Press Ctrl+C to stop..." << std::endl;
    
    // 简单的文件监视实现
    auto last_modified = std::filesystem::last_write_time(input_path);
    
    while (true) {
        try {
            auto current_modified = std::filesystem::last_write_time(input_path);
            if (current_modified != last_modified) {
                std::cout << "File changed: " << input_path << std::endl;
                
                if (std::filesystem::is_regular_file(input_path)) {
                    compileFile(input_path);
                } else if (std::filesystem::is_directory(input_path)) {
                    compileDirectory(input_path, options_.output_dir);
                }
                
                last_modified = current_modified;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        } catch (const std::exception& e) {
            addError("Error watching files: " + std::string(e.what()));
            return 1;
        }
    }
    
    return 0;
}

int CHTLCli::manageModule(const std::string& action, const std::string& module_name) {
    if (action == "list") {
        // 列出模块
        std::cout << "Available modules:" << std::endl;
        // 这里可以列出已安装的模块
        return 0;
    } else if (action == "install") {
        if (module_name.empty()) {
            addError("Module name required for install action");
            return 1;
        }
        
        std::cout << "Installing module: " << module_name << std::endl;
        // 这里可以实现模块安装逻辑
        return 0;
    } else if (action == "uninstall") {
        if (module_name.empty()) {
            addError("Module name required for uninstall action");
            return 1;
        }
        
        std::cout << "Uninstalling module: " << module_name << std::endl;
        // 这里可以实现模块卸载逻辑
        return 0;
    } else {
        addError("Unknown module action: " + action);
        return 1;
    }
}

int CHTLCli::initializeProject(const std::string& project_name) {
    std::cout << "Initializing CHTL project: " << project_name << std::endl;
    
    // 创建项目目录
    if (!createOutputDirectory(project_name)) {
        addError("Cannot create project directory: " + project_name);
        return 1;
    }
    
    // 创建基本项目结构
    std::string src_dir = project_name + "/src";
    std::string modules_dir = project_name + "/modules";
    std::string output_dir = project_name + "/dist";
    
    createOutputDirectory(src_dir);
    createOutputDirectory(modules_dir);
    createOutputDirectory(output_dir);
    
    // 创建示例文件
    std::string example_file = src_dir + "/index.chtl";
    std::ofstream file(example_file);
    if (file.is_open()) {
        file << R"(html {
    head {
        title { text { "CHTL Project" } }
    }
    body {
        div {
            text { "Hello from CHTL!" }
            style {
                color: blue;
                font-size: 24px;
                text-align: center;
                margin-top: 50px;
            }
        }
    }
})";
        file.close();
    }
    
    // 创建配置文件
    std::string config_file = project_name + "/chtl.config.json";
    std::ofstream config(config_file);
    if (config.is_open()) {
        config << R"({
    "name": ")" << project_name << R"(",
    "version": "1.0.0",
    "description": "CHTL Project",
    "output": "dist",
    "modules": "modules",
    "watch": false,
    "minify": false,
    "debug": false
})";
        config.close();
    }
    
    std::cout << "Project initialized successfully!" << std::endl;
    std::cout << "  Project directory: " << project_name << std::endl;
    std::cout << "  Source directory: " << src_dir << std::endl;
    std::cout << "  Modules directory: " << modules_dir << std::endl;
    std::cout << "  Output directory: " << output_dir << std::endl;
    
    return 0;
}

int CHTLCli::buildProject(const std::string& project_dir) {
    std::cout << "Building CHTL project: " << project_dir << std::endl;
    
    std::string src_dir = project_dir + "/src";
    std::string output_dir = project_dir + "/dist";
    
    return compileDirectory(src_dir, output_dir);
}

int CHTLCli::cleanProject(const std::string& project_dir) {
    std::cout << "Cleaning CHTL project: " << project_dir << std::endl;
    
    std::string output_dir = project_dir + "/dist";
    
    if (std::filesystem::exists(output_dir)) {
        try {
            std::filesystem::remove_all(output_dir);
            std::cout << "Cleaned output directory: " << output_dir << std::endl;
        } catch (const std::exception& e) {
            addError("Error cleaning project: " + std::string(e.what()));
            return 1;
        }
    }
    
    return 0;
}

int CHTLCli::startServer(int port, const std::string& root_dir) {
    std::cout << "Starting CHTL development server..." << std::endl;
    std::cout << "  Port: " << port << std::endl;
    std::cout << "  Root directory: " << root_dir << std::endl;
    std::cout << "  URL: http://localhost:" << port << std::endl;
    std::cout << "Press Ctrl+C to stop..." << std::endl;
    
    // 简单的HTTP服务器实现
    // 这里可以使用更完整的HTTP服务器库
    std::cout << "Development server started (simplified implementation)" << std::endl;
    
    return 0;
}

int CHTLCli::showHelp(const std::string& command) {
    if (command.empty()) {
        std::cout << "CHTL - CHTL Compiler and Development Tools" << std::endl;
        std::cout << "Version: 1.0.0" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: chtl <command> [options] [arguments]" << std::endl;
        std::cout << std::endl;
        std::cout << "Commands:" << std::endl;
        
        for (const auto& cmd : commands_) {
            std::cout << "  " << cmd.name;
            for (const auto& alias : cmd.aliases) {
                std::cout << ", " << alias;
            }
            std::cout << "    " << cmd.description << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -v, --verbose     Verbose output" << std::endl;
        std::cout << "  -d, --debug       Debug mode" << std::endl;
        std::cout << "  -w, --watch       Watch mode" << std::endl;
        std::cout << "  -m, --minify      Minify output" << std::endl;
        std::cout << "  -o, --output      Output file/directory" << std::endl;
        std::cout << "  -c, --config      Configuration file" << std::endl;
        std::cout << "  -h, --help        Show help" << std::endl;
        std::cout << "  --version         Show version" << std::endl;
    } else {
        const CLICommand* cmd = findCommand(command);
        if (cmd) {
            std::cout << "Command: " << cmd->name << std::endl;
            std::cout << "Description: " << cmd->description << std::endl;
            if (!cmd->aliases.empty()) {
                std::cout << "Aliases: ";
                for (size_t i = 0; i < cmd->aliases.size(); ++i) {
                    if (i > 0) std::cout << ", ";
                    std::cout << cmd->aliases[i];
                }
                std::cout << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
    
    return 0;
}

int CHTLCli::showVersion() {
    std::cout << "CHTL Compiler" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "C++ Standard: C++17" << std::endl;
    return 0;
}

void CHTLCli::setOptions(const CLIOptions& options) {
    options_ = options;
}

const CLIOptions& CHTLCli::getOptions() const {
    return options_;
}

void CHTLCli::setDebugMode(bool debug) {
    debug_mode_ = debug;
}

std::vector<std::string> CHTLCli::getErrors() const {
    return errors_;
}

void CHTLCli::clearErrors() {
    errors_.clear();
}

void CHTLCli::initializeCommands() {
    commands_ = {
        {CommandType::COMPILE, "compile", "Compile CHTL files", {"c"}, {}, {}},
        {CommandType::WATCH, "watch", "Watch files for changes", {"w"}, {}, {}},
        {CommandType::MODULE, "module", "Manage modules", {"m"}, {}, {}},
        {CommandType::INIT, "init", "Initialize a new project", {"i"}, {}, {}},
        {CommandType::BUILD, "build", "Build project", {"b"}, {}, {}},
        {CommandType::CLEAN, "clean", "Clean build files", {}, {}, {}},
        {CommandType::SERVE, "serve", "Start development server", {"s"}, {}, {}},
        {CommandType::HELP, "help", "Show help information", {"h"}, {}, {}},
        {CommandType::VERSION, "version", "Show version information", {"v"}, {}, {}}
    };
}

void CHTLCli::addError(const std::string& error) {
    errors_.push_back(error);
    if (debug_mode_) {
        std::cerr << "CLI Error: " << error << std::endl;
    }
}

bool CHTLCli::parseOption(const std::string& arg) {
    if (arg == "-v" || arg == "--verbose") {
        options_.verbose = true;
    } else if (arg == "-d" || arg == "--debug") {
        options_.debug = true;
    } else if (arg == "-w" || arg == "--watch") {
        options_.watch = true;
    } else if (arg == "-m" || arg == "--minify") {
        options_.minify = true;
    } else if (arg == "-o" || arg == "--output") {
        // 输出选项在parseArguments中处理
        return true;
    } else if (arg == "-c" || arg == "--config") {
        // 配置文件选项在parseArguments中处理
        return true;
    } else if (arg == "-h" || arg == "--help") {
        return true;
    } else if (arg == "--version") {
        return true;
    } else {
        addError("Unknown option: " + arg);
        return false;
    }
    
    return true;
}

const CLICommand* CHTLCli::findCommand(const std::string& name) {
    for (const auto& cmd : commands_) {
        if (cmd.name == name) {
            return &cmd;
        }
        for (const auto& alias : cmd.aliases) {
            if (alias == name) {
                return &cmd;
            }
        }
    }
    return nullptr;
}

bool CHTLCli::validateFilePath(const std::string& file_path) {
    return std::filesystem::exists(file_path);
}

bool CHTLCli::createOutputDirectory(const std::string& dir_path) {
    try {
        std::filesystem::create_directories(dir_path);
        return true;
    } catch (const std::exception& e) {
        addError("Cannot create directory " + dir_path + ": " + e.what());
        return false;
    }
}

std::string CHTLCli::getFileExtension(const std::string& file_path) {
    return std::filesystem::path(file_path).extension().string();
}

std::string CHTLCli::generateOutputFileName(const std::string& input_file, const std::string& extension) {
    std::string base_name = std::filesystem::path(input_file).stem().string();
    return options_.output_dir + "/" + base_name + extension;
}

bool CHTLCli::loadConfigFile(const std::string& config_file) {
    // 简单的配置文件加载实现
    std::ifstream file(config_file);
    if (!file.is_open()) {
        return false;
    }
    
    // 这里可以实现JSON配置文件解析
    file.close();
    return true;
}

bool CHTLCli::saveConfigFile(const std::string& config_file) {
    // 简单的配置文件保存实现
    std::ofstream file(config_file);
    if (!file.is_open()) {
        return false;
    }
    
    // 这里可以实现JSON配置文件生成
    file.close();
    return true;
}

std::vector<std::string> CHTLCli::scanCHTLFiles(const std::string& dir_path) {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".chtl" || ext == ".cmod") {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        addError("Error scanning directory " + dir_path + ": " + e.what());
    }
    
    return files;
}

void CHTLCli::watchFile(const std::string& file_path, std::function<void(const std::string&)> callback) {
    // 简单的文件监视实现
    // 这里可以使用更完整的文件监视库
}

} // namespace CHTL