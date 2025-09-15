#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include "../Scanner/UnifiedScanner.h"
#include "../CHTL/CHTLJS/CHTLJS.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

enum class CompilerType {
    CHTL,           // CHTL编译器
    CHTLJS,         // CHTL JS编译器
    CSS,            // CSS编译器
    JS,             // JavaScript编译器
    HTML,           // HTML编译器
    UNKNOWN         // 未知编译器
};

struct CompilationResult {
    CompilerType type;
    std::string output;
    bool success;
    std::string errorMessage;
    size_t line;
    size_t column;
    
    CompilationResult(CompilerType t = CompilerType::UNKNOWN, bool s = false)
        : type(t), success(s), line(0), column(0) {}
};

struct CompilationContext {
    std::string sourcePath;
    std::string outputPath;
    bool debugMode;
    bool minifyOutput;
    std::unordered_map<std::string, std::string> options;
    
    CompilationContext() : debugMode(false), minifyOutput(false) {}
};

class CompilerDispatcher {
private:
    std::unique_ptr<UnifiedScanner> scanner;
    std::unique_ptr<CHTLJS::CHTLJS> chtljsCompiler;
    
    // 编译器状态
    CompilationContext context;
    std::vector<CodeFragment> fragments;
    std::vector<CompilationResult> results;
    
    // 编译器注册表
    std::unordered_map<CompilerType, std::function<CompilationResult(const CodeFragment&)>> compilers;
    
    // 内部方法
    void initializeCompilers();
    CompilationResult compileCHTL(const CodeFragment& fragment);
    CompilationResult compileCHTLJS(const CodeFragment& fragment);
    CompilationResult compileCSS(const CodeFragment& fragment);
    CompilationResult compileJS(const CodeFragment& fragment);
    CompilationResult compileHTML(const CodeFragment& fragment);
    
    CompilerType getCompilerType(const CodeFragment& fragment) const;
    bool validateFragment(const CodeFragment& fragment) const;
    void logCompilation(const CompilationResult& result) const;
    
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;
    
    // 主要编译方法
    std::vector<CompilationResult> compile(const std::string& source);
    std::vector<CompilationResult> compileFile(const std::string& filePath);
    
    // 配置方法
    void setContext(const CompilationContext& ctx);
    CompilationContext getContext() const;
    void setDebugMode(bool debug);
    void setMinifyOutput(bool minify);
    void setOutputPath(const std::string& path);
    
    // 编译器管理
    void registerCompiler(CompilerType type, std::function<CompilationResult(const CodeFragment&)> compiler);
    void unregisterCompiler(CompilerType type);
    bool hasCompiler(CompilerType type) const;
    
    // 结果管理
    std::vector<CompilationResult> getResults() const;
    std::vector<CompilationResult> getResultsByType(CompilerType type) const;
    CompilationResult getResult(size_t index) const;
    size_t getResultCount() const;
    
    // 片段管理
    std::vector<CodeFragment> getFragments() const;
    std::vector<CodeFragment> getFragmentsByType(CodeType type) const;
    
    // 状态查询
    bool hasErrors() const;
    std::vector<std::string> getErrorMessages() const;
    bool isDebugMode() const;
    bool isMinifyOutput() const;
    
    // 工具方法
    void clear();
    void reset();
    
    // 调试方法
    void printResults() const;
    void printFragments() const;
    void printStatistics() const;
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H