#include "CHTL/CHTLCli.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// CliArgument 实现
CliArgument::CliArgument(const std::string& name, const std::string& description, 
                         CliArgType type, bool required, const std::string& defaultValue)
    : m_name(name), m_description(description), m_type(type), m_required(required), m_defaultValue(defaultValue) {
}

// CliOption 实现
CliOption::CliOption(const std::string& shortName, const std::string& longName, 
                     const std::string& description, CliArgType type, bool required, const std::string& defaultValue)
    : m_shortName(shortName), m_longName(longName), m_description(description), 
      m_type(type), m_required(required), m_defaultValue(defaultValue), m_isSet(false) {
}

// CliParser 实现
CliParser::CliParser(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void CliParser::addArgument(const std::string& name, const std::string& description, 
                           CliArgType type, bool required, const std::string& defaultValue) {
    auto argument = std::make_shared<CliArgument>(name, description, type, required, defaultValue);
    m_arguments[name] = argument;
}

void CliParser::addOption(const std::string& shortName, const std::string& longName, 
                         const std::string& description, CliArgType type, bool required, const std::string& defaultValue) {
    auto option = std::make_shared<CliOption>(shortName, longName, description, type, required, defaultValue);
    m_options[shortName] = option;
    m_options[longName] = option;
}

bool CliParser::parse(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return parse(args);
}

bool CliParser::parse(const std::vector<std::string>& args) {
    m_errors.clear();
    
    size_t argIndex = 0;
    
    for (size_t i = 0; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg.starts_with("-")) {
            // 处理选项
            std::string optionName = arg;
            std::string value = "";
            
            if (i + 1 < args.size() && !args[i + 1].starts_with("-")) {
                value = args[i + 1];
                ++i; // 跳过值
            }
            
            if (!parseOption(optionName, value)) {
                return false;
            }
        } else {
            // 处理参数
            if (!parseArgument(arg)) {
                return false;
            }
            ++argIndex;
        }
    }
    
    return validate();
}

bool CliParser::parseArgument(const std::string& arg) {
    // 按顺序分配参数
    size_t argCount = 0;
    for (const auto& [name, argument] : m_arguments) {
        if (argCount == 0) {
            argument->setValue(arg);
            return true;
        }
        ++argCount;
    }
    
    m_errors.push_back("Too many arguments: " + arg);
    return false;
}

bool CliParser::parseOption(const std::string& option, const std::string& value) {
    auto it = m_options.find(option);
    if (it == m_options.end()) {
        m_errors.push_back("Unknown option: " + option);
        return false;
    }
    
    auto cliOption = it->second;
    cliOption->setSet(true);
    
    if (cliOption->getType() != CliArgType::BOOLEAN && value.empty()) {
        m_errors.push_back("Option " + option + " requires a value");
        return false;
    }
    
    if (!value.empty()) {
        cliOption->setValue(value);
    }
    
    return true;
}

std::string CliParser::getArgument(const std::string& name) const {
    auto it = m_arguments.find(name);
    if (it != m_arguments.end()) {
        return it->second->getValue();
    }
    return "";
}

std::string CliParser::getOption(const std::string& name) const {
    auto it = m_options.find(name);
    if (it != m_options.end()) {
        return it->second->getValue();
    }
    return "";
}

bool CliParser::hasOption(const std::string& name) const {
    auto it = m_options.find(name);
    return it != m_options.end() && it->second->isSet();
}

bool CliParser::hasArgument(const std::string& name) const {
    auto it = m_arguments.find(name);
    return it != m_arguments.end() && it->second->hasValue();
}

bool CliParser::validate() const {
    m_errors.clear();
    
    // 验证必需参数
    for (const auto& [name, argument] : m_arguments) {
        if (argument->isRequired() && !argument->hasValue()) {
            m_errors.push_back("Required argument missing: " + name);
        }
    }
    
    // 验证必需选项
    for (const auto& [name, option] : m_options) {
        if (option->isRequired() && !option->isSet()) {
            m_errors.push_back("Required option missing: " + name);
        }
    }
    
    return m_errors.empty();
}

std::vector<std::string> CliParser::getErrors() const {
    return m_errors;
}

