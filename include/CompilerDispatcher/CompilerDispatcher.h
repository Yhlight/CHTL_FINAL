#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/CHTLContext.h"
#include "CHTL_JS/CHTLJSContext.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 编译器调度器
 * 负责协调各个编译器的执行，管理编译流程
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher() = default;
    
    /**
     * 编译 CHTL 源代码
     * @param sourceCode 源代码
     * @return 编译结果
     */
    std::string compile(const std::string& sourceCode);
    
    /**
     * 设置输出格式
     * @param format 输出格式 (html, css, js)
     */
    void setOutputFormat(const std::string& format);
    
    /**
     * 启用/禁用调试模式
     * @param enabled 是否启用调试
     */
    void setDebugMode(bool enabled);
    
    /**
     * 设置输出目录
     * @param outputDir 输出目录
     */
    void setOutputDirectory(const std::string& outputDir);
    
    /**
     * 添加编译器选项
     * @param option 选项名称
     * @param value 选项值
     */
    void addCompilerOption(const std::string& option, const std::string& value);

private:
    std::unique_ptr<CHTLUnifiedScanner> m_scanner;
    std::unique_ptr<CHTL::CHTLContext> m_chtlContext;
    std::unique_ptr<CHTL_JS::CHTLJSContext> m_chtlJSContext;
    
    std::string m_outputFormat;
    std::string m_outputDirectory;
    bool m_debugMode;
    
    std::map<std::string, std::string> m_compilerOptions;
    
    /**
     * 处理 CHTL 代码片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processCHTLFragment(const CodeFragment& fragment);
    
    /**
     * 处理 CHTL JS 代码片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processCHTLJSFragment(const CodeFragment& fragment);
    
    /**
     * 处理 CSS 代码片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processCSSFragment(const CodeFragment& fragment);
    
    /**
     * 处理 JS 代码片段
     * @param fragment 代码片段
     * @return 处理结果
     */
    std::string processJSFragment(const CodeFragment& fragment);
    
    /**
     * 合并编译结果
     * @param results 编译结果列表
     * @return 合并后的结果
     */
    std::string mergeResults(const std::vector<std::string>& results);
    
    /**
     * 生成最终输出
     * @param htmlContent HTML 内容
     * @param cssContent CSS 内容
     * @param jsContent JS 内容
     * @return 最终输出
     */
    std::string generateFinalOutput(const std::string& htmlContent, 
                                  const std::string& cssContent, 
                                  const std::string& jsContent);
    
    /**
     * 调试输出
     * @param message 调试消息
     */
    void debugOutput(const std::string& message);
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H