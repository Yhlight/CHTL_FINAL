#include "CompilerDispatcher.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() {
    scanner = std::make_unique<UnifiedScanner>();
    chtljsCompiler = std::make_unique<CHTLJS::CHTLJS>();
    initializeCompilers();
}

void CompilerDispatcher::initializeCompilers() {
    // 注册内置编译器
    compilers[CompilerType::CHTL] = [this](const CodeFragment& fragment) {
        return compileCHTL(fragment);
    };
    
    compilers[CompilerType::CHTLJS] = [this](const CodeFragment& fragment) {
        return compileCHTLJS(fragment);
    };
    
    compilers[CompilerType::CSS] = [this](const CodeFragment& fragment) {
        return compileCSS(fragment);
    };
    
    compilers[CompilerType::JS] = [this](const CodeFragment& fragment) {
        return compileJS(fragment);
    };
    
    compilers[CompilerType::HTML] = [this](const CodeFragment& fragment) {
        return compileHTML(fragment);
    };
}

std::vector<CompilationResult> CompilerDispatcher::compile(const std::string& source) {
    clear();
    
    try {
        // 第一步：使用统一扫描器分离代码片段
        fragments = scanner->scan(source);
        
        if (context.debugMode) {
            std::cout << "=== 代码片段分离完成 ===" << std::endl;
            scanner->printFragments();
            scanner->printStatistics();
        }
        
        // 第二步：为每个片段分配合适的编译器
        for (const auto& fragment : fragments) {
            if (!validateFragment(fragment)) {
                CompilationResult result(CompilerType::UNKNOWN, false);
                result.errorMessage = "Invalid fragment";
                result.line = fragment.startLine;
                result.column = fragment.startColumn;
                results.push_back(result);
                continue;
            }
            
            CompilerType compilerType = getCompilerType(fragment);
            auto it = compilers.find(compilerType);
            
            if (it != compilers.end()) {
                CompilationResult result = it->second(fragment);
                results.push_back(result);
                
                if (context.debugMode) {
                    logCompilation(result);
                }
            } else {
                CompilationResult result(compilerType, false);
                result.errorMessage = "No compiler available for this fragment type";
                result.line = fragment.startLine;
                result.column = fragment.startColumn;
                results.push_back(result);
            }
        }
        
        return results;
        
    } catch (const std::exception& e) {
        CompilationResult result(CompilerType::UNKNOWN, false);
        result.errorMessage = "Compilation failed: " + std::string(e.what());
        results.push_back(result);
        return results;
    }
}

std::vector<CompilationResult> CompilerDispatcher::compileFile(const std::string& filePath) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();
        
        context.sourcePath = filePath;
        return compile(content);
        
    } catch (const std::exception& e) {
        CompilationResult result(CompilerType::UNKNOWN, false);
        result.errorMessage = "File compilation failed: " + std::string(e.what());
        results.push_back(result);
        return results;
    }
}

