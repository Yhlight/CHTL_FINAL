#include "Compiler/CodeMerger.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <unordered_set>

namespace CHTL {

CodeMerger::CodeMerger() 
    : minify_(false), add_comments_(true), indent_char_("  ") {
    
    // 设置默认HTML模板
    html_template_ = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Generated Page</title>
    <style>
        {{CSS}}
    </style>
</head>
<body>
    {{HTML}}
    <script>
        {{JAVASCRIPT}}
    </script>
</body>
</html>)";
    
    // 设置默认CSS模板
    css_template_ = R"(/* CHTL Generated CSS */
{{CSS}})";
    
    // 设置默认JavaScript模板
    javascript_template_ = R"(// CHTL Generated JavaScript
{{JAVASCRIPT}})";
}

MergeResult CodeMerger::merge(const std::vector<CodeFragment>& fragments) {
    MergeResult result;
    
    try {
        // 验证代码片段
        std::vector<CodeFragment> valid_fragments;
        for (const auto& fragment : fragments) {
            if (validateFragment(fragment)) {
                valid_fragments.push_back(fragment);
            } else {
                result.warnings.push_back("Invalid fragment: " + fragment.source + ":" + std::to_string(fragment.line_number));
            }
        }
        
        // 去重
        auto deduplicated = deduplicateFragments(valid_fragments);
        
        // 排序
        auto sorted = sortFragments(deduplicated);
        
        // 按类型分组
        std::vector<CodeFragment> html_fragments;
        std::vector<CodeFragment> css_fragments;
        std::vector<CodeFragment> js_fragments;
        
        for (const auto& fragment : sorted) {
            if (fragment.type == "HTML") {
                html_fragments.push_back(fragment);
            } else if (fragment.type == "CSS") {
                css_fragments.push_back(fragment);
            } else if (fragment.type == "JS" || fragment.type == "JavaScript") {
                js_fragments.push_back(fragment);
            }
        }
        
        // 合并各类型代码
        result.html = mergeHTML(html_fragments);
        result.css = mergeCSS(css_fragments);
        result.javascript = mergeJavaScript(js_fragments);
        
    } catch (const std::exception& e) {
        result.errors.push_back("Merge error: " + std::string(e.what()));
    }
    
    return result;
}

std::string CodeMerger::mergeHTML(const std::vector<CodeFragment>& html_fragments) {
    std::stringstream html;
    
    if (add_comments_) {
        html << "<!-- CHTL Generated HTML -->\n";
    }
    
    for (const auto& fragment : html_fragments) {
        std::string content = fragment.content;
        
        if (add_comments_) {
            content = addComments(content, "HTML");
        }
        
        if (!minify_) {
            content = formatCode(content, "HTML");
        }
        
        html << content << "\n";
    }
    
    return html.str();
}

std::string CodeMerger::mergeCSS(const std::vector<CodeFragment>& css_fragments) {
    std::stringstream css;
    
    if (add_comments_) {
        css << "/* CHTL Generated CSS */\n";
    }
    
    for (const auto& fragment : css_fragments) {
        std::string content = fragment.content;
        
        if (add_comments_) {
            content = addComments(content, "CSS");
        }
        
        if (!minify_) {
            content = formatCode(content, "CSS");
        }
        
        css << content << "\n";
    }
    
    return css.str();
}

std::string CodeMerger::mergeJavaScript(const std::vector<CodeFragment>& js_fragments) {
    std::stringstream js;
    
    if (add_comments_) {
        js << "// CHTL Generated JavaScript\n";
    }
    
    for (const auto& fragment : js_fragments) {
        std::string content = fragment.content;
        
        if (add_comments_) {
            content = addComments(content, "JavaScript");
        }
        
        if (!minify_) {
            content = formatCode(content, "JavaScript");
        }
        
        js << content << "\n";
    }
    
    return js.str();
}