std::string CliParser::generateHelp() const {
    std::stringstream ss;
    
    ss << "Usage: chtl [options] [arguments]\n\n";
    
    if (!m_arguments.empty()) {
        ss << "Arguments:\n";
        for (const auto& [name, argument] : m_arguments) {
            ss << "  " << name << " " << (argument->isRequired() ? "(required)" : "(optional)") << "\n";
            ss << "    " << argument->getDescription() << "\n";
        }
        ss << "\n";
    }
    
    if (!m_options.empty()) {
        ss << "Options:\n";
        for (const auto& [name, option] : m_options) {
            if (name.starts_with("--")) {
                ss << "  " << name;
                if (!option->getShortName().empty()) {
                    ss << ", " << option->getShortName();
                }
                ss << " " << (option->isRequired() ? "(required)" : "(optional)") << "\n";
                ss << "    " << option->getDescription() << "\n";
            }
        }
    }
    
    return ss.str();
}

std::string CliParser::generateUsage() const {
    std::stringstream ss;
    
    ss << "Usage: chtl";
    
    for (const auto& [name, option] : m_options) {
        if (name.starts_with("--")) {
            ss << " [" << name << "]";
        }
    }
    
    for (const auto& [name, argument] : m_arguments) {
        ss << " <" << name << ">";
    }
    
    ss << "\n";
    
    return ss.str();
}

// CliProcessor 实现
CliProcessor::CliProcessor(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

void CliProcessor::processCommand(const std::string& command, const std::vector<std::string>& args) {
    if (command == "compile") {
        if (!args.empty()) {
            compileFile(args[0], args.size() > 1 ? args[1] : "");
        }
    } else if (command == "validate") {
        if (!args.empty()) {
            validateFile(args[0]);
        }
    } else if (command == "format") {
        if (!args.empty()) {
            formatFile(args[0], args.size() > 1 ? args[1] : "");
        }
    } else if (command == "module") {
        if (args.size() >= 2) {
            createModule(args[1], args[0]);
        }
    }
}

void CliProcessor::processFile(const std::string& inputFile, const std::string& outputFile) {
    if (!std::filesystem::exists(inputFile)) {
        m_context->addError("Input file does not exist: " + inputFile);
        return;
    }
    
    std::string output = getOutputPath(inputFile, outputFile);
    compileFile(inputFile, output);
}

void CliProcessor::processDirectory(const std::string& inputDir, const std::string& outputDir) {
    if (!std::filesystem::exists(inputDir)) {
        m_context->addError("Input directory does not exist: " + inputDir);
        return;
    }
    
    std::string output = outputDir.empty() ? inputDir + "_output" : outputDir;
    compileDirectory(inputDir, output);
}

void CliProcessor::compileFile(const std::string& inputFile, const std::string& outputFile) {
    if (!isCHTLFile(inputFile)) {
        m_context->addError("Not a CHTL file: " + inputFile);
        return;
    }
    
    std::string output = getOutputPath(inputFile, outputFile);
    
    // 这里需要实现实际的编译逻辑
    std::cout << "Compiling " << inputFile << " to " << output << std::endl;
    
    // 创建输出目录
    std::filesystem::path outputPath(output);
    std::filesystem::create_directories(outputPath.parent_path());
    
    // 编译文件
    // TODO: 实现实际的编译逻辑
}

void CliProcessor::compileDirectory(const std::string& inputDir, const std::string& outputDir) {
    auto files = findCHTLFiles(inputDir);
    
    for (const auto& file : files) {
        std::string relativePath = std::filesystem::relative(file, inputDir).string();
        std::string outputFile = outputDir + "/" + relativePath;
        
        // 创建输出目录
        std::filesystem::path outputPath(outputFile);
        std::filesystem::create_directories(outputPath.parent_path());
        
        compileFile(file, outputFile);
    }
}

void CliProcessor::validateFile(const std::string& inputFile) {
    if (!isCHTLFile(inputFile)) {
        m_context->addError("Not a CHTL file: " + inputFile);
        return;
    }
    
    std::cout << "Validating " << inputFile << std::endl;
    
    // TODO: 实现实际的验证逻辑
}

void CliProcessor::validateDirectory(const std::string& inputDir) {
    auto files = findCHTLFiles(inputDir);
    
    for (const auto& file : files) {
        validateFile(file);
    }
}

void CliProcessor::formatFile(const std::string& inputFile, const std::string& outputFile) {
    if (!isCHTLFile(inputFile)) {
        m_context->addError("Not a CHTL file: " + inputFile);
        return;
    }
    
    std::string output = getOutputPath(inputFile, outputFile);
    
    std::cout << "Formatting " << inputFile << " to " << output << std::endl;
    
    // TODO: 实现实际的格式化逻辑
}

void CliProcessor::formatDirectory(const std::string& inputDir, const std::string& outputDir) {
    auto files = findCHTLFiles(inputDir);
    
    for (const auto& file : files) {
        std::string relativePath = std::filesystem::relative(file, inputDir).string();
        std::string outputFile = outputDir + "/" + relativePath;
        
        formatFile(file, outputFile);
    }
}

void CliProcessor::createModule(const std::string& moduleName, const std::string& moduleType) {
    std::cout << "Creating " << moduleType << " module: " << moduleName << std::endl;
    
    // TODO: 实现实际的模块创建逻辑
}

void CliProcessor::buildModule(const std::string& modulePath) {
    std::cout << "Building module: " << modulePath << std::endl;
    
    // TODO: 实现实际的模块构建逻辑
}

void CliProcessor::installModule(const std::string& modulePath) {
    std::cout << "Installing module: " << modulePath << std::endl;
    
    // TODO: 实现实际的模块安装逻辑
}

std::string CliProcessor::getOutputPath(const std::string& inputPath, const std::string& outputPath) const {
    if (!outputPath.empty()) {
        return outputPath;
    }
    
    std::filesystem::path path(inputPath);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();
    
    if (extension == ".chtl") {
        return stem + ".html";
    } else if (extension == ".cmod") {
        return stem + ".cmod";
    } else if (extension == ".cjmod") {
        return stem + ".cjmod";
    }
    
    return stem + ".html";
}

bool CliProcessor::isCHTLFile(const std::string& filePath) const {
    return getFileExtension(filePath) == ".chtl";
}

bool CliProcessor::isCMODFile(const std::string& filePath) const {
    return getFileExtension(filePath) == ".cmod";
}

bool CliProcessor::isCJMODFile(const std::string& filePath) const {
    return getFileExtension(filePath) == ".cjmod";
}

std::string CliProcessor::getFileExtension(const std::string& filePath) const {
    std::filesystem::path path(filePath);
    return path.extension().string();
}

std::vector<std::string> CliProcessor::findCHTLFiles(const std::string& directory) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filePath = entry.path().string();
                if (isCHTLFile(filePath) || isCMODFile(filePath) || isCJMODFile(filePath)) {
                    files.push_back(filePath);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        m_context->addError("Error reading directory: " + std::string(e.what()));
    }
    
    return files;
}

