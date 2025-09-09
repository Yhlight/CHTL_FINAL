#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 代码合并器
 * 负责将不同类型的编译结果合并为最终输出
 */
class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger();
    
    // 合并编译结果
    std::string merge(const std::string& html, const std::string& css, const std::string& javascript);
    
    // 合并HTML和CSS
    std::string mergeHTMLAndCSS(const std::string& html, const std::string& css);
    
    // 合并HTML和JavaScript
    std::string mergeHTMLAndJavaScript(const std::string& html, const std::string& javascript);
    
    // 合并CSS和JavaScript
    std::string mergeCSSAndJavaScript(const std::string& css, const std::string& javascript);
    
    // 设置合并选项
    void setInlineCSS(bool enabled);
    void setInlineJavaScript(bool enabled);
    void setExternalCSS(bool enabled);
    void setExternalJavaScript(bool enabled);
    void setPrettyPrint(bool enabled);
    void setMinify(bool enabled);
    
    // 获取合并统计信息
    size_t getMergedSize() const;
    size_t getMergedLines() const;
    double getMergeTime() const;

private:
    bool m_inlineCSS;
    bool m_inlineJavaScript;
    bool m_externalCSS;
    bool m_externalJavaScript;
    bool m_prettyPrint;
    bool m_minify;
    
    size_t m_mergedSize;
    size_t m_mergedLines;
    double m_mergeTime;
    
    // 合并方法
    std::string mergeHTMLWithInlineCSS(const std::string& html, const std::string& css);
    std::string mergeHTMLWithInlineJavaScript(const std::string& html, const std::string& javascript);
    std::string mergeHTMLWithExternalCSS(const std::string& html, const std::string& css);
    std::string mergeHTMLWithExternalJavaScript(const std::string& html, const std::string& javascript);
    
    // HTML处理
    std::string insertCSSIntoHTML(const std::string& html, const std::string& css);
    std::string insertJavaScriptIntoHTML(const std::string& html, const std::string& javascript);
    std::string createCSSLink(const std::string& cssPath);
    std::string createJavaScriptLink(const std::string& jsPath);
    
    // 格式化
    std::string formatHTML(const std::string& html);
    std::string formatCSS(const std::string& css);
    std::string formatJavaScript(const std::string& javascript);
    
    // 压缩
    std::string minifyHTML(const std::string& html);
    std::string minifyCSS(const std::string& css);
    std::string minifyJavaScript(const std::string& javascript);
    
    // 辅助方法
    std::string escapeHTML(const std::string& text);
    std::string escapeCSS(const std::string& text);
    std::string escapeJavaScript(const std::string& text);
    std::string indent(int level);
    
    // 错误处理
    void addError(const std::string& error);
    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
    
    std::vector<std::string> m_errors;
};

} // namespace CHTL