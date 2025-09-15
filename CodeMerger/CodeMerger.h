#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include "../CompilerDispatcher/CompilerDispatcher.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

enum class OutputFormat {
    HTML,           // HTML输出
    CSS,            // CSS输出
    JS,             // JavaScript输出
    SEPARATE,       // 分离输出
    COMBINED        // 合并输出
};

struct MergeResult {
    std::string html;
    std::string css;
    std::string js;
    bool success;
    std::string errorMessage;
    
    MergeResult() : success(false) {}
};

struct MergeOptions {
    OutputFormat format;
    bool includeComments;
    bool minifyOutput;
    bool preserveStructure;
    std::string htmlTemplate;
    std::string cssTemplate;
    std::string jsTemplate;
    
    MergeOptions() : format(OutputFormat::HTML), includeComments(true), 
                    minifyOutput(false), preserveStructure(true) {}
};

class CodeMerger {
private:
    MergeOptions options;
    std::vector<CompilationResult> results;
    std::unordered_map<std::string, std::string> placeholders;
    
    // 内部方法
    std::string mergeHTML(const std::vector<CompilationResult>& results) const;
    std::string mergeCSS(const std::vector<CompilationResult>& results) const;
    std::string mergeJS(const std::vector<CompilationResult>& results) const;
    std::string mergeSeparate(const std::vector<CompilationResult>& results) const;
    std::string mergeCombined(const std::vector<CompilationResult>& results) const;
    
    std::string processPlaceholders(const std::string& content) const;
    std::string generateHTMLTemplate() const;
    std::string generateCSSTemplate() const;
    std::string generateJSTemplate() const;
    
    bool validateResults(const std::vector<CompilationResult>& results) const;
    std::string extractContent(const CompilationResult& result) const;
    std::string wrapInTag(const std::string& content, const std::string& tag) const;
    
public:
    CodeMerger();
    ~CodeMerger() = default;
    
    // 主要合并方法
    MergeResult merge(const std::vector<CompilationResult>& results);
    MergeResult merge(const CompilerDispatcher& dispatcher);
    
    // 配置方法
    void setOptions(const MergeOptions& opts);
    MergeOptions getOptions() const;
    void setOutputFormat(OutputFormat format);
    void setMinifyOutput(bool minify);
    void setIncludeComments(bool include);
    void setPreserveStructure(bool preserve);
    
    // 模板管理
    void setHTMLTemplate(const std::string& template);
    void setCSSTemplate(const std::string& template);
    void setJSTemplate(const std::string& template);
    
    // 占位符管理
    void addPlaceholder(const std::string& key, const std::string& value);
    void removePlaceholder(const std::string& key);
    void clearPlaceholders();
    std::unordered_map<std::string, std::string> getPlaceholders() const;
    
    // 结果处理
    std::string getHTML() const;
    std::string getCSS() const;
    std::string getJS() const;
    bool hasErrors() const;
    std::vector<std::string> getErrorMessages() const;
    
    // 工具方法
    void clear();
    void reset();
    
    // 调试方法
    void printMergeResult(const MergeResult& result) const;
    void printOptions() const;
    void printPlaceholders() const;
};

} // namespace CHTL

#endif // CODE_MERGER_H