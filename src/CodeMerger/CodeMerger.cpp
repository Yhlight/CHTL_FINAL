#include "CodeMerger.h"
#include <regex>
#include <sstream>

namespace CHTL {

CodeMerger::CodeMerger() : inlineCSS_(false), inlineJS_(false), minify_(false) {
}

MergeResult CodeMerger::merge(const std::string& html, const std::string& css, const std::string& js) {
    MergeResult result;
    
    try {
        result.html = html;
        result.css = css;
        result.js = js;
        
        if (inlineCSS_) {
            result.html = mergeCSS(result.html, result.css);
            result.css = "";
        }
        
        if (inlineJS_) {
            result.html = mergeJS(result.html, result.js);
            result.js = "";
        }
        
        if (minify_) {
            result.html = minifyHTML(result.html);
            result.css = minifyCSS(result.css);
            result.js = minifyJS(result.js);
        }
        
    } catch (const std::exception& e) {
        result.errors.push_back("Error merging code: " + std::string(e.what()));
    }
    
    return result;
}

MergeResult CodeMerger::mergeInline(const std::string& html, const std::string& css, const std::string& js) {
    bool originalInlineCSS = inlineCSS_;
    bool originalInlineJS = inlineJS_;
    
    inlineCSS_ = true;
    inlineJS_ = true;
    
    MergeResult result = merge(html, css, js);
    
    inlineCSS_ = originalInlineCSS;
    inlineJS_ = originalInlineJS;
    
    return result;
}

std::string CodeMerger::mergeCSS(const std::string& html, const std::string& css) {
    std::string result = html;
    
    // 在</head>标签前插入CSS
    size_t headEnd = result.find("</head>");
    if (headEnd != std::string::npos) {
        std::string cssTag = "<style>\n" + css + "\n</style>\n";
        result.insert(headEnd, cssTag);
    } else {
        // 如果没有</head>标签，在开头插入
        result = "<style>\n" + css + "\n</style>\n" + result;
    }
    
    return result;
}

std::string CodeMerger::mergeJS(const std::string& html, const std::string& js) {
    std::string result = html;
    
    // 在</body>标签前插入JS
    size_t bodyEnd = result.find("</body>");
    if (bodyEnd != std::string::npos) {
        std::string jsTag = "<script>\n" + js + "\n</script>\n";
        result.insert(bodyEnd, jsTag);
    } else {
        // 如果没有</body>标签，在结尾插入
        result += "<script>\n" + js + "\n</script>\n";
    }
    
    return result;
}

std::string CodeMerger::minifyHTML(const std::string& html) {
    std::string result = html;
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex(">\\s+<"), "><");
    
    return result;
}

std::string CodeMerger::minifyCSS(const std::string& css) {
    std::string result = css;
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    
    return result;
}

std::string CodeMerger::minifyJS(const std::string& js) {
    std::string result = js;
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    
    return result;
}

void CodeMerger::setErrorHandler(std::function<void(const std::string&)> handler) {
    errorHandler_ = handler;
}

void CodeMerger::reportError(const std::string& message) {
    if (errorHandler_) {
        errorHandler_(message);
    }
}

} // namespace CHTL