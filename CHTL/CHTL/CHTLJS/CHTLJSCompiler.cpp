#include "CHTLJSCompiler.hpp"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

CHTLJSCompiler::CHTLJSCompiler() : outputFormat_("javascript"), optimizationLevel_(1) {
    initializeDefaultOptions();
}

std::string CHTLJSCompiler::compile(const std::string& chtljsCode) {
    validationErrors_.clear();
    
    if (!validate(chtljsCode)) {
        return "";
    }
    
    // 这里需要根据具体的CHTL JS语法进行编译
    // 暂时返回一个基本的实现
    std::ostringstream compiled;
    
    // 添加文件头
    compiled << "// Compiled CHTL JS Code\n";
    compiled << "(function() {\n";
    compiled << "    'use strict';\n\n";
    
    // 编译代码
    compiled << chtljsCode;
    
    // 添加文件尾
    compiled << "\n})();\n";
    
    std::string result = compiled.str();
    
    // 应用优化
    if (optimizationLevel_ > 0) {
        result = optimize(result);
    }
    
    return result;
}

std::string CHTLJSCompiler::compileFileLoader(const FileLoaderNode& fileLoader) {
    return fileLoader.toJavaScript();
}

std::string CHTLJSCompiler::compileEnhancedSelector(const EnhancedSelectorNode& selector) {
    return selector.toJavaScript();
}

std::string CHTLJSCompiler::compileListen(const ListenNode& listen) {
    return listen.toJavaScript();
}

std::string CHTLJSCompiler::compileDelegate(const DelegateNode& delegate) {
    return delegate.toJavaScript();
}

std::string CHTLJSCompiler::compileAnimate(const AnimateNode& animate) {
    return animate.toJavaScript();
}

std::string CHTLJSCompiler::compileVir(const VirNode& vir) {
    return vir.toJavaScript();
}

std::string CHTLJSCompiler::compileRouter(const RouterNode& router) {
    return router.toJavaScript();
}

std::string CHTLJSCompiler::compileLocalScript(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& nodes) {
    std::ostringstream compiled;
    
    compiled << "// Local Script\n";
    compiled << "(function() {\n";
    compiled << "    'use strict';\n\n";
    
    for (const auto& node : nodes) {
        if (node) {
            compiled << node->toJavaScript() << "\n";
        }
    }
    
    compiled << "})();\n";
    
    return compiled.str();
}

std::string CHTLJSCompiler::compileGlobalScript(const std::vector<std::shared_ptr<CHTLJSBaseNode>>& nodes) {
    std::ostringstream compiled;
    
    compiled << "// Global Script\n";
    compiled << "'use strict';\n\n";
    
    for (const auto& node : nodes) {
        if (node) {
            compiled << node->toJavaScript() << "\n";
        }
    }
    
    return compiled.str();
}

void CHTLJSCompiler::setCompileOption(const std::string& option, const std::string& value) {
    compileOptions_[option] = value;
}

std::string CHTLJSCompiler::getCompileOption(const std::string& option) const {
    auto it = compileOptions_.find(option);
    return (it != compileOptions_.end()) ? it->second : "";
}

void CHTLJSCompiler::setOutputFormat(const std::string& format) {
    outputFormat_ = format;
}

std::string CHTLJSCompiler::getOutputFormat() const {
    return outputFormat_;
}

void CHTLJSCompiler::addCustomCompiler(const std::string& nodeType, 
                                      std::function<std::string(const CHTLJSBaseNode&)> compiler) {
    customCompilers_[nodeType] = compiler;
}

void CHTLJSCompiler::removeCustomCompiler(const std::string& nodeType) {
    customCompilers_.erase(nodeType);
}

std::map<std::string, std::string> CHTLJSCompiler::getAllCompileOptions() const {
    return compileOptions_;
}

void CHTLJSCompiler::clearCompileOptions() {
    compileOptions_.clear();
    initializeDefaultOptions();
}

bool CHTLJSCompiler::validate(const std::string& chtljsCode) {
    validationErrors_.clear();
    
    if (chtljsCode.empty()) {
        validationErrors_.push_back("Empty CHTL JS code");
        return false;
    }
    
    // 基本的语法验证
    if (chtljsCode.find("{{") != std::string::npos && chtljsCode.find("}}") == std::string::npos) {
        validationErrors_.push_back("Unclosed enhanced selector");
        return false;
    }
    
    if (chtljsCode.find("}}") != std::string::npos && chtljsCode.find("{{") == std::string::npos) {
        validationErrors_.push_back("Unopened enhanced selector");
        return false;
    }
    
    return true;
}

