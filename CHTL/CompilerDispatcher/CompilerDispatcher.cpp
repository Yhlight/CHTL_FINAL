#include "CompilerDispatcher.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CodeMerger/CodeMerger.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL_JS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CSSCompiler.h"
#include "JSCompiler.h"
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
    chtlGenerator = std::make_unique<CHTLGenerator>();
    chtlJSLexer = std::make_unique<CHTL_JS::CHTLJSLexer>();
    chtlJSParser = std::make_unique<CHTL_JS::CHTLJSParser>();
    chtlJSGenerator = std::make_unique<CHTL_JS::CHTLJSGenerator>();
    cssCompiler = std::make_unique<CSSCompiler>();
    jsCompiler = std::make_unique<JSCompiler>();
    scanner = std::make_unique<CHTLUnifiedScanner>();
    codeMerger = std::make_unique<CodeMerger>();
    
    // 设置配置
    if (chtlParser) {
        chtlParser->setDebugMode(debugMode);
        chtlParser->setStrictMode(strictMode);
    }
    
    if (chtlGenerator) {
        chtlGenerator->setDebugMode(debugMode);
        chtlGenerator->setStrictMode(strictMode);
        chtlGenerator->setGenerateDefaultStructure(generateDefaultStructure);
        chtlGenerator->setOutputFormat(outputFormat);
    }
    
    if (chtlJSParser) {
        chtlJSParser->setDebugMode(debugMode);
        chtlJSParser->setStrictMode(strictMode);
    }
    
    if (chtlJSGenerator) {
        chtlJSGenerator->setDebugMode(debugMode);
        chtlJSGenerator->setStrictMode(strictMode);
    }
    
    if (cssCompiler) {
        cssCompiler->setDebugMode(debugMode);
        cssCompiler->setStrictMode(strictMode);
    }
    
    if (jsCompiler) {
        jsCompiler->setDebugMode(debugMode);
        jsCompiler->setStrictMode(strictMode);
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
        // 根据CHTL.md规范，实现完整的编译流程
        
        if (debugMode) {
            std::cout << "开始编译 " << scanResult.fragments.size() << " 个代码片段" << std::endl;
        }
        
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
            } else if (fragment.type == "HTML") {
                // HTML片段直接使用
                compiled = fragment.content;
                if (debugMode) {
                    std::cout << "HTML片段直接使用: " << fragment.content.length() << " 字符" << std::endl;
                }
            } else {
                addWarning("未知的代码片段类型: " + fragment.type);
                compiled = fragment.content; // 保持原样
            }
            
            if (!compiled.empty()) {
                result.outputs[fragment.type] += compiled + "\n";
            }
        }
        
        // 合并错误和警告
        result.errors = errors;
        result.warnings = warnings;
        
        if (debugMode) {
            std::cout << "编译完成，生成了 " << result.outputs.size() << " 种类型的输出" << std::endl;
        }
        
    } catch (const std::exception& e) {
        addError("编译时发生错误: " + std::string(e.what()));
        result.errors = errors;
    }
    
    return result;
}

FinalResult CompilerDispatcher::merge(const CompileResult& compileResult) {
    FinalResult result;
    
    try {
        // 根据CHTL.md规范，实现完整的代码合并流程
        
        if (debugMode) {
            std::cout << "开始合并编译结果" << std::endl;
        }
        
        // 使用代码合并器合并结果
        if (codeMerger) {
            result = codeMerger->merge(compileResult);
        } else {
            addError("代码合并器未初始化");
            result.errors = compileResult.errors;
            result.warnings = compileResult.warnings;
            return result;
        }
        
        // 合并错误和警告
        result.errors = compileResult.errors;
        result.warnings = compileResult.warnings;
        
        if (debugMode) {
            std::cout << "代码合并完成" << std::endl;
            std::cout << "HTML长度: " << result.html.length() << " 字符" << std::endl;
            std::cout << "CSS长度: " << result.css.length() << " 字符" << std::endl;
            std::cout << "JavaScript长度: " << result.javascript.length() << " 字符" << std::endl;
        }
        
    } catch (const std::exception& e) {
        addError("合并代码时发生错误: " + std::string(e.what()));
        result.errors = errors;
    }
    
    return result;
}

