#include "ScriptNode.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

ScriptNode::ScriptNode(bool local)
    : BaseNode(NodeType::SCRIPT, "script"), isLocalScript(local) {
}

void ScriptNode::addScriptFunction(std::shared_ptr<BaseNode> function) {
    if (function) {
        scriptFunctions.push_back(function);
    }
}

void ScriptNode::removeScriptFunction(std::shared_ptr<BaseNode> function) {
    auto it = std::find(scriptFunctions.begin(), scriptFunctions.end(), function);
    if (it != scriptFunctions.end()) {
        scriptFunctions.erase(it);
    }
}

std::string ScriptNode::getMinifiedScript() const {
    return minifyJavaScript(scriptContent);
}

std::string ScriptNode::getFormattedScript() const {
    return formatJavaScript(scriptContent);
}

bool ScriptNode::isEmpty() const {
    return scriptContent.empty();
}

bool ScriptNode::isWhitespace() const {
    return std::all_of(scriptContent.begin(), scriptContent.end(), [this](char c) {
        return isWhitespaceChar(c);
    });
}

bool ScriptNode::isValid() const {
    return true; // 脚本节点总是有效的
}

std::string ScriptNode::toHTML() const {
    if (isLocalScript) {
        // 局部脚本块不直接输出HTML，会被合并到全局脚本中
        return "";
    } else {
        // 全局脚本块输出为<script>标签
        return "<script>\n" + toJavaScript() + "\n</script>";
    }
}

std::string ScriptNode::toJavaScript() const {
    if (scriptContent.empty()) {
        return "";
    }
    
    // 处理脚本函数
    std::ostringstream oss;
    
    for (const auto& function : scriptFunctions) {
        oss << function->getValue() << "\n";
    }
    
    // 添加脚本内容
    if (!scriptContent.empty()) {
        oss << scriptContent;
    }
    
    return oss.str();
}

std::string ScriptNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << BaseNode::getDebugInfo();
    oss << "IsLocalScript: " << (isLocalScript ? "true" : "false") << "\n";
    oss << "ScriptContent: \"" << scriptContent << "\"\n";
    oss << "ScriptFunctions: " << scriptFunctions.size() << "\n";
    return oss.str();
}

std::shared_ptr<BaseNode> ScriptNode::clone() const {
    auto cloned = std::make_shared<ScriptNode>(isLocalScript);
    cloned->scriptContent = scriptContent;
    cloned->line = line;
    cloned->column = column;
    cloned->position = position;
    
    for (const auto& function : scriptFunctions) {
        cloned->addScriptFunction(function->clone());
    }
    
    return cloned;
}

bool ScriptNode::operator==(const ScriptNode& other) const {
    return BaseNode::operator==(other) &&
           isLocalScript == other.isLocalScript &&
           scriptContent == other.scriptContent &&
           scriptFunctions.size() == other.scriptFunctions.size();
}

bool ScriptNode::operator!=(const ScriptNode& other) const {
    return !(*this == other);
}

std::string ScriptNode::minifyJavaScript(const std::string& script) const {
    std::string minified = script;
    
    // 移除单行注释
    std::regex singleCommentRegex(R"(\/\/.*$)");
    minified = std::regex_replace(minified, singleCommentRegex, "");
    
    // 移除多行注释
    std::regex multiCommentRegex(R"(\/\*[\s\S]*?\*\/)");
    minified = std::regex_replace(minified, multiCommentRegex, "");
    
    // 压缩空白
    minified = compressWhitespace(minified);
    
    return minified;
}

std::string ScriptNode::formatJavaScript(const std::string& script) const {
    std::string formatted = script;
    
    // 简单的JavaScript格式化
    // 在花括号后添加换行
    std::regex braceRegex(R"(\{([^}]*)\})");
    formatted = std::regex_replace(formatted, braceRegex, "{\n    $1\n}");
    
    // 在分号后添加换行
    std::regex semicolonRegex(R"(;([^;]*))");
    formatted = std::regex_replace(formatted, semicolonRegex, ";\n$1");
    
    return formatted;
}

bool ScriptNode::isWhitespaceChar(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

std::vector<std::string> ScriptNode::parseScriptLines(const std::string& script) const {
    std::vector<std::string> lines;
    std::istringstream iss(script);
    std::string line;
    
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

std::string ScriptNode::parseScriptFunction(const std::string& function) const {
    // 简化的脚本函数解析
    return function;
}

std::string ScriptNode::optimizeScript(const std::string& script) const {
    std::string optimized = script;
    
    // 移除注释
    optimized = removeComments(optimized);
    
    // 压缩空白
    optimized = compressWhitespace(optimized);
    
    return optimized;
}

std::string ScriptNode::removeComments(const std::string& script) const {
    std::string cleaned = script;
    
    // 移除单行注释
    std::regex singleCommentRegex(R"(\/\/.*$)");
    cleaned = std::regex_replace(cleaned, singleCommentRegex, "");
    
    // 移除多行注释
    std::regex multiCommentRegex(R"(\/\*[\s\S]*?\*\/)");
    cleaned = std::regex_replace(cleaned, multiCommentRegex, "");
    
    return cleaned;
}

std::string ScriptNode::compressWhitespace(const std::string& script) const {
    std::string compressed = script;
    
    // 压缩多个连续空白字符
    std::regex whitespaceRegex(R"(\s+)");
    compressed = std::regex_replace(compressed, whitespaceRegex, " ");
    
    // 移除分号前的空白
    std::regex semicolonRegex(R"(\s*;\s*)");
    compressed = std::regex_replace(compressed, semicolonRegex, ";");
    
    // 去除首尾空白
    compressed.erase(0, compressed.find_first_not_of(" \t\n\r"));
    compressed.erase(compressed.find_last_not_of(" \t\n\r") + 1);
    
    return compressed;
}

} // namespace CHTL