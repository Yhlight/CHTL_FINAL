#include "CJMODGenerator.h"
#include "Syntax.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {
namespace CJMOD {

CJMODGenerator::CJMODGenerator() {}

CJMODGenerator::CJMODGenerator(const GenerationOptions& options) : options(options) {}

std::string CJMODGenerator::exportResult(const Arg& args) {
    CJMODGenerator generator;
    return generator.generate(args);
}

std::string CJMODGenerator::exportResult(const Arg& args, const GenerationOptions& options) {
    CJMODGenerator generator(options);
    return generator.generate(args);
}

std::string CJMODGenerator::generate(const Arg& args) {
    return generate(args, options);
}

std::string CJMODGenerator::generate(const Arg& args, const GenerationOptions& options) {
    if (!validateArg(args)) {
        return "";
    }
    
    std::string result;
    
    switch (options.mode) {
        case GenerationMode::JAVASCRIPT:
            result = generateJavaScript(args);
            break;
        case GenerationMode::CHTLJS:
            result = generateCHTLJS(args);
            break;
        case GenerationMode::HTML:
            result = generateHTML(args);
            break;
        case GenerationMode::CSS:
            result = generateCSS(args);
            break;
        case GenerationMode::MIXED:
            result = generateMixed(args);
            break;
        default:
            result = generateJavaScript(args);
            break;
    }
    
    // 应用自定义替换
    result = applyCustomReplacements(result);
    
    // 格式化代码
    if (options.minify) {
        result = minifyCode(result);
    } else {
        result = formatCode(result);
    }
    
    // 验证生成的代码
    if (!validateGeneratedCode(result)) {
        std::cerr << "Warning: Generated code validation failed" << std::endl;
    }
    
    // 记录生成历史
    generationHistory.push_back(result);
    
    return result;
}

std::vector<std::string> CJMODGenerator::generateBatch(const std::vector<Arg>& argsList) {
    std::vector<std::string> results;
    
    for (const auto& args : argsList) {
        std::string result = generate(args);
        results.push_back(result);
    }
    
    return results;
}

std::unordered_map<std::string, std::string> CJMODGenerator::generateMap(const std::unordered_map<std::string, Arg>& argsMap) {
    std::unordered_map<std::string, std::string> results;
    
    for (const auto& pair : argsMap) {
        std::string result = generate(pair.second);
        results[pair.first] = result;
    }
    
    return results;
}

bool CJMODGenerator::exportToFile(const std::string& filename, const Arg& args) {
    std::string code = generate(args);
    return exportToFile(filename, code);
}

bool CJMODGenerator::exportToFile(const std::string& filename, const std::string& code) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << code;
    file.close();
    
    return true;
}

std::string CJMODGenerator::exportToString(const Arg& args) {
    return generate(args);
}

void CJMODGenerator::setOptions(const GenerationOptions& options) {
    this->options = options;
}

GenerationOptions CJMODGenerator::getOptions() const {
    return options;
}

void CJMODGenerator::setMode(GenerationMode mode) {
    options.mode = mode;
}

void CJMODGenerator::setMinify(bool minify) {
    options.minify = minify;
}

void CJMODGenerator::setIncludeComments(bool include) {
    options.includeComments = include;
}

void CJMODGenerator::setPreserveWhitespace(bool preserve) {
    options.preserveWhitespace = preserve;
}

void CJMODGenerator::setIndentString(const std::string& indent) {
    options.indentString = indent;
}

void CJMODGenerator::setIndentLevel(int level) {
    options.indentLevel = level;
}

void CJMODGenerator::addCustomReplacement(const std::string& placeholder, const std::string& replacement) {
    options.customReplacements[placeholder] = replacement;
}

void CJMODGenerator::removeCustomReplacement(const std::string& placeholder) {
    options.customReplacements.erase(placeholder);
}

void CJMODGenerator::clearCustomReplacements() {
    options.customReplacements.clear();
}

std::unordered_map<std::string, std::string> CJMODGenerator::getCustomReplacements() const {
    return options.customReplacements;
}

std::vector<std::string> CJMODGenerator::getGenerationHistory() const {
    return generationHistory;
}

void CJMODGenerator::clearHistory() {
    generationHistory.clear();
}

