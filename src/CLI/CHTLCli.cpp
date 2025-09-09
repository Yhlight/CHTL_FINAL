#include "CLI/CHTLCli.h"
#include "Scanner/CompilerDispatcher.h"
#include "Util/FileSystem/FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLCli::CHTLCli() 
    : m_outputPath(""), m_debugMode(false), m_prettyPrint(true), m_includeComments(true) {
    initializeCommands();
}

CHTLCli::~CHTLCli() = default;

int CHTLCli::run(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }
    
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    
    std::string command = args[0];
    args.erase(args.begin());
    
    // 处理全局选项
    for (auto it = args.begin(); it != args.end();) {
        if (*it == "--debug" || *it == "-d") {
            m_debugMode = true;
            it = args.erase(it);
        } else if (*it == "--no-pretty" || *it == "-n") {
            m_prettyPrint = false;
            it = args.erase(it);
        } else if (*it == "--no-comments" || *it == "-c") {
            m_includeComments = false;
            it = args.erase(it);
        } else if (*it == "--output" || *it == "-o") {
            if (it + 1 != args.end()) {
                setOutputPath(*(it + 1));
                it = args.erase(it, it + 2);
            } else {
                printError("--output requires a path argument");
                return 1;
            }
        } else {
            ++it;
        }
    }
    
    // 执行命令
    auto commandIt = m_commands.find(command);
    if (commandIt != m_commands.end()) {
        return (this->*commandIt->second)(args);
    } else {
        printError("Unknown command: " + command);
        printUsage();
        return 1;
    }
}

int CHTLCli::compileCommand(const std::vector<std::string>& args) {
    if (args.empty()) {
        printError("No input file specified");
        return 1;
    }
    
    std::string inputFile = args[0];
    if (!validateInputFile(inputFile)) {
        printError("Invalid input file: " + inputFile);
        return 1;
    }
    
    // 设置输出路径
    if (m_outputPath.empty()) {
        m_outputPath = getOutputFileName(inputFile);
    }
    
    try {
        // 创建编译器调度器
        CompilerDispatcher dispatcher;
        
        // 设置选项
        dispatcher.setDebugMode(m_debugMode);
        dispatcher.setPrettyPrint(m_prettyPrint);
        
        // 读取输入文件
        std::ifstream inputStream(inputFile);
        if (!inputStream.is_open()) {
            printError("Cannot open input file: " + inputFile);
            return 1;
        }
        
        std::string source((std::istreambuf_iterator<char>(inputStream)),
                          std::istreambuf_iterator<char>());
        inputStream.close();
        
        // 编译源代码
        CompileResult result = dispatcher.compile(source);
        if (!result.success) {
            printError("Compilation failed");
            for (const auto& error : result.errors) {
                printError("  " + error);
            }
            return 1;
        }
        
        // 合并输出
        std::string output = result.html;
        if (!result.css.empty()) {
            output += "\n<style>\n" + result.css + "\n</style>\n";
        }
        if (!result.javascript.empty()) {
            output += "\n<script>\n" + result.javascript + "\n</script>\n";
        }
        
        // 写入输出文件
        std::ofstream outFile(m_outputPath);
        if (!outFile.is_open()) {
            printError("Cannot create output file: " + m_outputPath);
            return 1;
        }
        
        outFile << output;
        outFile.close();
        
        printInfo("Compilation successful. Output written to: " + m_outputPath);
        return 0;
        
    } catch (const std::exception& e) {
        printError("Compilation failed: " + std::string(e.what()));
        return 1;
    }
}

int CHTLCli::helpCommand(const std::vector<std::string>& args) {
    std::cout << getHelpText() << std::endl;
    return 0;
}

int CHTLCli::versionCommand(const std::vector<std::string>& args) {
    std::cout << getVersionText() << std::endl;
    return 0;
}

int CHTLCli::initCommand(const std::vector<std::string>& args) {
    std::string projectName = "chtl-project";
    if (!args.empty()) {
        projectName = args[0];
    }
    
    // 创建项目目录
    FileSystem fs;
    if (!fs.createDirectory(projectName)) {
        printError("Cannot create project directory: " + projectName);
        return 1;
    }
    
    // 创建示例文件
    std::string exampleFile = projectName + "/main.chtl";
    std::ofstream file(exampleFile);
    if (!file.is_open()) {
        printError("Cannot create example file: " + exampleFile);
        return 1;
    }
    
    file << "html {\n";
    file << "  head {\n";
    file << "    title {\n";
    file << "      \"CHTL Project\"\n";
    file << "    }\n";
    file << "  }\n";
    file << "  body {\n";
    file << "    div {\n";
    file << "      class: \"container\";\n";
    file << "      h1 {\n";
    file << "        \"Welcome to CHTL!\"\n";
    file << "      }\n";
    file << "      p {\n";
    file << "        \"This is a sample CHTL project.\"\n";
    file << "      }\n";
    file << "    }\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    
    printInfo("Project initialized: " + projectName);
    printInfo("Example file created: " + exampleFile);
    return 0;
}

void CHTLCli::setOutputPath(const std::string& path) {
    m_outputPath = path;
}

void CHTLCli::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

void CHTLCli::setPrettyPrint(bool enabled) {
    m_prettyPrint = enabled;
}

void CHTLCli::setIncludeComments(bool enabled) {
    m_includeComments = enabled;
}

std::string CHTLCli::getHelpText() const {
    std::stringstream help;
    help << "CHTL Compiler v1.0.0\n";
    help << "====================\n\n";
    help << "Usage: chtl <command> [options] [arguments]\n\n";
    help << "Commands:\n";
    help << "  compile <file>    Compile a CHTL file to HTML\n";
    help << "  init [name]       Initialize a new CHTL project\n";
    help << "  help              Show this help message\n";
    help << "  version           Show version information\n\n";
    help << "Options:\n";
    help << "  -d, --debug       Enable debug mode\n";
    help << "  -n, --no-pretty   Disable pretty printing\n";
    help << "  -c, --no-comments Disable comment inclusion\n";
    help << "  -o, --output      Specify output file path\n\n";
    help << "Examples:\n";
    help << "  chtl compile main.chtl\n";
    help << "  chtl compile main.chtl -o output.html\n";
    help << "  chtl init my-project\n";
    help << "  chtl compile main.chtl --debug --no-pretty\n";
    
    return help.str();
}

std::string CHTLCli::getVersionText() const {
    return "CHTL Compiler v1.0.0\nBuilt with C++17";
}

void CHTLCli::initializeCommands() {
    m_commands["compile"] = &CHTLCli::compileCommand;
    m_commands["help"] = &CHTLCli::helpCommand;
    m_commands["version"] = &CHTLCli::versionCommand;
    m_commands["init"] = &CHTLCli::initCommand;
}

void CHTLCli::printUsage() const {
    std::cout << getHelpText() << std::endl;
}

void CHTLCli::printError(const std::string& message) const {
    std::cerr << "Error: " << message << std::endl;
}

void CHTLCli::printInfo(const std::string& message) const {
    std::cout << message << std::endl;
}

bool CHTLCli::validateInputFile(const std::string& filePath) const {
    FileSystem fs;
    return fs.fileExists(filePath) && 
           filePath.substr(filePath.find_last_of(".") + 1) == "chtl";
}

std::string CHTLCli::getOutputFileName(const std::string& inputFile) const {
    size_t lastDot = inputFile.find_last_of(".");
    if (lastDot != std::string::npos) {
        return inputFile.substr(0, lastDot) + ".html";
    }
    return inputFile + ".html";
}

} // namespace CHTL