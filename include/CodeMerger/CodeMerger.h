#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 代码类型枚举
 */
enum class CodeType {
    HTML,
    CSS,
    JAVASCRIPT,
    CHTL,
    CHTL_JS
};

/**
 * @brief 代码块结构
 */
struct CodeBlock {
    CodeType type;
    std::string content;
    std::string sourceFile;
    size_t line;
    size_t column;
    std::unordered_map<std::string, std::string> metadata;
    
    CodeBlock(CodeType t = CodeType::HTML,
              const std::string& c = "",
              const std::string& file = "",
              size_t l = 0,
              size_t col = 0)
        : type(t), content(c), sourceFile(file), line(l), column(col) {}
};

/**
 * @brief 合并选项结构
 */
struct MergeOptions {
    bool minifyHTML;
    bool minifyCSS;
    bool minifyJS;
    bool includeSourceMaps;
    bool preserveComments;
    bool inlineCSS;
    bool inlineJS;
    std::string cssOutputPath;
    std::string jsOutputPath;
    
    MergeOptions() 
        : minifyHTML(false), minifyCSS(false), minifyJS(false),
          includeSourceMaps(false), preserveComments(true),
          inlineCSS(false), inlineJS(false) {}
};

/**
 * @brief 合并结果结构
 */
struct MergeResult {
    bool success;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> metadata;
    
    MergeResult() : success(false) {}
};

/**
 * @brief CHTL代码合并器
 * 
 * 负责将编译后的代码片段合并成最终的HTML文档
 */
class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger() = default;

    /**
     * @brief 合并代码块
     * @param blocks 代码块列表
     * @param options 合并选项
     * @return 合并结果
     */
    MergeResult merge(const std::vector<CodeBlock>& blocks, const MergeOptions& options = MergeOptions());
    
    /**
     * @brief 合并HTML代码
     * @param htmlBlocks HTML代码块列表
     * @param options 合并选项
     * @return 合并结果
     */
    MergeResult mergeHTML(const std::vector<CodeBlock>& htmlBlocks, const MergeOptions& options = MergeOptions());
    
    /**
     * @brief 合并CSS代码
     * @param cssBlocks CSS代码块列表
     * @param options 合并选项
     * @return 合并结果
     */
    MergeResult mergeCSS(const std::vector<CodeBlock>& cssBlocks, const MergeOptions& options = MergeOptions());
    
    /**
     * @brief 合并JavaScript代码
     * @param jsBlocks JavaScript代码块列表
     * @param options 合并选项
     * @return 合并结果
     */
    MergeResult mergeJS(const std::vector<CodeBlock>& jsBlocks, const MergeOptions& options = MergeOptions());

    /**
     * @brief 设置默认合并选项
     * @param options 默认选项
     */
    void setDefaultOptions(const MergeOptions& options);
    
    /**
     * @brief 获取默认合并选项
     * @return 默认选项
     */
    const MergeOptions& getDefaultOptions() const;

    /**
     * @brief 添加CSS预处理器
     * @param name 预处理器名称
     * @param processor 处理函数
     */
    void addCSSPreprocessor(const std::string& name, 
                           std::function<std::string(const std::string&)> processor);
    
    /**
     * @brief 添加JavaScript预处理器
     * @param name 预处理器名称
     * @param processor 处理函数
     */
    void addJSPreprocessor(const std::string& name, 
                          std::function<std::string(const std::string&)> processor);

    /**
     * @brief 获取合并统计信息
     * @return 统计信息映射表
     */
    const std::unordered_map<std::string, int>& getMergeStats() const;
    
    /**
     * @brief 重置统计信息
     */
    void resetStats();

private:
    // 默认选项
    MergeOptions m_defaultOptions;
    
    // 预处理器
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> m_cssPreprocessors;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> m_jsPreprocessors;
    
    // 统计信息
    std::unordered_map<std::string, int> m_stats;
    
    // 内部方法
    std::string processHTML(const std::string& html, const MergeOptions& options);
    std::string processCSS(const std::string& css, const MergeOptions& options);
    std::string processJS(const std::string& js, const MergeOptions& options);
    
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    
    std::string inlineCSS(const std::string& html, const std::string& css);
    std::string inlineJS(const std::string& html, const std::string& js);
    
    std::string generateSourceMap(const std::string& code, const std::string& sourceFile);
    std::string combineSourceMaps(const std::vector<std::string>& sourceMaps);
    
    void updateStats(const std::string& key, int value);
    void incrementStats(const std::string& key);
    
    // 错误处理
    void addError(MergeResult& result, const std::string& error);
    void addWarning(MergeResult& result, const std::string& warning);
    
    // 工具函数
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJS(const std::string& text);
    
    std::string removeComments(const std::string& code, bool preserveImportant = true);
    std::string removeWhitespace(const std::string& code);
    
    // 依赖分析
    std::vector<std::string> analyzeDependencies(const std::vector<CodeBlock>& blocks);
    std::vector<CodeBlock> sortByDependencies(const std::vector<CodeBlock>& blocks);
    
    // 冲突检测
    bool detectConflicts(const std::vector<CodeBlock>& blocks);
    void resolveConflicts(std::vector<CodeBlock>& blocks);
};

} // namespace CHTL