#include "CompilerDispatcher.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <future>
#include <regex>

namespace CHTL {
namespace Compiler {

CompilerDispatcher::CompilerDispatcher() : debugMode(false) {
    statistics = CompileStatistics{};
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::registerCompiler(CompilerType type, std::unique_ptr<ICompiler> compiler) {
    compilers[type] = std::move(compiler);
}

void CompilerDispatcher::unregisterCompiler(CompilerType type) {
    compilers.erase(type);
}

void CompilerDispatcher::setConfig(const CompilerConfig& config) {
    this->config = config;
}

CompilerConfig CompilerDispatcher::getConfig() const {
    return config;
}

CompilerType CompilerDispatcher::detectCompilerType(const std::string& filename) const {
    std::string ext = std::filesystem::path(filename).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".chtl") {
        return CompilerType::CHTL;
    } else if (ext == ".chtljs" || ext == ".chtl.js") {
        return CompilerType::CHTL_JS;
    } else if (ext == ".css") {
        return CompilerType::CSS;
    } else if (ext == ".js" || ext == ".mjs") {
        return CompilerType::JS;
    } else if (ext == ".html" || ext == ".htm") {
        return CompilerType::HTML;
    }
    
    return CompilerType::CHTL; // 默认
}

std::vector<std::string> CompilerDispatcher::resolveDependencies(const std::string& source, const std::string& filename) const {
    std::vector<std::string> dependencies;
    
    // 简单的依赖解析实现
    // 实际实现应该使用更复杂的解析器
    std::regex importRegex(R"((?:import|require)\s*\(\s*['"]([^'"]+)['"]\s*\))");
    std::sregex_iterator iter(source.begin(), source.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string dep = (*iter)[1].str();
        dependencies.push_back(dep);
    }
    
    return dependencies;
}

std::vector<std::string> CompilerDispatcher::optimizeCompileOrder(const std::vector<std::string>& files) const {
    // 简单的编译顺序优化
    // 实际实现应该使用拓扑排序
    std::vector<std::string> orderedFiles = files;
    
    // 按文件类型分组
    std::map<CompilerType, std::vector<std::string>> groupedFiles;
    for (const auto& file : files) {
        CompilerType type = detectCompilerType(file);
        groupedFiles[type].push_back(file);
    }
    
    // 重新排序：CHTL -> CHTL_JS -> CSS -> JS -> HTML
    orderedFiles.clear();
    std::vector<CompilerType> order = {
        CompilerType::CHTL,
        CompilerType::CHTL_JS,
        CompilerType::CSS,
        CompilerType::JS,
        CompilerType::HTML
    };
    
    for (CompilerType type : order) {
        if (groupedFiles.find(type) != groupedFiles.end()) {
            for (const auto& file : groupedFiles[type]) {
                orderedFiles.push_back(file);
            }
        }
    }
    
    return orderedFiles;
}

CompileResult CompilerDispatcher::compileFile(const std::string& filename) {
    if (!std::filesystem::exists(filename)) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("文件不存在: " + filename);
        return result;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("无法打开文件: " + filename);
        return result;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return compileSource(buffer.str(), filename);
}

CompileResult CompilerDispatcher::compileSource(const std::string& source, const std::string& filename) {
    CompilerType type = detectCompilerType(filename);
    
    if (compilers.find(type) == compilers.end()) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("未注册的编译器类型: " + std::to_string(static_cast<int>(type)));
        return result;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompileResult result = compilers[type]->compile(source, filename);
    result.compilerType = type;
    result.sourceFile = filename;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double compileTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    updateStatistics(result, compileTime);
    notifyCompileComplete(result);
    
    return result;
}

std::vector<CompileResult> CompilerDispatcher::compileFiles(const std::vector<std::string>& filenames) {
    std::vector<CompileResult> results;
    std::vector<std::string> orderedFiles = optimizeCompileOrder(filenames);
    
    for (size_t i = 0; i < orderedFiles.size(); ++i) {
        notifyProgress(i + 1, orderedFiles.size());
        CompileResult result = compileFile(orderedFiles[i]);
        results.push_back(result);
        
        if (!result.success) {
            globalErrors.insert(globalErrors.end(), result.errors.begin(), result.errors.end());
        }
        globalWarnings.insert(globalWarnings.end(), result.warnings.begin(), result.warnings.end());
    }
    
    return results;
}

std::vector<CompileResult> CompilerDispatcher::compileProject(const std::string& projectPath) {
    std::vector<std::string> files;
    
    if (!std::filesystem::exists(projectPath)) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("项目路径不存在: " + projectPath);
        return {result};
    }
    
    // 扫描项目目录
    for (const auto& entry : std::filesystem::recursive_directory_iterator(projectPath)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().string();
            CompilerType type = detectCompilerType(filename);
            if (compilers.find(type) != compilers.end()) {
                files.push_back(filename);
            }
        }
    }
    
