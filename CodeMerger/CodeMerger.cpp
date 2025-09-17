#include "CodeMerger.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

CodeMerger::CodeMerger()
    : minify_output_(false), optimize_css_(false), optimize_js_(false),
      remove_comments_(false), combine_imports_(false), output_format_("html"),
      input_size_(0), output_size_(0) {
}

CodeMerger::~CodeMerger() {
}

std::string CodeMerger::merge(const std::map<std::string, std::string>& codeSegments) {
    std::stringstream ss;
    
    // 获取各种代码段
    std::string html = codeSegments.count("html") ? codeSegments.at("html") : "";
    std::string css = codeSegments.count("css") ? codeSegments.at("css") : "";
    std::string js = codeSegments.count("js") ? codeSegments.at("js") : "";
    std::string chtljs = codeSegments.count("chtljs") ? codeSegments.at("chtljs") : "";
    
    // 合并CSS
    if (!css.empty()) {
        if (optimize_css_) {
            css = optimizeCSS(css);
        }
        if (minify_output_) {
            css = minifyCSS(css);
        }
    }
    
    // 合并JS
    if (!js.empty()) {
        if (optimize_js_) {
            js = optimizeJS(js);
        }
        if (minify_output_) {
            js = minifyJS(js);
        }
    }
    
    // 合并CHTL JS
    if (!chtljs.empty()) {
        if (minify_output_) {
            chtljs = minifyJS(chtljs);
        }
    }
    
    // 生成最终HTML文档
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head>\n";
    ss << "    <meta charset=\"UTF-8\">\n";
    ss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    ss << "    <title>CHTL Generated Document</title>\n";
    
    // 添加CSS
    if (!css.empty()) {
        ss << "    <style>\n";
        ss << css << "\n";
        ss << "    </style>\n";
    }
    
    ss << "</head>\n";
    ss << "<body>\n";
    
    // 添加HTML内容
    if (!html.empty()) {
        ss << html << "\n";
    }
    
    ss << "</body>\n";
    
    // 添加JavaScript
    if (!js.empty() || !chtljs.empty()) {
        ss << "<script>\n";
        if (!js.empty()) {
            ss << js << "\n";
        }
        if (!chtljs.empty()) {
            ss << chtljs << "\n";
        }
        ss << "</script>\n";
    }
    
    ss << "</html>";
    
    std::string result = ss.str();
    
    // 最终优化
    if (minify_output_) {
        result = minifyHTML(result);
    }
    
    updateStatistics(html.size() + css.size() + js.size() + chtljs.size(), result.size());
    
    return result;
}

std::string CodeMerger::mergeHTML(const std::string& html, const std::string& css, const std::string& js) {
    std::map<std::string, std::string> segments;
    segments["html"] = html;
    segments["css"] = css;
    segments["js"] = js;
    return merge(segments);
}

std::string CodeMerger::mergeDocument(const std::vector<std::string>& htmlParts, 
                                     const std::vector<std::string>& cssParts,
                                     const std::vector<std::string>& jsParts) {
    // 合并HTML部分
    std::string html = mergeHTML(htmlParts);
    
    // 合并CSS部分
    std::string css = mergeCSS(cssParts);
    
    // 合并JS部分
    std::string js = mergeJS(jsParts);
    
    return mergeHTML(html, css, js);
}

std::string CodeMerger::mergeCSS(const std::vector<std::string>& cssSegments) {
    std::stringstream ss;
    
    for (const auto& segment : cssSegments) {
        if (!segment.empty()) {
            ss << segment << "\n";
        }
    }
    
    std::string result = ss.str();
    
    if (optimize_css_) {
        result = optimizeCSS(result);
    }
    
    if (minify_output_) {
        result = minifyCSS(result);
    }
    
    return result;
}

std::string CodeMerger::mergeJS(const std::vector<std::string>& jsSegments) {
    std::stringstream ss;
    
    for (const auto& segment : jsSegments) {
        if (!segment.empty()) {
            ss << segment << "\n";
        }
    }
    
    std::string result = ss.str();
    
    if (optimize_js_) {
        result = optimizeJS(result);
    }
    
    if (minify_output_) {
        result = minifyJS(result);
    }
    
    return result;
}

std::string CodeMerger::mergeHTML(const std::vector<std::string>& htmlSegments) {
    std::stringstream ss;
    
    for (const auto& segment : htmlSegments) {
        if (!segment.empty()) {
            ss << segment << "\n";
        }
    }
    
    return ss.str();
}

void CodeMerger::setMinifyOutput(bool minify) {
    minify_output_ = minify;
}

void CodeMerger::setOptimizeCSS(bool optimize) {
    optimize_css_ = optimize;
}

void CodeMerger::setOptimizeJS(bool optimize) {
    optimize_js_ = optimize;
}

void CodeMerger::setRemoveComments(bool remove) {
    remove_comments_ = remove;
}

void CodeMerger::setCombineImports(bool combine) {
    combine_imports_ = combine;
}