std::vector<std::string> CHTLJSCompiler::getValidationErrors() const {
    return validationErrors_;
}

std::string CHTLJSCompiler::optimize(const std::string& compiledCode) {
    if (optimizationLevel_ <= 0) {
        return compiledCode;
    }
    
    std::string optimized = compiledCode;
    
    if (optimizationLevel_ >= 1) {
        // 基本优化
        optimized = cleanCode(optimized);
    }
    
    if (optimizationLevel_ >= 2) {
        // 中级优化
        optimized = optimizeCode(optimized);
    }
    
    if (optimizationLevel_ >= 3) {
        // 高级优化
        optimized = minifyCode(optimized);
    }
    
    return optimized;
}

void CHTLJSCompiler::setOptimizationLevel(int level) {
    optimizationLevel_ = std::max(0, std::min(3, level));
}

int CHTLJSCompiler::getOptimizationLevel() const {
    return optimizationLevel_;
}

void CHTLJSCompiler::initializeDefaultOptions() {
    compileOptions_["strictMode"] = "true";
    compileOptions_["minify"] = "false";
    compileOptions_["sourceMap"] = "false";
    compileOptions_["beautify"] = "true";
    compileOptions_["removeComments"] = "false";
    compileOptions_["removeWhitespace"] = "false";
    compileOptions_["optimize"] = "true";
    compileOptions_["warnings"] = "true";
    compileOptions_["errors"] = "true";
}

bool CHTLJSCompiler::validateNode(const CHTLJSBaseNode& node) {
    // 基本的节点验证
    if (node.getType() == CHTLJSBaseNode::NodeType::FILELOADER) {
        // 验证文件载入器
        return true;
    } else if (node.getType() == CHTLJSBaseNode::NodeType::ENHANCED_SELECTOR) {
        // 验证增强选择器
        return true;
    } else if (node.getType() == CHTLJSBaseNode::NodeType::LISTEN) {
        // 验证事件监听器
        return true;
    } else if (node.getType() == CHTLJSBaseNode::NodeType::DELEGATE) {
        // 验证事件委托
        return true;
    } else if (node.getType() == CHTLJSBaseNode::NodeType::ANIMATE) {
        // 验证动画
        return true;
    } else if (node.getType() == CHTLJSBaseNode::NodeType::VIR) {
        // 验证虚对象
        return true;
    } else if (node.getType() == CHTLJSBaseNode::NodeType::ROUTER) {
        // 验证路由
        return true;
    }
    
    return true;
}

std::string CHTLJSCompiler::optimizeCode(const std::string& code) {
    std::string optimized = code;
    
    // 移除多余的空行
    std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
    optimized = std::regex_replace(optimized, multipleNewlines, "\n\n");
    
    // 移除行尾空格
    std::regex trailingSpaces(R"(\s+\n)");
    optimized = std::regex_replace(optimized, trailingSpaces, "\n");
    
    return optimized;
}

std::string CHTLJSCompiler::minifyCode(const std::string& code) {
    std::string minified = code;
    
    // 移除注释
    if (getCompileOption("removeComments") == "true") {
        std::regex singleLineComment(R"(//.*$)");
        std::regex multiLineComment(R"(/\*.*?\*/)");
        minified = std::regex_replace(minified, singleLineComment, "");
        minified = std::regex_replace(minified, multiLineComment, "");
    }
    
    // 移除多余空格
    if (getCompileOption("removeWhitespace") == "true") {
        std::regex multipleSpaces(R"(\s+)");
        minified = std::regex_replace(minified, multipleSpaces, " ");
        
        // 移除不必要的空格
        std::regex unnecessarySpaces(R"(\s*([{}();,=+\-*/%<>!&|^~?:])\s*)");
        minified = std::regex_replace(minified, unnecessarySpaces, "$1");
    }
    
    return minified;
}

std::string CHTLJSCompiler::beautifyCode(const std::string& code) {
    if (getCompileOption("beautify") != "true") {
        return code;
    }
    
    std::string beautified = code;
    
    // 添加适当的缩进
    int indentLevel = 0;
    std::ostringstream result;
    
    for (size_t i = 0; i < beautified.length(); ++i) {
        char c = beautified[i];
        
        if (c == '{' || c == '[') {
            result << c << "\n";
            indentLevel++;
            for (int j = 0; j < indentLevel; ++j) {
                result << "    ";
            }
        } else if (c == '}' || c == ']') {
            result << "\n";
            indentLevel--;
            for (int j = 0; j < indentLevel; ++j) {
                result << "    ";
            }
            result << c;
        } else if (c == ';') {
            result << c << "\n";
            for (int j = 0; j < indentLevel; ++j) {
                result << "    ";
            }
        } else {
            result << c;
        }
    }
    
    return result.str();
}

