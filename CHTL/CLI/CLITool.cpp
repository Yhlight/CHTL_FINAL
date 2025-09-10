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
CLITool::CLITool() : version("1.0.0"), description("CHTL Compiler CLI Tool"), verbose(false), debug(false), force(false) {
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
    
    if (!result.success) {
        throw std::runtime_error("编译失败");
    }
    
    // 创建代码合并器
    CodeMerger merger;
    merger.setDebugMode(debug);
    merger.setTitle(FileProcessor::getFileName(inputFile));
    
    // 合并代码
    auto finalResult = merger.merge(result.html, result.css, result.javascript);
    
    // 写入输出文件
    if (!FileProcessor::writeFile(outputFile, finalResult)) {
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

} // namespace CHTL