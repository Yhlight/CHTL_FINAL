#include "CompilerDispatcher.h"
#include <fstream>
#include <iostream>
#include <filesystem>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : debugMode_(false)
    , defaultStruct_(false) {
    
    // 初始化核心组件
    scanner_ = std::make_unique<UnifiedScanner>();
    // 其他编译器将在后续实现中初始化
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::compile(const std::string& inputFile, const std::string& outputFile) {
    if (debugMode_) {
        std::cout << "Starting compilation of: " << inputFile << std::endl;
    }
    
    // 检查输入文件是否存在
    if (!fileExists(inputFile)) {
        throw std::runtime_error("Input file does not exist: " + inputFile);
    }
    
    // 读取源文件
    std::string sourceCode = readFile(inputFile);
    if (sourceCode.empty()) {
        throw std::runtime_error("Input file is empty: " + inputFile);
    }
    
    if (debugMode_) {
        std::cout << "Source code length: " << sourceCode.length() << " characters" << std::endl;
    }
    
    // 使用统一扫描器分离代码片段
    std::vector<CodeFragment> fragments = scanner_->scan(sourceCode);
    
    if (debugMode_) {
        std::cout << "Found " << fragments.size() << " code fragments" << std::endl;
        for (const auto& fragment : fragments) {
            std::cout << "Fragment type: " << static_cast<int>(fragment.type) 
                      << ", length: " << fragment.content.length() << std::endl;
        }
    }
    
    // 处理不同类型的代码片段
    processCHTLFragments(fragments);
    processCHTLJSFragments(fragments);
    processCSSFragments(fragments);
    processJSFragments(fragments);
    
    // 合并代码并输出
    // 这里将在CodeMerger实现后完成
    
    if (debugMode_) {
        std::cout << "Compilation completed successfully" << std::endl;
    }
}

void CompilerDispatcher::setDebugMode(bool enabled) {
    debugMode_ = enabled;
    if (scanner_) {
        scanner_->setDebugMode(enabled);
    }
}

void CompilerDispatcher::setDefaultStruct(bool enabled) {
    defaultStruct_ = enabled;
}

void CompilerDispatcher::addModulePath(const std::string& path) {
    modulePaths_.push_back(path);
}

void CompilerDispatcher::setOfficialModulePath(const std::string& path) {
    officialModulePath_ = path;
}

std::string CompilerDispatcher::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    return content;
}

void CompilerDispatcher::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create file: " + filePath);
    }
    
    file << content;
    file.close();
}

std::string CompilerDispatcher::getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return filePath.substr(dotPos + 1);
}

bool CompilerDispatcher::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

void CompilerDispatcher::processCHTLFragments(const std::vector<CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        if (fragment.type == CodeType::CHTL) {
            if (debugMode_) {
                std::cout << "Processing CHTL fragment: " << fragment.content.substr(0, 50) << "..." << std::endl;
            }
            // 这里将调用CHTL编译器处理
        }
    }
}

void CompilerDispatcher::processCHTLJSFragments(const std::vector<CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        if (fragment.type == CodeType::CHTL_JS) {
            if (debugMode_) {
                std::cout << "Processing CHTL JS fragment: " << fragment.content.substr(0, 50) << "..." << std::endl;
            }
            // 这里将调用CHTL JS编译器处理
        }
    }
}

void CompilerDispatcher::processCSSFragments(const std::vector<CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        if (fragment.type == CodeType::CSS) {
            if (debugMode_) {
                std::cout << "Processing CSS fragment: " << fragment.content.substr(0, 50) << "..." << std::endl;
            }
            // 这里将调用CSS编译器处理
        }
    }
}

void CompilerDispatcher::processJSFragments(const std::vector<CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        if (fragment.type == CodeType::JS) {
            if (debugMode_) {
                std::cout << "Processing JS fragment: " << fragment.content.substr(0, 50) << "..." << std::endl;
            }
            // 这里将调用JS编译器处理
        }
    }
}

} // namespace CHTL