#pragma once

#include "CHTLUnifiedScanner.h"
#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "CodeMerger.h"
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief 编译结果
 */
struct CompileResult {
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    bool success;
    
    CompileResult() : success(false) {}
};

/**
 * @brief 编译器调度器
 * 负责协调不同类型的编译器处理代码片段
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 编译源代码
    CompileResult compile(const std::string& source);
    
    // 编译代码片段
    CompileResult compileFragments(const std::vector<CodeFragment>& fragments);
    
    // 设置编译选项
    void setPrettyPrint(bool enabled);
    void setMinify(bool enabled);
    void setDebugMode(bool enabled);
    void setStrictMode(bool enabled);
    
    // 获取编译统计信息
    size_t getCompiledFragments() const;
    size_t getCompiledLines() const;
    size_t getCompiledSize() const;
    double getCompileTime() const;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::shared_ptr<CHTLUnifiedScanner> m_scanner;
    std::shared_ptr<CodeMerger> m_merger;
    
    bool m_prettyPrint;
    bool m_minify;
    bool m_debugMode;
    bool m_strictMode;
    
    size_t m_compiledFragments;
    size_t m_compiledLines;
    size_t m_compiledSize;
    double m_compileTime;
    
    // 编译器实例
    std::shared_ptr<CHTLLexer> m_chtlLexer;
    std::shared_ptr<CHTLParser> m_chtlParser;
    std::shared_ptr<CHTLGenerator> m_chtlGenerator;
    
    // CHTL JS编译器（待实现）
    // std::shared_ptr<CHTLJSLexer> m_chtljsLexer;
    // std::shared_ptr<CHTLJSParser> m_chtljsParser;
    // std::shared_ptr<CHTLJSGenerator> m_chtljsGenerator;
    
    // CSS编译器（ANTLR）
    // std::shared_ptr<CSSLexer> m_cssLexer;
    // std::shared_ptr<CSSParser> m_cssParser;
    // std::shared_ptr<CSSGenerator> m_cssGenerator;
    
    // JavaScript编译器（ANTLR）
    // std::shared_ptr<JavaScriptLexer> m_jsLexer;
    // std::shared_ptr<JavaScriptParser> m_jsParser;
    // std::shared_ptr<JavaScriptGenerator> m_jsGenerator;
    
    // 编译方法
    CompileResult compileCHTL(const CodeFragment& fragment);
    CompileResult compileCHTLJS(const CodeFragment& fragment);
    CompileResult compileCSS(const CodeFragment& fragment);
    CompileResult compileJavaScript(const CodeFragment& fragment);
    
    // 初始化编译器
    void initializeCompilers();
    void initializeCHTLCompiler();
    void initializeCHTLJSCompiler();
    void initializeCSSCompiler();
    void initializeJavaScriptCompiler();
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    bool hasErrors() const;
    bool hasWarnings() const;
    
    std::vector<std::string> m_errors;
    std::vector<std::string> m_warnings;
};

} // namespace CHTL