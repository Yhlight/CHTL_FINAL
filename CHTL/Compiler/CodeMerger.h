#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {
namespace Compiler {

// 代码类型枚举
enum class CodeType {
    HTML,
    CSS,
    JavaScript,
    CHTL,
    CHTL_JS
};

// 代码块结构
struct CodeBlock {
    CodeType type;
    std::string content;
    std::string filename;
    int lineNumber;
    int columnNumber;
    std::map<std::string, std::string> metadata;
    
    CodeBlock() : type(CodeType::HTML), lineNumber(0), columnNumber(0) {}
    CodeBlock(CodeType t, const std::string& c) : type(t), content(c), lineNumber(0), columnNumber(0) {}
};

// 合并配置
struct MergeConfig {
    bool minifyHTML;
    bool minifyCSS;
    bool minifyJavaScript;
    bool removeComments;
    bool removeEmptyLines;
    bool preserveWhitespace;
    std::string indentation;
    std::map<std::string, std::string> customOptions;
    
    MergeConfig() : minifyHTML(false), minifyCSS(false), minifyJavaScript(false), 
                   removeComments(false), removeEmptyLines(false), preserveWhitespace(true), 
                   indentation("  ") {}
};

// 合并结果
struct MergeResult {
    bool success;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata;
    
    MergeResult() : success(false) {}
};

// 代码合并器
class CodeMerger {
private:
    MergeConfig config;
    std::vector<CodeBlock> codeBlocks;
    std::vector<std::string> globalErrors;
    std::vector<std::string> globalWarnings;
    
    // 代码处理
    std::string processHTML(const std::string& html) const;
    std::string processCSS(const std::string& css) const;
    std::string processJavaScript(const std::string& js) const;
    
    // 代码优化
    std::string optimizeHTML(const std::string& html) const;
    std::string optimizeCSS(const std::string& css) const;
    std::string optimizeJavaScript(const std::string& js) const;
    
    // 代码清理
    std::string cleanHTML(const std::string& html) const;
    std::string cleanCSS(const std::string& css) const;
    std::string cleanJavaScript(const std::string& js) const;
    
    // 代码格式化
    std::string formatHTML(const std::string& html) const;
    std::string formatCSS(const std::string& css) const;
    std::string formatJavaScript(const std::string& js) const;
    
    // 依赖解析
    std::vector<std::string> extractDependencies(const CodeBlock& block) const;
    std::vector<CodeBlock> resolveDependencies(const std::vector<CodeBlock>& blocks) const;
    
    // 冲突检测
    bool detectConflicts(const std::vector<CodeBlock>& blocks) const;
    std::vector<std::string> resolveConflicts(const std::vector<CodeBlock>& blocks) const;
    
public:
    CodeMerger();
    ~CodeMerger();
    
    // 配置管理
    void setConfig(const MergeConfig& config);
    MergeConfig getConfig() const;
    
    // 代码块管理
    void addCodeBlock(const CodeBlock& block);
    void addCodeBlock(CodeType type, const std::string& content, const std::string& filename = "");
    void removeCodeBlock(size_t index);
    void clearCodeBlocks();
    
    // 代码合并
    MergeResult merge();
    MergeResult merge(const std::vector<CodeBlock>& blocks);
    MergeResult mergeHTML(const std::vector<CodeBlock>& blocks);
    MergeResult mergeCSS(const std::vector<CodeBlock>& blocks);
    MergeResult mergeJavaScript(const std::vector<CodeBlock>& blocks);
    
    // 智能合并
    MergeResult smartMerge();
    MergeResult smartMerge(const std::vector<CodeBlock>& blocks);
    
    // 增量合并
    MergeResult incrementalMerge(const std::vector<CodeBlock>& newBlocks);
    
    // 并行合并
    MergeResult parallelMerge();
    MergeResult parallelMerge(const std::vector<CodeBlock>& blocks);
    
    // 代码分析
    struct CodeAnalysis {
        int totalLines;
        int totalCharacters;
        std::map<CodeType, int> typeCount;
        std::map<std::string, int> fileCount;
        std::vector<std::string> dependencies;
        std::vector<std::string> conflicts;
    };
    
    CodeAnalysis analyzeCode() const;
    CodeAnalysis analyzeCode(const std::vector<CodeBlock>& blocks) const;
    
    // 代码验证
    bool validateCode(const CodeBlock& block) const;
    bool validateCode(const std::vector<CodeBlock>& blocks) const;
    std::vector<std::string> getValidationErrors(const CodeBlock& block) const;
    std::vector<std::string> getValidationErrors(const std::vector<CodeBlock>& blocks) const;
    
    // 代码转换
    std::string convertToHTML(const CodeBlock& block) const;
    std::string convertToCSS(const CodeBlock& block) const;
    std::string convertToJavaScript(const CodeBlock& block) const;
    
    // 代码生成
    std::string generateHTML(const MergeResult& result) const;
    std::string generateCSS(const MergeResult& result) const;
    std::string generateJavaScript(const MergeResult& result) const;
    std::string generateCombined(const MergeResult& result) const;
    
    // 错误和警告管理
    std::vector<std::string> getGlobalErrors() const;
    std::vector<std::string> getGlobalWarnings() const;
    void clearGlobalErrors();
    void clearGlobalWarnings();
    
    // 调试和诊断
    std::string generateMergeReport() const;
    void enableDebugMode(bool enable);
    bool isDebugMode() const;
    
    // 回调函数
    void setMergeCallback(std::function<void(const MergeResult&)> callback);
    void setProgressCallback(std::function<void(int, int)> callback);
    
private:
    std::function<void(const MergeResult&)> mergeCallback;
    std::function<void(int, int)> progressCallback;
    bool debugMode;
    
    // 内部辅助方法
    void notifyProgress(int current, int total);
    void notifyMergeComplete(const MergeResult& result);
    std::string escapeHTML(const std::string& text) const;
    std::string escapeCSS(const std::string& text) const;
    std::string escapeJavaScript(const std::string& text) const;
    std::string generateSourceMap(const MergeResult& result) const;
};

} // namespace Compiler
} // namespace CHTL

#endif // CODE_MERGER_H