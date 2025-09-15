#ifndef COMPILERDISPATCHER_H
#define COMPILERDISPATCHER_H

#include "../Scanner/UnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 编译结果
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
 * 编译器调度器
 * 负责协调各个编译器的执行
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();

    // 编译方法
    CompileResult compile(const std::string& source);
    CompileResult compileFile(const std::string& filename);
    
    // 配置
    void setOutputDirectory(const std::string& directory);
    void setDebugMode(bool enabled);
    void setOptimizationLevel(int level);
    
    // 状态查询
    bool isCompiling() const;
    std::vector<std::string> getLastErrors() const;
    std::vector<std::string> getLastWarnings() const;

private:
    std::string outputDirectory;
    bool debugMode;
    int optimizationLevel;
    bool compiling;
    std::vector<std::string> lastErrors;
    std::vector<std::string> lastWarnings;
    
    // 编译器实例
    std::unique_ptr<UnifiedScanner> scanner;
    
    // 编译方法
    CompileResult compileFragments(const std::vector<CodeFragment>& fragments);
    std::string compileCHTL(const std::string& code);
    std::string compileCHTLJS(const std::string& code);
    std::string compileCSS(const std::string& code);
    std::string compileJS(const std::string& code);
    
    // 代码合并
    std::string mergeCode(const CompileResult& result);
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    void clearMessages();
    
    // 文件操作
    bool writeFile(const std::string& filename, const std::string& content);
    std::string readFile(const std::string& filename);
};

} // namespace CHTL

#endif // COMPILERDISPATCHER_H