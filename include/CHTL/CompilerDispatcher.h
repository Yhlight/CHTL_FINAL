#pragma once

#include "CHTL/Common.h"
#include "CHTL/UnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

// 前向声明
class UnifiedScanner;

/**
 * 编译器调度器 - 协调各个编译器的执行
 */
class CompilerDispatcher {
public:
    // 编译配置
    struct CompileConfig {
        bool enableInlineCSS = false;
        bool enableInlineJS = false;
        bool enableDefaultStruct = false;
        bool enableDebugMode = false;
        std::string outputDirectory = ".";
        std::string moduleDirectory = "module";
    };

    // 编译结果
    struct CompileResult {
        bool success;
        std::string htmlOutput;
        std::string cssOutput;
        std::string jsOutput;
        std::string errorMessage;
        std::vector<std::string> warnings;
        double compileTime; // 编译时间（秒）
        size_t memoryUsage; // 内存使用量（字节）
        
        CompileResult() : success(false), compileTime(0.0), memoryUsage(0) {}
    };

public:
    CompilerDispatcher();
    ~CompilerDispatcher();

    /**
     * 编译CHTL源代码
     * @param sourceCode 源代码
     * @param config 编译配置
     * @return 编译结果
     */
    CompileResult compile(const std::string& sourceCode, const CompileConfig& config);

    /**
     * 从文件编译
     * @param filePath 文件路径
     * @param config 编译配置
     * @return 编译结果
     */
    CompileResult compileFromFile(const std::string& filePath, const CompileConfig& config);

    /**
     * 设置模块搜索路径
     * @param paths 路径列表
     */
    void setModuleSearchPaths(const std::vector<std::string>& paths);

    /**
     * 注册自定义编译器
     * @param name 编译器名称
     * @param compiler 编译器实例
     */
    void registerCompiler(const std::string& name, std::shared_ptr<void> compiler);

    /**
     * 获取编译统计信息
     * @return 统计信息
     */
    std::map<std::string, std::string> getCompileStatistics() const;

private:
    // 初始化编译器
    void initializeCompilers();
    
    // 处理模块导入
    std::string resolveModuleImports(const std::string& sourceCode, 
                                   const CompileConfig& config);
    
    // 执行编译流程
    CompileResult executeCompilePipeline(const std::string& sourceCode, 
                                       const CompileConfig& config);
    
    // 内联处理
    std::string insertCSSIntoHTML(const std::string& html, const std::string& css);
    std::string insertJSIntoHTML(const std::string& html, const std::string& js);
    
    // 错误处理
    void handleCompileError(const std::string& error, CompileResult& result);
    void handleCompileWarning(const std::string& warning, CompileResult& result);

private:
    std::unique_ptr<UnifiedScanner> scanner_;
    // 其他编译器将在后续实现中添加
    
    std::vector<std::string> moduleSearchPaths_;
    std::map<std::string, std::shared_ptr<void>> customCompilers_;
    
    // 编译统计
    std::map<std::string, std::string> statistics_;
    size_t totalCompileCount_;
    double totalCompileTime_;
    size_t totalMemoryUsage_;
};

} // namespace CHTL