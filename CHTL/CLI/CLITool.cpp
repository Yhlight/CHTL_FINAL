#include "CLITool.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include "../CodeMerger/CodeMerger.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace CHTL {

// CLIParser 实现
CLIParser::CLIParser() : programName("chtl"), version("1.0.0"), description("CHTL Compiler CLI Tool") {
}

void CLIParser::addOption(const Option& option) {
    options.push_back(option);
}

bool CLIParser::parse(int argc, char* argv[]) {
    if (argc < 2) {
        return false;
    }

    programName = argv[0];
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            return false;
        }
        
        if (arg == "--version" || arg == "-v") {
            return false;
        }
        
        // 查找匹配的选项
        bool found = false;
        for (const auto& option : options) {
            if (arg == "--" + option.name || arg == "-" + option.shortName) {
                if (option.hasValue && i + 1 < argc) {
                    values[option.name] = argv[++i];
                } else if (!option.hasValue) {
                    values[option.name] = "true";
                }
                found = true;
                break;
            }
        }
        
        if (!found && arg[0] == '-') {
            // 未知选项
            return false;
        }
    }
    
    // 检查必需选项
    for (const auto& option : options) {
        if (option.required && values.find(option.name) == values.end()) {
            return false;
        }
    }
    
    return true;
}

std::string CLIParser::getValue(const std::string& name) const {
    auto it = values.find(name);
    if (it != values.end()) {
        return it->second;
    }
    
    // 查找默认值
    for (const auto& option : options) {
        if (option.name == name) {
            return option.defaultValue;
        }
    }
    
    return "";
}

bool CLIParser::hasOption(const std::string& name) const {
    return values.find(name) != values.end();
}

void CLIParser::printHelp() const {
    std::cout << "用法: " << programName << " [选项] <输入文件> [输出文件]\n\n";
    std::cout << "描述: " << description << "\n\n";
    std::cout << "选项:\n";
    
    for (const auto& option : options) {
        std::cout << "  -" << option.shortName << ", --" << option.name;
        if (option.hasValue) {
            std::cout << " <值>";
        }
        std::cout << "    " << option.description;
        if (option.required) {
            std::cout << " (必需)";
        }
        std::cout << "\n";
    }
    
    std::cout << "\n示例:\n";
    std::cout << "  " << programName << " input.chtl output.html\n";
    std::cout << "  " << programName << " -o output.html input.chtl\n";
    std::cout << "  " << programName << " --verbose --debug input.chtl\n";
}

void CLIParser::printVersion() const {
    std::cout << programName << " 版本 " << version << "\n";
}

// ColorOutput 实现
void ColorOutput::setColor(std::ostream& os, Color color) {
    os << "\033[" << color << "m";
}

void ColorOutput::resetColor(std::ostream& os) {
    os << "\033[0m";
}

void ColorOutput::printColored(std::ostream& os, const std::string& text, Color color) {
    setColor(os, color);
    os << text;
    resetColor(os);
}

void ColorOutput::printError(const std::string& message) {
    printColored(std::cerr, "错误: " + message, RED);
    std::cerr << std::endl;
}

void ColorOutput::printSuccess(const std::string& message) {
    printColored(std::cout, "成功: " + message, GREEN);
    std::cout << std::endl;
}

void ColorOutput::printWarning(const std::string& message) {
    printColored(std::cout, "警告: " + message, YELLOW);
    std::cout << std::endl;
}

void ColorOutput::printInfo(const std::string& message) {
    printColored(std::cout, "信息: " + message, BLUE);
    std::cout << std::endl;
}

// FileProcessor 实现
bool FileProcessor::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string FileProcessor::readFile(const std::string& path) {
    if (path.empty()) {
        throw std::invalid_argument("文件路径不能为空");
    }
    
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path + " (文件不存在或权限不足)");
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    
    if (file.fail() && !file.eof()) {
        throw std::runtime_error("读取文件时发生错误: " + path);
    }
    
    return buffer.str();
}

bool FileProcessor::writeFile(const std::string& path, const std::string& content) {
    if (path.empty()) {
        return false;
    }
    
    // 确保目录存在
    std::string dir = getDirectory(path);
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        if (!createDirectory(dir)) {
            return false;
        }
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    
    if (file.fail()) {
        return false;
    }
    
    return true;
}

