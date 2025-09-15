#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include "../Scanner/UnifiedScanner.h"
#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * 编译器调度器
 * 协调CHTL、CHTL JS、CSS、JS编译器的执行
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 主要编译方法
    std::string compile(const std::string& sourceCode);
    std::string compileFile(const std::string& filePath);
    
    // 分步编译
    std::vector<CodeFragment> scanCode(const std::string& sourceCode);
    std::string compileCHTL(const std::string& chtlCode);
    std::string compileCHTLJS(const std::string& chtljsCode);
    std::string compileCSS(const std::string& cssCode);
    std::string compileJS(const std::string& jsCode);
    
    // 配置选项
    void setOutputFormat(const std::string& format);
    void setMinifyOutput(bool minify);
    void setIncludeSourceMaps(bool include);
    void setOptimizeOutput(bool optimize);
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&, int, int)> handler);
    void reportError(const std::string& message, int line = 0, int column = 0);
    
    // 状态查询
    bool hasErrors() const;
    const std::vector<std::string>& getErrors() const;
    void clearErrors();
    
    // 编译统计
    int getCompilationTime() const;
    size_t getInputSize() const;
    size_t getOutputSize() const;

private:
    // 编译器实例
    std::unique_ptr<UnifiedScanner> scanner_;
    std::unique_ptr<CHTLLexer> chtl_lexer_;
    std::unique_ptr<CHTLParser> chtl_parser_;
    std::unique_ptr<CHTLGenerator> chtl_generator_;
    std::unique_ptr<CHTLJSLexer> chtljs_lexer_;
    std::unique_ptr<CHTLJSParser> chtljs_parser_;
    std::unique_ptr<CHTLJSGenerator> chtljs_generator_;
    
    // 配置
    std::string output_format_;
    bool minify_output_;
    bool include_source_maps_;
    bool optimize_output_;
    
    // 错误处理
    std::function<void(const std::string&, int, int)> error_handler_;
    std::vector<std::string> errors_;
    
    // 编译统计
    int compilation_time_;
    size_t input_size_;
    size_t output_size_;
    
    // 辅助方法
    void initializeCompilers();
    std::string processCodeFragment(const CodeFragment& fragment);
    std::string mergeCompilationResults(const std::map<CodeFragmentType, std::string>& results);
    void validateCompilationResults(const std::map<CodeFragmentType, std::string>& results);
    
    // 错误处理辅助
    void logError(const std::string& message, int line = 0, int column = 0);
    void validateInput(const std::string& sourceCode);
    
    // 性能监控
    void startTimer();
    void endTimer();
    void updateStatistics(size_t inputSize, size_t outputSize);
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H