CompilationResult CompilerDispatcher::compileCHTL(const CodeFragment& fragment) {
    CompilationResult result(CompilerType::CHTL);
    
    try {
        // TODO: 实现CHTL编译器
        // 这里应该调用CHTL编译器的实际实现
        // 目前返回占位符实现
        
        result.output = "<!-- CHTL compiled output -->\n";
        result.output += fragment.content;
        result.success = true;
        
        if (context.debugMode) {
            result.output += "\n<!-- CHTL compilation completed -->";
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "CHTL compilation failed: " + std::string(e.what());
        result.line = fragment.startLine;
        result.column = fragment.startColumn;
    }
    
    return result;
}

CompilationResult CompilerDispatcher::compileCHTLJS(const CodeFragment& fragment) {
    CompilationResult result(CompilerType::CHTLJS);
    
    try {
        // 使用CHTL JS编译器编译片段
        chtljsCompiler->setDebugMode(context.debugMode);
        chtljsCompiler->setMinifyOutput(context.minifyOutput);
        
        result.output = chtljsCompiler->compile(fragment.content);
        
        if (chtljsCompiler->hasError()) {
            result.success = false;
            result.errorMessage = chtljsCompiler->getLastError();
        } else {
            result.success = true;
        }
        
        result.line = fragment.startLine;
        result.column = fragment.startColumn;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "CHTL JS compilation failed: " + std::string(e.what());
        result.line = fragment.startLine;
        result.column = fragment.startColumn;
    }
    
    return result;
}

CompilationResult CompilerDispatcher::compileCSS(const CodeFragment& fragment) {
    CompilationResult result(CompilerType::CSS);
    
    try {
        // TODO: 集成CSS编译器（libcss）
        // 目前返回占位符实现
        
        result.output = "/* CSS compiled output */\n";
        result.output += fragment.content;
        result.success = true;
        
        if (context.debugMode) {
            result.output += "\n/* CSS compilation completed */";
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "CSS compilation failed: " + std::string(e.what());
        result.line = fragment.startLine;
        result.column = fragment.startColumn;
    }
    
    return result;
}

CompilationResult CompilerDispatcher::compileJS(const CodeFragment& fragment) {
    CompilationResult result(CompilerType::JS);
    
    try {
        // TODO: 集成JavaScript编译器（V8）
        // 目前返回占位符实现
        
        result.output = "// JavaScript compiled output\n";
        result.output += fragment.content;
        result.success = true;
        
        if (context.debugMode) {
            result.output += "\n// JavaScript compilation completed";
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "JavaScript compilation failed: " + std::string(e.what());
        result.line = fragment.startLine;
        result.column = fragment.startColumn;
    }
    
    return result;
}

CompilationResult CompilerDispatcher::compileHTML(const CodeFragment& fragment) {
    CompilationResult result(CompilerType::HTML);
    
    try {
        // HTML编译器通常不需要特殊处理，直接输出
        result.output = fragment.content;
        result.success = true;
        
        if (context.debugMode) {
            result.output += "\n<!-- HTML compilation completed -->";
        }
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "HTML compilation failed: " + std::string(e.what());
        result.line = fragment.startLine;
        result.column = fragment.startColumn;
    }
    
    return result;
}

CompilerType CompilerDispatcher::getCompilerType(const CodeFragment& fragment) const {
    switch (fragment.type) {
        case CodeType::CHTL:
            return CompilerType::CHTL;
        case CodeType::CHTLJS:
            return CompilerType::CHTLJS;
        case CodeType::CSS:
            return CompilerType::CSS;
        case CodeType::JS:
            return CompilerType::JS;
        case CodeType::HTML:
            return CompilerType::HTML;
        default:
            return CompilerType::UNKNOWN;
    }
}

bool CompilerDispatcher::validateFragment(const CodeFragment& fragment) const {
    return !fragment.content.empty() && fragment.type != CodeType::UNKNOWN;
}

void CompilerDispatcher::logCompilation(const CompilationResult& result) const {
    std::cout << "=== 编译结果 ===" << std::endl;
    std::cout << "编译器类型: " << static_cast<int>(result.type) << std::endl;
    std::cout << "编译状态: " << (result.success ? "成功" : "失败") << std::endl;
    if (!result.success) {
        std::cout << "错误信息: " << result.errorMessage << std::endl;
        std::cout << "错误位置: 行 " << result.line << ", 列 " << result.column << std::endl;
    }
    std::cout << "输出长度: " << result.output.length() << " 字符" << std::endl;
    std::cout << "=================" << std::endl;
}

void CompilerDispatcher::setContext(const CompilationContext& ctx) {
    context = ctx;
    scanner->setDebugMode(ctx.debugMode);
    chtljsCompiler->setDebugMode(ctx.debugMode);
    chtljsCompiler->setMinifyOutput(ctx.minifyOutput);
}

CompilationContext CompilerDispatcher::getContext() const {
    return context;
}

void CompilerDispatcher::setDebugMode(bool debug) {
    context.debugMode = debug;
    scanner->setDebugMode(debug);
    chtljsCompiler->setDebugMode(debug);
}

void CompilerDispatcher::setMinifyOutput(bool minify) {
    context.minifyOutput = minify;
    chtljsCompiler->setMinifyOutput(minify);
}

void CompilerDispatcher::setOutputPath(const std::string& path) {
    context.outputPath = path;
}

void CompilerDispatcher::registerCompiler(CompilerType type, 
                                        std::function<CompilationResult(const CodeFragment&)> compiler) {
    compilers[type] = compiler;
}

void CompilerDispatcher::unregisterCompiler(CompilerType type) {
    compilers.erase(type);
}

bool CompilerDispatcher::hasCompiler(CompilerType type) const {
    return compilers.find(type) != compilers.end();
}

std::vector<CompilationResult> CompilerDispatcher::getResults() const {
    return results;
}

std::vector<CompilationResult> CompilerDispatcher::getResultsByType(CompilerType type) const {
    std::vector<CompilationResult> filteredResults;
    for (const auto& result : results) {
        if (result.type == type) {
            filteredResults.push_back(result);
        }
    }
    return filteredResults;
}

CompilationResult CompilerDispatcher::getResult(size_t index) const {
    if (index < results.size()) {
        return results[index];
    }
    return CompilationResult();
}

size_t CompilerDispatcher::getResultCount() const {
    return results.size();
}

std::vector<CodeFragment> CompilerDispatcher::getFragments() const {
    return fragments;
}

std::vector<CodeFragment> CompilerDispatcher::getFragmentsByType(CodeType type) const {
    return scanner->getFragmentsByType(type);
}

bool CompilerDispatcher::hasErrors() const {
    for (const auto& result : results) {
        if (!result.success) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> CompilerDispatcher::getErrorMessages() const {
    std::vector<std::string> errors;
    for (const auto& result : results) {
        if (!result.success) {
            errors.push_back(result.errorMessage);
        }
    }
    return errors;
}

bool CompilerDispatcher::isDebugMode() const {
    return context.debugMode;
}

bool CompilerDispatcher::isMinifyOutput() const {
    return context.minifyOutput;
}

void CompilerDispatcher::clear() {
    fragments.clear();
    results.clear();
    scanner->clear();
}

void CompilerDispatcher::reset() {
    clear();
    context = CompilationContext();
}

void CompilerDispatcher::printResults() const {
    std::cout << "=== 编译结果 ===" << std::endl;
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        std::cout << "结果 " << i << ":" << std::endl;
        std::cout << "  类型: " << static_cast<int>(result.type) << std::endl;
        std::cout << "  状态: " << (result.success ? "成功" : "失败") << std::endl;
        if (!result.success) {
            std::cout << "  错误: " << result.errorMessage << std::endl;
            std::cout << "  位置: 行 " << result.line << ", 列 " << result.column << std::endl;
        }
        std::cout << "  输出: " << result.output.substr(0, 100) 
                  << (result.output.length() > 100 ? "..." : "") << std::endl;
        std::cout << std::endl;
    }
}

void CompilerDispatcher::printFragments() const {
    scanner->printFragments();
}

void CompilerDispatcher::printStatistics() const {
    std::cout << "=== 编译统计 ===" << std::endl;
    std::cout << "片段总数: " << fragments.size() << std::endl;
    std::cout << "结果总数: " << results.size() << std::endl;
    
    std::unordered_map<CompilerType, int> typeCount;
    int successCount = 0;
    int errorCount = 0;
    
    for (const auto& result : results) {
        typeCount[result.type]++;
        if (result.success) {
            successCount++;
        } else {
            errorCount++;
        }
    }
    
    std::cout << "成功编译: " << successCount << std::endl;
    std::cout << "编译失败: " << errorCount << std::endl;
    
    for (const auto& pair : typeCount) {
        std::cout << "类型 " << static_cast<int>(pair.first) << ": " << pair.second << " 个结果" << std::endl;
    }
}

} // namespace CHTL