void CodeMerger::setOutputFormat(const std::string& format) {
    output_format_ = format;
}

std::string CodeMerger::getOutputFormat() const {
    return output_format_;
}

void CodeMerger::setErrorHandler(std::function<void(const std::string&)> handler) {
    error_handler_ = handler;
}

void CodeMerger::reportError(const std::string& message) {
    if (error_handler_) {
        error_handler_(message);
    }
}

size_t CodeMerger::getInputSize() const {
    return input_size_;
}

size_t CodeMerger::getOutputSize() const {
    return output_size_;
}

double CodeMerger::getCompressionRatio() const {
    if (input_size_ == 0) return 0.0;
    return static_cast<double>(output_size_) / static_cast<double>(input_size_);
}

std::string CodeMerger::minifyHTML(const std::string& html) const {
    std::string result = html;
    
    // 移除HTML注释
    if (remove_comments_) {
        result = std::regex_replace(result, std::regex("<!--.*?-->"), "");
    }
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex(">\\s+<"), "><");
    
    // 移除标签间的空白
    result = std::regex_replace(result, std::regex(">\\s+"), ">");
    result = std::regex_replace(result, std::regex("\\s+<"), "<");
    
    return result;
}

std::string CodeMerger::minifyCSS(const std::string& css) const {
    std::string result = css;
    
    // 移除注释
    if (remove_comments_) {
        result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
    }
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*:\\s*"), ":");
    result = std::regex_replace(result, std::regex("\\s*,\\s*"), ",");
    
    return result;
}

std::string CodeMerger::minifyJS(const std::string& js) const {
    std::string result = js;
    
    // 移除注释
    if (remove_comments_) {
        result = std::regex_replace(result, std::regex("//.*"), "");
        result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
    }
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*,\\s*"), ",");
    result = std::regex_replace(result, std::regex("\\s*=\\s*"), "=");
    
    return result;
}

std::string CodeMerger::optimizeCSS(const std::string& css) const {
    std::string result = css;
    
    // 合并相同的选择器
    // 这里可以实现更复杂的CSS优化逻辑
    
    // 移除未使用的CSS规则
    // 这里可以实现CSS规则使用分析
    
    return result;
}

std::string CodeMerger::optimizeJS(const std::string& js) const {
    std::string result = js;
    
    // 简单的JS优化
    // 这里可以实现更复杂的JS优化逻辑
    
    return result;
}

std::string CodeMerger::removeComments(const std::string& code) const {
    std::string result = code;
    
    // 移除单行注释
    result = std::regex_replace(result, std::regex("//.*"), "");
    
    // 移除多行注释
    result = std::regex_replace(result, std::regex("/\\*.*?\\*/"), "");
    
    return result;
}

std::string CodeMerger::combineImports(const std::string& code) const {
    std::string result = code;
    
    // 合并import语句
    // 这里可以实现import语句合并逻辑
    
    return result;
}

bool CodeMerger::validateHTML(const std::string& html) const {
    // 简单的HTML验证
    if (html.empty()) return false;
    
    // 检查基本HTML结构
    bool hasHtml = html.find("<html") != std::string::npos;
    bool hasHead = html.find("<head") != std::string::npos;
    bool hasBody = html.find("<body") != std::string::npos;
    
    return hasHtml && hasHead && hasBody;
}

bool CodeMerger::validateCSS(const std::string& css) const {
    // 简单的CSS验证
    if (css.empty()) return true;
    
    // 检查基本CSS语法
    size_t openBraces = std::count(css.begin(), css.end(), '{');
    size_t closeBraces = std::count(css.begin(), css.end(), '}');
    
    return openBraces == closeBraces;
}

bool CodeMerger::validateJS(const std::string& js) const {
    // 简单的JS验证
    if (js.empty()) return true;
    
    // 检查基本JS语法
    size_t openBraces = std::count(js.begin(), js.end(), '{');
    size_t closeBraces = std::count(js.begin(), js.end(), '}');
    size_t openParens = std::count(js.begin(), js.end(), '(');
    size_t closeParens = std::count(js.begin(), js.end(), ')');
    
    return openBraces == closeBraces && openParens == closeParens;
}

std::string CodeMerger::escapeHTML(const std::string& str) const {
    std::string result = str;
    
    result = std::regex_replace(result, std::regex("&"), "&amp;");
    result = std::regex_replace(result, std::regex("<"), "&lt;");
    result = std::regex_replace(result, std::regex(">"), "&gt;");
    result = std::regex_replace(result, std::regex("\""), "&quot;");
    result = std::regex_replace(result, std::regex("'"), "&#39;");
    
    return result;
}

std::string CodeMerger::escapeCSS(const std::string& str) const {
    // CSS转义
    return str;
}

std::string CodeMerger::escapeJS(const std::string& str) const {
    // JavaScript转义
    return str;
}

void CodeMerger::updateStatistics(size_t inputSize, size_t outputSize) {
    input_size_ = inputSize;
    output_size_ = outputSize;
}

} // namespace CHTL