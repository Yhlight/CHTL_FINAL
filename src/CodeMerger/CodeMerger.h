#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

struct MergeResult {
    std::string html;
    std::string css;
    std::string js;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger() = default;
    
    // 合并代码
    MergeResult merge(const std::string& html, const std::string& css, const std::string& js);
    MergeResult mergeInline(const std::string& html, const std::string& css, const std::string& js);
    
    // 配置
    void setInlineCSS(bool inlineCSS) { inlineCSS_ = inlineCSS; }
    void setInlineJS(bool inlineJS) { inlineJS_ = inlineJS; }
    void setMinify(bool minify) { minify_ = minify; }
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    
private:
    bool inlineCSS_;
    bool inlineJS_;
    bool minify_;
    std::function<void(const std::string&)> errorHandler_;
    
    std::string mergeCSS(const std::string& html, const std::string& css);
    std::string mergeJS(const std::string& html, const std::string& js);
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJS(const std::string& js);
    
    void reportError(const std::string& message);
};

} // namespace CHTL

#endif // CODE_MERGER_H