#pragma once

#include "CHTL/CHTLContext.h"
#include "CHTL/CodeMerger/CodeMerger.h"
#include "CHTL/Scanner/UnifiedScanner.h"
#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLGenerator.h"
#include "CHTL/CJMOD/CJMODCompiler.h"
#include <string>
#include <memory>
#include <vector>

namespace CHTL {

/**
 * @brief 编译器调度器类
 * 
 * 负责协调各个编译器的执行
 */
class CompilerDispatcher {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit CompilerDispatcher(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~CompilerDispatcher();
    
    /**
     * @brief 编译CHTL源代码
     * @param sourceCode 源代码
     * @param sourceFile 源文件路径
     * @return 编译结果
     */
    std::string compile(const std::string& sourceCode, const std::string& sourceFile = "");
    
    /**
     * @brief 编译文件
     * @param filePath 文件路径
     * @return 编译结果
     */
    std::string compileFile(const std::string& filePath);
    
    /**
     * @brief 重置调度器状态
     */
    void reset();
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 处理CHTL片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processCHTLFragment(const CodeFragment& fragment);
    
    /**
     * @brief 处理CHTL JS片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processCHTLJSFragment(const CodeFragment& fragment);
    
    /**
     * @brief 处理CSS片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processCSSFragment(const CodeFragment& fragment);
    
    /**
     * @brief 处理JavaScript片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processJavaScriptFragment(const CodeFragment& fragment);
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * @brief 添加警告信息
     * @param message 警告信息
     */
    void addWarning(const std::string& message);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<UnifiedScanner> m_scanner;
    std::unique_ptr<CHTLParser> m_chtlParser;
    std::unique_ptr<CHTLGenerator> m_chtlGenerator;
    std::unique_ptr<CJMODCompiler> m_cjmodCompiler;
    std::unique_ptr<CodeMerger> m_codeMerger;
    bool m_debugMode;
    
    // 统计信息
    size_t m_compilationCount;
    size_t m_successfulCompilations;
    size_t m_failedCompilations;
    size_t m_chtlFragmentsProcessed;
    size_t m_chtlJSFragmentsProcessed;
    size_t m_cssFragmentsProcessed;
    size_t m_jsFragmentsProcessed;
};

} // namespace CHTL