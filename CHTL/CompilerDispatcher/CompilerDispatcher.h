#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include "Common/CommonTypes.h"

// 包含完整的头文件以避免前向声明问题
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL_JS/CHTLJSLexer/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include "CSSCompiler.h"
#include "JSCompiler.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "CodeMerger/CodeMerger.h"

namespace CHTL {

class CompilerDispatcher {
private:
    // 配置
    bool debugMode;
    bool strictMode;
    bool generateDefaultStructure;
    std::string outputFormat;
    
    // 编译器实例
    std::unique_ptr<CHTLLexer> chtlLexer;
    std::unique_ptr<CHTLParser> chtlParser;
    std::unique_ptr<CHTLGenerator> chtlGenerator;
    std::unique_ptr<CHTL_JS::CHTLJSLexer> chtlJSLexer;
    std::unique_ptr<CHTL_JS::CHTLJSParser> chtlJSParser;
    std::unique_ptr<CHTL_JS::CHTLJSGenerator> chtlJSGenerator;
    std::unique_ptr<CSSCompiler> cssCompiler;
    std::unique_ptr<JSCompiler> jsCompiler;
    
    // 统一扫描器
    std::unique_ptr<CHTLUnifiedScanner> scanner;
    
    // 代码合并器
    std::unique_ptr<CodeMerger> codeMerger;
    
    // 错误和警告
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 辅助方法
    void initializeCompilers();
    void clearErrors();
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    
    // 编译各类型代码片段
    std::string compileCHTLFragment(const CodeFragment& fragment);
    std::string compileCHTLJSFragment(const CodeFragment& fragment);
    std::string compileCSSFragment(const CodeFragment& fragment);
    std::string compileJSFragment(const CodeFragment& fragment);

public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 配置方法
    void setDebugMode(bool debug) { debugMode = debug; }
    void setStrictMode(bool strict) { strictMode = strict; }
    void setGenerateDefaultStructure(bool generate) { generateDefaultStructure = generate; }
    void setOutputFormat(const std::string& format) { outputFormat = format; }
    
    // 主要编译流程
    ScanResult scan(const std::string& inputFile);
    CompileResult compile(const ScanResult& scanResult);
    FinalResult merge(const CompileResult& compileResult);
    void output(const FinalResult& result, const std::string& outputFile);
    
    // 一步完成编译
    FinalResult compileFile(const std::string& inputFile);
    void compileFileToOutput(const std::string& inputFile, const std::string& outputFile);
    
    // 获取错误和警告
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    
    // 清除错误和警告
    void clearMessages();
    
    // 验证配置
    bool validateConfiguration() const;
    
    // 获取编译器信息
    std::string getCompilerInfo() const;
    std::string getVersion() const;
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H