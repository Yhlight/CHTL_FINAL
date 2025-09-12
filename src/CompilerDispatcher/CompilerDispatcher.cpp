#include "CompilerDispatcher.h"
#include "CHTL/CHTLManage.h"
#include "CHTL_JS/CHTLJSManage.h"
#include "CodeMerger/CodeMerger.h"
#include <iostream>
#include <filesystem>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {}

CompilerDispatcher::~CompilerDispatcher() {}

bool CompilerDispatcher::dispatch(const std::string& sourcePath, const std::string& outputDir) {
    clearErrors();
    
    if (!validateOptions()) {
        addError("Invalid compilation options");
        return false;
    }
    
    try {
        // 创建输出目录
        std::filesystem::create_directories(outputDir);
        
        // 处理CHTL编译
        if (!processCHTLCompilation(sourcePath, outputDir)) {
            addError("CHTL compilation failed");
            return false;
        }
        
        // 处理CHTL JS编译
        if (!processCHTLJSCompilation(sourcePath, outputDir)) {
            addError("CHTL JS compilation failed");
            return false;
        }
        
        // 处理代码合并
        if (!processCodeMerging(outputDir)) {
            addError("Code merging failed");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        addError("Compilation error: " + std::string(e.what()));
        return false;
    }
}

void CompilerDispatcher::setOption(const std::string& key, const std::string& value) {
    options[key] = value;
}

std::string CompilerDispatcher::getOption(const std::string& key) const {
    auto it = options.find(key);
    return it != options.end() ? it->second : "";
}

const std::vector<std::string>& CompilerDispatcher::getErrors() const {
    return errors;
}

void CompilerDispatcher::clearErrors() {
    errors.clear();
}

void CompilerDispatcher::addError(const std::string& error) {
    errors.push_back(error);
    std::cerr << "Error: " << error << std::endl;
}

bool CompilerDispatcher::validateOptions() const {
    // 验证编译选项
    return true;
}

bool CompilerDispatcher::processCHTLCompilation(const std::string& sourcePath, const std::string& outputDir) {
    try {
        CHTLManage chtlManager;
        
        // 设置编译选项
        for (const auto& option : options) {
            chtlManager.setOption(option.first, option.second);
        }
        
        // 编译CHTL文件
        bool success = chtlManager.compileFile(sourcePath, outputDir);
        
        if (!success) {
            const auto& chtlErrors = chtlManager.getErrors();
            for (const auto& error : chtlErrors) {
                addError("CHTL: " + error);
            }
        }
        
        return success;
        
    } catch (const std::exception& e) {
        addError("CHTL compilation error: " + std::string(e.what()));
        return false;
    }
}

bool CompilerDispatcher::processCHTLJSCompilation(const std::string& sourcePath, const std::string& outputDir) {
    try {
        CHTLJSManage chtlJSManager;
        
        // 设置编译选项
        for (const auto& option : options) {
            chtlJSManager.setOption(option.first, option.second);
        }
        
        // 编译CHTL JS文件
        bool success = chtlJSManager.compileFile(sourcePath, outputDir);
        
        if (!success) {
            const auto& chtlJSErrors = chtlJSManager.getErrors();
            for (const auto& error : chtlJSErrors) {
                addError("CHTL JS: " + error);
            }
        }
        
        return success;
        
    } catch (const std::exception& e) {
        addError("CHTL JS compilation error: " + std::string(e.what()));
        return false;
    }
}

bool CompilerDispatcher::processCodeMerging(const std::string& outputDir) {
    try {
        CodeMerger codeMerger;
        
        // 设置合并选项
        for (const auto& option : options) {
            codeMerger.setOption(option.first, option.second);
        }
        
        // 读取生成的文件
        std::string baseName = std::filesystem::path(outputDir).stem().string();
        
        std::string htmlFile = outputDir + "/" + baseName + ".html";
        std::string cssFile = outputDir + "/" + baseName + ".css";
        std::string jsFile = outputDir + "/" + baseName + ".js";
        
        std::string htmlCode = "";
        std::string cssCode = "";
        std::string jsCode = "";
        
        // 读取HTML文件
        if (std::filesystem::exists(htmlFile)) {
            std::ifstream htmlStream(htmlFile);
            if (htmlStream.is_open()) {
                htmlCode = std::string((std::istreambuf_iterator<char>(htmlStream)),
                                     std::istreambuf_iterator<char>());
                htmlStream.close();
            }
        }
        
        // 读取CSS文件
        if (std::filesystem::exists(cssFile)) {
            std::ifstream cssStream(cssFile);
            if (cssStream.is_open()) {
                cssCode = std::string((std::istreambuf_iterator<char>(cssStream)),
                                    std::istreambuf_iterator<char>());
                cssStream.close();
            }
        }
        
        // 读取JavaScript文件
        if (std::filesystem::exists(jsFile)) {
            std::ifstream jsStream(jsFile);
            if (jsStream.is_open()) {
                jsCode = std::string((std::istreambuf_iterator<char>(jsStream)),
                                   std::istreambuf_iterator<char>());
                jsStream.close();
            }
        }
        
        // 合并代码
        std::string mergedCode = codeMerger.mergeAll(htmlCode, cssCode, jsCode);
        
        // 写入合并后的文件
        std::ofstream mergedStream(outputDir + "/" + baseName + "_merged.html");
        if (mergedStream.is_open()) {
            mergedStream << mergedCode;
            mergedStream.close();
        } else {
            addError("Failed to write merged file");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        addError("Code merging error: " + std::string(e.what()));
        return false;
    }
}

} // namespace CHTL