#pragma once

#include "CHTL/CHTLContext.h"
#include "CHTL_JS/CHTLJSContext.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

// 前向声明
class CHTLLexer;
class CHTLParser;
class CHTLGenerator;
class CHTLLoader;

class CHTLJSLexer;
class CHTLJSParser;
class CHTLJSGenerator;

class CodeMerger;

/**
 * @brief 编译器类型枚举
 */
enum class CompilerType {
    CHTL,           // CHTL编译器
    CHTL_JS,        // CHTL JS编译器
    CSS,            // CSS编译器
    JS,             // JavaScript编译器
    HTML            // HTML编译器
};

/**
 * @brief 编译结果结构
 */
struct CompilationResult {
    bool success;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> metadata;
    
    CompilationResult() : success(false) {}
};

/**
 * @brief CHTL编译器调度器
 * 
 * 负责协调各个编译器的执行，管理编译流程
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;

    /**
     * @brief 编译CHTL文件
     * @param filePath 文件路径
     * @return 编译结果
     */
    CompilationResult compileFile(const std::string& filePath);
    
    /**
     * @brief 编译CHTL代码
     * @param code 代码内容
     * @return 编译结果
     */
    CompilationResult compileCode(const std::string& code);
    
    /**
     * @brief 编译多个文件
     * @param filePaths 文件路径列表
     * @return 编译结果
     */
    CompilationResult compileFiles(const std::vector<std::string>& filePaths);

    /**
     * @brief 设置编译选项
     * @param minify 是否压缩输出
     * @param debug 是否调试模式
     * @param includeSourceMaps 是否包含源码映射
     */
    void setCompileOptions(bool minify, bool debug, bool includeSourceMaps);
    
    /**
     * @brief 设置输出目录
     * @param outputDir 输出目录
     */
    void setOutputDirectory(const std::string& outputDir);
    
    /**
     * @brief 添加模块搜索路径
     * @param path 搜索路径
     */
    void addModulePath(const std::string& path);
    
    /**
     * @brief 设置官方模块路径
     * @param path 官方模块路径
     */
    void setOfficialModulePath(const std::string& path);

    /**
     * @brief 获取编译统计信息
     * @return 统计信息映射表
     */
    const std::unordered_map<std::string, int>& getCompilationStats() const;
    
    /**
     * @brief 重置编译统计信息
     */
    void resetStats();

    /**
     * @brief 获取编译器实例
     * @param type 编译器类型
     * @return 编译器实例指针
     */
    template<typename T>
    std::shared_ptr<T> getCompiler(CompilerType type) const;

private:
    // 编译器实例
    std::shared_ptr<CHTLLexer> m_chtlLexer;
    std::shared_ptr<CHTLParser> m_chtlParser;
    std::shared_ptr<CHTLGenerator> m_chtlGenerator;
    std::shared_ptr<CHTLLoader> m_chtlLoader;
    
    std::shared_ptr<CHTLJSLexer> m_chtljsLexer;
    std::shared_ptr<CHTLJSParser> m_chtljsParser;
    std::shared_ptr<CHTLJSGenerator> m_chtljsGenerator;
    
    std::shared_ptr<CodeMerger> m_codeMerger;
    
    // 上下文
    std::shared_ptr<CHTLContext> m_chtlContext;
    std::shared_ptr<CHTL_JS::CHTLJSContext> m_chtljsContext;
    
    // 编译选项
    bool m_minify;
    bool m_debug;
    bool m_includeSourceMaps;
    std::string m_outputDirectory;
    
    // 统计信息
    std::unordered_map<std::string, int> m_stats;
    
    // 内部方法
    void initializeCompilers();
    void initializeContexts();
    
    CompilationResult processCodeFragments(const std::vector<CodeFragment>& fragments);
    CompilationResult compileCHTLFragment(const CodeFragment& fragment);
    CompilationResult compileCHTLJSFragment(const CodeFragment& fragment);
    CompilationResult compileCSSFragment(const CodeFragment& fragment);
    CompilationResult compileJSFragment(const CodeFragment& fragment);
    CompilationResult compileHTMLFragment(const CodeFragment& fragment);
    
    void updateStats(const std::string& key, int value);
    void incrementStats(const std::string& key);
    
    // 错误处理
    void addError(CompilationResult& result, const std::string& error);
    void addWarning(CompilationResult& result, const std::string& warning);
    
    // 文件处理
    std::string readFile(const std::string& filePath) const;
    void writeFile(const std::string& filePath, const std::string& content) const;
    bool fileExists(const std::string& filePath) const;
    
    // 路径处理
    std::string resolvePath(const std::string& path) const;
    std::string getOutputPath(const std::string& inputPath, const std::string& extension) const;
};

} // namespace CHTL