void CompilerDispatcher::output(const FinalResult& result, const std::string& outputFile) {
    try {
        // 根据CHTL.md规范，实现完整的输出流程
        
        if (debugMode) {
            std::cout << "开始输出到文件: " << outputFile << std::endl;
        }
        
        std::ofstream file(outputFile);
        if (!file.is_open()) {
            addError("无法创建输出文件: " + outputFile);
            return;
        }
        
        // 根据输出格式写入内容
        if (outputFormat == "html") {
            file << result.html;
            if (debugMode) {
                std::cout << "输出HTML格式: " << result.html.length() << " 字符" << std::endl;
            }
        } else if (outputFormat == "css") {
            file << result.css;
            if (debugMode) {
                std::cout << "输出CSS格式: " << result.css.length() << " 字符" << std::endl;
            }
        } else if (outputFormat == "js") {
            file << result.javascript;
            if (debugMode) {
                std::cout << "输出JS格式: " << result.javascript.length() << " 字符" << std::endl;
            }
        } else if (outputFormat == "all") {
            // 输出所有格式
            file << "<!-- CHTL Generated HTML -->\n";
            file << result.html << "\n\n";
            file << "<!-- CHTL Generated CSS -->\n";
            file << "<style>\n" << result.css << "\n</style>\n\n";
            file << "<!-- CHTL Generated JavaScript -->\n";
            file << "<script>\n" << result.javascript << "\n</script>\n";
            if (debugMode) {
                std::cout << "输出所有格式" << std::endl;
            }
        } else {
            // 默认输出HTML
            file << result.html;
            if (debugMode) {
                std::cout << "输出默认HTML格式: " << result.html.length() << " 字符" << std::endl;
            }
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
        // 根据CHTL.md规范，实现完整的CHTL编译流程
        
        if (debugMode) {
            std::cout << "编译CHTL片段: " << fragment.type << " (行 " << fragment.startLine << ")" << std::endl;
        }
        
        // 使用CHTL词法分析器
        chtlLexer->setSource(fragment.content);
        auto tokens = chtlLexer->tokenize();
        
        if (tokens.size() == 0) {
            addWarning("CHTL片段没有生成任何词法单元");
            return fragment.content;
        }
        
        // 使用CHTL语法分析器
        chtlParser->setTokens(tokens);
        auto ast = chtlParser->parse();
        
        if (!ast) {
            addError("CHTL语法分析失败");
            return fragment.content;
        }
        
        // 使用CHTL代码生成器
        if (chtlGenerator) {
            std::string generated = chtlGenerator->generate(ast);
            if (generated.empty()) {
                addWarning("CHTL代码生成器没有生成任何内容");
                return fragment.content;
            }
            return generated;
        }
        
        addError("CHTL代码生成器未初始化");
        return fragment.content;
        
    } catch (const std::exception& e) {
        addError("编译CHTL片段时发生错误: " + std::string(e.what()));
        return fragment.content; // 返回原始内容
    }
}

std::string CompilerDispatcher::compileCHTLJSFragment(const CodeFragment& fragment) {
    try {
        // 根据CHTL.md规范，实现完整的CHTL JS编译流程
        
        if (debugMode) {
            std::cout << "编译CHTL JS片段: " << fragment.type << " (行 " << fragment.startLine << ")" << std::endl;
        }
        
        // 使用CHTL JS词法分析器
        chtlJSLexer->setSource(fragment.content);
        auto tokens = chtlJSLexer->tokenize();
        
        if (tokens.size() == 0) {
            addWarning("CHTL JS片段没有生成任何词法单元");
            return fragment.content;
        }
        
        // 使用CHTL JS语法分析器
        chtlJSParser->setTokens(tokens);
        auto ast = chtlJSParser->parse();
        
        if (!ast) {
            addError("CHTL JS语法分析失败");
            return fragment.content;
        }
        
        // 使用CHTL JS代码生成器
        if (chtlJSGenerator) {
            std::string generated = chtlJSGenerator->generate(ast);
            if (generated.empty()) {
                addWarning("CHTL JS代码生成器没有生成任何内容");
                return fragment.content;
            }
            return generated;
        }
        
        addError("CHTL JS代码生成器未初始化");
        return fragment.content;
        
    } catch (const std::exception& e) {
        addError("编译CHTL JS片段时发生错误: " + std::string(e.what()));
        return fragment.content; // 返回原始内容
    }
}

std::string CompilerDispatcher::compileCSSFragment(const CodeFragment& fragment) {
    try {
        // 根据CHTL.md规范，实现完整的CSS编译流程
        
        if (debugMode) {
            std::cout << "编译CSS片段: " << fragment.type << " (行 " << fragment.startLine << ")" << std::endl;
        }
        
        // 使用CSS编译器
        auto result = cssCompiler->compile(fragment.content);
        
        if (result.success) {
            if (result.css.empty()) {
                addWarning("CSS编译器没有生成任何内容");
                return fragment.content;
            }
            return result.css;
        } else {
            // 合并错误
            for (const auto& error : result.errors) {
                addError("CSS编译错误: " + error);
            }
            for (const auto& warning : result.warnings) {
                addWarning("CSS编译警告: " + warning);
            }
            return fragment.content; // 返回原始内容
        }
        
    } catch (const std::exception& e) {
        addError("编译CSS片段时发生错误: " + std::string(e.what()));
        return fragment.content; // 返回原始内容
    }
}

std::string CompilerDispatcher::compileJSFragment(const CodeFragment& fragment) {
    try {
        // 根据CHTL.md规范，实现完整的JS编译流程
        
        if (debugMode) {
            std::cout << "编译JS片段: " << fragment.type << " (行 " << fragment.startLine << ")" << std::endl;
        }
        
        // 使用JS编译器
        auto result = jsCompiler->compile(fragment.content);
        
        if (result.success) {
            if (result.javascript.empty()) {
                addWarning("JS编译器没有生成任何内容");
                return fragment.content;
            }
            return result.javascript;
        } else {
            // 合并错误
            for (const auto& error : result.errors) {
                addError("JS编译错误: " + error);
            }
            for (const auto& warning : result.warnings) {
                addWarning("JS编译警告: " + warning);
            }
            return fragment.content; // 返回原始内容
        }
        
    } catch (const std::exception& e) {
        addError("编译JS片段时发生错误: " + std::string(e.what()));
        return fragment.content; // 返回原始内容
    }
}

} // namespace CHTL