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

// FileProcessor 实现
FileProcessor::FileInfo FileProcessor::getFileInfo(const std::string& path) {
    FileInfo info;
    info.path = path;
    info.name = std::filesystem::path(path).filename().string();
    info.extension = std::filesystem::path(path).extension().string();
    
    if (std::filesystem::exists(path)) {
        info.exists = true;
        info.size = std::filesystem::file_size(path);
        // 简化时间处理
        info.lastModified = 0;
    }
    
    return info;
}

bool FileProcessor::fileExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}

bool FileProcessor::directoryExists(const std::string& path) {
    return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}

std::vector<std::string> FileProcessor::listFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> files;
    
    if (!directoryExists(directory)) {
        return files;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (pattern == "*" || std::regex_match(filename, std::regex(pattern))) {
                    files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::exception& e) {
        // 忽略错误
    }
    
    return files;
}

std::vector<std::string> FileProcessor::listDirectories(const std::string& directory) {
    std::vector<std::string> directories;
    
    if (!directoryExists(directory)) {
        return directories;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_directory()) {
                directories.push_back(entry.path().string());
            }
        }
    } catch (const std::exception& e) {
        // 忽略错误
    }
    
    return directories;
}

bool FileProcessor::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileProcessor::removeFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (const std::exception& e) {
        return false;
    }
}

bool FileProcessor::removeDirectory(const std::string& path) {
    try {
        return std::filesystem::remove_all(path) > 0;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string FileProcessor::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileProcessor::writeFile(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        file << content;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::string FileProcessor::getRelativePath(const std::string& path, const std::string& base) {
    try {
        return std::filesystem::relative(path, base).string();
    } catch (const std::exception& e) {
        return path;
    }
}

std::string FileProcessor::getAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (const std::exception& e) {
        return path;
    }
}

std::string FileProcessor::normalizePath(const std::string& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (const std::exception& e) {
        return path;
    }
}

// CLIParser 实现
CLIParser::CLIParser(int argc, char* argv[]) {
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    parseArguments();
}

CLIParser::CLIParser(const std::vector<std::string>& args) : args(args) {
    parseArguments();
}

void CLIParser::parseArguments() {
    for (size_t i = 1; i < args.size(); ++i) {
        const std::string& arg = args[i];
        
        if (arg.substr(0, 2) == "--") {
            // 长选项
            std::string name = arg.substr(2);
            if (i + 1 < args.size() && args[i + 1].substr(0, 1) != "-") {
                options[name] = args[++i];
            } else {
                options[name] = "true";
            }
        } else if (arg.substr(0, 1) == "-") {
            // 短选项
            std::string name = arg.substr(1);
            if (i + 1 < args.size() && args[i + 1].substr(0, 1) != "-") {
                options[name] = args[++i];
            } else {
                options[name] = "true";
            }
        } else {
            // 位置参数
            positionalArgs.push_back(arg);
        }
    }
}

void CLIParser::validateArguments(const Command& command) {
    for (const auto& arg : command.arguments) {
        if (arg.required && !hasOption(arg.name)) {
            errors.push_back("缺少必需参数: " + arg.name);
        }
    }
}

std::map<std::string, std::string> CLIParser::getOptions() const {
    return options;
}

std::vector<std::string> CLIParser::getPositionalArgs() const {
    return positionalArgs;
}

std::vector<std::string> CLIParser::getErrors() const {
    return errors;
}

bool CLIParser::hasOption(const std::string& name) const {
    return options.find(name) != options.end();
}

std::string CLIParser::getOption(const std::string& name, const std::string& defaultValue) const {
    auto it = options.find(name);
    return it != options.end() ? it->second : defaultValue;
}

int CLIParser::getIntOption(const std::string& name, int defaultValue) const {
    auto it = options.find(name);
    if (it != options.end()) {
        try {
            return std::stoi(it->second);
        } catch (const std::exception& e) {
            return defaultValue;
        }
    }
    return defaultValue;
}

bool CLIParser::getBoolOption(const std::string& name, bool defaultValue) const {
    auto it = options.find(name);
    if (it != options.end()) {
        return it->second == "true" || it->second == "1" || it->second == "yes";
    }
    return defaultValue;
}

std::string CLIParser::getPositionalArg(int index, const std::string& defaultValue) const {
    if (index < static_cast<int>(positionalArgs.size())) {
        return positionalArgs[index];
    }
    return defaultValue;
}

int CLIParser::getPositionalArgCount() const {
    return static_cast<int>(positionalArgs.size());
}

void CLIParser::printUsage(const Command& command) const {
    std::cout << "用法: " << command.name;
    for (const auto& arg : command.arguments) {
        if (arg.required) {
            std::cout << " <" << arg.name << ">";
        } else {
            std::cout << " [" << arg.name << "]";
        }
    }
    std::cout << std::endl;
    
    std::cout << command.description << std::endl;
    
    if (!command.arguments.empty()) {
        std::cout << "参数:" << std::endl;
        for (const auto& arg : command.arguments) {
            std::cout << "  " << arg.name << (arg.required ? " (必需)" : " (可选)") << ": " << arg.description << std::endl;
        }
    }
}

void CLIParser::printHelp(const std::vector<Command>& commands) const {
    std::cout << "可用命令:" << std::endl;
    for (const auto& command : commands) {
        std::cout << "  " << command.name << ": " << command.description << std::endl;
    }
}

} // namespace CLI
} // namespace CHTL