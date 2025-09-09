#include "CHTL/CHTLVSCodeExtension.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <filesystem>

namespace CHTL {

// SyntaxHighlightRule 实现
SyntaxHighlightRule::SyntaxHighlightRule(const std::string& pattern, const std::string& tokenType, 
                                         const std::string& foreground, const std::string& background)
    : m_pattern(pattern), m_tokenType(tokenType), m_foreground(foreground), m_background(background) {
}

// SyntaxHighlighter 实现
SyntaxHighlighter::SyntaxHighlighter(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_currentTheme("default") {
    initializeDefaultRules();
}

std::vector<std::pair<int, std::string>> SyntaxHighlighter::highlight(const std::string& code) {
    std::vector<std::pair<int, std::string>> highlights;
    
    if (code.empty()) {
        return highlights;
    }
    
    std::stringstream ss(code);
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(ss, line)) {
        std::string highlightedLine = line;
        
        for (const auto& rule : m_rules) {
            std::regex pattern(rule.getPattern());
            std::smatch match;
            
            std::string::const_iterator start = line.cbegin();
            std::string::const_iterator end = line.cend();
            
            while (std::regex_search(start, end, match, pattern)) {
                std::string matched = match.str();
                std::string replacement = "<span class=\"" + rule.getTokenType() + "\">" + escapeHTML(matched) + "</span>";
                
                highlightedLine.replace(match.position(), matched.length(), replacement);
                start = match.suffix().first;
            }
        }
        
        highlights.push_back({lineNumber, highlightedLine});
        lineNumber++;
    }
    
    return highlights;
}

std::string SyntaxHighlighter::generateHighlightedHTML(const std::string& code) {
    auto highlights = highlight(code);
    
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "  <title>CHTL Syntax Highlighted Code</title>\n";
    html << "  <style>\n";
    html << "    .code-container { font-family: 'Courier New', monospace; background: #f5f5f5; padding: 20px; }\n";
    html << "    .keyword { color: #0000ff; font-weight: bold; }\n";
    html << "    .string { color: #008000; }\n";
    html << "    .comment { color: #808080; font-style: italic; }\n";
    html << "    .element { color: #800080; font-weight: bold; }\n";
    html << "    .attribute { color: #ff0000; }\n";
    html << "    .template { color: #008080; font-weight: bold; }\n";
    html << "    .custom { color: #ff8000; font-weight: bold; }\n";
    html << "    .origin { color: #800000; font-weight: bold; }\n";
    html << "    .import { color: #000080; font-weight: bold; }\n";
    html << "    .constraint { color: #ff0080; font-weight: bold; }\n";
    html << "    .config { color: #808000; font-weight: bold; }\n";
    html << "  </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "  <div class=\"code-container\">\n";
    html << "    <pre>\n";
    
    for (const auto& [lineNumber, highlightedLine] : highlights) {
        html << highlightedLine << "\n";
    }
    
    html << "    </pre>\n";
    html << "  </div>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string SyntaxHighlighter::generateHighlightedCSS(const std::string& code) {
    auto highlights = highlight(code);
    
    std::stringstream css;
    css << "/* CHTL Syntax Highlighted Code CSS */\n";
    css << ".code-container {\n";
    css << "  font-family: 'Courier New', monospace;\n";
    css << "  background: #f5f5f5;\n";
    css << "  padding: 20px;\n";
    css << "}\n";
    css << ".keyword { color: #0000ff; font-weight: bold; }\n";
    css << ".string { color: #008000; }\n";
    css << ".comment { color: #808080; font-style: italic; }\n";
    css << ".element { color: #800080; font-weight: bold; }\n";
    css << ".attribute { color: #ff0000; }\n";
    css << ".template { color: #008080; font-weight: bold; }\n";
    css << ".custom { color: #ff8000; font-weight: bold; }\n";
    css << ".origin { color: #800000; font-weight: bold; }\n";
    css << ".import { color: #000080; font-weight: bold; }\n";
    css << ".constraint { color: #ff0080; font-weight: bold; }\n";
    css << ".config { color: #808000; font-weight: bold; }\n";
    
    return css.str();
}

void SyntaxHighlighter::addRule(const SyntaxHighlightRule& rule) {
    m_rules.push_back(rule);
}

void SyntaxHighlighter::addRule(const std::string& pattern, const std::string& tokenType, 
                               const std::string& foreground, const std::string& background) {
    m_rules.emplace_back(pattern, tokenType, foreground, background);
}

void SyntaxHighlighter::removeRule(const std::string& pattern) {
    m_rules.erase(std::remove_if(m_rules.begin(), m_rules.end(), 
        [&pattern](const SyntaxHighlightRule& rule) {
            return rule.getPattern() == pattern;
        }), m_rules.end());
}

void SyntaxHighlighter::clearRules() {
    m_rules.clear();
}

void SyntaxHighlighter::setTheme(const std::string& themeName) {
    m_currentTheme = themeName;
    // 这里可以加载主题特定的颜色配置
}

void SyntaxHighlighter::loadTheme(const std::string& themeFile) {
    std::ifstream file(themeFile);
    if (!file.is_open()) {
        m_context->addError("Cannot open theme file: " + themeFile);
        return;
    }
    
    // 这里可以解析主题文件并应用颜色配置
    // 简化实现，实际应该解析 JSON 或 XML 格式的主题文件
}

void SyntaxHighlighter::saveTheme(const std::string& themeFile) {
    std::ofstream file(themeFile);
    if (!file.is_open()) {
        m_context->addError("Cannot create theme file: " + themeFile);
        return;
    }
    
    // 这里可以保存当前主题配置
    // 简化实现，实际应该生成 JSON 或 XML 格式的主题文件
}

void SyntaxHighlighter::initializeDefaultRules() {
    // HTML 元素
    addRule(R"(html|head|body|div|span|p|h1|h2|h3|h4|h5|h6|a|img|ul|ol|li|table|tr|td|th|form|input|button|script|style)", "element");
    
    // CHTL 关键字
    addRule(R"\b(text|style|script|template|custom|origin|import|constraint|config|use|except|vir|listen|animate|router|fileloader|delegate)\b", "keyword");
    
    // 模板关键字
    addRule(R"(\[Template\]|@Style|@Element|@Var)", "template");
    
    // 自定义关键字
    addRule(R"(\[Custom\]|@Style|@Element|@Var)", "custom");
    
    // 原始嵌入关键字
    addRule(R"(\[Origin\]|@Html|@Style|@JavaScript)", "origin");
    
    // 导入关键字
    addRule(R"(\[Import\]|as)", "import");
    
    // 约束关键字
    addRule(R"(\[Constraints\]|except)", "constraint");
    
    // 配置关键字
    addRule(R"(\[Configuration\]|\[Name\]|\[OriginType\]|use)", "config");
    
    // 字符串
    addRule(R"("([^"\\]|\\.)*")", "string");
    addRule(R"'([^'\\]|\\.)*'", "string");
    
    // 注释
    addRule(R"(//.*$)", "comment");
    addRule(R"(/\*[\s\S]*?\*/)", "comment");
    
    // 属性
    addRule(R"(\w+\s*:)", "attribute");
    
    // 括号
    addRule(R"([{}()\[\]])", "bracket");
    
    // 操作符
    addRule(R"([=+\-*/%&|^!<>?:.,;])", "operator");
}

std::string SyntaxHighlighter::getTokenColor(const std::string& tokenType) const {
    static const std::unordered_map<std::string, std::string> colors = {
        {"keyword", "#0000ff"},
        {"string", "#008000"},
        {"comment", "#808080"},
        {"element", "#800080"},
        {"attribute", "#ff0000"},
        {"template", "#008080"},
        {"custom", "#ff8000"},
        {"origin", "#800000"},
        {"import", "#000080"},
        {"constraint", "#ff0080"},
        {"config", "#808000"},
        {"bracket", "#000000"},
        {"operator", "#000000"}
    };
    
    auto it = colors.find(tokenType);
    return (it != colors.end()) ? it->second : "#000000";
}

std::string SyntaxHighlighter::escapeHTML(const std::string& text) const {
    std::string result = text;
    
    // 转义 HTML 特殊字符
    std::replace(result.begin(), result.end(), '&', '&amp;');
    std::replace(result.begin(), result.end(), '<', '&lt;');
    std::replace(result.begin(), result.end(), '>', '&gt;');
    std::replace(result.begin(), result.end(), '"', '&quot;');
    std::replace(result.begin(), result.end(), '\'', '&#39;');
    
    return result;
}

// CodeFormatter 实现
CodeFormatter::CodeFormatter(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_indentSize(2), m_useTabs(false), m_maxLineLength(120), m_insertFinalNewline(true) {
    initializeDefaultRules();
}

std::string CodeFormatter::format(const std::string& code) {
    if (code.empty()) {
        return code;
    }
    
    std::string result = code;
    
    // 应用格式化规则
    for (const auto& [pattern, replacement] : m_formattingRules) {
        std::regex regexPattern(pattern);
        result = std::regex_replace(result, regexPattern, replacement);
    }
    
    // 处理缩进
    std::stringstream ss(result);
    std::string line;
    std::stringstream output;
    int indentLevel = 0;
    
    while (std::getline(ss, line)) {
        // 计算缩进级别
        if (line.find('}') != std::string::npos) {
            indentLevel = std::max(0, indentLevel - 1);
        }
        
        // 应用缩进
        std::string indentedLine = indentLine(line, indentLevel);
        output << indentedLine << "\n";
        
        // 更新缩进级别
        if (line.find('{') != std::string::npos) {
            indentLevel++;
        }
    }
    
    result = output.str();
    
    // 移除末尾空行
    while (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    // 添加最终换行符
    if (m_insertFinalNewline) {
        result += "\n";
    }
    
    return result;
}

std::string CodeFormatter::formatFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot open file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    
    return format(code);
}

void CodeFormatter::formatFileInPlace(const std::string& filePath) {
    std::string formattedCode = formatFile(filePath);
    if (formattedCode.empty()) {
        return;
    }
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot write to file: " + filePath);
        return;
    }
    
    file << formattedCode;
}

void CodeFormatter::addFormattingRule(const std::string& pattern, const std::string& replacement) {
    m_formattingRules[pattern] = replacement;
}

void CodeFormatter::removeFormattingRule(const std::string& pattern) {
    m_formattingRules.erase(pattern);
}

void CodeFormatter::clearFormattingRules() {
    m_formattingRules.clear();
}

void CodeFormatter::initializeDefaultRules() {
    // 在操作符周围添加空格
    addFormattingRule(R"(\s*([=+\-*/%&|^!<>?:])\s*", " $1 ");
    
    // 在逗号后添加空格
    addFormattingRule(R"(\s*,\s*", ", ");
    
    // 在分号后添加空格
    addFormattingRule(R"(\s*;\s*", "; ");
    
    // 在冒号后添加空格
    addFormattingRule(R"(\s*:\s*", ": ");
    
    // 移除多余的空格
    addFormattingRule(R"(\s{2,})", " ");
    
    // 在括号周围添加空格
    addFormattingRule(R"(\s*\(\s*", " (");
    addFormattingRule(R"(\s*\)\s*", ") ");
    addFormattingRule(R"(\s*\{\s*", " {");
    addFormattingRule(R"(\s*\}\s*", "} ");
}

std::string CodeFormatter::indentLine(const std::string& line, int level) const {
    if (line.empty()) {
        return line;
    }
    
    std::string indent = getIndentString();
    std::string result;
    
    for (int i = 0; i < level; ++i) {
        result += indent;
    }
    
    // 移除行首空格
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    
    result += trimmed;
    return result;
}

std::string CodeFormatter::getIndentString() const {
    if (m_useTabs) {
        return "\t";
    } else {
        return std::string(m_indentSize, ' ');
    }
}

bool CodeFormatter::shouldBreakLine(const std::string& line) const {
    return static_cast<int>(line.length()) > m_maxLineLength;
}

// CodeCompleter 实现
CodeCompleter::CodeCompleter(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    initializeDefaultCompletions();
}

std::vector<std::string> CodeCompleter::getCompletions(const std::string& code, int position) {
    std::vector<std::string> completions;
    
    if (position < 0 || position > static_cast<int>(code.length())) {
        return completions;
    }
    
    std::string context = getContextAtPosition(code, position);
    
    if (isInElementContext(code, position)) {
        auto elementCompletions = getElementCompletions();
        completions.insert(completions.end(), elementCompletions.begin(), elementCompletions.end());
    }
    
    if (isInAttributeContext(code, position)) {
        auto attributeCompletions = getAttributeCompletions();
        completions.insert(completions.end(), attributeCompletions.begin(), attributeCompletions.end());
    }
    
    if (isInTemplateContext(code, position)) {
        auto templateCompletions = getTemplateCompletions();
        completions.insert(completions.end(), templateCompletions.begin(), templateCompletions.end());
    }
    
    if (isInCustomContext(code, position)) {
        auto customCompletions = getCustomCompletions();
        completions.insert(completions.end(), customCompletions.begin(), customCompletions.end());
    }
    
    if (isInImportContext(code, position)) {
        auto importCompletions = getImportCompletions();
        completions.insert(completions.end(), importCompletions.begin(), importCompletions.end());
    }
    
    if (isInConfigContext(code, position)) {
        auto configCompletions = getConfigCompletions();
        completions.insert(completions.end(), configCompletions.begin(), configCompletions.end());
    }
    
    return completions;
}

std::vector<std::string> CodeCompleter::getCompletionsForContext(const std::string& code, int position) {
    return getCompletions(code, position);
}

std::vector<std::string> CodeCompleter::getElementCompletions() {
    return m_elementCompletions;
}

std::vector<std::string> CodeCompleter::getAttributeCompletions() {
    return m_attributeCompletions;
}

std::vector<std::string> CodeCompleter::getTemplateCompletions() {
    return m_templateCompletions;
}

std::vector<std::string> CodeCompleter::getCustomCompletions() {
    return m_customCompletions;
}

std::vector<std::string> CodeCompleter::getImportCompletions() {
    return m_importCompletions;
}

std::vector<std::string> CodeCompleter::getConfigCompletions() {
    return m_configCompletions;
}

std::vector<std::string> CodeCompleter::getSmartCompletions(const std::string& code, int position) {
    return getCompletions(code, position);
}

std::vector<std::string> CodeCompleter::getContextualCompletions(const std::string& code, int position) {
    return getCompletions(code, position);
}

std::vector<std::string> CodeCompleter::getSnippetCompletions(const std::string& code, int position) {
    std::vector<std::string> snippets;
    
    // 添加代码片段
    snippets.push_back("html { head { title { \"${1:Title}\" } } body { ${2:Content} } }");
    snippets.push_back("div { class: \"${1:class-name}\"; ${2:attributes} }");
    snippets.push_back("[Template] @Style ${1:StyleName} { ${2:styles} }");
    snippets.push_back("[Custom] @Style ${1:CustomName} { ${2:properties} }");
    snippets.push_back("[Origin] @Html { ${1:HTML content} }");
    snippets.push_back("[Import] \"${1:file-path}\";");
    snippets.push_back("[Constraints] except ${1:constraint} { }");
    snippets.push_back("[Configuration] ${1:ConfigName} { ${2:config} }");
    
    return snippets;
}

void CodeCompleter::addCompletion(const std::string& trigger, const std::string& completion) {
    m_completions[trigger] = completion;
}

void CodeCompleter::removeCompletion(const std::string& trigger) {
    m_completions.erase(trigger);
}

void CodeCompleter::clearCompletions() {
    m_completions.clear();
}

void CodeCompleter::initializeDefaultCompletions() {
    // HTML 元素补全
    m_elementCompletions = {
        "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "form", "input", "button",
        "script", "style", "title", "meta", "link"
    };
    
    // 属性补全
    m_attributeCompletions = {
        "class", "id", "style", "src", "href", "alt", "title", "type", "name", "value",
        "placeholder", "required", "disabled", "readonly", "checked", "selected"
    };
    
    // 模板补全
    m_templateCompletions = {
        "[Template] @Style", "[Template] @Element", "[Template] @Var",
        "inherit", "compositional"
    };
    
    // 自定义补全
    m_customCompletions = {
        "[Custom] @Style", "[Custom] @Element", "[Custom] @Var",
        "delete", "insert", "modify"
    };
    
    // 导入补全
    m_importCompletions = {
        "[Import]", "as", "from", "import"
    };
    
    // 配置补全
    m_configCompletions = {
        "[Configuration]", "[Name]", "[OriginType]", "use",
        "DEBUG_MODE", "INDEX_INITIAL_COUNT", "DISABLE_NAME_GROUP"
    };
}

std::string CodeCompleter::getContextAtPosition(const std::string& code, int position) const {
    if (position <= 0) {
        return "";
    }
    
    // 向前查找最近的上下文
    int start = position - 1;
    while (start >= 0 && code[start] != '\n') {
        start--;
    }
    
    return code.substr(start + 1, position - start - 1);
}

bool CodeCompleter::isInElementContext(const std::string& code, int position) const {
    std::string context = getContextAtPosition(code, position);
    return context.find('{') != std::string::npos && context.find('}') == std::string::npos;
}

bool CodeCompleter::isInAttributeContext(const std::string& code, int position) const {
    std::string context = getContextAtPosition(code, position);
    return context.find(':') != std::string::npos;
}

bool CodeCompleter::isInTemplateContext(const std::string& code, int position) const {
    std::string context = getContextAtPosition(code, position);
    return context.find("[Template]") != std::string::npos;
}

bool CodeCompleter::isInCustomContext(const std::string& code, int position) const {
    std::string context = getContextAtPosition(code, position);
    return context.find("[Custom]") != std::string::npos;
}

bool CodeCompleter::isInImportContext(const std::string& code, int position) const {
    std::string context = getContextAtPosition(code, position);
    return context.find("[Import]") != std::string::npos;
}

bool CodeCompleter::isInConfigContext(const std::string& code, int position) const {
    std::string context = getContextAtPosition(code, position);
    return context.find("[Configuration]") != std::string::npos;
}

} // namespace CHTL