#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * @brief 代码片段结构
 */
struct CodeFragment {
    std::string type;        // 代码类型 (HTML, CSS, JS)
    std::string content;     // 代码内容
    int priority;           // 优先级 (数字越小优先级越高)
    std::string source;     // 来源文件
    int line_number;        // 行号
    
    CodeFragment(const std::string& t = "", const std::string& c = "", 
                 int p = 0, const std::string& s = "", int l = 0)
        : type(t), content(c), priority(p), source(s), line_number(l) {}
};

/**
 * @brief 合并结果结构
 */
struct MergeResult {
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    MergeResult() {}
};

/**
 * @brief 代码合并器
 * 负责将多个代码片段合并成最终的输出文件
 */
class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger() = default;

    /**
     * @brief 合并代码片段
     * @param fragments 代码片段列表
     * @return 合并结果
     */
    MergeResult merge(const std::vector<CodeFragment>& fragments);

    /**
     * @brief 合并HTML代码
     * @param html_fragments HTML代码片段
     * @return 合并后的HTML
     */
    std::string mergeHTML(const std::vector<CodeFragment>& html_fragments);

    /**
     * @brief 合并CSS代码
     * @param css_fragments CSS代码片段
     * @return 合并后的CSS
     */
    std::string mergeCSS(const std::vector<CodeFragment>& css_fragments);

    /**
     * @brief 合并JavaScript代码
     * @param js_fragments JavaScript代码片段
     * @return 合并后的JavaScript
     */
    std::string mergeJavaScript(const std::vector<CodeFragment>& js_fragments);

    /**
     * @brief 生成完整的HTML文档
     * @param html_body HTML主体
     * @param css CSS样式
     * @param javascript JavaScript代码
     * @return 完整的HTML文档
     */
    std::string generateCompleteHTML(const std::string& html_body, 
                                   const std::string& css, 
                                   const std::string& javascript);

    /**
     * @brief 设置HTML模板
     * @param template_html HTML模板
     */
    void setHTMLTemplate(const std::string& template_html);

    /**
     * @brief 设置CSS模板
     * @param template_css CSS模板
     */
    void setCSSTemplate(const std::string& template_css);

    /**
     * @brief 设置JavaScript模板
     * @param template_js JavaScript模板
     */
    void setJavaScriptTemplate(const std::string& template_js);

    /**
     * @brief 设置是否压缩输出
     * @param minify 是否压缩
     */
    void setMinify(bool minify);

    /**
     * @brief 设置是否添加注释
     * @param add_comments 是否添加注释
     */
    void setAddComments(bool add_comments);

    /**
     * @brief 设置缩进字符
     * @param indent 缩进字符
     */
    void setIndent(const std::string& indent);

private:
    /**
     * @brief 排序代码片段
     * @param fragments 代码片段列表
     * @return 排序后的代码片段列表
     */
    std::vector<CodeFragment> sortFragments(const std::vector<CodeFragment>& fragments);

    /**
     * @brief 去重代码片段
     * @param fragments 代码片段列表
     * @return 去重后的代码片段列表
     */
    std::vector<CodeFragment> deduplicateFragments(const std::vector<CodeFragment>& fragments);

    /**
     * @brief 验证代码片段
     * @param fragment 代码片段
     * @return 是否有效
     */
    bool validateFragment(const CodeFragment& fragment);

    /**
     * @brief 压缩代码
     * @param code 代码
     * @return 压缩后的代码
     */
    std::string minifyCode(const std::string& code);

    /**
     * @brief 添加注释
     * @param code 代码
     * @param type 代码类型
     * @return 添加注释后的代码
     */
    std::string addComments(const std::string& code, const std::string& type);

    /**
     * @brief 格式化代码
     * @param code 代码
     * @param type 代码类型
     * @return 格式化后的代码
     */
    std::string formatCode(const std::string& code, const std::string& type);

    /**
     * @brief 获取缩进字符串
     * @param level 缩进级别
     * @return 缩进字符串
     */
    std::string getIndent(int level) const;

private:
    std::string html_template_;
    std::string css_template_;
    std::string javascript_template_;
    bool minify_;
    bool add_comments_;
    std::string indent_char_;
};

} // namespace CHTL