std::string CHTLJSCompiler::addSourceMap(const std::string& code, const std::string& sourceMap) {
    if (getCompileOption("sourceMap") != "true") {
        return code;
    }
    
    return code + "\n//# sourceMappingURL=data:application/json;base64," + sourceMap;
}

std::string CHTLJSCompiler::generateSourceMap(const std::string& originalCode, const std::string& compiledCode) {
    // 简单的源映射生成
    std::ostringstream sourceMap;
    sourceMap << "{\n";
    sourceMap << "  \"version\": 3,\n";
    sourceMap << "  \"sources\": [\"input.chtljs\"],\n";
    sourceMap << "  \"names\": [],\n";
    sourceMap << "  \"mappings\": \"AAAA\"\n";
    sourceMap << "}";
    
    return sourceMap.str();
}

void CHTLJSCompiler::handleError(const std::string& error) {
    if (getCompileOption("errors") == "true") {
        validationErrors_.push_back("Error: " + error);
    }
}

void CHTLJSCompiler::handleWarning(const std::string& warning) {
    if (getCompileOption("warnings") == "true") {
        validationErrors_.push_back("Warning: " + warning);
    }
}

std::string CHTLJSCompiler::cleanCode(const std::string& code) {
    std::string cleaned = code;
    
    // 移除BOM
    if (cleaned.length() >= 3 && 
        static_cast<unsigned char>(cleaned[0]) == 0xEF &&
        static_cast<unsigned char>(cleaned[1]) == 0xBB &&
        static_cast<unsigned char>(cleaned[2]) == 0xBF) {
        cleaned = cleaned.substr(3);
    }
    
    // 统一换行符
    std::regex crlf(R"(\r\n)");
    std::regex cr(R"(\r)");
    cleaned = std::regex_replace(cleaned, crlf, "\n");
    cleaned = std::regex_replace(cleaned, cr, "\n");
    
    return cleaned;
}

std::string CHTLJSCompiler::escapeString(const std::string& str) {
    std::string escaped = str;
    
    // 转义特殊字符
    std::regex backslash(R"(\\)");
    std::regex quote(R"(")");
    std::regex newline(R"(\n)");
    std::regex tab(R"(\t)");
    
    escaped = std::regex_replace(escaped, backslash, "\\\\");
    escaped = std::regex_replace(escaped, quote, "\\\"");
    escaped = std::regex_replace(escaped, newline, "\\n");
    escaped = std::regex_replace(escaped, tab, "\\t");
    
    return escaped;
}

std::string CHTLJSCompiler::unescapeString(const std::string& str) {
    std::string unescaped = str;
    
    // 取消转义特殊字符
    std::regex escapedBackslash(R"(\\\\)");
    std::regex escapedQuote(R"(\\\")");
    std::regex escapedNewline(R"(\\n)");
    std::regex escapedTab(R"(\\t)");
    
    unescaped = std::regex_replace(unescaped, escapedBackslash, "\\");
    unescaped = std::regex_replace(unescaped, escapedQuote, "\"");
    unescaped = std::regex_replace(unescaped, escapedNewline, "\n");
    unescaped = std::regex_replace(unescaped, escapedTab, "\t");
    
    return unescaped;
}

bool CHTLJSCompiler::validateJavaScriptSyntax(const std::string& code) {
    // 基本的JavaScript语法验证
    if (code.empty()) {
        return true;
    }
    
    // 检查括号匹配
    int parenCount = 0;
    int braceCount = 0;
    int bracketCount = 0;
    
    for (char c : code) {
        switch (c) {
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
    }
    
    return parenCount == 0 && braceCount == 0 && bracketCount == 0;
}

bool CHTLJSCompiler::validateCSSSyntax(const std::string& code) {
    // 基本的CSS语法验证
    if (code.empty()) {
        return true;
    }
    
    // 检查大括号匹配
    int braceCount = 0;
    for (char c : code) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
    }
    
    return braceCount == 0;
}

bool CHTLJSCompiler::validateHTMLSyntax(const std::string& code) {
    // 基本的HTML语法验证
    if (code.empty()) {
        return true;
    }
    
    // 检查标签匹配
    std::regex tagRegex(R"(<[^>]*>)");
    std::smatch matches;
    std::string codeCopy = code;
    
    while (std::regex_search(codeCopy, matches, tagRegex)) {
        std::string tag = matches[0].str();
        if (tag[1] == '/') {
            // 结束标签
        } else {
            // 开始标签
        }
        codeCopy = matches.suffix().str();
    }
    
    return true;
}

} // namespace CHTL