    return compileFiles(files);
}

CompileResult CompilerDispatcher::compileBatch(const std::vector<std::string>& sources, const std::vector<std::string>& filenames) {
    if (sources.size() != filenames.size()) {
        CompileResult result;
        result.success = false;
        result.errors.push_back("源文件数量与文件名数量不匹配");
        return result;
    }
    
    CompileResult batchResult;
    batchResult.success = true;
    
    for (size_t i = 0; i < sources.size(); ++i) {
        CompileResult result = compileSource(sources[i], filenames[i]);
        if (!result.success) {
            batchResult.success = false;
            batchResult.errors.insert(batchResult.errors.end(), result.errors.begin(), result.errors.end());
        }
        batchResult.warnings.insert(batchResult.warnings.end(), result.warnings.begin(), result.warnings.end());
    }
    
    return batchResult;
}

std::vector<CompileResult> CompilerDispatcher::incrementalCompile(const std::vector<std::string>& changedFiles) {
    // 简单的增量编译实现
    // 实际实现应该使用文件时间戳和依赖分析
    return compileFiles(changedFiles);
}

std::vector<CompileResult> CompilerDispatcher::parallelCompile(const std::vector<std::string>& filenames) {
    std::vector<std::future<CompileResult>> futures;
    
    for (const auto& filename : filenames) {
        futures.push_back(std::async(std::launch::async, [this, filename]() {
            return compileFile(filename);
        }));
    }
    
    std::vector<CompileResult> results;
    for (auto& future : futures) {
        results.push_back(future.get());
    }
    
    return results;
}

void CompilerDispatcher::setCompileCallback(std::function<void(const CompileResult&)> callback) {
    compileCallback = callback;
}

void CompilerDispatcher::setProgressCallback(std::function<void(int, int)> callback) {
    progressCallback = callback;
}

std::vector<std::string> CompilerDispatcher::getGlobalErrors() const {
    return globalErrors;
}

std::vector<std::string> CompilerDispatcher::getGlobalWarnings() const {
    return globalWarnings;
}

void CompilerDispatcher::clearGlobalErrors() {
    globalErrors.clear();
}

void CompilerDispatcher::clearGlobalWarnings() {
    globalWarnings.clear();
}

bool CompilerDispatcher::isCompilerRegistered(CompilerType type) const {
    return compilers.find(type) != compilers.end();
}

std::vector<CompilerType> CompilerDispatcher::getRegisteredCompilers() const {
    std::vector<CompilerType> types;
    for (const auto& pair : compilers) {
        types.push_back(pair.first);
    }
    return types;
}

CompilerDispatcher::CompileStatistics CompilerDispatcher::getCompileStatistics() const {
    return statistics;
}

void CompilerDispatcher::resetStatistics() {
    statistics = CompileStatistics{};
}

std::string CompilerDispatcher::generateCompileReport() const {
    std::ostringstream oss;
    oss << "=== 编译报告 ===\n";
    oss << "总文件数: " << statistics.totalFiles << "\n";
    oss << "成功文件数: " << statistics.successfulFiles << "\n";
    oss << "失败文件数: " << statistics.failedFiles << "\n";
    oss << "总编译时间: " << statistics.totalTime << "ms\n";
    oss << "平均编译时间: " << statistics.averageTime << "ms\n";
    
    oss << "\n编译器使用统计:\n";
    for (const auto& pair : statistics.compilerUsage) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " 次\n";
    }
    
    return oss.str();
}

void CompilerDispatcher::enableDebugMode(bool enable) {
    debugMode = enable;
}

bool CompilerDispatcher::isDebugMode() const {
    return debugMode;
}

void CompilerDispatcher::updateStatistics(const CompileResult& result, double compileTime) {
    statistics.totalFiles++;
    if (result.success) {
        statistics.successfulFiles++;
    } else {
        statistics.failedFiles++;
    }
    
    statistics.totalTime += compileTime;
    statistics.averageTime = statistics.totalTime / statistics.totalFiles;
    
    statistics.compilerUsage[result.compilerType]++;
}

void CompilerDispatcher::notifyProgress(int current, int total) {
    if (progressCallback) {
        progressCallback(current, total);
    }
}

void CompilerDispatcher::notifyCompileComplete(const CompileResult& result) {
    if (compileCallback) {
        compileCallback(result);
    }
}

std::string CompilerDispatcher::generateSourceMap(const CompileResult& result) const {
    // 简单的源码映射生成
    // 实际实现应该使用专门的源码映射库
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"version\": 3,\n";
    oss << "  \"sources\": [\"" << result.sourceFile << "\"],\n";
    oss << "  \"names\": [],\n";
    oss << "  \"mappings\": \"\"\n";
    oss << "}\n";
    return oss.str();
}

} // namespace Compiler
} // namespace CHTL