// CliApplication 实现
CliApplication::CliApplication(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_parser = std::make_unique<CliParser>(context);
    m_processor = std::make_unique<CliProcessor>(context);
}

void CliApplication::initialize() {
    registerDefaultCommands();
    
    // 注册默认参数和选项
    m_parser->addArgument("command", "Command to execute", CliArgType::STRING, true);
    m_parser->addOption("-h", "--help", "Show help information", CliArgType::BOOLEAN);
    m_parser->addOption("-v", "--version", "Show version information", CliArgType::BOOLEAN);
    m_parser->addOption("-o", "--output", "Output file or directory", CliArgType::FILE);
    m_parser->addOption("-w", "--watch", "Watch for file changes", CliArgType::BOOLEAN);
    m_parser->addOption("", "--default-struct", "Generate HTML with default structure", CliArgType::BOOLEAN);
}

void CliApplication::run(int argc, char* argv[]) {
    if (!m_parser->parse(argc, argv)) {
        std::cerr << "Error parsing command line arguments:" << std::endl;
        for (const auto& error : m_parser->getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return;
    }
    
    if (m_parser->hasOption("--help")) {
        showHelp();
        return;
    }
    
    if (m_parser->hasOption("--version")) {
        showVersion();
        return;
    }
    
    std::string command = m_parser->getArgument("command");
    if (command.empty()) {
        showUsage();
        return;
    }
    
    // 收集剩余参数
    std::vector<std::string> args;
    // TODO: 实现参数收集逻辑
    
    executeCommand(command, args);
}

void CliApplication::shutdown() {
    // 清理资源
}

void CliApplication::registerCommand(const std::string& name, const std::string& description, 
                                   std::function<void(const std::vector<std::string>&)> handler) {
    m_commands[name] = handler;
    m_commandDescriptions[name] = description;
}

void CliApplication::registerDefaultCommands() {
    registerCommand("compile", "Compile CHTL files", [this](const std::vector<std::string>& args) { cmdCompile(args); });
    registerCommand("validate", "Validate CHTL files", [this](const std::vector<std::string>& args) { cmdValidate(args); });
    registerCommand("format", "Format CHTL files", [this](const std::vector<std::string>& args) { cmdFormat(args); });
    registerCommand("module", "Manage CHTL modules", [this](const std::vector<std::string>& args) { cmdModule(args); });
    registerCommand("help", "Show help information", [this](const std::vector<std::string>& args) { cmdHelp(args); });
    registerCommand("version", "Show version information", [this](const std::vector<std::string>& args) { cmdVersion(args); });
}

void CliApplication::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    auto it = m_commands.find(command);
    if (it != m_commands.end()) {
        it->second(args);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
        showUsage();
    }
}