std::string CJMODGenerator::getLastGenerated() const {
    if (generationHistory.empty()) {
        return "";
    }
    return generationHistory.back();
}

void CJMODGenerator::cacheGeneratedCode(const std::string& key, const std::string& code) {
    generatedCode[key] = code;
}

std::string CJMODGenerator::getCachedCode(const std::string& key) const {
    auto it = generatedCode.find(key);
    if (it != generatedCode.end()) {
        return it->second;
    }
    return "";
}

bool CJMODGenerator::hasCachedCode(const std::string& key) const {
    return generatedCode.find(key) != generatedCode.end();
}

void CJMODGenerator::clearCache() {
    generatedCode.clear();
}

std::unordered_map<std::string, std::string> CJMODGenerator::getAllCachedCode() const {
    return generatedCode;
}

void CJMODGenerator::registerTemplate(const std::string& name, const std::string& template) {
    templates[name] = template;
}

void CJMODGenerator::unregisterTemplate(const std::string& name) {
    templates.erase(name);
}

bool CJMODGenerator::hasTemplate(const std::string& name) const {
    return templates.find(name) != templates.end();
}

std::string CJMODGenerator::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    if (it != templates.end()) {
        return it->second;
    }
    return "";
}

std::unordered_map<std::string, std::string> CJMODGenerator::getAllTemplates() const {
    return templates;
}

bool CJMODGenerator::validateArg(const Arg& args) const {
    return args.validate();
}

std::vector<std::string> CJMODGenerator::getValidationErrors(const Arg& args) const {
    std::vector<std::string> errors;
    
    if (args.empty()) {
        errors.push_back("Empty argument list");
        return errors;
    }
    
    // 检查占位符
    std::vector<std::string> placeholders = args.getAllPlaceholders();
    for (const auto& placeholder : placeholders) {
        if (placeholder.empty()) {
            errors.push_back("Empty placeholder found");
        }
    }
    
    // 检查必需占位符
    if (args.hasRequiredPlaceholders()) {
        // 这里可以添加更详细的验证逻辑
    }
    
    return errors;
}

void CJMODGenerator::printGenerationStatistics() const {
    std::cout << "=== CJMOD Generator Statistics ===" << std::endl;
    std::cout << "Generation mode: " << static_cast<int>(options.mode) << std::endl;
    std::cout << "Minify: " << (options.minify ? "Yes" : "No") << std::endl;
    std::cout << "Include comments: " << (options.includeComments ? "Yes" : "No") << std::endl;
    std::cout << "Preserve whitespace: " << (options.preserveWhitespace ? "Yes" : "No") << std::endl;
    std::cout << "Indent string: '" << options.indentString << "'" << std::endl;
    std::cout << "Indent level: " << options.indentLevel << std::endl;
    std::cout << "Generation history size: " << generationHistory.size() << std::endl;
    std::cout << "Cached code entries: " << generatedCode.size() << std::endl;
    std::cout << "Registered templates: " << templates.size() << std::endl;
    std::cout << "Custom replacements: " << options.customReplacements.size() << std::endl;
    std::cout << "==================================" << std::endl;
}

void CJMODGenerator::printCacheStatistics() const {
    std::cout << "=== Cache Statistics ===" << std::endl;
    std::cout << "Cached entries: " << generatedCode.size() << std::endl;
    
    size_t totalSize = 0;
    for (const auto& pair : generatedCode) {
        totalSize += pair.second.length();
    }
    
    std::cout << "Total cached size: " << totalSize << " bytes" << std::endl;
    std::cout << "Average entry size: " << (generatedCode.empty() ? 0 : totalSize / generatedCode.size()) << " bytes" << std::endl;
    std::cout << "=======================" << std::endl;
}

void CJMODGenerator::printTemplateStatistics() const {
    std::cout << "=== Template Statistics ===" << std::endl;
    std::cout << "Registered templates: " << templates.size() << std::endl;
    
    for (const auto& pair : templates) {
        std::cout << "  " << pair.first << ": " << pair.second.length() << " characters" << std::endl;
    }
    
    std::cout << "==========================" << std::endl;
}

void CJMODGenerator::reset() {
    options = GenerationOptions();
    clearHistory();
    clearCache();
    clearCustomReplacements();
}

