#include "CompilerDispatcher.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "Scanner/UnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>
#include <algorithm>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : debugMode(false), verbose(false) {
    stats = {};
}

CompilerDispatcher::CompilerDispatcher(const CompileOptions& options) 
    : options(options), debugMode(false), verbose(false) {
    stats = {};
}

CompileResult CompilerDispatcher::compile(const std::string& inputFile, const std::string& outputFile) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompileResult result;
    result.success = false;
    result.message = "";
    result.htmlOutput = "";
    result.cssOutput = "";
    result.jsOutput = "";
    result.warnings.clear();
    result.errors.clear();
    
    try {
        // 读取输入文件
        std::string source = readFile(inputFile);
        if (source.empty()) {
            result.message = "Failed to read input file: " + inputFile;
            return result;
        }
        
        // 预处理
        std::string preprocessedSource = preprocess(source);
        
        // 扫描和分离代码片段
        std::vector<std::string> codeFragments = scanAndSeparate(preprocessedSource);
        
        // 编译各个片段
        std::string htmlOutput, cssOutput, jsOutput;
        
        for (const auto& fragment : codeFragments) {
            // 这里需要根据片段类型进行不同的编译
            // 暂时简化处理
            if (fragment.find("<") != std::string::npos) {
                htmlOutput += fragment;
            } else if (fragment.find("{") != std::string::npos) {
                cssOutput += fragment;
            } else {
                jsOutput += fragment;
            }
        }
        
        // 合并输出
        std::string finalOutput = mergeOutputs(htmlOutput, cssOutput, jsOutput);
        
        // 写入输出文件
        if (!outputFile.empty()) {
            writeFile(outputFile, finalOutput);
        }
        
        result.success = true;
        result.message = "Compilation successful";
        result.htmlOutput = htmlOutput;
        result.cssOutput = cssOutput;
        result.jsOutput = jsOutput;
        
        // 更新统计信息
        stats.filesProcessed++;
        stats.linesProcessed += std::count(source.begin(), source.end(), '\n');
        
    } catch (const std::exception& e) {
        result.success = false;
        result.message = "Compilation error: " + std::string(e.what());
        addError(result.message);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    result.compileTime = duration.count();
    stats.totalTime += result.compileTime;
    
    result.errors = errors;
    result.warnings = warnings;
    
    return result;
}

CompileResult CompilerDispatcher::compileString(const std::string& source, const std::string& outputFile) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompileResult result;
    result.success = false;
    result.message = "";
    result.htmlOutput = "";
    result.cssOutput = "";
    result.jsOutput = "";
    result.warnings.clear();
    result.errors.clear();
    
    try {
        // 预处理
        std::string preprocessedSource = preprocess(source);
        
        // 扫描和分离代码片段
        std::vector<std::string> codeFragments = scanAndSeparate(preprocessedSource);
        
        // 编译各个片段
        std::string htmlOutput, cssOutput, jsOutput;
        
        for (const auto& fragment : codeFragments) {
            // 这里需要根据片段类型进行不同的编译
            // 暂时简化处理
            if (fragment.find("<") != std::string::npos) {
                htmlOutput += fragment;
            } else if (fragment.find("{") != std::string::npos) {
                cssOutput += fragment;
            } else {
                jsOutput += fragment;
            }
        }
        
        // 合并输出
        std::string finalOutput = mergeOutputs(htmlOutput, cssOutput, jsOutput);
        
        // 写入输出文件
        if (!outputFile.empty()) {
            writeFile(outputFile, finalOutput);
        }
        
        result.success = true;
        result.message = "Compilation successful";
        result.htmlOutput = htmlOutput;
        result.cssOutput = cssOutput;
        result.jsOutput = jsOutput;
        
        // 更新统计信息
        stats.linesProcessed += std::count(source.begin(), source.end(), '\n');
        
    } catch (const std::exception& e) {
        result.success = false;
        result.message = "Compilation error: " + std::string(e.what());
        addError(result.message);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    result.compileTime = duration.count();
    stats.totalTime += result.compileTime;
    
    result.errors = errors;
    result.warnings = warnings;
    
    return result;
}

