#include "CodeMerger.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace CHTL {

CodeMerger::CodeMerger() {
    // 设置默认模板
    options.htmlTemplate = generateHTMLTemplate();
    options.cssTemplate = generateCSSTemplate();
    options.jsTemplate = generateJSTemplate();
}

MergeResult CodeMerger::merge(const std::vector<CompilationResult>& results) {
    MergeResult result;
    
    try {
        if (!validateResults(results)) {
            result.success = false;
            result.errorMessage = "Invalid compilation results";
            return result;
        }
        
        this->results = results;
        
        switch (options.format) {
            case OutputFormat::HTML:
                result.html = mergeHTML(results);
                break;
            case OutputFormat::CSS:
                result.css = mergeCSS(results);
                break;
            case OutputFormat::JS:
                result.js = mergeJS(results);
                break;
            case OutputFormat::SEPARATE:
                result.html = mergeSeparate(results);
                break;
            case OutputFormat::COMBINED:
                result.html = mergeCombined(results);
                break;
            default:
                result.success = false;
                result.errorMessage = "Unknown output format";
                return result;
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = "Merge failed: " + std::string(e.what());
    }
    
    return result;
}

MergeResult CodeMerger::merge(const CompilerDispatcher& dispatcher) {
    return merge(dispatcher.getResults());
}

std::string CodeMerger::mergeHTML(const std::vector<CompilationResult>& results) const {
    std::ostringstream html;
    
    // 开始HTML文档
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    
    // 添加CSS
    std::string cssContent = mergeCSS(results);
    if (!cssContent.empty()) {
        html << "<style>\n";
        html << cssContent;
        html << "\n</style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    
    // 添加HTML内容
    for (const auto& result : results) {
        if (result.type == CompilerType::CHTL || result.type == CompilerType::HTML) {
            if (result.success) {
                html << processPlaceholders(result.output);
                html << "\n";
            }
        }
    }
    
    // 添加JavaScript
    std::string jsContent = mergeJS(results);
    if (!jsContent.empty()) {
        html << "<script>\n";
        html << jsContent;
        html << "\n</script>\n";
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string CodeMerger::mergeCSS(const std::vector<CompilationResult>& results) const {
    std::ostringstream css;
    
    if (options.includeComments) {
        css << "/* CHTL CSS Output */\n";
    }
    
    for (const auto& result : results) {
        if (result.type == CompilerType::CSS) {
            if (result.success) {
                css << processPlaceholders(result.output);
                css << "\n";
            }
        }
    }
    
    return css.str();
}

std::string CodeMerger::mergeJS(const std::vector<CompilationResult>& results) const {
    std::ostringstream js;
    
    if (options.includeComments) {
        js << "// CHTL JavaScript Output\n";
    }
    
    for (const auto& result : results) {
        if (result.type == CompilerType::CHTLJS || result.type == CompilerType::JS) {
            if (result.success) {
                js << processPlaceholders(result.output);
                js << "\n";
            }
        }
    }
    
    return js.str();
}

std::string CodeMerger::mergeSeparate(const std::vector<CompilationResult>& results) const {
    std::ostringstream output;
    
    output << "=== HTML ===\n";
    for (const auto& result : results) {
        if (result.type == CompilerType::CHTL || result.type == CompilerType::HTML) {
            if (result.success) {
                output << result.output << "\n";
            }
        }
    }
    
    output << "\n=== CSS ===\n";
    for (const auto& result : results) {
        if (result.type == CompilerType::CSS) {
            if (result.success) {
                output << result.output << "\n";
            }
        }
    }
    
    output << "\n=== JavaScript ===\n";
    for (const auto& result : results) {
        if (result.type == CompilerType::CHTLJS || result.type == CompilerType::JS) {
            if (result.success) {
                output << result.output << "\n";
            }
        }
    }
    
    return output.str();
}

std::string CodeMerger::mergeCombined(const std::vector<CompilationResult>& results) const {
    std::ostringstream output;
    
    for (const auto& result : results) {
        if (result.success) {
            output << "/* " << static_cast<int>(result.type) << " */\n";
            output << result.output << "\n\n";
        }
    }
    
    return output.str();
}

std::string CodeMerger::processPlaceholders(const std::string& content) const {
    std::string result = content;
    
    for (const auto& pair : placeholders) {
        size_t pos = 0;
        while ((pos = result.find(pair.first, pos)) != std::string::npos) {
            result.replace(pos, pair.first.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

std::string CodeMerger::generateHTMLTemplate() const {
    return R"(<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Output</title>
    <!-- CSS will be inserted here -->
</head>
<body>
    <!-- HTML content will be inserted here -->
    <!-- JavaScript will be inserted here -->
</body>
</html>)";
}

std::string CodeMerger::generateCSSTemplate() const {
    return R"(/* CHTL CSS Output */
/* CSS content will be inserted here */)";
}

std::string CodeMerger::generateJSTemplate() const {
    return R"(// CHTL JavaScript Output
// JavaScript content will be inserted here)";
}

bool CodeMerger::validateResults(const std::vector<CompilationResult>& results) const {
    if (results.empty()) {
        return false;
    }
    
    for (const auto& result : results) {
        if (result.type == CompilerType::UNKNOWN) {
            return false;
        }
    }
    
    return true;
}

std::string CodeMerger::extractContent(const CompilationResult& result) const {
    if (!result.success) {
        return "";
    }
    
    return result.output;
}

std::string CodeMerger::wrapInTag(const std::string& content, const std::string& tag) const {
    return "<" + tag + ">\n" + content + "\n</" + tag + ">";
}

void CodeMerger::setOptions(const MergeOptions& opts) {
    options = opts;
}

MergeOptions CodeMerger::getOptions() const {
    return options;
}

void CodeMerger::setOutputFormat(OutputFormat format) {
    options.format = format;
}

void CodeMerger::setMinifyOutput(bool minify) {
    options.minifyOutput = minify;
}

void CodeMerger::setIncludeComments(bool include) {
    options.includeComments = include;
}

void CodeMerger::setPreserveStructure(bool preserve) {
    options.preserveStructure = preserve;
}

void CodeMerger::setHTMLTemplate(const std::string& template) {
    options.htmlTemplate = template;
}

void CodeMerger::setCSSTemplate(const std::string& template) {
    options.cssTemplate = template;
}

void CodeMerger::setJSTemplate(const std::string& template) {
    options.jsTemplate = template;
}

void CodeMerger::addPlaceholder(const std::string& key, const std::string& value) {
    placeholders[key] = value;
}

void CodeMerger::removePlaceholder(const std::string& key) {
    placeholders.erase(key);
}

void CodeMerger::clearPlaceholders() {
    placeholders.clear();
}

std::unordered_map<std::string, std::string> CodeMerger::getPlaceholders() const {
    return placeholders;
}

std::string CodeMerger::getHTML() const {
    // 从最后合并结果中提取HTML
    return ""; // 需要存储最后的结果
}

std::string CodeMerger::getCSS() const {
    // 从最后合并结果中提取CSS
    return ""; // 需要存储最后的结果
}

std::string CodeMerger::getJS() const {
    // 从最后合并结果中提取JavaScript
    return ""; // 需要存储最后的结果
}

bool CodeMerger::hasErrors() const {
    for (const auto& result : results) {
        if (!result.success) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> CodeMerger::getErrorMessages() const {
    std::vector<std::string> errors;
    for (const auto& result : results) {
        if (!result.success) {
            errors.push_back(result.errorMessage);
        }
    }
    return errors;
}

void CodeMerger::clear() {
    results.clear();
    placeholders.clear();
}

void CodeMerger::reset() {
    clear();
    options = MergeOptions();
    options.htmlTemplate = generateHTMLTemplate();
    options.cssTemplate = generateCSSTemplate();
    options.jsTemplate = generateJSTemplate();
}

void CodeMerger::printMergeResult(const MergeResult& result) const {
    std::cout << "=== 合并结果 ===" << std::endl;
    std::cout << "状态: " << (result.success ? "成功" : "失败") << std::endl;
    if (!result.success) {
        std::cout << "错误: " << result.errorMessage << std::endl;
    }
    std::cout << "HTML长度: " << result.html.length() << " 字符" << std::endl;
    std::cout << "CSS长度: " << result.css.length() << " 字符" << std::endl;
    std::cout << "JS长度: " << result.js.length() << " 字符" << std::endl;
    std::cout << "===============" << std::endl;
}

void CodeMerger::printOptions() const {
    std::cout << "=== 合并选项 ===" << std::endl;
    std::cout << "输出格式: " << static_cast<int>(options.format) << std::endl;
    std::cout << "包含注释: " << (options.includeComments ? "是" : "否") << std::endl;
    std::cout << "压缩输出: " << (options.minifyOutput ? "是" : "否") << std::endl;
    std::cout << "保持结构: " << (options.preserveStructure ? "是" : "否") << std::endl;
    std::cout << "===============" << std::endl;
}

void CodeMerger::printPlaceholders() const {
    std::cout << "=== 占位符 ===" << std::endl;
    for (const auto& pair : placeholders) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }
    std::cout << "=============" << std::endl;
}

} // namespace CHTL