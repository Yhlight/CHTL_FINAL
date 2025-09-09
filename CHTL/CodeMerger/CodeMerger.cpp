#include "CodeMerger.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CodeMerger::CodeMerger() 
    : debugMode(false), generateDefaultStructure(true), outputFormat("html5") {
    initializeHTMLTemplate();
}

void CodeMerger::initializeHTMLTemplate() {
    htmlTemplate = R"(<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CHTL Document</title>
    <style>
        /* CSS样式将在这里插入 */
    </style>
</head>
<body>
    <!-- HTML内容将在这里插入 -->
    <script>
        // JavaScript代码将在这里插入
    </script>
</body>
</html>)";
}

FinalResult CodeMerger::merge(const CompileResult& result) {
    FinalResult finalResult;
    
    try {
        // 生成HTML
        finalResult.html = generateHTML(result);
        
        // 生成CSS
        finalResult.css = generateCSS(result);
        
        // 生成JavaScript
        finalResult.javascript = generateJavaScript(result);
        
        // 合并错误和警告
        finalResult.errors = result.errors;
        finalResult.warnings = result.warnings;
        
        // 添加合并器的错误和警告
        finalResult.errors.insert(finalResult.errors.end(), errors.begin(), errors.end());
        finalResult.warnings.insert(finalResult.warnings.end(), warnings.begin(), warnings.end());
        
        // 验证结果
        if (!validate(finalResult)) {
            addError("合并后的代码验证失败");
        }
        
    } catch (const std::exception& e) {
        addError("合并代码时发生错误: " + std::string(e.what()));
        finalResult.errors = errors;
    }
    
    return finalResult;
}

std::string CodeMerger::generateHTML(const CompileResult& result) {
    std::string html;
    
    // 获取CHTL输出
    auto chtlIt = result.outputs.find("CHTL");
    std::string chtlOutput = (chtlIt != result.outputs.end()) ? chtlIt->second : "";
    
    // 获取CSS输出
    auto cssIt = result.outputs.find("CSS");
    std::string cssOutput = (cssIt != result.outputs.end()) ? cssIt->second : "";
    
    // 获取JavaScript输出
    auto jsIt = result.outputs.find("JS");
    std::string jsOutput = (jsIt != result.outputs.end()) ? jsIt->second : "";
    
    // 获取CHTL JS输出
    auto chtlJSIt = result.outputs.find("CHTL_JS");
    std::string chtlJSOutput = (chtlJSIt != result.outputs.end()) ? chtlJSIt->second : "";
    
    // 合并JavaScript
    std::string combinedJS = jsOutput;
    if (!chtlJSOutput.empty()) {
        if (!combinedJS.empty()) {
            combinedJS += "\n" + chtlJSOutput;
        } else {
            combinedJS = chtlJSOutput;
        }
    }
    
    // 处理CSS
    std::string processedCSS = processCSS(cssOutput);
    
    // 处理JavaScript
    std::string processedJS = processJavaScript(combinedJS);
    
    // 生成HTML文档
    if (outputFormat == "html5") {
        html = generateHTML5Document(chtlOutput, processedCSS, processedJS);
    } else if (outputFormat == "html4") {
        html = generateHTML4Document(chtlOutput, processedCSS, processedJS);
    } else if (outputFormat == "xhtml") {
        html = generateXHTMLDocument(chtlOutput, processedCSS, processedJS);
    } else {
        // 默认HTML5
        html = generateHTML5Document(chtlOutput, processedCSS, processedJS);
    }
    
    // 优化HTML
    if (!debugMode) {
        html = optimizeHTML(html);
    }
    
    return html;
}

std::string CodeMerger::generateCSS(const CompileResult& result) {
    auto cssIt = result.outputs.find("CSS");
    std::string css = (cssIt != result.outputs.end()) ? cssIt->second : "";
    
    return processCSS(css);
}

std::string CodeMerger::generateJavaScript(const CompileResult& result) {
    auto jsIt = result.outputs.find("JS");
    std::string js = (jsIt != result.outputs.end()) ? jsIt->second : "";
    
    auto chtlJSIt = result.outputs.find("CHTL_JS");
    std::string chtlJS = (chtlJSIt != result.outputs.end()) ? chtlJSIt->second : "";
    
    std::string combined = js;
    if (!chtlJS.empty()) {
        if (!combined.empty()) {
            combined += "\n" + chtlJS;
        } else {
            combined = chtlJS;
        }
    }
    
    return processJavaScript(combined);
}

