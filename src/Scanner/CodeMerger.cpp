#include "Scanner/CodeMerger.h"
#include <algorithm>

namespace CHTL {

CodeMerger::CodeMerger() 
    : m_inlineCSS(true), m_inlineJavaScript(true), m_externalCSS(false), 
      m_externalJavaScript(false), m_prettyPrint(true), m_minify(false),
      m_mergedSize(0), m_mergedLines(0), m_mergeTime(0.0) {
}

CodeMerger::~CodeMerger() = default;

std::string CodeMerger::merge(const std::string& html, const std::string& css, const std::string& javascript) {
    std::string result = html;
    
    if (!css.empty()) {
        result = mergeHTMLAndCSS(result, css);
    }
    
    if (!javascript.empty()) {
        result = mergeHTMLAndJavaScript(result, javascript);
    }
    
    m_mergedSize = result.length();
    return result;
}

std::string CodeMerger::mergeHTMLAndCSS(const std::string& html, const std::string& css) {
    if (css.empty()) {
        return html;
    }
    
    if (m_inlineCSS) {
        return mergeHTMLWithInlineCSS(html, css);
    } else {
        return mergeHTMLWithExternalCSS(html, css);
    }
}

std::string CodeMerger::mergeHTMLAndJavaScript(const std::string& html, const std::string& javascript) {
    if (javascript.empty()) {
        return html;
    }
    
    if (m_inlineJavaScript) {
        return mergeHTMLWithInlineJavaScript(html, javascript);
    } else {
        return mergeHTMLWithExternalJavaScript(html, javascript);
    }
}

std::string CodeMerger::mergeCSSAndJavaScript(const std::string& css, const std::string& javascript) {
    // TODO: 实现CSS和JavaScript合并
    return css + "\n" + javascript;
}

void CodeMerger::setInlineCSS(bool enabled) {
    m_inlineCSS = enabled;
}

void CodeMerger::setInlineJavaScript(bool enabled) {
    m_inlineJavaScript = enabled;
}

void CodeMerger::setExternalCSS(bool enabled) {
    m_externalCSS = enabled;
}

void CodeMerger::setExternalJavaScript(bool enabled) {
    m_externalJavaScript = enabled;
}

void CodeMerger::setPrettyPrint(bool enabled) {
    m_prettyPrint = enabled;
}

void CodeMerger::setMinify(bool enabled) {
    m_minify = enabled;
}

size_t CodeMerger::getMergedSize() const {
    return m_mergedSize;
}

size_t CodeMerger::getMergedLines() const {
    return m_mergedLines;
}

double CodeMerger::getMergeTime() const {
    return m_mergeTime;
}

std::string CodeMerger::mergeHTMLWithInlineCSS(const std::string& html, const std::string& css) {
    return insertCSSIntoHTML(html, css);
}

std::string CodeMerger::mergeHTMLWithInlineJavaScript(const std::string& html, const std::string& javascript) {
    return insertJavaScriptIntoHTML(html, javascript);
}

std::string CodeMerger::mergeHTMLWithExternalCSS(const std::string& html, const std::string& css) {
    // TODO: 实现外部CSS合并
    return html;
}

std::string CodeMerger::mergeHTMLWithExternalJavaScript(const std::string& html, const std::string& javascript) {
    // TODO: 实现外部JavaScript合并
    return html;
}

std::string CodeMerger::insertCSSIntoHTML(const std::string& html, const std::string& css) {
    size_t headEnd = html.find("</head>");
    if (headEnd != std::string::npos) {
        return html.substr(0, headEnd) + 
               "\n<style>\n" + css + "\n</style>\n" + 
               html.substr(headEnd);
    }
    
    // 如果没有找到</head>标签，在<html>标签后插入
    size_t htmlEnd = html.find(">");
    if (htmlEnd != std::string::npos) {
        return html.substr(0, htmlEnd + 1) + 
               "\n<head>\n<style>\n" + css + "\n</style>\n</head>\n" + 
               html.substr(htmlEnd + 1);
    }
    
    return html;
}

std::string CodeMerger::insertJavaScriptIntoHTML(const std::string& html, const std::string& javascript) {
    size_t bodyEnd = html.find("</body>");
    if (bodyEnd != std::string::npos) {
        return html.substr(0, bodyEnd) + 
               "\n<script>\n" + javascript + "\n</script>\n" + 
               html.substr(bodyEnd);
    }
    
    // 如果没有找到</body>标签，在</html>标签前插入
    size_t htmlEnd = html.find("</html>");
    if (htmlEnd != std::string::npos) {
        return html.substr(0, htmlEnd) + 
               "\n<script>\n" + javascript + "\n</script>\n" + 
               html.substr(htmlEnd);
    }
    
    return html;
}

std::string CodeMerger::createCSSLink(const std::string& cssPath) {
    return "<link rel=\"stylesheet\" href=\"" + cssPath + "\">";
}

std::string CodeMerger::createJavaScriptLink(const std::string& jsPath) {
    return "<script src=\"" + jsPath + "\"></script>";
}

std::string CodeMerger::formatHTML(const std::string& html) {
    // TODO: 实现HTML格式化
    return html;
}

std::string CodeMerger::formatCSS(const std::string& css) {
    // TODO: 实现CSS格式化
    return css;
}

std::string CodeMerger::formatJavaScript(const std::string& javascript) {
    // TODO: 实现JavaScript格式化
    return javascript;
}

std::string CodeMerger::minifyHTML(const std::string& html) {
    // TODO: 实现HTML压缩
    return html;
}

std::string CodeMerger::minifyCSS(const std::string& css) {
    // TODO: 实现CSS压缩
    return css;
}

std::string CodeMerger::minifyJavaScript(const std::string& javascript) {
    // TODO: 实现JavaScript压缩
    return javascript;
}

std::string CodeMerger::escapeHTML(const std::string& text) {
    std::string result = text;
    
    // 替换HTML特殊字符
    // 使用循环替换多字符字符串
    size_t pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        result.replace(pos, 1, "&amp;");
        pos += 5;
    }
    pos = 0;
    while ((pos = result.find('<', pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find('>', pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4;
    }
    pos = 0;
    while ((pos = result.find('"', pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6;
    }
    pos = 0;
    while ((pos = result.find('\'', pos)) != std::string::npos) {
        result.replace(pos, 1, "&#39;");
        pos += 5;
    }
    
    return result;
}

std::string CodeMerger::escapeCSS(const std::string& text) {
    // TODO: 实现CSS转义
    return text;
}

std::string CodeMerger::escapeJavaScript(const std::string& text) {
    // TODO: 实现JavaScript转义
    return text;
}

std::string CodeMerger::indent(int level) {
    if (!m_prettyPrint) {
        return "";
    }
    
    return std::string(level * 2, ' ');
}

void CodeMerger::addError(const std::string& error) {
    m_errors.push_back(error);
}

const std::vector<std::string>& CodeMerger::getErrors() const {
    return m_errors;
}

bool CodeMerger::hasErrors() const {
    return !m_errors.empty();
}

} // namespace CHTL