std::string FileProcessor::getFileExtension(const std::string& path) {
    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return path.substr(pos + 1);
}

std::string FileProcessor::getFileName(const std::string& path) {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        pos = path.find_last_of('\\');
    }
    if (pos == std::string::npos) {
        return path;
    }
    return path.substr(pos + 1);
}

std::string FileProcessor::getDirectory(const std::string& path) {
    size_t pos = path.find_last_of('/');
    if (pos == std::string::npos) {
        pos = path.find_last_of('\\');
    }
    if (pos == std::string::npos) {
        return ".";
    }
    return path.substr(0, pos);
}

bool FileProcessor::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (...) {
        return false;
    }
}

std::vector<std::string> FileProcessor::listFiles(const std::string& directory, const std::string& extension) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (extension.empty() || getFileExtension(filePath) == extension) {
                    files.push_back(filePath);
                }
            }
        }
    } catch (...) {
        // 忽略错误
    }
    return files;
}

// ProgressBar 实现
ProgressBar::ProgressBar(int total, int width) : total(total), width(width), current(0), finished(false) {
}

void ProgressBar::update(int current) {
    this->current = current;
    if (finished) return;
    
    float progress = static_cast<float>(current) / total;
    int pos = static_cast<int>(width * progress);
    
    // 只在进度变化足够大时才更新显示
    static int lastPos = -1;
    if (pos != lastPos || current == total) {
        std::cout << "\r[";
        for (int i = 0; i < width; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
        std::cout.flush();
        lastPos = pos;
    }
}

void ProgressBar::finish() {
    if (finished) return;
    finished = true;
    std::cout << "\r[";
    for (int i = 0; i < width; ++i) {
        std::cout << "=";
    }
    std::cout << "] 100.0%\n";
}

void ProgressBar::reset() {
    current = 0;
    finished = false;
}

// CLITool 实现
CLITool::CLITool() : version("1.0.0"), description("CHTL Compiler CLI Tool"), 
    verbose(false), debug(false), force(false), watch(false), server(false), 
    interactive(false), benchmark(false), validate(false), format(false), 
    minify(false), beautify(false), outputFormat("html"), serverPort(8080) {
    initializeParser();
}

void CLITool::initializeParser() {
    parser = std::make_unique<CLIParser>();
    
    // 添加基本选项
    parser->addOption({"input", "i", "输入文件", true, "", true});
    parser->addOption({"output", "o", "输出文件", true, "", false});
    parser->addOption({"verbose", "v", "详细输出", false, "false", false});
    parser->addOption({"debug", "d", "调试模式", false, "false", false});
    parser->addOption({"force", "f", "强制覆盖", false, "false", false});
    parser->addOption({"config", "c", "配置文件", true, "", false});
    parser->addOption({"help", "h", "显示帮助", false, "false", false});
    parser->addOption({"version", "V", "显示版本", false, "false", false});
}

int CLITool::run(int argc, char* argv[]) {
    try {
        if (!parser->parse(argc, argv)) {
            if (argc > 1 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h")) {
                parser->printHelp();
                return 0;
            }
            if (argc > 1 && (std::string(argv[1]) == "--version" || std::string(argv[1]) == "-v")) {
                parser->printVersion();
                return 0;
            }
            parser->printHelp();
            return 1;
        }
        
        // 获取选项值
        verbose = parser->hasOption("verbose");
        debug = parser->hasOption("debug");
        force = parser->hasOption("force");
        configFile = parser->getValue("config");
        
        std::string inputFile = parser->getValue("input");
        std::string outputFile = parser->getValue("output");
        
        if (inputFile.empty()) {
            ColorOutput::printError("未指定输入文件");
            return 1;
        }
        
        if (!FileProcessor::fileExists(inputFile)) {
            ColorOutput::printError("输入文件不存在: " + inputFile);
            return 1;
        }
        
        // 确定输出文件
        if (outputFile.empty()) {
            std::string ext = FileProcessor::getFileExtension(inputFile);
            if (ext == "chtl") {
                outputFile = inputFile.substr(0, inputFile.length() - 6) + ".html";
            } else {
                outputFile = inputFile + ".html";
            }
        }
        
        // 检查输出文件是否存在
        if (FileProcessor::fileExists(outputFile) && !force) {
            ColorOutput::printWarning("输出文件已存在: " + outputFile);
            ColorOutput::printInfo("使用 --force 选项强制覆盖");
            return 1;
        }
        
        // 处理文件
        processFile(inputFile, outputFile);
        
        ColorOutput::printSuccess("编译完成: " + outputFile);
        return 0;
        
    } catch (const std::exception& e) {
        ColorOutput::printError(e.what());
        return 1;
    }
}

void CLITool::processFile(const std::string& inputFile, const std::string& outputFile) {
    if (verbose) {
        ColorOutput::printInfo("开始处理文件: " + inputFile);
    }
    
    // 读取输入文件
    std::string inputContent = FileProcessor::readFile(inputFile);
    
    // 创建编译器调度器
    CompilerDispatcher dispatcher;
    dispatcher.setDebugMode(debug);
    
    // 编译文件
    auto result = dispatcher.compileFile(inputFile);
    
    if (!result.metadata["success"].empty() && result.metadata["success"] != "true") {
        throw std::runtime_error("编译失败");
    }
    
    // 创建代码合并器
    CodeMerger merger;
    merger.setDebugMode(debug);
    merger.setTitle(FileProcessor::getFileName(inputFile));
    
    // 合并代码
    CompileResult compileResult;
    compileResult.outputs["html"] = result.html;
    compileResult.outputs["css"] = result.css;
    compileResult.outputs["javascript"] = result.javascript;
    compileResult.errors = result.errors;
    compileResult.warnings = result.warnings;
    compileResult.metadata = result.metadata;
    
    auto finalResult = merger.merge(compileResult);
    
    // 写入输出文件
    if (!FileProcessor::writeFile(outputFile, finalResult.html)) {
        throw std::runtime_error("无法写入输出文件: " + outputFile);
    }
    
    if (verbose) {
        ColorOutput::printInfo("文件处理完成: " + outputFile);
    }
}

void CLITool::setVersion(const std::string& version) {
    this->version = version;
}

void CLITool::setDescription(const std::string& description) {
    this->description = description;
}

// 新增方法实现
void CLITool::processBatch(const std::vector<std::string>& files) {
    if (verbose) {
        ColorOutput::printInfo("开始批量处理 " + std::to_string(files.size()) + " 个文件");
    }
    
    ProgressBar progress(files.size());
    int successCount = 0;
    int errorCount = 0;
    
    for (const auto& file : files) {
        try {
            std::string outputFile = file;
            if (outputFile.length() >= 6 && outputFile.substr(outputFile.length() - 6) == ".chtl") {
                outputFile = outputFile.substr(0, outputFile.length() - 6) + ".html";
            } else {
                outputFile += ".html";
            }
            
            processFile(file, outputFile);
            successCount++;
            
            if (verbose) {
                ColorOutput::printSuccess("✓ " + file + " -> " + outputFile);
            }
        } catch (const std::exception& e) {
            errorCount++;
            ColorOutput::printError("✗ " + file + ": " + e.what());
        }
        
        progress.update(successCount + errorCount);
    }
    
    progress.finish();
    
    ColorOutput::printInfo("批量处理完成: " + std::to_string(successCount) + " 成功, " + 
                          std::to_string(errorCount) + " 失败");
}

void CLITool::showInfo() {
    ColorOutput::printInfo("CHTL Compiler CLI Tool");
    ColorOutput::printInfo("版本: " + version);
    ColorOutput::printInfo("描述: " + description);
    ColorOutput::printInfo("构建时间: " + std::string(__DATE__) + " " + std::string(__TIME__));
    ColorOutput::printInfo("编译器: " + std::string(__VERSION__));
    ColorOutput::printInfo("平台: Linux");
    ColorOutput::printInfo("架构: x86_64");
}

void CLITool::showExamples() {
    ColorOutput::printInfo("CHTL Compiler CLI Tool - 使用示例");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("基本用法:");
    ColorOutput::printInfo("  chtl-cli input.chtl                    # 编译单个文件");
    ColorOutput::printInfo("  chtl-cli input.chtl -o output.html     # 指定输出文件");
    ColorOutput::printInfo("  chtl-cli input.chtl -v                 # 详细输出");
    ColorOutput::printInfo("  chtl-cli input.chtl -d                 # 调试模式");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("批量处理:");
    ColorOutput::printInfo("  chtl-cli *.chtl                        # 编译所有.chtl文件");
    ColorOutput::printInfo("  chtl-cli -f *.chtl                     # 强制覆盖输出文件");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("高级功能:");
    ColorOutput::printInfo("  chtl-cli --watch input.chtl            # 监视模式");
    ColorOutput::printInfo("  chtl-cli --server --port 8080          # 服务器模式");
    ColorOutput::printInfo("  chtl-cli --interactive                 # 交互模式");
    ColorOutput::printInfo("  chtl-cli --benchmark input.chtl        # 基准测试");
    ColorOutput::printInfo("  chtl-cli --validate input.chtl         # 验证模式");
    ColorOutput::printInfo("  chtl-cli --format input.chtl           # 格式化");
    ColorOutput::printInfo("  chtl-cli --minify input.chtl           # 压缩");
    ColorOutput::printInfo("  chtl-cli --beautify input.chtl         # 美化");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("配置选项:");
    ColorOutput::printInfo("  chtl-cli -c config.json input.chtl     # 使用配置文件");
    ColorOutput::printInfo("  chtl-cli --output-format html input.chtl  # 指定输出格式");
    ColorOutput::printInfo("  chtl-cli --output-dir ./dist input.chtl    # 指定输出目录");
}

void CLITool::showLicense() {
    ColorOutput::printInfo("CHTL Compiler CLI Tool - 许可证");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("MIT License");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("Copyright (c) 2024 CHTL Project");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("Permission is hereby granted, free of charge, to any person obtaining a copy");
    ColorOutput::printInfo("of this software and associated documentation files (the \"Software\"), to deal");
    ColorOutput::printInfo("in the Software without restriction, including without limitation the rights");
    ColorOutput::printInfo("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
    ColorOutput::printInfo("copies of the Software, and to permit persons to whom the Software is");
    ColorOutput::printInfo("furnished to do so, subject to the following conditions:");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("The above copyright notice and this permission notice shall be included in all");
    ColorOutput::printInfo("copies or substantial portions of the Software.");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
    ColorOutput::printInfo("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
    ColorOutput::printInfo("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE");
    ColorOutput::printInfo("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
    ColorOutput::printInfo("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
    ColorOutput::printInfo("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
    ColorOutput::printInfo("SOFTWARE.");
}

void CLITool::showCredits() {
    ColorOutput::printInfo("CHTL Compiler CLI Tool - 致谢");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("开发团队:");
    ColorOutput::printInfo("  - 核心开发: CHTL Project Team");
    ColorOutput::printInfo("  - 架构设计: CHTL Architecture Team");
    ColorOutput::printInfo("  - 测试团队: CHTL Testing Team");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("特别感谢:");
    ColorOutput::printInfo("  - 开源社区的支持和贡献");
    ColorOutput::printInfo("  - 用户反馈和建议");
    ColorOutput::printInfo("  - 测试用户的耐心和帮助");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("技术栈:");
    ColorOutput::printInfo("  - C++17 标准库");
    ColorOutput::printInfo("  - CMake 构建系统");
    ColorOutput::printInfo("  - 现代C++特性");
    ColorOutput::printInfo("  - 跨平台支持");
}

void CLITool::watchMode() {
    ColorOutput::printInfo("监视模式启动...");
    ColorOutput::printInfo("按 Ctrl+C 退出监视模式");
    
    // 这里应该实现文件监视逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("监视模式功能正在开发中...");
}

void CLITool::serverMode() {
    ColorOutput::printInfo("服务器模式启动...");
    ColorOutput::printInfo("服务器端口: " + std::to_string(serverPort));
    ColorOutput::printInfo("按 Ctrl+C 停止服务器");
    
    // 这里应该实现HTTP服务器逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("服务器模式功能正在开发中...");
}

void CLITool::interactiveMode() {
    ColorOutput::printInfo("交互模式启动...");
    ColorOutput::printInfo("输入 'help' 查看可用命令，输入 'exit' 退出");
    
    std::string input;
    while (true) {
        ColorOutput::printInfo("chtl> ");
        std::getline(std::cin, input);
        
        if (input == "exit" || input == "quit") {
            break;
        } else if (input == "help") {
            showExamples();
        } else if (input == "version") {
            showInfo();
        } else if (input == "license") {
            showLicense();
        } else if (input == "credits") {
            showCredits();
        } else if (input.empty()) {
            continue;
        } else {
            ColorOutput::printWarning("未知命令: " + input);
            ColorOutput::printInfo("输入 'help' 查看可用命令");
        }
    }
    
    ColorOutput::printInfo("交互模式已退出");
}

void CLITool::benchmarkMode() {
    ColorOutput::printInfo("基准测试模式启动...");
    
    // 这里应该实现基准测试逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("基准测试模式功能正在开发中...");
}

void CLITool::validateMode() {
    ColorOutput::printInfo("验证模式启动...");
    
    // 这里应该实现验证逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("验证模式功能正在开发中...");
}

void CLITool::formatMode() {
    ColorOutput::printInfo("格式化模式启动...");
    
    // 这里应该实现格式化逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("格式化模式功能正在开发中...");
}

void CLITool::minifyMode() {
    ColorOutput::printInfo("压缩模式启动...");
    
    // 这里应该实现压缩逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("压缩模式功能正在开发中...");
}

void CLITool::beautifyMode() {
    ColorOutput::printInfo("美化模式启动...");
    
    // 这里应该实现美化逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("美化模式功能正在开发中...");
}

void CLITool::loadConfig(const std::string& configFile) {
    if (verbose) {
        ColorOutput::printInfo("加载配置文件: " + configFile);
    }
    
    // 这里应该实现配置文件加载逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("配置文件加载功能正在开发中...");
}

void CLITool::saveConfig(const std::string& configFile) {
    if (verbose) {
        ColorOutput::printInfo("保存配置文件: " + configFile);
    }
    
    // 这里应该实现配置文件保存逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("配置文件保存功能正在开发中...");
}

void CLITool::resetConfig() {
    if (verbose) {
        ColorOutput::printInfo("重置配置到默认值");
    }
    
    // 重置所有配置到默认值
    verbose = false;
    debug = false;
    force = false;
    watch = false;
    server = false;
    interactive = false;
    benchmark = false;
    validate = false;
    format = false;
    minify = false;
    beautify = false;
    outputFormat = "html";
    outputDir = "";
    serverPort = 8080;
    logFile = "";
    errorFile = "";
    
    ColorOutput::printSuccess("配置已重置");
}

void CLITool::showConfigHelp() {
    ColorOutput::printInfo("CHTL Compiler CLI Tool - 配置帮助");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("配置文件格式: JSON");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("可用配置项:");
    ColorOutput::printInfo("  verbose: true/false          # 详细输出");
    ColorOutput::printInfo("  debug: true/false            # 调试模式");
    ColorOutput::printInfo("  force: true/false            # 强制覆盖");
    ColorOutput::printInfo("  watch: true/false            # 监视模式");
    ColorOutput::printInfo("  server: true/false           # 服务器模式");
    ColorOutput::printInfo("  interactive: true/false      # 交互模式");
    ColorOutput::printInfo("  benchmark: true/false        # 基准测试");
    ColorOutput::printInfo("  validate: true/false         # 验证模式");
    ColorOutput::printInfo("  format: true/false           # 格式化");
    ColorOutput::printInfo("  minify: true/false           # 压缩");
    ColorOutput::printInfo("  beautify: true/false         # 美化");
    ColorOutput::printInfo("  outputFormat: string         # 输出格式");
    ColorOutput::printInfo("  outputDir: string            # 输出目录");
    ColorOutput::printInfo("  serverPort: number           # 服务器端口");
    ColorOutput::printInfo("  logFile: string              # 日志文件");
    ColorOutput::printInfo("  errorFile: string            # 错误文件");
    ColorOutput::printInfo("");
    ColorOutput::printInfo("示例配置文件:");
    ColorOutput::printInfo("{");
    ColorOutput::printInfo("  \"verbose\": true,");
    ColorOutput::printInfo("  \"debug\": false,");
    ColorOutput::printInfo("  \"force\": false,");
    ColorOutput::printInfo("  \"outputFormat\": \"html\",");
    ColorOutput::printInfo("  \"outputDir\": \"./dist\",");
    ColorOutput::printInfo("  \"serverPort\": 8080");
    ColorOutput::printInfo("}");
}

void CLITool::setupOutput() {
    if (!outputDir.empty()) {
        if (!FileProcessor::fileExists(outputDir)) {
            if (!FileProcessor::writeFile(outputDir + "/.chtl_dir", "")) {
                ColorOutput::printError("无法创建输出目录: " + outputDir);
                return;
            }
        }
    }
    
    if (!logFile.empty()) {
        // 设置日志文件输出
        ColorOutput::printInfo("日志文件: " + logFile);
    }
    
    if (!errorFile.empty()) {
        // 设置错误文件输出
        ColorOutput::printInfo("错误文件: " + errorFile);
    }
}

void CLITool::cleanupOutput() {
    if (verbose) {
        ColorOutput::printInfo("清理输出文件...");
    }
    
    // 这里应该实现输出清理逻辑
    // 由于这是一个复杂的实现，这里提供占位符
    ColorOutput::printWarning("输出清理功能正在开发中...");
}

void CLITool::redirectOutput() {
    if (!logFile.empty()) {
        // 重定向标准输出到日志文件
        ColorOutput::printInfo("重定向输出到日志文件: " + logFile);
    }
    
    if (!errorFile.empty()) {
        // 重定向标准错误到错误文件
        ColorOutput::printInfo("重定向错误到错误文件: " + errorFile);
    }
}

// Renderer 实现
Renderer::Renderer() : mode(TEXT), theme("default"), width(80), height(24), interactive(false) {
}

void Renderer::setMode(RenderMode mode) {
    this->mode = mode;
}

void Renderer::setTheme(const std::string& theme) {
    this->theme = theme;
}

void Renderer::setWidth(int width) {
    this->width = width;
}

void Renderer::setHeight(int height) {
    this->height = height;
}

std::string Renderer::render(const std::string& content) {
    switch (mode) {
        case TEXT:
            return renderText(content);
        case HTML:
            return renderHTML(content);
        case MARKDOWN:
            return renderMarkdown(content);
        case JSON:
            return renderJSON(content);
        case XML:
            return renderXML(content);
        case YAML:
            return renderYAML(content);
        default:
            return content;
    }
}

std::string Renderer::renderFile(const std::string& filePath) {
    std::string content = FileProcessor::readFile(filePath);
    if (content.empty()) {
        return "";
    }
    
    return render(content);
}

std::string Renderer::renderDirectory(const std::string& dirPath) {
    std::vector<std::string> files = FileProcessor::listFiles(dirPath, ".chtl");
    std::string result;
    
    for (const auto& file : files) {
        std::string content = renderFile(file);
        if (!content.empty()) {
            result += content + "\n\n";
        }
    }
    
    return result;
}

void Renderer::startInteractive() {
    interactive = true;
    // 启动交互式渲染
}

void Renderer::stopInteractive() {
    interactive = false;
    // 停止交互式渲染
}

bool Renderer::isInteractive() const {
    return interactive;
}

void Renderer::setConfig(const std::map<std::string, std::string>& config) {
    this->config = config;
}

std::map<std::string, std::string> Renderer::getConfig() const {
    return config;
}

std::map<std::string, int> Renderer::getStatistics() const {
    return statistics;
}

std::string Renderer::getReport() const {
    std::ostringstream oss;
    oss << "Renderer Report" << std::endl;
    oss << "Mode: " << static_cast<int>(mode) << std::endl;
    oss << "Theme: " << theme << std::endl;
    oss << "Width: " << width << std::endl;
    oss << "Height: " << height << std::endl;
    oss << "Interactive: " << (interactive ? "Yes" : "No") << std::endl;
    return oss.str();
}

std::string Renderer::renderText(const std::string& content) {
    // 文本渲染
    return content;
}

std::string Renderer::renderHTML(const std::string& content) {
    // HTML渲染
    std::ostringstream oss;
    oss << "<!DOCTYPE html>" << std::endl;
    oss << "<html>" << std::endl;
    oss << "<head><title>CHTL Output</title></head>" << std::endl;
    oss << "<body>" << std::endl;
    oss << "<pre>" << content << "</pre>" << std::endl;
    oss << "</body>" << std::endl;
    oss << "</html>" << std::endl;
    return oss.str();
}

std::string Renderer::renderMarkdown(const std::string& content) {
    // Markdown渲染
    return content;
}

std::string Renderer::renderJSON(const std::string& content) {
    // JSON渲染
    return content;
}

std::string Renderer::renderXML(const std::string& content) {
    // XML渲染
    return content;
}

std::string Renderer::renderYAML(const std::string& content) {
    // YAML渲染
    return content;
}

} // namespace CHTL