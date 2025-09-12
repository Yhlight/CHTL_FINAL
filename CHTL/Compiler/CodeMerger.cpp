#include "CodeMerger.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <fstream>
#include <chrono>
#include <future>

namespace CHTL {
namespace Compiler {

CodeMerger::CodeMerger() : debugMode(false) {}

CodeMerger::~CodeMerger() = default;

void CodeMerger::setConfig(const MergeConfig& config) {
    this->config = config;
}

MergeConfig CodeMerger::getConfig() const {
    return config;
}

void CodeMerger::addCodeBlock(const CodeBlock& block) {
    codeBlocks.push_back(block);
}

void CodeMerger::addCodeBlock(CodeType type, const std::string& content, const std::string& filename) {
    CodeBlock block;
    block.type = type;
    block.content = content;
    block.filename = filename;
    codeBlocks.push_back(block);
}

void CodeMerger::removeCodeBlock(size_t index) {
    if (index < codeBlocks.size()) {
        codeBlocks.erase(codeBlocks.begin() + index);
    }
}

void CodeMerger::clearCodeBlocks() {
    codeBlocks.clear();
}

std::string CodeMerger::processHTML(const std::string& html) const {
    std::string result = html;
    
    if (config.removeComments) {
        result = cleanHTML(result);
    }
    
    if (config.minifyHTML) {
        result = optimizeHTML(result);
    }
    
    if (!config.preserveWhitespace) {
        result = formatHTML(result);
    }
    
    return result;
}

std::string CodeMerger::processCSS(const std::string& css) const {
    std::string result = css;
    
    if (config.removeComments) {
        result = cleanCSS(result);
    }
    
    if (config.minifyCSS) {
        result = optimizeCSS(result);
    }
    
    if (!config.preserveWhitespace) {
        result = formatCSS(result);
    }
    
    return result;
}

std::string CodeMerger::processJavaScript(const std::string& js) const {
    std::string result = js;
    
    if (config.removeComments) {
        result = cleanJavaScript(result);
    }
    
    if (config.minifyJavaScript) {
        result = optimizeJavaScript(result);
    }
    
    if (!config.preserveWhitespace) {
        result = formatJavaScript(result);
    }
    
    return result;
}

std::string CodeMerger::optimizeHTML(const std::string& html) const {
    std::string result = html;
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除HTML注释
    result = std::regex_replace(result, std::regex(R"(<!--.*?-->)"), "");
    
    // 移除多余换行
    result = std::regex_replace(result, std::regex(R"(\n\s*\n)"), "\n");
    
    return result;
}

std::string CodeMerger::optimizeCSS(const std::string& css) const {
    std::string result = css;
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除CSS注释
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除多余分号
    result = std::regex_replace(result, std::regex(R"(\s*;\s*})"), "}");
    
    return result;
}

std::string CodeMerger::optimizeJavaScript(const std::string& js) const {
    std::string result = js;
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除JavaScript注释
    result = std::regex_replace(result, std::regex(R"(//.*?\n)"), "");
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除多余分号
    result = std::regex_replace(result, std::regex(R"(\s*;\s*})"), "}");
    
    return result;
}

std::string CodeMerger::cleanHTML(const std::string& html) const {
    std::string result = html;
    
    // 移除HTML注释
    result = std::regex_replace(result, std::regex(R"(<!--.*?-->)"), "");
    
    // 移除空行
    if (config.removeEmptyLines) {
        result = std::regex_replace(result, std::regex(R"(\n\s*\n)"), "\n");
    }
    
    return result;
}

std::string CodeMerger::cleanCSS(const std::string& css) const {
    std::string result = css;
    
    // 移除CSS注释
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除空行
    if (config.removeEmptyLines) {
        result = std::regex_replace(result, std::regex(R"(\n\s*\n)"), "\n");
    }
    
    return result;
}

std::string CodeMerger::cleanJavaScript(const std::string& js) const {
    std::string result = js;
    
    // 移除JavaScript注释
    result = std::regex_replace(result, std::regex(R"(//.*?\n)"), "");
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除空行
    if (config.removeEmptyLines) {
        result = std::regex_replace(result, std::regex(R"(\n\s*\n)"), "\n");
    }
    
    return result;
}

std::string CodeMerger::formatHTML(const std::string& html) const {
    std::string result = html;
    
    // 简单的HTML格式化
    result = std::regex_replace(result, std::regex(R"(<([^>]+)>)"), "\n<\\1>");
    result = std::regex_replace(result, std::regex(R"(</([^>]+)>)"), "\n</\\1>");
    
    return result;
}

std::string CodeMerger::formatCSS(const std::string& css) const {
    std::string result = css;
    
    // 简单的CSS格式化
    result = std::regex_replace(result, std::regex(R"(\{)"), " {\n");
    result = std::regex_replace(result, std::regex(R"(\})"), "\n}\n");
    result = std::regex_replace(result, std::regex(R"(;)"), ";\n");
    
    return result;
}

std::string CodeMerger::formatJavaScript(const std::string& js) const {
    std::string result = js;
    
    // 简单的JavaScript格式化
    result = std::regex_replace(result, std::regex(R"(\{)"), " {\n");
    result = std::regex_replace(result, std::regex(R"(\})"), "\n}\n");
    result = std::regex_replace(result, std::regex(R"(;)"), ";\n");
    
    return result;
}

std::vector<std::string> CodeMerger::extractDependencies(const CodeBlock& block) const {
    std::vector<std::string> dependencies;
    
    // 简单的依赖提取
    std::regex importRegex(R"((?:import|require)\s*\(\s*['"]([^'"]+)['"]\s*\))");
    std::sregex_iterator iter(block.content.begin(), block.content.end(), importRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        dependencies.push_back((*iter)[1].str());
    }
    
    return dependencies;
}

std::vector<CodeBlock> CodeMerger::resolveDependencies(const std::vector<CodeBlock>& blocks) const {
    // 简单的依赖解析
    // 实际实现应该使用更复杂的依赖解析算法
    return blocks;
}

bool CodeMerger::detectConflicts(const std::vector<CodeBlock>& blocks) const {
    // 简单的冲突检测
    // 实际实现应该检测变量名冲突、函数名冲突等
    return false;
}

std::vector<std::string> CodeMerger::resolveConflicts(const std::vector<CodeBlock>& blocks) const {
    std::vector<std::string> resolutions;
    
    // 简单的冲突解决
    // 实际实现应该提供具体的冲突解决策略
    
    return resolutions;
}

MergeResult CodeMerger::merge() {
    return merge(codeBlocks);
}

MergeResult CodeMerger::merge(const std::vector<CodeBlock>& blocks) {
    MergeResult result;
    
    try {
        // 分离不同类型的代码块
        std::vector<CodeBlock> htmlBlocks, cssBlocks, jsBlocks;
        
        for (const auto& block : blocks) {
            switch (block.type) {
                case CodeType::HTML:
                    htmlBlocks.push_back(block);
                    break;
                case CodeType::CSS:
                    cssBlocks.push_back(block);
                    break;
                case CodeType::JavaScript:
                    jsBlocks.push_back(block);
                    break;
                default:
                    break;
            }
        }
        
        // 合并HTML
        for (const auto& block : htmlBlocks) {
            result.html += processHTML(block.content) + "\n";
        }
        
        // 合并CSS
        for (const auto& block : cssBlocks) {
            result.css += processCSS(block.content) + "\n";
        }
        
        // 合并JavaScript
        for (const auto& block : jsBlocks) {
            result.javascript += processJavaScript(block.content) + "\n";
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("合并错误: " + std::string(e.what()));
    }
    
    notifyMergeComplete(result);
    return result;
}

MergeResult CodeMerger::mergeHTML(const std::vector<CodeBlock>& blocks) {
    MergeResult result;
    
    try {
        for (const auto& block : blocks) {
            if (block.type == CodeType::HTML) {
                result.html += processHTML(block.content) + "\n";
            }
        }
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("HTML合并错误: " + std::string(e.what()));
    }
    
    return result;
}

MergeResult CodeMerger::mergeCSS(const std::vector<CodeBlock>& blocks) {
    MergeResult result;
    
    try {
        for (const auto& block : blocks) {
            if (block.type == CodeType::CSS) {
                result.css += processCSS(block.content) + "\n";
            }
        }
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("CSS合并错误: " + std::string(e.what()));
    }
    
    return result;
}

MergeResult CodeMerger::mergeJavaScript(const std::vector<CodeBlock>& blocks) {
    MergeResult result;
    
    try {
        for (const auto& block : blocks) {
            if (block.type == CodeType::JavaScript) {
                result.javascript += processJavaScript(block.content) + "\n";
            }
        }
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("JavaScript合并错误: " + std::string(e.what()));
    }
    
    return result;
}

MergeResult CodeMerger::smartMerge() {
    return smartMerge(codeBlocks);
}

MergeResult CodeMerger::smartMerge(const std::vector<CodeBlock>& blocks) {
    MergeResult result;
    
    try {
        // 解析依赖
        std::vector<CodeBlock> resolvedBlocks = resolveDependencies(blocks);
        
        // 检测冲突
        if (detectConflicts(resolvedBlocks)) {
            std::vector<std::string> resolutions = resolveConflicts(resolvedBlocks);
            result.warnings.insert(result.warnings.end(), resolutions.begin(), resolutions.end());
        }
        
        // 智能合并
        result = merge(resolvedBlocks);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("智能合并错误: " + std::string(e.what()));
    }
    
    return result;
}

MergeResult CodeMerger::incrementalMerge(const std::vector<CodeBlock>& newBlocks) {
    MergeResult result;
    
    try {
        // 增量合并逻辑
        // 实际实现应该只处理新增或修改的代码块
        
        std::vector<CodeBlock> allBlocks = codeBlocks;
        allBlocks.insert(allBlocks.end(), newBlocks.begin(), newBlocks.end());
        
        result = merge(allBlocks);
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("增量合并错误: " + std::string(e.what()));
    }
    
    return result;
}

MergeResult CodeMerger::parallelMerge() {
    return parallelMerge(codeBlocks);
}

MergeResult CodeMerger::parallelMerge(const std::vector<CodeBlock>& blocks) {
    MergeResult result;
    
    try {
        // 并行合并逻辑
        // 实际实现应该使用多线程处理不同类型的代码块
        
        std::vector<CodeBlock> htmlBlocks, cssBlocks, jsBlocks;
        
        for (const auto& block : blocks) {
            switch (block.type) {
                case CodeType::HTML:
                    htmlBlocks.push_back(block);
                    break;
                case CodeType::CSS:
                    cssBlocks.push_back(block);
                    break;
                case CodeType::JavaScript:
                    jsBlocks.push_back(block);
                    break;
                default:
                    break;
            }
        }
        
        // 并行处理
        std::future<MergeResult> htmlFuture = std::async(std::launch::async, [this, &htmlBlocks]() {
            return mergeHTML(htmlBlocks);
        });
        
        std::future<MergeResult> cssFuture = std::async(std::launch::async, [this, &cssBlocks]() {
            return mergeCSS(cssBlocks);
        });
        
        std::future<MergeResult> jsFuture = std::async(std::launch::async, [this, &jsBlocks]() {
            return mergeJavaScript(jsBlocks);
        });
        
        // 等待结果
        MergeResult htmlResult = htmlFuture.get();
        MergeResult cssResult = cssFuture.get();
        MergeResult jsResult = jsFuture.get();
        
        // 合并结果
        result.success = htmlResult.success && cssResult.success && jsResult.success;
        result.html = htmlResult.html;
        result.css = cssResult.css;
        result.javascript = jsResult.javascript;
        
        result.errors.insert(result.errors.end(), htmlResult.errors.begin(), htmlResult.errors.end());
        result.errors.insert(result.errors.end(), cssResult.errors.begin(), cssResult.errors.end());
        result.errors.insert(result.errors.end(), jsResult.errors.begin(), jsResult.errors.end());
        
        result.warnings.insert(result.warnings.end(), htmlResult.warnings.begin(), htmlResult.warnings.end());
        result.warnings.insert(result.warnings.end(), cssResult.warnings.begin(), cssResult.warnings.end());
        result.warnings.insert(result.warnings.end(), jsResult.warnings.begin(), jsResult.warnings.end());
        
    } catch (const std::exception& e) {
        result.success = false;
        result.errors.push_back("并行合并错误: " + std::string(e.what()));
    }
    
    return result;
}

CodeMerger::CodeAnalysis CodeMerger::analyzeCode() const {
    return analyzeCode(codeBlocks);
}

CodeMerger::CodeAnalysis CodeMerger::analyzeCode(const std::vector<CodeBlock>& blocks) const {
    CodeAnalysis analysis;
    
    for (const auto& block : blocks) {
        analysis.totalLines += std::count(block.content.begin(), block.content.end(), '\n') + 1;
        analysis.totalCharacters += block.content.length();
        analysis.typeCount[block.type]++;
        
        if (!block.filename.empty()) {
            analysis.fileCount[block.filename]++;
        }
        
        std::vector<std::string> deps = extractDependencies(block);
        analysis.dependencies.insert(analysis.dependencies.end(), deps.begin(), deps.end());
    }
    
    if (detectConflicts(blocks)) {
        analysis.conflicts = resolveConflicts(blocks);
    }
    
    return analysis;
}

bool CodeMerger::validateCode(const CodeBlock& block) const {
    // 简单的代码验证
    // 实际实现应该使用专门的验证器
    
    switch (block.type) {
        case CodeType::HTML:
            return block.content.find('<') != std::string::npos;
        case CodeType::CSS:
            return block.content.find('{') != std::string::npos;
        case CodeType::JavaScript:
            return block.content.find("function") != std::string::npos || 
                   block.content.find("var") != std::string::npos ||
                   block.content.find("let") != std::string::npos ||
                   block.content.find("const") != std::string::npos;
        default:
            return true;
    }
}

bool CodeMerger::validateCode(const std::vector<CodeBlock>& blocks) const {
    for (const auto& block : blocks) {
        if (!validateCode(block)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> CodeMerger::getValidationErrors(const CodeBlock& block) const {
    std::vector<std::string> errors;
    
    if (!validateCode(block)) {
        errors.push_back("代码验证失败: " + block.filename);
    }
    
    return errors;
}

std::vector<std::string> CodeMerger::getValidationErrors(const std::vector<CodeBlock>& blocks) const {
    std::vector<std::string> errors;
    
    for (const auto& block : blocks) {
        std::vector<std::string> blockErrors = getValidationErrors(block);
        errors.insert(errors.end(), blockErrors.begin(), blockErrors.end());
    }
    
    return errors;
}

std::string CodeMerger::convertToHTML(const CodeBlock& block) const {
    switch (block.type) {
        case CodeType::HTML:
            return block.content;
        case CodeType::CSS:
            return "<style>\n" + block.content + "\n</style>";
        case CodeType::JavaScript:
            return "<script>\n" + block.content + "\n</script>";
        default:
            return block.content;
    }
}

std::string CodeMerger::convertToCSS(const CodeBlock& block) const {
    switch (block.type) {
        case CodeType::CSS:
            return block.content;
        case CodeType::HTML: {
            // 提取HTML中的CSS
            std::regex styleRegex(R"(<style[^>]*>(.*?)</style>)", std::regex_constants::ECMAScript);
            std::smatch match;
            if (std::regex_search(block.content, match, styleRegex)) {
                return match[1].str();
            }
            return "";
        }
        default:
            return "";
    }
}

std::string CodeMerger::convertToJavaScript(const CodeBlock& block) const {
    switch (block.type) {
        case CodeType::JavaScript:
            return block.content;
        case CodeType::HTML: {
            // 提取HTML中的JavaScript
            std::regex scriptRegex(R"(<script[^>]*>(.*?)</script>)", std::regex_constants::ECMAScript);
            std::smatch match;
            if (std::regex_search(block.content, match, scriptRegex)) {
                return match[1].str();
            }
            return "";
        }
        default:
            return "";
    }
}

std::string CodeMerger::generateHTML(const MergeResult& result) const {
    std::ostringstream oss;
    
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    
    if (!result.css.empty()) {
        oss << "<style>\n" << result.css << "\n</style>\n";
    }
    
    oss << "</head>\n";
    oss << "<body>\n";
    
    if (!result.html.empty()) {
        oss << result.html << "\n";
    }
    
    if (!result.javascript.empty()) {
        oss << "<script>\n" << result.javascript << "\n</script>\n";
    }
    
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

std::string CodeMerger::generateCSS(const MergeResult& result) const {
    return result.css;
}

std::string CodeMerger::generateJavaScript(const MergeResult& result) const {
    return result.javascript;
}

std::string CodeMerger::generateCombined(const MergeResult& result) const {
    return generateHTML(result);
}

std::vector<std::string> CodeMerger::getGlobalErrors() const {
    return globalErrors;
}

std::vector<std::string> CodeMerger::getGlobalWarnings() const {
    return globalWarnings;
}

void CodeMerger::clearGlobalErrors() {
    globalErrors.clear();
}

void CodeMerger::clearGlobalWarnings() {
    globalWarnings.clear();
}

std::string CodeMerger::generateMergeReport() const {
    CodeAnalysis analysis = analyzeCode();
    
    std::ostringstream oss;
    oss << "=== 代码合并报告 ===\n";
    oss << "总行数: " << analysis.totalLines << "\n";
    oss << "总字符数: " << analysis.totalCharacters << "\n";
    
    oss << "\n代码类型统计:\n";
    for (const auto& pair : analysis.typeCount) {
        oss << "  " << static_cast<int>(pair.first) << ": " << pair.second << " 个\n";
    }
    
    oss << "\n文件统计:\n";
    for (const auto& pair : analysis.fileCount) {
        oss << "  " << pair.first << ": " << pair.second << " 次\n";
    }
    
    if (!analysis.dependencies.empty()) {
        oss << "\n依赖关系:\n";
        for (const auto& dep : analysis.dependencies) {
            oss << "  " << dep << "\n";
        }
    }
    
    if (!analysis.conflicts.empty()) {
        oss << "\n冲突:\n";
        for (const auto& conflict : analysis.conflicts) {
            oss << "  " << conflict << "\n";
        }
    }
    
    return oss.str();
}

void CodeMerger::enableDebugMode(bool enable) {
    debugMode = enable;
}

bool CodeMerger::isDebugMode() const {
    return debugMode;
}

void CodeMerger::setMergeCallback(std::function<void(const MergeResult&)> callback) {
    mergeCallback = callback;
}

void CodeMerger::setProgressCallback(std::function<void(int, int)> callback) {
    progressCallback = callback;
}

void CodeMerger::notifyProgress(int current, int total) {
    if (progressCallback) {
        progressCallback(current, total);
    }
}

void CodeMerger::notifyMergeComplete(const MergeResult& result) {
    if (mergeCallback) {
        mergeCallback(result);
    }
}

std::string CodeMerger::escapeHTML(const std::string& text) const {
    std::string result = text;
    result = std::regex_replace(result, std::regex("&"), "&amp;");
    result = std::regex_replace(result, std::regex("<"), "&lt;");
    result = std::regex_replace(result, std::regex(">"), "&gt;");
    result = std::regex_replace(result, std::regex("\""), "&quot;");
    result = std::regex_replace(result, std::regex("'"), "&#39;");
    return result;
}

std::string CodeMerger::escapeCSS(const std::string& text) const {
    // CSS转义
    return text;
}

std::string CodeMerger::escapeJavaScript(const std::string& text) const {
    // JavaScript转义
    return text;
}

std::string CodeMerger::generateSourceMap(const MergeResult& result) const {
    // 简单的源码映射生成
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"version\": 3,\n";
    oss << "  \"sources\": [],\n";
    oss << "  \"names\": [],\n";
    oss << "  \"mappings\": \"\"\n";
    oss << "}\n";
    return oss.str();
}

} // namespace Compiler
} // namespace CHTL