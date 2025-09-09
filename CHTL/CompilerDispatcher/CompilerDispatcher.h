#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

// 代码片段结构
struct CodeFragment {
    std::string type;        // 片段类型：CHTL, CHTL_JS, CSS, JS
    std::string content;     // 片段内容
    size_t startLine;        // 起始行号
    size_t endLine;          // 结束行号
    size_t startColumn;      // 起始列号
    size_t endColumn;        // 结束列号
    std::map<std::string, std::string> metadata; // 元数据
};

// 扫描结果
struct ScanResult {
    std::vector<CodeFragment> fragments;
    std::string originalSource;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// 编译结果
struct CompileResult {
    std::map<std::string, std::string> outputs; // 类型 -> 输出内容
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata; // 元数据
};

// 最终结果
struct FinalResult {
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata;
};

class CompilerDispatcher {
private:
    // 配置
    bool debugMode;
    bool strictMode;
    bool generateDefaultStructure;
    std::string outputFormat;
    
    // 编译器实例
    std::unique_ptr<class CHTLLexer> chtlLexer;
    std::unique_ptr<class CHTLParser> chtlParser;
    // std::unique_ptr<class CHTLJSCompiler> chtlJSCompiler;
    // std::unique_ptr<class CSSCompiler> cssCompiler;
    // std::unique_ptr<class JSCompiler> jsCompiler;
    
    // 统一扫描器
    std::unique_ptr<class CHTLUnifiedScanner> scanner;
    
    // 代码合并器
    std::unique_ptr<class CodeMerger> codeMerger;
    
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