#include "CHTL/CompilerDispatcher.h"
#include "CHTL/UnifiedScanner.h"
#include <chrono>
#include <fstream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : totalCompileCount_(0), totalCompileTime_(0.0), totalMemoryUsage_(0) {
    initializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::initializeCompilers() {
    scanner_ = std::make_unique<UnifiedScanner>();
    // 其他编译器将在后续实现中初始化
}

CompilerDispatcher::CompileResult CompilerDispatcher::compile(const std::string& sourceCode, const CompileConfig& config) {
    CompileResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // 使用统一扫描器分离代码片段
        auto scanResult = scanner_->scan(sourceCode);
        
        if (!scanResult.success) {
            result.errorMessage = "Scanning failed: " + scanResult.errorMessage;
            return result;
        }
        
        // 处理代码片段
        std::string htmlOutput = "<!DOCTYPE html>\n<html>\n<head>\n<title>CHTL Output</title>\n</head>\n<body>\n";
        std::string cssOutput = "";
        std::string jsOutput = "";
        
        for (const auto& fragment : scanResult.fragments) {
            switch (fragment.type) {
                case UnifiedScanner::FragmentType::CHTL:
                    // 处理CHTL片段
                    htmlOutput += "<!-- CHTL Fragment -->\n";
                    htmlOutput += fragment.content;
                    htmlOutput += "\n";
                    break;
                    
                case UnifiedScanner::FragmentType::CHTL_JS:
                    // 处理CHTL JS片段
                    jsOutput += "// CHTL JS Fragment\n";
                    jsOutput += fragment.content;
                    jsOutput += "\n";
                    break;
                    
                case UnifiedScanner::FragmentType::CSS:
                    // 处理CSS片段
                    cssOutput += "/* CSS Fragment */\n";
                    cssOutput += fragment.content;
                    cssOutput += "\n";
                    break;
                    
                case UnifiedScanner::FragmentType::JS:
                    // 处理JS片段
                    jsOutput += "// JS Fragment\n";
                    jsOutput += fragment.content;
                    jsOutput += "\n";
                    break;
                    
                case UnifiedScanner::FragmentType::MIXED:
                    // 处理混合片段
                    htmlOutput += "<!-- Mixed Fragment -->\n";
                    htmlOutput += fragment.content;
                    htmlOutput += "\n";
                    break;
            }
        }
        
        htmlOutput += "</body>\n</html>\n";
        
        // 内联处理
        if (config.enableInlineCSS && !cssOutput.empty()) {
            htmlOutput = insertCSSIntoHTML(htmlOutput, cssOutput);
            cssOutput = "";
        }
        
        if (config.enableInlineJS && !jsOutput.empty()) {
            htmlOutput = insertJSIntoHTML(htmlOutput, jsOutput);
            jsOutput = "";
        }
        
        result.success = true;
        result.htmlOutput = htmlOutput;
        result.cssOutput = cssOutput;
        result.jsOutput = jsOutput;
        
        // 更新统计信息
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        result.compileTime = duration.count() / 1000.0;
        result.memoryUsage = sourceCode.length() * 2; // 简单估算
        
        totalCompileCount_++;
        totalCompileTime_ += result.compileTime;
        totalMemoryUsage_ += result.memoryUsage;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
    }
    
    return result;
}

CompilerDispatcher::CompileResult CompilerDispatcher::compileFromFile(const std::string& filePath, const CompileConfig& config) {
    CompileResult result;
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            result.errorMessage = "Cannot open file: " + filePath;
            return result;
        }
        
        std::string sourceCode((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        return compile(sourceCode, config);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = e.what();
        return result;
    }
}

void CompilerDispatcher::setModuleSearchPaths(const std::vector<std::string>& paths) {
    moduleSearchPaths_ = paths;
}

void CompilerDispatcher::registerCompiler(const std::string& name, std::shared_ptr<void> compiler) {
    customCompilers_[name] = compiler;
}

std::map<std::string, std::string> CompilerDispatcher::getCompileStatistics() const {
    std::map<std::string, std::string> stats;
    stats["total_compiles"] = std::to_string(totalCompileCount_);
    stats["total_time"] = std::to_string(totalCompileTime_);
    stats["total_memory"] = std::to_string(totalMemoryUsage_);
    stats["average_time"] = totalCompileCount_ > 0 ? std::to_string(totalCompileTime_ / totalCompileCount_) : "0";
    return stats;
}

std::string CompilerDispatcher::insertCSSIntoHTML(const std::string& html, const std::string& css) {
    std::string result = html;
    size_t headEnd = result.find("</head>");
    if (headEnd != std::string::npos) {
        std::string styleTag = "<style>\n" + css + "\n</style>\n";
        result.insert(headEnd, styleTag);
    }
    return result;
}

std::string CompilerDispatcher::insertJSIntoHTML(const std::string& html, const std::string& js) {
    std::string result = html;
    size_t bodyEnd = result.find("</body>");
    if (bodyEnd != std::string::npos) {
        std::string scriptTag = "<script>\n" + js + "\n</script>\n";
        result.insert(bodyEnd, scriptTag);
    }
    return result;
}

void CompilerDispatcher::handleCompileError(const std::string& error, CompileResult& result) {
    result.success = false;
    result.errorMessage = error;
}

void CompilerDispatcher::handleCompileWarning(const std::string& warning, CompileResult& result) {
    result.warnings.push_back(warning);
}

} // namespace CHTL