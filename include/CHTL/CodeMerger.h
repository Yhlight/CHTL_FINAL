#pragma once

#include "CHTL/Common.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * 代码合并器 - 将各编译器输出合并为最终HTML
 */
class CodeMerger {
public:
    // 合并配置
    struct MergeConfig {
        bool enableInlineCSS = false;
        bool enableInlineJS = false;
        bool enableDefaultStruct = false;
        bool enableMinification = false;
        bool enableSourceMaps = false;
        std::string outputFormat = "html5"; // html5, xhtml, html4
        std::string cssOutputPath = "styles.css";
        std::string jsOutputPath = "scripts.js";
        std::string sourceMapPath = "source.map";
    };

    // 合并结果
    struct MergeResult {
        bool success;
        std::string htmlOutput;
        std::string cssOutput;
        std::string jsOutput;
        std::string sourceMapOutput;
        std::string errorMessage;
        std::vector<std::string> warnings;
        
        MergeResult() : success(false) {}
    };

    // 代码片段
    struct CodeFragment {
        std::string content;
        std::string type; // html, css, js
        SourceLocation location;
        int priority = 0; // 优先级，数字越大优先级越高
        bool isInline = false;
        
        CodeFragment(const std::string& c, const std::string& t, const SourceLocation& loc)
            : content(c), type(t), location(loc) {}
    };

public:
    CodeMerger();
    ~CodeMerger();

    /**
     * 合并代码片段
     * @param fragments 代码片段列表
     * @param config 合并配置
     * @return 合并结果
     */
    MergeResult merge(const std::vector<CodeFragment>& fragments, const MergeConfig& config = {});

    /**
     * 合并HTML、CSS、JS代码
     * @param htmlCode HTML代码
     * @param cssCode CSS代码
     * @param jsCode JS代码
     * @param config 合并配置
     * @return 合并结果
     */
    MergeResult merge(const std::string& htmlCode, 
                     const std::string& cssCode, 
                     const std::string& jsCode, 
                     const MergeConfig& config = {});

    /**
     * 设置HTML模板
     * @param template 模板内容
     */
    void setHTMLTemplate(const std::string& template);

    /**
     * 设置CSS模板
     * @param template 模板内容
     */
    void setCSSTemplate(const std::string& template);

    /**
     * 设置JS模板
     * @param template 模板内容
     */
    void setJSTemplate(const std::string& template);

    /**
     * 添加代码片段
     * @param fragment 代码片段
     */
    void addFragment(const CodeFragment& fragment);

    /**
     * 清空代码片段
     */
    void clearFragments();

    /**
     * 生成默认HTML结构
     * @param htmlContent HTML内容
     * @param cssContent CSS内容
     * @param jsContent JS内容
     * @param config 合并配置
     * @return 完整的HTML文档
     */
    std::string generateDefaultHTML(const std::string& htmlContent, 
                                  const std::string& cssContent, 
                                  const std::string& jsContent, 
                                  const MergeConfig& config);

    /**
     * 内联CSS
     * @param htmlContent HTML内容
     * @param cssContent CSS内容
     * @return 内联后的HTML
     */
    std::string inlineCSS(const std::string& htmlContent, const std::string& cssContent);

    /**
     * 内联JS
     * @param htmlContent HTML内容
     * @param jsContent JS内容
     * @return 内联后的HTML
     */
    std::string inlineJS(const std::string& htmlContent, const std::string& jsContent);

    /**
     * 生成源映射
     * @param originalCode 原始代码
     * @param generatedCode 生成代码
     * @param sourceMapPath 源映射路径
     * @return 源映射内容
     */
    std::string generateSourceMap(const std::string& originalCode, 
                                const std::string& generatedCode, 
                                const std::string& sourceMapPath);

    /**
     * 压缩代码
     * @param code 代码
     * @param type 类型 (html, css, js)
     * @return 压缩后的代码
     */
    std::string minifyCode(const std::string& code, const std::string& type);

    /**
     * 验证HTML
     * @param htmlContent HTML内容
     * @return 验证结果
     */
    bool validateHTML(const std::string& htmlContent);

    /**
     * 验证CSS
     * @param cssContent CSS内容
     * @return 验证结果
     */
    bool validateCSS(const std::string& cssContent);

    /**
     * 验证JS
     * @param jsContent JS内容
     * @return 验证结果
     */
    bool validateJS(const std::string& jsContent);

private:
    // 内部合并方法
    MergeResult mergeFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config);
    
    // HTML处理
    std::string processHTML(const std::string& htmlContent, const MergeConfig& config);
    std::string insertCSSIntoHTML(const std::string& htmlContent, const std::string& cssContent);
    std::string insertJSIntoHTML(const std::string& htmlContent, const std::string& jsContent);
    
    // CSS处理
    std::string processCSS(const std::string& cssContent, const MergeConfig& config);
    std::string optimizeCSS(const std::string& cssContent);
    std::string prefixCSS(const std::string& cssContent);
    
    // JS处理
    std::string processJS(const std::string& jsContent, const MergeConfig& config);
    std::string optimizeJS(const std::string& jsContent);
    std::string wrapJS(const std::string& jsContent);
    
    // 模板处理
    std::string applyHTMLTemplate(const std::string& content, const MergeConfig& config);
    std::string applyCSSTemplate(const std::string& content, const MergeConfig& config);
    std::string applyJSTemplate(const std::string& content, const MergeConfig& config);
    
    // 工具方法
    std::string extractTitle(const std::string& htmlContent);
    std::string extractMetaTags(const std::string& htmlContent);
    std::string extractBodyContent(const std::string& htmlContent);
    std::string extractHeadContent(const std::string& htmlContent);
    
    // 错误处理
    void handleError(const std::string& error, MergeResult& result);
    void handleWarning(const std::string& warning, MergeResult& result);

private:
    std::vector<CodeFragment> fragments_;
    std::string htmlTemplate_;
    std::string cssTemplate_;
    std::string jsTemplate_;
    
    // 默认模板
    static const std::string DEFAULT_HTML_TEMPLATE;
    static const std::string DEFAULT_CSS_TEMPLATE;
    static const std::string DEFAULT_JS_TEMPLATE;
    
    // 验证器
    bool htmlValidationEnabled_;
    bool cssValidationEnabled_;
    bool jsValidationEnabled_;
};

} // namespace CHTL