std::string CompilerDispatcher::preprocess(const std::string& source) {
    // 预处理步骤：
    // 1. 处理宏定义
    // 2. 处理条件编译
    // 3. 处理包含文件
    // 4. 处理其他预处理指令
    
    std::string result = source;
    
    // 处理定义
    for (const auto& define : options.defines) {
        std::string search = define.first;
        std::string replace = define.second;
        
        size_t pos = 0;
        while ((pos = result.find(search, pos)) != std::string::npos) {
            result.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }
    
    return result;
}

std::vector<std::string> CompilerDispatcher::scanAndSeparate(const std::string& source) {
    // 使用统一扫描器分离代码片段
    if (!scanner) {
        scanner = std::make_unique<UnifiedScanner>();
    }
    
    return scanner->scan(source);
}

std::string CompilerDispatcher::compileCHTL(const std::string& chtlCode) {
    // 词法分析
    CHTLLexer lexer(chtlCode);
    TokenStream tokens = lexer.tokenize();
    
    // 语法分析
    CHTLParser parser(tokens);
    std::shared_ptr<BaseNode> ast = parser.parse();
    
    // 代码生成
    CHTLGenerator generator;
    return generator.generate(ast);
}

std::string CompilerDispatcher::compileCSS(const std::string& cssCode) {
    // CSS编译（暂时直接返回）
    return cssCode;
}

std::string CompilerDispatcher::compileJS(const std::string& jsCode) {
    // JS编译（暂时直接返回）
    return jsCode;
}

std::string CompilerDispatcher::mergeOutputs(const std::string& html, const std::string& css, const std::string& js) {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    
    if (!css.empty()) {
        oss << "<style>\n" << css << "\n</style>\n";
    }
    
    oss << "</head>\n";
    oss << "<body>\n";
    oss << html << "\n";
    oss << "</body>\n";
    
    if (!js.empty()) {
        oss << "<script>\n" << js << "\n</script>\n";
    }
    
    oss << "</html>\n";
    
    return oss.str();
}

std::string CompilerDispatcher::readFile(const std::string& filePath) {
    // 检查缓存
    std::string cached = getCachedFile(filePath);
    if (!cached.empty()) {
        return cached;
    }
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filePath);
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // 缓存文件内容
    cacheFile(filePath, content);
    
    return content;
}

void CompilerDispatcher::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot write to file: " + filePath);
        return;
    }
    
    file << content;
    file.close();
}

bool CompilerDispatcher::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

std::string CompilerDispatcher::getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filePath.substr(dotPos + 1);
    }
    return "";
}

void CompilerDispatcher::addError(const std::string& error) {
    errors.push_back(error);
}

void CompilerDispatcher::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CompilerDispatcher::clearCache() {
    fileCache.clear();
}

std::string CompilerDispatcher::getCachedFile(const std::string& filePath) {
    auto it = fileCache.find(filePath);
    if (it != fileCache.end()) {
        return it->second;
    }
    return "";
}

void CompilerDispatcher::cacheFile(const std::string& filePath, const std::string& content) {
    fileCache[filePath] = content;
}

void CompilerDispatcher::setOptions(const CompileOptions& options) {
    this->options = options;
}

CompileOptions CompilerDispatcher::getOptions() const {
    return options;
}

const std::vector<std::string>& CompilerDispatcher::getErrors() const {
    return errors;
}

const std::vector<std::string>& CompilerDispatcher::getWarnings() const {
    return warnings;
}

void CompilerDispatcher::clearErrors() {
    errors.clear();
}

void CompilerDispatcher::clearWarnings() {
    warnings.clear();
}

void CompilerDispatcher::addIncludePath(const std::string& path) {
    includePaths.push_back(path);
}

void CompilerDispatcher::removeIncludePath(const std::string& path) {
    auto it = std::find(includePaths.begin(), includePaths.end(), path);
    if (it != includePaths.end()) {
        includePaths.erase(it);
    }
}

std::vector<std::string> CompilerDispatcher::getIncludePaths() const {
    return includePaths;
}

void CompilerDispatcher::enableDebugMode(bool enable) {
    debugMode = enable;
}

void CompilerDispatcher::setVerbose(bool verbose) {
    this->verbose = verbose;
}

std::string CompilerDispatcher::getDebugInfo() const {
    std::ostringstream oss;
    oss << "Debug Info:\n";
    oss << "Files processed: " << stats.filesProcessed << "\n";
    oss << "Lines processed: " << stats.linesProcessed << "\n";
    oss << "Total time: " << stats.totalTime << "ms\n";
    return oss.str();
}

CompilerDispatcher::CompileStats CompilerDispatcher::getStats() const {
    return stats;
}

void CompilerDispatcher::resetStats() {
    stats = {};
}

} // namespace CHTL