std::string CodeMerger::generateHTML5Document(const std::string& body, const std::string& css, const std::string& js) {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>\n";
    oss << "<html lang=\"zh-CN\">\n";
    oss << "<head>\n";
    oss << "    <meta charset=\"UTF-8\">\n";
    oss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    oss << "    <title>CHTL Document</title>\n";
    
    if (!css.empty()) {
        oss << "    <style>\n";
        oss << css << "\n";
        oss << "    </style>\n";
    }
    
    oss << "</head>\n";
    oss << "<body>\n";
    
    if (!body.empty()) {
        oss << body << "\n";
    }
    
    if (!js.empty()) {
        oss << "    <script>\n";
        oss << js << "\n";
        oss << "    </script>\n";
    }
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string CodeMerger::generateHTML4Document(const std::string& body, const std::string& css, const std::string& js) {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
    oss << "<html lang=\"zh-CN\">\n";
    oss << "<head>\n";
    oss << "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n";
    oss << "    <title>CHTL Document</title>\n";
    
    if (!css.empty()) {
        oss << "    <style type=\"text/css\">\n";
        oss << css << "\n";
        oss << "    </style>\n";
    }
    
    oss << "</head>\n";
    oss << "<body>\n";
    
    if (!body.empty()) {
        oss << body << "\n";
    }
    
    if (!js.empty()) {
        oss << "    <script type=\"text/javascript\">\n";
        oss << js << "\n";
        oss << "    </script>\n";
    }
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string CodeMerger::generateXHTMLDocument(const std::string& body, const std::string& css, const std::string& js) {
    std::ostringstream oss;
    
    oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    oss << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
    oss << "<html xmlns=\"http://www.w3.org/1999/xhtml\" lang=\"zh-CN\">\n";
    oss << "<head>\n";
    oss << "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
    oss << "    <title>CHTL Document</title>\n";
    
    if (!css.empty()) {
        oss << "    <style type=\"text/css\">\n";
        oss << css << "\n";
        oss << "    </style>\n";
    }
    
    oss << "</head>\n";
    oss << "<body>\n";
    
    if (!body.empty()) {
        oss << body << "\n";
    }
    
    if (!js.empty()) {
        oss << "    <script type=\"text/javascript\">\n";
        oss << js << "\n";
        oss << "    </script>\n";
    }
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string CodeMerger::processCSS(const std::string& css) {
    if (css.empty()) {
        return css;
    }
    
    std::string processed = css;
    
    // 格式化CSS
    if (debugMode) {
        processed = formatCSS(processed);
    } else {
        processed = minifyCSS(processed);
    }
    
    return processed;
}

std::string CodeMerger::processJavaScript(const std::string& js) {
    if (js.empty()) {
        return js;
    }
    
    std::string processed = js;
    
    // 格式化JavaScript
    if (debugMode) {
        processed = formatJavaScript(processed);
    } else {
        processed = minifyJavaScript(processed);
    }
    
    return processed;
}

std::string CodeMerger::minifyCSS(const std::string& css) {
    std::string minified = css;
    
    // 移除注释
    std::regex commentRegex(R"(\/\*[\s\S]*?\*\/)");
    minified = std::regex_replace(minified, commentRegex, "");
    
    // 移除多余的空白
    std::regex whitespaceRegex(R"(\s+)");
    minified = std::regex_replace(minified, whitespaceRegex, " ");
    
    // 移除分号前的空白
    std::regex semicolonRegex(R"(\s*;\s*)");
    minified = std::regex_replace(minified, semicolonRegex, ";");
    
    // 移除花括号前的空白
    std::regex braceRegex(R"(\s*\{\s*)");
    minified = std::regex_replace(minified, braceRegex, "{");
    
    // 移除花括号后的空白
    std::regex braceCloseRegex(R"(\s*\}\s*)");
    minified = std::regex_replace(minified, braceCloseRegex, "}");
    
    // 移除冒号后的空白
    std::regex colonRegex(R"(\s*:\s*)");
    minified = std::regex_replace(minified, colonRegex, ":");
    
    // 移除逗号后的空白
    std::regex commaRegex(R"(\s*,\s*)");
    minified = std::regex_replace(minified, commaRegex, ",");
    
    // 去除首尾空白
    minified.erase(0, minified.find_first_not_of(" \t\n\r"));
    minified.erase(minified.find_last_not_of(" \t\n\r") + 1);
    
    return minified;
}

std::string CodeMerger::formatCSS(const std::string& css) {
    std::string formatted = css;
    
    // 简单的CSS格式化
    std::regex ruleRegex(R"(([^{}]+)\{([^{}]*)\})");
    formatted = std::regex_replace(formatted, ruleRegex, "$1 {\n    $2\n}");
    
    // 格式化属性
    std::regex propertyRegex(R"(([^:]+):([^;]+);)");
    formatted = std::regex_replace(formatted, propertyRegex, "    $1: $2;\n");
    
    return formatted;
}

std::string CodeMerger::minifyJavaScript(const std::string& js) {
    std::string minified = js;
    
    // 移除单行注释
    std::regex singleCommentRegex(R"(\/\/.*$)");
    minified = std::regex_replace(minified, singleCommentRegex, "");
    
    // 移除多行注释
    std::regex multiCommentRegex(R"(\/\*[\s\S]*?\*\/)");
    minified = std::regex_replace(minified, multiCommentRegex, "");
    
    // 移除多余的空白
    std::regex whitespaceRegex(R"(\s+)");
    minified = std::regex_replace(minified, whitespaceRegex, " ");
    
    // 移除分号前的空白
    std::regex semicolonRegex(R"(\s*;\s*)");
    minified = std::regex_replace(minified, semicolonRegex, ";");
    
    // 去除首尾空白
    minified.erase(0, minified.find_first_not_of(" \t\n\r"));
    minified.erase(minified.find_last_not_of(" \t\n\r") + 1);
    
    return minified;
}

std::string CodeMerger::formatJavaScript(const std::string& js) {
    // 简单的JavaScript格式化
    std::string formatted = js;
    
    // 在花括号后添加换行
    std::regex braceRegex(R"(\{([^}]*)\})");
    formatted = std::regex_replace(formatted, braceRegex, "{\n    $1\n}");
    
    // 在分号后添加换行
    std::regex semicolonRegex(R"(;([^;]*))");
    formatted = std::regex_replace(formatted, semicolonRegex, ";\n$1");
    
    return formatted;
}

std::string CodeMerger::optimizeHTML(const std::string& html) {
    std::string optimized = html;
    
    // 移除空元素
    optimized = removeEmptyElements(optimized);
    
    // 压缩空白
    optimized = compressWhitespace(optimized);
    
    return optimized;
}

std::string CodeMerger::removeEmptyElements(const std::string& html) {
    std::string cleaned = html;
    
    // 移除空的div、span等元素
    std::regex emptyDivRegex(R"(<div[^>]*>\s*</div>)");
    cleaned = std::regex_replace(cleaned, emptyDivRegex, "");
    
    std::regex emptySpanRegex(R"(<span[^>]*>\s*</span>)");
    cleaned = std::regex_replace(cleaned, emptySpanRegex, "");
    
    return cleaned;
}

std::string CodeMerger::compressWhitespace(const std::string& html) {
    std::string compressed = html;
    
    // 压缩多个连续空白字符
    std::regex whitespaceRegex(R"(\s+)");
    compressed = std::regex_replace(compressed, whitespaceRegex, " ");
    
    // 移除标签间的空白
    std::regex tagWhitespaceRegex(R"(>\s+<)");
    compressed = std::regex_replace(compressed, tagWhitespaceRegex, "><");
    
    return compressed;
}

void CodeMerger::addError(const std::string& error) {
    errors.push_back(error);
}

void CodeMerger::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CodeMerger::clearMessages() {
    errors.clear();
    warnings.clear();
}

bool CodeMerger::validate(const FinalResult& result) const {
    // 简单的验证
    if (result.html.empty()) {
        return false;
    }
    
    // 检查HTML基本结构
    if (result.html.find("<html") == std::string::npos) {
        return false;
    }
    
    if (result.html.find("</html>") == std::string::npos) {
        return false;
    }
    
    return true;
}

bool CodeMerger::validateHTML(const std::string& html) {
    // 简单的HTML验证
    return !html.empty() && 
           html.find("<html") != std::string::npos && 
           html.find("</html>") != std::string::npos;
}

bool CodeMerger::validateCSS(const std::string& css) {
    // 简单的CSS验证
    return true; // CSS验证比较复杂，这里简化处理
}

bool CodeMerger::validateJavaScript(const std::string& js) {
    // 简单的JavaScript验证
    return true; // JavaScript验证比较复杂，这里简化处理
}

std::string CodeMerger::getDebugInfo() const {
    std::ostringstream oss;
    oss << "代码合并器调试信息:\n";
    oss << "调试模式: " << (debugMode ? "开启" : "关闭") << "\n";
    oss << "生成默认结构: " << (generateDefaultStructure ? "是" : "否") << "\n";
    oss << "输出格式: " << outputFormat << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL