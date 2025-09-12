#pragma once

#include "CHTL/CHTLContext.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "CHTL_JS/CHTLJSLexer.h"
#include "CHTL_JS/CHTLJSParser.h"
#include "CHTL_JS/CHTLJSGenerator.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 编译器类型枚举
 */
enum class CompilerType {
    CHTL,           // CHTL编译器
    CHTL_JS,        // CHTL JS编译器
    CSS,            // CSS编译器
    JAVASCRIPT,     // JavaScript编译器
    UNKNOWN         // 未知类型
};

/**
 * @brief 编译结果结构
 */
struct CompileResult {
    bool success;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    CompileResult() : success(false) {}
};

/**
 * @brief 编译器调度器
 * 负责协调各个编译器，实现完整的编译流程
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;

    /**
     * @brief 编译CHTL源代码
     * @param source_code 源代码
     * @return 编译结果
     */
    CompileResult compile(const std::string& source_code);

    /**
     * @brief 编译CHTL文件
     * @param file_path 文件路径
     * @return 编译结果
     */
    CompileResult compileFile(const std::string& file_path);

    /**
     * @brief 设置输出选项
     * @param minify_html 是否压缩HTML
     * @param minify_css 是否压缩CSS
     * @param minify_js 是否压缩JavaScript
     * @param strict_mode 是否使用严格模式
     */
    void setOutputOptions(bool minify_html, bool minify_css, bool minify_js, bool strict_mode);

    /**
     * @brief 设置调试模式
     * @param enabled 是否启用调试模式
     */
    void setDebugMode(bool enabled);

    /**
     * @brief 获取编译统计信息
     * @return 统计信息
     */
    std::unordered_map<std::string, int> getCompileStats() const;

    /**
     * @brief 重置编译统计信息
     */
    void resetStats();

private:
    /**
     * @brief 识别编译器类型
     * @param source_code 源代码
     * @return 编译器类型
     */
    CompilerType identifyCompilerType(const std::string& source_code);

    /**
     * @brief 编译CHTL代码
     * @param source_code 源代码
     * @return 编译结果
     */
    CompileResult compileCHTL(const std::string& source_code);

    /**
     * @brief 编译CHTL JS代码
     * @param source_code 源代码
     * @return 编译结果
     */
    CompileResult compileCHTLJS(const std::string& source_code);

    /**
     * @brief 编译CSS代码
     * @param source_code 源代码
     * @return 编译结果
     */
    CompileResult compileCSS(const std::string& source_code);

    /**
     * @brief 编译JavaScript代码
     * @param source_code 源代码
     * @return 编译结果
     */
    CompileResult compileJavaScript(const std::string& source_code);

    /**
     * @brief 合并编译结果
     * @param results 编译结果列表
     * @return 合并后的结果
     */
    CompileResult mergeResults(const std::vector<CompileResult>& results);

    /**
     * @brief 验证编译结果
     * @param result 编译结果
     * @return 是否有效
     */
    bool validateResult(const CompileResult& result);

    /**
     * @brief 添加错误信息
     * @param result 编译结果
     * @param error 错误信息
     */
    void addError(CompileResult& result, const std::string& error);

    /**
     * @brief 添加警告信息
     * @param result 编译结果
     * @param warning 警告信息
     */
    void addWarning(CompileResult& result, const std::string& warning);

private:
    // 编译器实例
    std::unique_ptr<UnifiedScanner> scanner_;
    std::unique_ptr<CHTLLexer> chtl_lexer_;
    std::unique_ptr<CHTLParser> chtl_parser_;
    std::unique_ptr<CHTLGenerator> chtl_generator_;
    std::unique_ptr<CHTLJSLexer> chtl_js_lexer_;
    std::unique_ptr<CHTLJSParser> chtl_js_parser_;
    std::unique_ptr<CHTLJSGenerator> chtl_js_generator_;
    
    // 配置选项
    bool minify_html_;
    bool minify_css_;
    bool minify_js_;
    bool strict_mode_;
    bool debug_mode_;
    
    // 统计信息
    std::unordered_map<std::string, int> stats_;
    
    // 错误和警告计数
    int error_count_;
    int warning_count_;
};

} // namespace CHTL