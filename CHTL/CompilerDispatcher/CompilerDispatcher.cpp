#include "CompilerDispatcher.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace CHTL {

CompilerDispatcher::CompilerDispatcher() 
    : debugMode(false), strictMode(false), generateDefaultStructure(false), outputFormat("html") {
    initializeCompilers();
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::initializeCompilers() {
    chtlLexer = std::make_unique<CHTLLexer>();
    chtlParser = std::make_unique<CHTLParser>();
    scanner = std::make_unique<CHTLUnifiedScanner>();
    codeMerger = std::make_unique<CodeMerger>();
    
    // 设置配置
    if (chtlParser) {
        chtlParser->setDebugMode(debugMode);
        chtlParser->setStrictMode(strictMode);
    }
}

void CompilerDispatcher::clearErrors() {
    errors.clear();
    warnings.clear();
}

void CompilerDispatcher::addError(const std::string& error) {
    errors.push_back(error);
}

void CompilerDispatcher::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

ScanResult CompilerDispatcher::scan(const std::string& inputFile) {
    clearErrors();
    ScanResult result;
    
    try {
        // 读取输入文件
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            addError("无法打开文件: " + inputFile);
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        result.originalSource = buffer.str();
        file.close();
        
        // 使用统一扫描器扫描代码
        result = scanner->scan(inputFile);
        
        // 合并错误和警告
        errors.insert(errors.end(), result.errors.begin(), result.errors.end());
        warnings.insert(warnings.end(), result.warnings.begin(), result.warnings.end());
        
    } catch (const std::exception& e) {
        addError("扫描文件时发生错误: " + std::string(e.what()));
    }
    
    return result;
}

CompileResult CompilerDispatcher::compile(const ScanResult& scanResult) {
    CompileResult result;
    
    try {
        // 编译各个代码片段
        for (const auto& fragment : scanResult.fragments) {
            std::string compiled;
            
            if (fragment.type == "CHTL") {
                compiled = compileCHTLFragment(fragment);
            } else if (fragment.type == "CHTL_JS") {
                compiled = compileCHTLJSFragment(fragment);
            } else if (fragment.type == "CSS") {
                compiled = compileCSSFragment(fragment);
            } else if (fragment.type == "JS") {
                compiled = compileJSFragment(fragment);
            } else {
                addWarning("未知的代码片段类型: " + fragment.type);
                compiled = fragment.content; // 保持原样
            }
            
            result.outputs[fragment.type] += compiled + "\n";
        }
        
        // 合并错误和警告
        result.errors = errors;
        result.warnings = warnings;
        
    } catch (const std::exception& e) {
        addError("编译时发生错误: " + std::string(e.what()));
        result.errors = errors;
    }
    
    return result;
}

FinalResult CompilerDispatcher::merge(const CompileResult& compileResult) {
    FinalResult result;
    
    try {
        // 使用代码合并器合并结果
        result = codeMerger->merge(compileResult);
        
        // 合并错误和警告
        result.errors = compileResult.errors;
        result.warnings = compileResult.warnings;
        
    } catch (const std::exception& e) {
        addError("合并代码时发生错误: " + std::string(e.what()));
        result.errors = errors;
    }
    
    return result;
}

void CompilerDispatcher::output(const FinalResult& result, const std::string& outputFile) {
    try {
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            addError("无法创建输出文件: " + outputFile);
            return;
        }
        
        // 根据输出格式写入内容
        if (outputFormat == "html") {
            file << result.html;
        } else if (outputFormat == "css") {
            file << result.css;
        } else if (outputFormat == "js") {
            file << result.javascript;
        } else {
            // 默认输出HTML
            file << result.html;
        }
        
        file.close();
        
        if (debugMode) {
            std::cout << "输出已写入: " << outputFile << std::endl;
        }
        
    } catch (const std::exception& e) {
        addError("输出文件时发生错误: " + std::string(e.what()));
    }
}

FinalResult CompilerDispatcher::compileFile(const std::string& inputFile) {
    auto scanResult = scan(inputFile);
    auto compileResult = compile(scanResult);
    return merge(compileResult);
}

void CompilerDispatcher::compileFileToOutput(const std::string& inputFile, const std::string& outputFile) {
    auto result = compileFile(inputFile);
    output(result, outputFile);
}

void CompilerDispatcher::clearMessages() {
    clearErrors();
}

bool CompilerDispatcher::validateConfiguration() const {
    // 验证编译器配置
    if (!chtlLexer) {
        return false;
    }
    
    if (!chtlParser) {
        return false;
    }
    
    if (!scanner) {
        return false;
    }
    
    if (!codeMerger) {
        return false;
    }
    
    return true;
}

std::string CompilerDispatcher::getCompilerInfo() const {
    std::ostringstream oss;
    oss << "CHTL Compiler v1.0.0\n";
    oss << "基于C++语言实现的超文本语言编译器\n";
    oss << "MIT开源协议\n";
    oss << "调试模式: " << (debugMode ? "开启" : "关闭") << "\n";
    oss << "严格模式: " << (strictMode ? "开启" : "关闭") << "\n";
    oss << "默认结构: " << (generateDefaultStructure ? "生成" : "不生成") << "\n";
    oss << "输出格式: " << outputFormat << "\n";
    return oss.str();
}

std::string CompilerDispatcher::getVersion() const {
    return "1.0.0";
}

std::string CompilerDispatcher::compileCHTLFragment(const CodeFragment& fragment) {
    try {
        // 使用CHTL词法分析器
        chtlLexer->setSource(fragment.content);
        auto tokens = chtlLexer->tokenize();
        
        // 使用CHTL语法分析器
        chtlParser->setTokens(tokens);
        auto ast = chtlParser->parse();
        
        // 生成HTML代码
        if (ast) {
            return ast->toHTML();
        }
        
        return "";
        
    } catch (const std::exception& e) {
        addError("编译CHTL片段时发生错误: " + std::string(e.what()));
        return fragment.content; // 返回原始内容
    }
}

std::string CompilerDispatcher::compileCHTLJSFragment(const CodeFragment& fragment) {
    // TODO: 实现CHTL JS编译
    addWarning("CHTL JS编译器尚未实现，返回原始内容");
    return fragment.content;
}

std::string CompilerDispatcher::compileCSSFragment(const CodeFragment& fragment) {
    // TODO: 实现CSS编译
    addWarning("CSS编译器尚未实现，返回原始内容");
    return fragment.content;
}

std::string CompilerDispatcher::compileJSFragment(const CodeFragment& fragment) {
    // TODO: 实现JS编译
    addWarning("JS编译器尚未实现，返回原始内容");
    return fragment.content;
}

} // namespace CHTL