#pragma once

#include "CHTL/CHTLNode.h"
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLContext.h"
#include "CHTL/TemplateParser.h"
#include <string>
#include <memory>
#include <vector>
#include <map>

namespace CHTL {

/**
 * @brief 编译器调度器 - 根据代码片段类型分发到不同的编译器
 * 
 * 这是CHTL编译系统的核心组件，负责：
 * 1. 接收统一扫描器的输出
 * 2. 识别代码片段类型（CHTL、CHTL JS、CSS、JavaScript）
 * 3. 将代码片段分发给相应的编译器
 * 4. 合并所有编译结果
 */
class CompilerDispatcher {
public:
    /**
     * @brief 代码片段类型
     */
    enum class FragmentType {
        CHTL,           // CHTL模板代码
        CHTL_JS,        // CHTL JS代码
        CSS,            // CSS代码
        JAVASCRIPT,     // JavaScript代码
        HTML,           // 原始HTML代码
        UNKNOWN         // 未知类型
    };

    /**
     * @brief 代码片段信息
     */
    struct CodeFragment {
        FragmentType type;
        std::string content;
        std::string language_hint;
        int start_line;
        int end_line;
        std::map<std::string, std::string> metadata;
    };

    /**
     * @brief 编译结果
     */
    struct CompilationResult {
        bool success;
        std::string output;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> metadata;
        std::vector<TemplateInfo> templates;
    };

    CompilerDispatcher();
    ~CompilerDispatcher() = default;

    /**
     * @brief 编译CHTL文件
     * @param input_file 输入文件路径
     * @param output_file 输出文件路径
     * @return 编译结果
     */
    CompilationResult compileFile(const std::string& input_file, const std::string& output_file);

    /**
     * @brief 编译CHTL代码
     * @param code CHTL代码
     * @return 编译结果
     */
    CompilationResult compileCode(const std::string& code);

    /**
     * @brief 设置编译上下文
     * @param context 编译上下文
     */
    void setContext(std::shared_ptr<CHTLContext> context);

    /**
     * @brief 获取编译上下文
     * @return 编译上下文
     */
    std::shared_ptr<CHTLContext> getContext() const;

    /**
     * @brief 添加代码片段
     * @param fragment 代码片段
     */
    void addFragment(const CodeFragment& fragment);

    /**
     * @brief 清空代码片段
     */
    void clearFragments();

    /**
     * @brief 获取所有代码片段
     * @return 代码片段列表
     */
    const std::vector<CodeFragment>& getFragments() const;

private:
    std::shared_ptr<CHTLContext> context_;
    std::vector<CodeFragment> fragments_;
    std::unique_ptr<UnifiedScanner> scanner_;
    std::unique_ptr<TemplateParser> template_parser_;

    /**
     * @brief 识别代码片段类型
     * @param content 代码内容
     * @param language_hint 语言提示
     * @return 代码片段类型
     */
    FragmentType identifyFragmentType(const std::string& content, const std::string& language_hint = "");

    /**
     * @brief 编译CHTL片段
     * @param fragment 代码片段
     * @return 编译结果
     */
    CompilationResult compileCHTLFragment(const CodeFragment& fragment);

    /**
     * @brief 编译CHTL JS片段
     * @param fragment 代码片段
     * @return 编译结果
     */
    CompilationResult compileCHTLJSFragment(const CodeFragment& fragment);

    /**
     * @brief 编译CSS片段
     * @param fragment 代码片段
     * @return 编译结果
     */
    CompilationResult compileCSSFragment(const CodeFragment& fragment);

    /**
     * @brief 编译JavaScript片段
     * @param fragment 代码片段
     * @return 编译结果
     */
    CompilationResult compileJavaScriptFragment(const CodeFragment& fragment);

    /**
     * @brief 编译HTML片段
     * @param fragment 代码片段
     * @return 编译结果
     */
    CompilationResult compileHTMLFragment(const CodeFragment& fragment);

    /**
     * @brief 合并编译结果
     * @param results 编译结果列表
     * @return 合并后的结果
     */
    CompilationResult mergeResults(const std::vector<CompilationResult>& results);

    /**
     * @brief 处理模板应用
     * @param content 内容
     * @return 处理后的内容
     */
    std::string applyTemplates(const std::string& content);

    /**
     * @brief 处理自定义元素
     * @param content 内容
     * @return 处理后的内容
     */
    std::string applyCustomElements(const std::string& content);

    /**
     * @brief 处理导入
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processImports(const std::string& content);

    /**
     * @brief 处理命名空间
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processNamespaces(const std::string& content);

    /**
     * @brief 处理配置
     * @param content 内容
     * @return 处理后的内容
     */
    std::string processConfiguration(const std::string& content);
    void applyTemplatesToFragments(const std::vector<TemplateInfo>& templates);
};

} // namespace CHTL