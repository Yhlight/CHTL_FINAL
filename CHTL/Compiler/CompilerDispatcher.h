#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {
namespace Compiler {

// 编译器类型枚举
enum class CompilerType {
    CHTL,      // CHTL编译器
    CHTL_JS,   // CHTL JS编译器
    CSS,       // CSS编译器
    JS,        // JavaScript编译器
    HTML       // HTML编译器
};

// 编译结果结构
struct CompileResult {
    bool success;
    std::string output;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    CompilerType compilerType;
    std::string sourceFile;
    std::string outputFile;
    
    CompileResult() : success(false), compilerType(CompilerType::CHTL) {}
};

// 编译器接口
class ICompiler {
public:
    virtual ~ICompiler() = default;
    virtual CompileResult compile(const std::string& source, const std::string& filename = "") = 0;
    virtual CompilerType getType() const = 0;
    virtual std::string getOutputExtension() const = 0;
    virtual bool supportsFile(const std::string& filename) const = 0;
};

// 编译器配置
struct CompilerConfig {
    bool enableOptimization;
    bool enableMinification;
    bool enableSourceMaps;
    std::string outputDirectory;
    std::string tempDirectory;
    std::map<std::string, std::string> customOptions;
    
    CompilerConfig() : enableOptimization(true), enableMinification(false), enableSourceMaps(false) {}
};

// 编译器调度器
class CompilerDispatcher {
private:
    std::map<CompilerType, std::unique_ptr<ICompiler>> compilers;
    CompilerConfig config;
    std::vector<std::string> globalErrors;
    std::vector<std::string> globalWarnings;
    
    // 文件类型检测
    CompilerType detectCompilerType(const std::string& filename) const;
    
    // 依赖解析
    std::vector<std::string> resolveDependencies(const std::string& source, const std::string& filename) const;
    
    // 编译顺序优化
    std::vector<std::string> optimizeCompileOrder(const std::vector<std::string>& files) const;
    
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 编译器注册
    void registerCompiler(CompilerType type, std::unique_ptr<ICompiler> compiler);
    void unregisterCompiler(CompilerType type);
    
    // 配置管理
    void setConfig(const CompilerConfig& config);
    CompilerConfig getConfig() const;
    
    // 单文件编译
    CompileResult compileFile(const std::string& filename);
    CompileResult compileSource(const std::string& source, const std::string& filename);
    
    // 多文件编译
    std::vector<CompileResult> compileFiles(const std::vector<std::string>& filenames);
    std::vector<CompileResult> compileProject(const std::string& projectPath);
    
    // 批量编译
    CompileResult compileBatch(const std::vector<std::string>& sources, const std::vector<std::string>& filenames);
    
    // 增量编译
    std::vector<CompileResult> incrementalCompile(const std::vector<std::string>& changedFiles);
    
    // 并行编译
    std::vector<CompileResult> parallelCompile(const std::vector<std::string>& filenames);
    
    // 编译监控
    void setCompileCallback(std::function<void(const CompileResult&)> callback);
    void setProgressCallback(std::function<void(int, int)> callback);
    
    // 错误和警告管理
    std::vector<std::string> getGlobalErrors() const;
    std::vector<std::string> getGlobalWarnings() const;
    void clearGlobalErrors();
    void clearGlobalWarnings();
    
    // 编译器状态
    bool isCompilerRegistered(CompilerType type) const;
    std::vector<CompilerType> getRegisteredCompilers() const;
    
    // 编译统计
    struct CompileStatistics {
        int totalFiles;
        int successfulFiles;
        int failedFiles;
        double totalTime;
        double averageTime;
        std::map<CompilerType, int> compilerUsage;
    };
    
    CompileStatistics getCompileStatistics() const;
    void resetStatistics();
    
    // 调试和诊断
    std::string generateCompileReport() const;
    void enableDebugMode(bool enable);
    bool isDebugMode() const;
    
private:
    std::function<void(const CompileResult&)> compileCallback;
    std::function<void(int, int)> progressCallback;
    bool debugMode;
    CompileStatistics statistics;
    
    // 内部辅助方法
    void updateStatistics(const CompileResult& result, double compileTime);
    void notifyProgress(int current, int total);
    void notifyCompileComplete(const CompileResult& result);
    std::string generateSourceMap(const CompileResult& result) const;
};

} // namespace Compiler
} // namespace CHTL

#endif // COMPILER_DISPATCHER_H