void CJMODGenerator::clear() {
    reset();
    templates.clear();
}

std::string CJMODGenerator::generateJavaScript(const Arg& args) {
    std::ostringstream js;
    
    if (options.includeComments) {
        js << "// Generated JavaScript code\n";
    }
    
    // 处理参数转换
    std::string transformResult = args.getTransformResult();
    if (!transformResult.empty()) {
        js << transformResult;
    } else {
        // 默认JavaScript生成
        for (const auto& arg : args) {
            if (arg) {
                if (arg->isPlaceholder()) {
                    js << arg->getValue();
                } else {
                    js << arg->getToken();
                }
                js << " ";
            }
        }
    }
    
    return js.str();
}

std::string CJMODGenerator::generateCHTLJS(const Arg& args) {
    std::ostringstream chtljs;
    
    if (options.includeComments) {
        chtljs << "// Generated CHTL JS code\n";
    }
    
    // CHTL JS特定生成逻辑
    for (const auto& arg : args) {
        if (arg) {
            if (arg->isPlaceholder()) {
                chtljs << arg->getValue();
            } else {
                chtljs << arg->getToken();
            }
            chtljs << " ";
        }
    }
    
    return chtljs.str();
}

std::string CJMODGenerator::generateHTML(const Arg& args) {
    std::ostringstream html;
    
    if (options.includeComments) {
        html << "<!-- Generated HTML code -->\n";
    }
    
    // HTML生成逻辑
    for (const auto& arg : args) {
        if (arg) {
            if (arg->isPlaceholder()) {
                html << arg->getValue();
            } else {
                html << arg->getToken();
            }
        }
    }
    
    return html.str();
}

std::string CJMODGenerator::generateCSS(const Arg& args) {
    std::ostringstream css;
    
    if (options.includeComments) {
        css << "/* Generated CSS code */\n";
    }
    
    // CSS生成逻辑
    for (const auto& arg : args) {
        if (arg) {
            if (arg->isPlaceholder()) {
                css << arg->getValue();
            } else {
                css << arg->getToken();
            }
            css << " ";
        }
    }
    
    return css.str();
}

std::string CJMODGenerator::generateMixed(const Arg& args) {
    std::ostringstream mixed;
    
    if (options.includeComments) {
        mixed << "/* Generated mixed code */\n";
    }
    
    // 混合代码生成逻辑
    for (const auto& arg : args) {
        if (arg) {
            if (arg->isPlaceholder()) {
                mixed << arg->getValue();
            } else {
                mixed << arg->getToken();
            }
            mixed << " ";
        }
    }
    
    return mixed.str();
}

std::string CJMODGenerator::processTemplate(const std::string& template, const Arg& args) {
    std::string result = template;
    
    // 替换占位符
    result = replacePlaceholders(result, args);
    
    // 应用自定义替换
    result = applyCustomReplacements(result);
    
    return result;
}

std::string CJMODGenerator::replacePlaceholders(const std::string& template, const Arg& args) {
    std::string result = template;
    
    for (const auto& arg : args) {
        if (arg && arg->isPlaceholder()) {
            std::string placeholder = arg->getToken();
            std::string value = arg->getValue();
            
            // 替换模板中的占位符
            size_t pos = 0;
            while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                result.replace(pos, placeholder.length(), value);
                pos += value.length();
            }
        }
    }
    
    return result;
}

std::string CJMODGenerator::applyCustomReplacements(const std::string& code) {
    std::string result = code;
    
    for (const auto& replacement : options.customReplacements) {
        size_t pos = 0;
        while ((pos = result.find(replacement.first, pos)) != std::string::npos) {
            result.replace(pos, replacement.first.length(), replacement.second);
            pos += replacement.second.length();
        }
    }
    
    return result;
}

std::string CJMODGenerator::formatCode(const std::string& code) {
    if (!options.preserveWhitespace) {
        return code;
    }
    
    // 基本格式化
    std::string formatted = code;
    
    // 标准化换行
    formatted = std::regex_replace(formatted, std::regex(R"(\r\n)"), "\n");
    formatted = std::regex_replace(formatted, std::regex(R"(\r)"), "\n");
    
    // 标准化缩进
    if (options.indentLevel > 0) {
        formatted = indent(formatted, options.indentLevel);
    }
    
    // 添加注释
    if (options.includeComments) {
        formatted = addComments(formatted);
    }
    
    return formatted;
}