std::string CodeMerger::generateCompleteHTML(const std::string& html_body, 
                                           const std::string& css, 
                                           const std::string& javascript) {
    std::string result = html_template_;
    
    // 替换占位符
    result = std::regex_replace(result, std::regex("\\{\\{HTML\\}\\}"), html_body);
    result = std::regex_replace(result, std::regex("\\{\\{CSS\\}\\}"), css);
    result = std::regex_replace(result, std::regex("\\{\\{JAVASCRIPT\\}\\}"), javascript);
    
    return result;
}

void CodeMerger::setHTMLTemplate(const std::string& template_html) {
    html_template_ = template_html;
}

void CodeMerger::setCSSTemplate(const std::string& template_css) {
    css_template_ = template_css;
}

void CodeMerger::setJavaScriptTemplate(const std::string& template_js) {
    javascript_template_ = template_js;
}

void CodeMerger::setMinify(bool minify) {
    minify_ = minify;
}

void CodeMerger::setAddComments(bool add_comments) {
    add_comments_ = add_comments;
}

void CodeMerger::setIndent(const std::string& indent) {
    indent_char_ = indent;
}

std::vector<CodeFragment> CodeMerger::sortFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CodeFragment> sorted = fragments;
    
    std::sort(sorted.begin(), sorted.end(), [](const CodeFragment& a, const CodeFragment& b) {
        // 首先按优先级排序
        if (a.priority != b.priority) {
            return a.priority < b.priority;
        }
        
        // 然后按来源文件排序
        if (a.source != b.source) {
            return a.source < b.source;
        }
        
        // 最后按行号排序
        return a.line_number < b.line_number;
    });
    
    return sorted;
}

std::vector<CodeFragment> CodeMerger::deduplicateFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CodeFragment> deduplicated;
    std::unordered_set<std::string> seen;
    
    for (const auto& fragment : fragments) {
        std::string key = fragment.type + ":" + fragment.content;
        if (seen.find(key) == seen.end()) {
            seen.insert(key);
            deduplicated.push_back(fragment);
        }
    }
    
    return deduplicated;
}

bool CodeMerger::validateFragment(const CodeFragment& fragment) {
    // 简单的验证逻辑
    if (fragment.type.empty() || fragment.content.empty()) {
        return false;
    }
    
    // 检查类型是否有效
    if (fragment.type != "HTML" && fragment.type != "CSS" && 
        fragment.type != "JS" && fragment.type != "JavaScript") {
        return false;
    }
    
    return true;
}

std::string CodeMerger::minifyCode(const std::string& code) {
    std::string result = code;
    
    // 简单的压缩逻辑
    // 移除多余的空格和换行
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*:\\s*"), ":");
    result = std::regex_replace(result, std::regex("\\s*,\\s*"), ",");
    
    return result;
}

std::string CodeMerger::addComments(const std::string& code, const std::string& type) {
    std::stringstream result;
    
    if (type == "HTML") {
        result << "<!-- " << type << " fragment -->\n";
    } else if (type == "CSS") {
        result << "/* " << type << " fragment */\n";
    } else if (type == "JavaScript") {
        result << "// " << type << " fragment\n";
    }
    
    result << code;
    
    return result.str();
}

std::string CodeMerger::formatCode(const std::string& code, const std::string& type) {
    // 简单的格式化逻辑
    std::string result = code;
    
    if (type == "HTML") {
        // HTML格式化
        result = std::regex_replace(result, std::regex("><"), ">\n<");
        result = std::regex_replace(result, std::regex(">"), ">\n");
    } else if (type == "CSS") {
        // CSS格式化
        result = std::regex_replace(result, std::regex("\\{"), "{\n");
        result = std::regex_replace(result, std::regex("\\}"), "\n}\n");
        result = std::regex_replace(result, std::regex(";"), ";\n");
    } else if (type == "JavaScript") {
        // JavaScript格式化
        result = std::regex_replace(result, std::regex("\\{"), "{\n");
        result = std::regex_replace(result, std::regex("\\}"), "\n}\n");
        result = std::regex_replace(result, std::regex(";"), ";\n");
    }
    
    return result;
}

std::string CodeMerger::getIndent(int level) const {
    std::string result;
    for (int i = 0; i < level; i++) {
        result += indent_char_;
    }
    return result;
}

} // namespace CHTL