bool CliApplication::hasCommand(const std::string& command) const {
    return m_commands.find(command) != m_commands.end();
}

void CliApplication::showHelp() {
    std::cout << "CHTL Compiler - A C++ based hypertext language\n\n";
    std::cout << m_parser->generateHelp() << std::endl;
    
    std::cout << "Commands:\n";
    for (const auto& [name, description] : m_commandDescriptions) {
        std::cout << "  " << name << " - " << description << std::endl;
    }
}

void CliApplication::showHelp(const std::string& command) {
    auto it = m_commandDescriptions.find(command);
    if (it != m_commandDescriptions.end()) {
        std::cout << command << " - " << it->second << std::endl;
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }
}

void CliApplication::showVersion() {
    std::cout << "CHTL Compiler version 1.0.0" << std::endl;
    std::cout << "Built with C++ 17" << std::endl;
}

void CliApplication::showUsage() {
    std::cout << m_parser->generateUsage() << std::endl;
}

void CliApplication::cmdCompile(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: chtl compile <input> [output]" << std::endl;
        return;
    }
    
    // 检查是否启用默认结构
    if (m_parser->hasOption("--default-struct")) {
        m_context->setConfiguration("default-struct", "true");
    } else {
        m_context->setConfiguration("default-struct", "false");
    }
    
    std::string input = args[0];
    std::string output = args.size() > 1 ? args[1] : "";
    
    if (std::filesystem::is_directory(input)) {
        m_processor->compileDirectory(input, output);
    } else {
        m_processor->compileFile(input, output);
    }
}

void CliApplication::cmdValidate(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: chtl validate <input>" << std::endl;
        return;
    }
    
    std::string input = args[0];
    
    if (std::filesystem::is_directory(input)) {
        m_processor->validateDirectory(input);
    } else {
        m_processor->validateFile(input);
    }
}

void CliApplication::cmdFormat(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Usage: chtl format <input> [output]" << std::endl;
        return;
    }
    
    std::string input = args[0];
    std::string output = args.size() > 1 ? args[1] : "";
    
    if (std::filesystem::is_directory(input)) {
        m_processor->formatDirectory(input, output);
    } else {
        m_processor->formatFile(input, output);
    }
}

void CliApplication::cmdModule(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cerr << "Usage: chtl module <type> <name>" << std::endl;
        return;
    }
    
    std::string type = args[0];
    std::string name = args[1];
    
    m_processor->createModule(name, type);
}

void CliApplication::cmdHelp(const std::vector<std::string>& args) {
    if (args.empty()) {
        showHelp();
    } else {
        showHelp(args[0]);
    }
}

void CliApplication::cmdVersion(const std::vector<std::string>& args) {
    showVersion();
}

// CliRenderer 实现
CliRenderer::CliRenderer(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_isLiveRendering(false) {
}

void CliRenderer::renderFile(const std::string& inputFile, const std::string& outputFile) {
    std::cout << "Rendering " << inputFile << " to " << outputFile << std::endl;
    
    // TODO: 实现实际的渲染逻辑
}

void CliRenderer::renderDirectory(const std::string& inputDir, const std::string& outputDir) {
    std::cout << "Rendering directory " << inputDir << " to " << outputDir << std::endl;
    
    // TODO: 实现实际的目录渲染逻辑
}

void CliRenderer::previewFile(const std::string& inputFile) {
    std::cout << "Previewing " << inputFile << std::endl;
    
    // TODO: 实现实际的预览逻辑
}

void CliRenderer::previewDirectory(const std::string& inputDir) {
    std::cout << "Previewing directory " << inputDir << std::endl;
    
    // TODO: 实现实际的目录预览逻辑
}

void CliRenderer::startLiveRender(const std::string& inputFile) {
    m_isLiveRendering = true;
    m_currentFile = inputFile;
    
    std::cout << "Starting live render for " << inputFile << std::endl;
    
    // TODO: 实现实际的实时渲染逻辑
}

void CliRenderer::stopLiveRender() {
    m_isLiveRendering = false;
    m_currentFile.clear();
    
    std::cout << "Stopping live render" << std::endl;
}

bool CliRenderer::isLiveRendering() const {
    return m_isLiveRendering;
}

} // namespace CHTL