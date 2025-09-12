#include "CodeMerger.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

CodeMerger::CodeMerger() {}

CodeMerger::~CodeMerger() {}

std::string CodeMerger::mergeHTML(const std::string& htmlCode) {
    std::stringstream mergedHTML;
    
    // 添加HTML头部
    mergedHTML << "<!DOCTYPE html>\n";
    mergedHTML << "<html>\n";
    mergedHTML << "<head>\n";
    
    // 添加外部CSS文件
    for (const auto& cssFile : externalCSSFiles) {
        if (fileExists(cssFile)) {
            std::string cssContent = readFile(cssFile);
            if (getOption("minify") == "true") {
                cssContent = minifyCSS(cssContent);
            }
            mergedHTML << "<style>\n" << cssContent << "\n</style>\n";
        }
    }
    
    mergedHTML << "</head>\n";
    mergedHTML << "<body>\n";
    
    // 添加HTML内容
    mergedHTML << htmlCode << "\n";
    
    // 添加外部JavaScript文件
    for (const auto& jsFile : externalJSFiles) {
        if (fileExists(jsFile)) {
            std::string jsContent = readFile(jsFile);
            if (getOption("minify") == "true") {
                jsContent = minifyJS(jsContent);
            }
            mergedHTML << "<script>\n" << jsContent << "\n</script>\n";
        }
    }
    
    mergedHTML << "</body>\n";
    mergedHTML << "</html>\n";
    
    return mergedHTML.str();
}

std::string CodeMerger::mergeCSS(const std::string& cssCode) {
    std::stringstream mergedCSS;
    
    // 添加外部CSS文件
    for (const auto& cssFile : externalCSSFiles) {
        if (fileExists(cssFile)) {
            std::string cssContent = readFile(cssFile);
            mergedCSS << cssContent << "\n";
        }
    }
    
    // 添加当前CSS代码
    mergedCSS << cssCode << "\n";
    
    std::string result = mergedCSS.str();
    
    // 如果启用压缩，则压缩CSS
    if (getOption("minify") == "true") {
        result = minifyCSS(result);
    }
    
    return result;
}

std::string CodeMerger::mergeJavaScript(const std::string& jsCode) {
    std::stringstream mergedJS;
    
    // 添加外部JavaScript文件
    for (const auto& jsFile : externalJSFiles) {
        if (fileExists(jsFile)) {
            std::string jsContent = readFile(jsFile);
            mergedJS << jsContent << "\n";
        }
    }
    
    // 添加当前JavaScript代码
    mergedJS << jsCode << "\n";
    
    std::string result = mergedJS.str();
    
    // 如果启用压缩，则压缩JavaScript
    if (getOption("minify") == "true") {
        result = minifyJS(result);
    }
    
    return result;
}

std::string CodeMerger::mergeAll(const std::string& htmlCode, 
                                const std::string& cssCode, 
                                const std::string& jsCode) {
    std::stringstream mergedHTML;
    
    // 添加HTML头部
    mergedHTML << "<!DOCTYPE html>\n";
    mergedHTML << "<html>\n";
    mergedHTML << "<head>\n";
    mergedHTML << "<meta charset=\"UTF-8\">\n";
    mergedHTML << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    
    // 添加CSS
    std::string mergedCSS = mergeCSS(cssCode);
    if (!mergedCSS.empty()) {
        mergedHTML << "<style>\n" << mergedCSS << "\n</style>\n";
    }
    
    mergedHTML << "</head>\n";
    mergedHTML << "<body>\n";
    
    // 添加HTML内容
    mergedHTML << htmlCode << "\n";
    
    // 添加JavaScript
    std::string mergedJS = mergeJavaScript(jsCode);
    if (!mergedJS.empty()) {
        mergedHTML << "<script>\n" << mergedJS << "\n</script>\n";
    }
    
    mergedHTML << "</body>\n";
    mergedHTML << "</html>\n";
    
    return mergedHTML.str();
}

void CodeMerger::addExternalCSS(const std::string& cssFile) {
    externalCSSFiles.push_back(cssFile);
}

void CodeMerger::addExternalJS(const std::string& jsFile) {
    externalJSFiles.push_back(jsFile);
}

void CodeMerger::setOption(const std::string& key, const std::string& value) {
    options[key] = value;
}

std::string CodeMerger::getOption(const std::string& key) const {
    auto it = options.find(key);
    return it != options.end() ? it->second : "";
}

void CodeMerger::clearExternalFiles() {
    externalCSSFiles.clear();
    externalJSFiles.clear();
}

std::string CodeMerger::readFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    return content;
}

bool CodeMerger::fileExists(const std::string& filePath) const {
    std::ifstream file(filePath);
    return file.good();
}

std::string CodeMerger::minifyCSS(const std::string& cssCode) const {
    std::string result = cssCode;
    
    // 移除注释
    std::regex commentRegex(R"(\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空格
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除冒号前后的空格
    result = std::regex_replace(result, std::regex(R"(\s*:\s*)"), ":");
    
    // 移除大括号前后的空格
    result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), "{");
    result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), "}");
    
    // 移除逗号后的空格
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除行首行尾的空白字符
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    
    return result;
}

std::string CodeMerger::minifyJS(const std::string& jsCode) const {
    std::string result = jsCode;
    
    // 移除单行注释
    std::regex singleLineCommentRegex(R"(\/\/.*$)");
    result = std::regex_replace(result, singleLineCommentRegex, "");
    
    // 移除多行注释
    std::regex multiLineCommentRegex(R"(\/\*[^*]*\*+(?:[^/*][^*]*\*+)*\/)");
    result = std::regex_replace(result, multiLineCommentRegex, "");
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空格
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除逗号后的空格
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除行首行尾的空白字符
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    
    return result;
}

std::string CodeMerger::minifyHTML(const std::string& htmlCode) const {
    std::string result = htmlCode;
    
    // 移除HTML注释
    std::regex commentRegex(R"(\<!--.*?--\>)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 移除多余的空白字符
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除标签间的空白字符
    result = std::regex_replace(result, std::regex(R"(>\s+<)"), "><");
    
    // 移除行首行尾的空白字符
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    
    return result;
}

} // namespace CHTL