std::string CJMODGenerator::minifyCode(const std::string& code) {
    std::string minified = code;
    
    // 移除注释
    minified = removeComments(minified);
    
    // 移除多余空白
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    minified = std::regex_replace(minified, std::regex(R"(\n\s*)"), "\n");
    
    // 移除行尾空白
    minified = std::regex_replace(minified, std::regex(R"(\s+\n)"), "\n");
    
    return minified;
}

std::string CJMODGenerator::beautifyCode(const std::string& code) {
    // 美化代码的简单实现
    std::string beautified = code;
    
    // 标准化缩进
    beautified = indent(beautified, 0);
    
    // 添加适当的换行
    beautified = std::regex_replace(beautified, std::regex(R"(\{([^\n]))"), "{\n$1");
    beautified = std::regex_replace(beautified, std::regex(R"(([^\n])\})"), "$1\n}");
    
    return beautified;
}

bool CJMODGenerator::validateGeneratedCode(const std::string& code) {
    // 基本验证
    if (code.empty()) {
        return false;
    }
    
    // 检查括号平衡
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
        
        if (parenCount < 0 || braceCount < 0 || bracketCount < 0) {
            return false;
        }
    }
    
    return parenCount == 0 && braceCount == 0 && bracketCount == 0;
}

std::vector<std::string> CJMODGenerator::getValidationErrors(const std::string& code) {
    std::vector<std::string> errors;
    
    if (code.empty()) {
        errors.push_back("Empty code");
        return errors;
    }
    
    // 检查括号平衡
    int parenCount = 0;
    int braceCount = 0;
    int bracketCount = 0;
    
    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];
        switch (c) {
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
        
        if (parenCount < 0) {
            errors.push_back("Unmatched closing parenthesis at position " + std::to_string(i));
        }
        if (braceCount < 0) {
            errors.push_back("Unmatched closing brace at position " + std::to_string(i));
        }
        if (bracketCount < 0) {
            errors.push_back("Unmatched closing bracket at position " + std::to_string(i));
        }
    }
    
    if (parenCount > 0) {
        errors.push_back("Unmatched opening parenthesis");
    }
    if (braceCount > 0) {
        errors.push_back("Unmatched opening brace");
    }
    if (bracketCount > 0) {
        errors.push_back("Unmatched opening bracket");
    }
    
    return errors;
}

std::string CJMODGenerator::indent(const std::string& code, int level) {
    if (level <= 0) {
        return code;
    }
    
    std::ostringstream result;
    std::istringstream iss(code);
    std::string line;
    
    std::string indentStr;
    for (int i = 0; i < level; ++i) {
        indentStr += options.indentString;
    }
    
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            result << indentStr << line << "\n";
        } else {
            result << line << "\n";
        }
    }
    
    return result.str();
}

std::string CJMODGenerator::addComments(const std::string& code) {
    if (!options.includeComments) {
        return code;
    }
    
    std::ostringstream result;
    
    switch (options.mode) {
        case GenerationMode::JAVASCRIPT:
            result << "// Generated by CJMOD Generator\n";
            break;
        case GenerationMode::CHTLJS:
            result << "// Generated CHTL JS code\n";
            break;
        case GenerationMode::HTML:
            result << "<!-- Generated HTML code -->\n";
            break;
        case GenerationMode::CSS:
            result << "/* Generated CSS code */\n";
            break;
        default:
            result << "// Generated code\n";
            break;
    }
    
    result << code;
    return result.str();
}

std::string CJMODGenerator::removeComments(const std::string& code) {
    std::string result = code;
    
    // 移除单行注释
    result = std::regex_replace(result, std::regex(R"(//.*$)"), "", std::regex_constants::multiline);
    
    // 移除多行注释
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "", std::regex_constants::multiline);
    
    // 移除HTML注释
    result = std::regex_replace(result, std::regex(R"(<!--.*?-->)"), "", std::regex_constants::multiline);
    
    return result;
}

} // namespace CJMOD
} // namespace CHTL