#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

namespace CHTL {

// 前向声明
class CHTLLexer;
class CHTLParser;
class CHTLGenerator;
class UnifiedScanner;

enum class CompileMode {
    FULL_COMPILE,       // 完整编译
    HTML_ONLY,          // 仅生成HTML
    CSS_ONLY,           // 仅生成CSS
    JS_ONLY,            // 仅生成JS
    DEBUG_MODE          // 调试模式
};

struct CompileOptions {
    CompileMode mode = CompileMode::FULL_COMPILE;
    bool prettyPrint = true;
    bool minify = false;
    bool includeComments = false;
    bool validateHTML = true;
    bool validateCSS = true;
    bool validateJS = true;
    std::string outputDir = "";
    std::map<std::string, std::string> defines;
};

struct CompileResult {
    bool success;
    std::string message;
    std::string htmlOutput;
    std::string cssOutput;
    std::string jsOutput;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    double compileTime; // 编译时间（毫秒）
};

class CompilerDispatcher {
private:
    CompileOptions options;
    std::unique_ptr<UnifiedScanner> scanner;
    std::map<std::string, std::string> fileCache;
    
    // 编译阶段
    std::string preprocess(const std::string& source);
    std::vector<std::string> scanAndSeparate(const std::string& source);
    std::string compileCHTL(const std::string& chtlCode);
    std::string compileCSS(const std::string& cssCode);
    std::string compileJS(const std::string& jsCode);
    std::string mergeOutputs(const std::string& html, const std::string& css, const std::string& js);
    
    // 文件处理
    std::string readFile(const std::string& filePath);
    void writeFile(const std::string& filePath, const std::string& content);
    bool fileExists(const std::string& filePath);
    std::string getFileExtension(const std::string& filePath);
    
    // 错误处理
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // 缓存管理
    void clearCache();
    std::string getCachedFile(const std::string& filePath);
    void cacheFile(const std::string& filePath, const std::string& content);
    
public:
    CompilerDispatcher();
    CompilerDispatcher(const CompileOptions& options);
    ~CompilerDispatcher() = default;
    
    // 主要编译方法
    CompileResult compile(const std::string& inputFile, const std::string& outputFile = "");
    CompileResult compileString(const std::string& source, const std::string& outputFile = "");
    
    // 选项管理
    void setOptions(const CompileOptions& options);
    CompileOptions getOptions() const;
    
    // 错误和警告
    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    void clearErrors();
    void clearWarnings();
    
    // 文件管理
    void addIncludePath(const std::string& path);
    void removeIncludePath(const std::string& path);
    std::vector<std::string> getIncludePaths() const;
    
    // 调试方法
    void enableDebugMode(bool enable = true);
    void setVerbose(bool verbose = true);
    std::string getDebugInfo() const;
    
    // 统计信息
    struct CompileStats {
        size_t filesProcessed;
        size_t linesProcessed;
        size_t tokensGenerated;
        size_t nodesGenerated;
        double totalTime;
    };
    
    CompileStats getStats() const;
    void resetStats();
    
private:
    CompileStats stats;
    bool debugMode;
    bool verbose;
    std::vector<std::string> includePaths;
};

} // namespace CHTL