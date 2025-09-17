#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * 代码合并器
 * 将CHTL和CHTL JS处理后的代码与CSS、JS代码合并
 */
class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger();
    
    // 主要合并方法
    std::string merge(const std::map<std::string, std::string>& codeSegments);
    std::string mergeHTML(const std::string& html, const std::string& css, const std::string& js);
    std::string mergeDocument(const std::vector<std::string>& htmlParts, 
                             const std::vector<std::string>& cssParts,
                             const std::vector<std::string>& jsParts);
    
    // 分段合并
    std::string mergeCSS(const std::vector<std::string>& cssSegments);
    std::string mergeJS(const std::vector<std::string>& jsSegments);
    std::string mergeHTML(const std::vector<std::string>& htmlSegments);
    
    // 优化选项
    void setMinifyOutput(bool minify);
    void setOptimizeCSS(bool optimize);
    void setOptimizeJS(bool optimize);
    void setRemoveComments(bool remove);
    void setCombineImports(bool combine);
    
    // 输出格式
    void setOutputFormat(const std::string& format);
    std::string getOutputFormat() const;
    
    // 错误处理
    void setErrorHandler(std::function<void(const std::string&)> handler);
    void reportError(const std::string& message);
    
    // 统计信息
    size_t getInputSize() const;
    size_t getOutputSize() const;
    double getCompressionRatio() const;

private:
    // 配置选项
    bool minify_output_;
    bool optimize_css_;
    bool optimize_js_;
    bool remove_comments_;
    bool combine_imports_;
    std::string output_format_;
    
    // 错误处理
    std::function<void(const std::string&)> error_handler_;
    
    // 统计信息
    size_t input_size_;
    size_t output_size_;
    
    // 辅助方法
    std::string minifyHTML(const std::string& html) const;
    std::string minifyCSS(const std::string& css) const;
    std::string minifyJS(const std::string& js) const;
    
    std::string optimizeCSS(const std::string& css) const;
    std::string optimizeJS(const std::string& js) const;
    
    std::string removeComments(const std::string& code) const;
    std::string combineImports(const std::string& code) const;
    
    // 验证方法
    bool validateHTML(const std::string& html) const;
    bool validateCSS(const std::string& css) const;
    bool validateJS(const std::string& js) const;
    
    // 工具方法
    std::string escapeHTML(const std::string& str) const;
    std::string escapeCSS(const std::string& str) const;
    std::string escapeJS(const std::string& str) const;
    
    void updateStatistics(size_t inputSize, size_t outputSize);
};

} // namespace CHTL

#endif // CODE_MERGER_H